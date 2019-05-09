#include <stdio.h>

// int main(int argc, char** argv)
// {
//     printf("hello world!\n");
//     return 0;
// }


#include "tbox/tbox.h" 
#define __tb_debug__

static tb_bool_t tb_http_demo_head_func(tb_char_t const* line, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(line, tb_false);

    // trace
    tb_trace_i("head: %s", line);

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_network_http_main(void)
{
    // done
    tb_http_ref_t http = tb_null;
    do
    {
        // init http
        http = tb_http_init();
        tb_assert_and_check_break(http);

        // init cookies
        if (!tb_http_ctrl(http, TB_HTTP_OPTION_SET_COOKIES, tb_cookies())) break;
        
        // init head func
        if (!tb_http_ctrl(http, TB_HTTP_OPTION_SET_HEAD_FUNC, tb_http_demo_head_func)) break;
        
        // init url
        if (!tb_http_ctrl(http, TB_HTTP_OPTION_SET_URL, "http://www.baidu.com")) break;

        // init head 
        if (!tb_http_ctrl(http, TB_HTTP_OPTION_SET_HEAD, "Connection", "keep-alive")) break;

        // init timeout 
        tb_size_t timeout = 0;
    //  if (!tb_http_ctrl(http, TB_HTTP_OPTION_SET_TIMEOUT, 10000)) break;
        if (!tb_http_ctrl(http, TB_HTTP_OPTION_GET_TIMEOUT, &timeout)) break;

        // init redirect maxn
        if (!tb_http_ctrl(http, TB_HTTP_OPTION_SET_REDIRECT, 0)) break;

        // open http
        tb_hong_t t = tb_mclock();
        if (!tb_http_open(http)) break;
        t = tb_mclock() - t;
        tb_trace_i("open: %llu ms", t);

        // read data
        tb_byte_t       data[8192];
        tb_size_t       read = 0;
        tb_hize_t       size = tb_http_status(http)->content_size;
        do
        {
            // read data
            tb_long_t real = tb_http_read(http, data, 8192);
            tb_trace_i("read: %d", real);
            if (real > 0)
            {
                // dump data
                tb_char_t const*    p = (tb_char_t const*)data;
                tb_char_t const*    e = (tb_char_t const*)data + real;
                tb_char_t           b[8192 + 1];
                while (p < e && *p)
                {
                    tb_char_t*          q = b;
                    tb_char_t const*    d = b + 4096;
                    for (; q < d && p < e && *p; p++, q++) *q = *p;
                    *q = '\0';
                    tb_printf("%s", b);
                }
                tb_printf("\n");

                // save read
                read += real;
            }
            else if (!real) 
            {
                // wait
                tb_trace_i("wait");
                tb_long_t e = tb_http_wait(http, TB_SOCKET_EVENT_RECV, timeout);
                tb_assert_and_check_break(e >= 0);

                // timeout?
                tb_check_break(e);

                // has read?
                tb_assert_and_check_break(e & TB_SOCKET_EVENT_RECV);
            }
            else break;

            // is end?
            if (size && read >= size) break;

        } while (1);

    } while (0);

    // exit http
    if (http) tb_http_exit(http);

    // end
    return 0;
}


static void _http_demo_(void)
{
    tb_http_ref_t http = tb_null;
    tb_size_t timeout = 0;
    do{
        http = tb_http_init();
        tb_assert_and_check_break(http);
        
        tb_trace_i("init!");

        if(!tb_http_ctrl(http, TB_HTTP_OPTION_SET_COOKIES, tb_cookies())) {
            tb_trace_i("cookies failed!");
            break;
        }
        if(!tb_http_ctrl(http, TB_HTTP_OPTION_SET_URL, "http://www.baidu.com")) {
            tb_trace_i("url failed");
            break;
        }
        if(!tb_http_ctrl(http, TB_HTTP_OPTION_GET_TIMEOUT, &timeout)) {
            tb_trace_i("timeout failed");
            break;
        }

        tb_trace_i("!!!");
        
        tb_hong_t t = tb_mclock();
        if(!tb_http_open(http)) {
            tb_trace_i("open failed");
            break;
        }
        t = tb_mclock() - t;
        tb_trace_i("opetion: %llu ms", t);

        tb_byte_t data[8192];
        tb_size_t read = 0;
        tb_hize_t size = tb_http_status(http)->content_size;
        do
        {
            // read data
            tb_long_t real = tb_http_read(http, data, 8192);
            tb_trace_i("read: %d", real);
            if (real > 0)
            {
                // dump data
                tb_char_t const*    p = (tb_char_t const*)data;
                tb_char_t const*    e = (tb_char_t const*)data + real;
                tb_char_t           b[8192 + 1];
                while (p < e && *p)
                {
                    tb_char_t*          q = b;
                    tb_char_t const*    d = b + 4096;
                    for (; q < d && p < e && *p; p++, q++) *q = *p;
                    *q = '\0';
                    tb_printf("%s", b);
                }
                tb_printf("\n");

                // save read
                read += real;
            }
            else if (!real) 
            {
                // wait
                tb_trace_i("wait");
                tb_long_t e = tb_http_wait(http, TB_SOCKET_EVENT_RECV, timeout);
                tb_assert_and_check_break(e >= 0);

                // timeout?
                tb_check_break(e);

                // has read?
                tb_assert_and_check_break(e & TB_SOCKET_EVENT_RECV);
            }
            else break;

            // is end?
            if (size && read >= size) break;

        } while (1);
    } while(0);

    if(http) tb_http_exit(http);
}

int main(int argc, char** argv)
{
    // 初始化整个tbox库环境，这个是必须的，但是只需要在程序最开始执行一下
    if (!tb_init(tb_null, tb_null)) return 0;

    // 打印输出，需要自己加换行符
    tb_printf("hello world!\n");

    // 只在debug下打印输出一行
    tb_trace_d("hello world debug");

    // 在debug/release下打印输出一行
    tb_trace_i("hello world release");

    // debug下进行断言检测
    tb_assert(1 == 1);
    // tb_assert_abort(1 != 2);

    // 空等待
    getchar();

    // _http_demo_();
    tb_demo_network_http_main();

    // 退出整个tbox环境，这步会释放tbox占用的所有资源，如果是debug版本，还会在终端输出内存泄露检测信息等等。
    tb_exit();
    return 0;
}