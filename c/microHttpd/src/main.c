/* Feel free to use this example code in any way
   you see fit (Public Domain) */

#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "microhttpd.h"

#define ENCODE 0
#if ENCODE
#include "encode.h"
#endif

#ifdef _MSC_VER
#ifndef strcasecmp
#define strcasecmp(a, b) _stricmp((a), (b))
#endif /* !strcasecmp */
#endif /* _MSC_VER */

#if defined(_MSC_VER) && _MSC_VER + 0 <= 1800
/* Substitution is OK while return value is not used */
#define snprintf _snprintf
#endif

#define PORT 8888
#define POSTBUFFERSIZE 512
#define MAXCLIENTS 2

enum ConnectionType
{
    GET = 0,
    POST = 1
};

static unsigned int nr_of_uploading_clients = 0;

/**
 * Information we keep per connection.
 */
struct connection_info_struct
{
    enum ConnectionType connectiontype;

    /**
   * Handle to the POST processing state.
   */
    struct MHD_PostProcessor *postprocessor;

    /**
   * File handle where we write uploaded data.
   */
    FILE *fp;

    /**
   * HTTP response body we will return, NULL if not yet known.
   */
    const char *answerstring;

    /**
   * HTTP status code we will return, 0 for undecided.
   */
    unsigned int answercode;

    char *cmd;
    char *filename;
    char *reply;
};
const char *askpage =
    "<html><body>\n\
                <form action=\"/filepost\" method=\"post\" enctype=\"multipart/form-data\">\n\
                Input CMD here: \n\
                <input type=\"text\" cols=\"80\" name=\"cmd\" value=\"echo hello\"><br>\n\
                Upload a file here: \n\
                <input name=\"file\" type=\"file\">\n\
                <input type=\"submit\" value=\" Send \"></form>\n\
                </body></html>";
const char *busypage =
    "<html><body>This server is busy, please try again later.</body></html>";
const char *completepage =
    "<html><body>The upload has been completed.</body></html>";
const char *errorpage =
    "<html><body>This doesn't seem to be right.</body></html>";
const char *servererrorpage =
    "<html><body>Invalid request.</body></html>";
const char *fileexistspage =
    "<html><body>This file already exists.</body></html>";
const char *fileioerror =
    "<html><body>IO error writing to disk.</body></html>";
const char *const postprocerror =
    "<html><head><title>Error</title></head><body>Error processing POST data</body></html>";

#define REPLY_FMT "<html>\
                        <head charset=\"UTF-8\"><title>LOG</title></head>\
                        <body charset=\"UTF-8\">Upload %s %s<br><br>\
                        <textarea charset=\"UTF-8\" lang=\"en_US\" rows=\"40\" cols=\"80\">%s</textarea>\
                        </body>\
                    </html>"

#define S_REPLY_FMT strlen(REPLY_FMT)

static enum MHD_Result
send_page(struct MHD_Connection *connection,
          const char *page,
          int status_code)
{
    enum MHD_Result ret;
    struct MHD_Response *response;

    response =
        MHD_create_response_from_buffer(strlen(page),
                                        (void *)page,
                                        MHD_RESPMEM_MUST_COPY);
    if (!response)
        return MHD_NO;
    MHD_add_response_header(response,
                            MHD_HTTP_HEADER_CONTENT_TYPE,
                            "text/html");
    ret = MHD_queue_response(connection,
                             status_code,
                             response);
    MHD_destroy_response(response);

    return ret;
}

typedef enum MHD_Result (*cmd_cb)(void *,
                                  enum MHD_ValueKind, const char *,
                                  const char *, const char *,
                                  const char *, const char *,
                                  uint64_t, size_t);

typedef struct _cmd_hash
{
    const char *cmd;
    cmd_cb cb;
} _cmd_hash_t;

#define _SER_HASHS \
    _SER_XXX(file) \
    _SER_XXX(cmd)

static enum MHD_Result _file_handler(void *coninfo_cls,
                                     enum MHD_ValueKind kind,
                                     const char *key,
                                     const char *filename,
                                     const char *content_type,
                                     const char *transfer_encoding,
                                     const char *data,
                                     uint64_t off,
                                     size_t size)
{
    struct connection_info_struct *con_info = coninfo_cls;
    FILE *fp;
    (void)kind;              /* Unused. Silent compiler warning. */
    (void)content_type;      /* Unused. Silent compiler warning. */
    (void)transfer_encoding; /* Unused. Silent compiler warning. */
    (void)off;               /* Unused. Silent compiler warning. */

    if (con_info->answercode)
    {
        return MHD_YES;
    }

    if (strcasecmp(key, "file") != 0)
    {
        return MHD_YES;
    }

    if (!con_info->fp)
    {
        if (0 != con_info->answercode) /* something went wrong */
            return MHD_YES;
        if (NULL != (fp = fopen(filename, "rb")))
        {
            fclose(fp);
            remove(filename);
        }
        con_info->fp = fopen(filename, "ab");
        if (!con_info->fp)
        {
            con_info->answerstring = fileioerror;
            con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;
            return MHD_YES;
        }
    }

    if (size > 0)
    {
        if (!fwrite(data, sizeof(char), size, con_info->fp))
        {
            con_info->answerstring = fileioerror;
            con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;
            return MHD_YES;
        }
    }

    if (!con_info->filename)
    {
        con_info->filename = strdup(filename);
    }

    return MHD_YES;
}

static enum MHD_Result _cmd_handler(void *coninfo_cls,
                                    enum MHD_ValueKind kind,
                                    const char *key,
                                    const char *filename,
                                    const char *content_type,
                                    const char *transfer_encoding,
                                    const char *data,
                                    uint64_t off,
                                    size_t size)
{
    struct connection_info_struct *con_info = coninfo_cls;
    FILE *fp;
    (void)kind;              /* Unused. Silent compiler warning. */
    (void)content_type;      /* Unused. Silent compiler warning. */
    (void)transfer_encoding; /* Unused. Silent compiler warning. */
    (void)off;               /* Unused. Silent compiler warning. */
    if (strcasecmp(key, "cmd") != 0)
    {
        return MHD_YES;
    }
    con_info->cmd = strdup(data);
    return MHD_YES;
}

_cmd_hash_t cbs[] = {
#define _SER_XXX(key) {#key, _##key##_handler},
    _SER_HASHS
#undef _SER_XXX
    {NULL, NULL}};

static enum MHD_Result
iterate_post(void *coninfo_cls,
             enum MHD_ValueKind kind,
             const char *key,
             const char *filename,
             const char *content_type,
             const char *transfer_encoding,
             const char *data,
             uint64_t off,
             size_t size)
{
    _cmd_hash_t *pc = cbs;
    enum MHD_Result result = MHD_YES;
    while (pc->cb && pc->cmd)
    {
        if (0 == strcasecmp(key, pc->cmd))
        {
            result &= pc->cb(coninfo_cls, kind, key, filename, content_type, transfer_encoding, data, off, size);
        }
        ++pc;
    }
    return result;
}

#define _TAR_ ("##FILE##")
#define _TAR_LEN_ strlen(_TAR_)

char *replace(char *tar, char *arg)
{
    if (!tar)
    {
        return NULL;
    }
    char *ret = NULL;
    char *ph = tar;
    char *pt = NULL;
    size_t offset = 0;
    size_t arglen = arg ? strlen(arg) : 0;
    size_t tarlen = strlen(tar);

    while ((pt = strstr(ph, _TAR_)) != NULL)
    {
        ret = realloc(ret, (ret ? strlen(ret) : 0) + tarlen + arglen);
        memcpy(ret + offset, ph, pt - ph);
        offset += pt - ph;
        memcpy(ret + offset, arg, arglen);
        offset += arglen;
        ret[offset] = 0;
        ph = pt + _TAR_LEN_;
    }
    if (!ret)
    {
        ret = strdup(tar);
    }
    else
    {
        size_t l = strlen(ph);
        memcpy(ret + offset, ph, l);
        *(ret + offset + l) = 0;
    }
    return ret;
}

static char *_call_system(const char *cmd)
{
    if (!cmd)
    {
        return NULL;
    }
    char *ret = NULL;
    char buf[1024] = {0};
    int len = 0;
    int offset = 0;
    FILE *fp = popen(cmd, "r");
    while ((len = fread(buf, 1, sizeof(buf), fp)) != 0)
    {
        ret = realloc(ret, offset + len);
        if (!ret)
        {
            return ret;
        }
        memcpy(ret + offset, buf, len);
        offset += len;
    }
    if (ret)
    {
        ret[offset] = 0;
    }
#if ENCODE
    char *encode_ret = encode(ret);
    free(ret);
    return encode_ret;
#else
    return ret;
#endif
}

static char *sreply = NULL;

static void
request_completed(void *cls,
                  struct MHD_Connection *connection,
                  void **con_cls,
                  enum MHD_RequestTerminationCode toe)
{
    struct connection_info_struct *con_info = *con_cls;
    (void)cls; /* Unused. Silent compiler warning. */
    (void)toe; /* Unused. Silent compiler warning. */
    if (sreply)
    {
        free(sreply);
        sreply = NULL;
    }
    if (NULL == con_info)
        return;

    if (con_info->connectiontype == POST)
    {
        if (NULL != con_info->postprocessor)
        {
            MHD_destroy_post_processor(con_info->postprocessor);
            nr_of_uploading_clients--;
        }

        if (con_info->fp)
            fclose(con_info->fp);
    }

    if (con_info->cmd)
    {
        free(con_info->cmd);
        con_info->cmd = NULL;
    }
    if (con_info->filename)
    {
        free(con_info->filename);
        con_info->filename = NULL;
    }
    if (con_info->reply)
    {
        free(con_info->reply);
        con_info->reply = NULL;
    }
    free(con_info);
    *con_cls = NULL;
}

static enum MHD_Result
answer_to_connection(void *cls,
                     struct MHD_Connection *connection,
                     const char *url,
                     const char *method,
                     const char *version,
                     const char *upload_data,
                     size_t *upload_data_size,
                     void **con_cls)
{
    (void)cls;     /* Unused. Silent compiler warning. */
    (void)url;     /* Unused. Silent compiler warning. */
    (void)version; /* Unused. Silent compiler warning. */
    if (NULL == *con_cls)
    {
        /* First call, setup data structures */
        struct connection_info_struct *con_info;

        if (nr_of_uploading_clients >= MAXCLIENTS)
            return send_page(connection,
                             busypage,
                             MHD_HTTP_SERVICE_UNAVAILABLE);

        con_info = malloc(sizeof(struct connection_info_struct));
        if (NULL == con_info)
            return MHD_NO;
        con_info->answercode = 0; /* none yet */
        con_info->fp = NULL;
        con_info->cmd = NULL;
        con_info->filename = NULL;
        con_info->reply = NULL;

        if (0 == strcasecmp(method, MHD_HTTP_METHOD_POST))
        {
            con_info->postprocessor =
                MHD_create_post_processor(connection,
                                          POSTBUFFERSIZE,
                                          &iterate_post,
                                          (void *)con_info);

            if (NULL == con_info->postprocessor)
            {
                free(con_info);
                return MHD_NO;
            }

            nr_of_uploading_clients++;

            con_info->connectiontype = POST;
        }
        else
        {
            con_info->connectiontype = GET;
        }

        *con_cls = (void *)con_info;

        return MHD_YES;
    }

    if (0 == strcasecmp(method, MHD_HTTP_METHOD_GET))
    {
        /* We just return the standard form for uploads on all GET requests */
        char buffer[1024] = {0};

        strncpy(buffer, askpage, sizeof(buffer) - 1);
        return send_page(connection,
                         buffer,
                         MHD_HTTP_OK);
    }

    if (0 == strcasecmp(method, MHD_HTTP_METHOD_POST))
    {
        struct connection_info_struct *con_info = *con_cls;

        if (0 != *upload_data_size)
        {
            /* Upload not yet done */
            if (0 != con_info->answercode)
            {
                /* we already know the answer, skip rest of upload */
                *upload_data_size = 0;
                return MHD_YES;
            }
            if (MHD_YES !=
                MHD_post_process(con_info->postprocessor,
                                 upload_data,
                                 *upload_data_size))
            {
                con_info->answerstring = postprocerror;
                con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;
            }
            *upload_data_size = 0;

            return MHD_YES;
        }
        /* Upload finished */
        if (NULL != con_info->fp)
        {
            fclose(con_info->fp);
            con_info->fp = NULL;
        }

        char *cmd = replace(con_info->cmd, con_info->filename);
        if (cmd)
        {
            char *reply = NULL;
            reply = _call_system(cmd);
            size_t len = S_REPLY_FMT + (reply ? strlen(reply) : 1) + (con_info->filename ? strlen(con_info->filename) : 0) + 10;
            sreply = malloc(len);
            snprintf(sreply, len, REPLY_FMT, con_info->filename, con_info->answercode == 0 ? "Success" : "Failed", reply ?: "");
            if (reply)
            {
                free(reply);
            }
            free(cmd);
            con_info->answerstring = sreply;
            con_info->answercode = MHD_HTTP_OK;
        }
        else if (0 == con_info->answercode)
        {
            /* No errors encountered, declare success */
            con_info->answerstring = completepage;
            con_info->answercode = MHD_HTTP_OK;
        }

        return send_page(connection,
                         con_info->answerstring,
                         con_info->answercode);
        // return MHD_YES;
    }

    /* Note a GET or a POST, generate error */
    return send_page(connection,
                     errorpage,
                     MHD_HTTP_BAD_REQUEST);
}

int main(int argc, char *argv[])
{
    int port = 80;
    if (argc > 1)
    {
        port = atoi(argv[1]);
    }

    if (port <= 0)
    {
        port = 80;
    }

    printf("Server Listen on %d\n", port);
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD,
                              port, NULL, NULL,
                              &answer_to_connection, NULL,
                              MHD_OPTION_NOTIFY_COMPLETED, &request_completed,
                              NULL,
                              MHD_OPTION_END);
    if (NULL == daemon)
    {
        fprintf(stderr,
                "Failed to start daemon.\n");
        return 1;
    }
    (void)getchar();
    MHD_stop_daemon(daemon);
    return 0;
}
