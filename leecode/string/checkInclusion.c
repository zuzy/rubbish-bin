#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct str_check_tag {
    int ch[26];
};
typedef struct str_check_tag str_check;

void _setStr2Chk(const char *s, str_check *chk, size_t len)
{
    int i;
    memset(chk, 0, sizeof(*chk));

    for(i = 0; i < len && s[i]; ++i) {
        ++chk->ch[s[i] - 'a'];
    }
}

bool _check_(str_check *chk, str_check *chk2)
{
    int i = 0;
    for(; i < 26; ++i) {
        // printf("%d check %d %d\n",i , chk->ch[i], chk2->ch[i]);
        if(chk->ch[i] != chk2->ch[i]) {
            return false;
        }
    }
    return true;
}

bool checkInclusion(char* s1, char* s2) {
    str_check *chk = calloc(1, sizeof(str_check));
    int len = strlen(s1);
    int len2 = strlen(s2);
    int len3 = len2 - len;
    _setStr2Chk(s1, chk, len);
    char *p = s2;
    int en = 0;
    int i = 0;
    for(; i <= len3; ++i) {
        str_check chk2;
        _setStr2Chk(&s2[i], &chk2, len);
        if(memcmp(chk, &chk2, sizeof(str_check)) == 0) {
            return true;
        }
    }
    return false;
}
/*
"adc"
"dcda"

"hello"
"ooolleoooleh"

"abc"
"bbbca"

*/

/*
""
""
*/

int main(int argc, char*argv[])
{
    char *s1 = "hello";
    char *s2 = "ooolleoooleh";
    bool ret = checkInclusion(s1, s2);
    printf("get ret is %d\n", ret);

}