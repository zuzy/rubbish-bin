#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief :  https://leetcode-cn.com/problems/orderly-queue/submissions/
 * @note : 
 * ? 参考题解......
 * ? 当K == 1时为循环数组, 无法改变顺序, 
 * ? 当K == 2时可以交换任意相邻内容, 这样就满足了冒泡排序的条件, 整个数组就可以被排序!
 * @param S 
 * @param K 
 * @return char* 
 */
static char *_find_min(char *S, int K)
{
    char *ptr = S;
    int i;
    for(i = 0; i < K; i++) {
        if(S[i] < *ptr) {
            ptr = &S[i];
        }
    }
    return ptr;
}

char * orderlyQueue(char * S, int K)
{
    char *ret = calloc(1,strlen(S) + 1);
    if(K == 1) {
        int size = strlen(S);
        char *min = _find_min(S, size);
        char *min_back = _find_min(min + 1, strlen(min + 1));
        while((*min == *min_back) && (min != min_back)) {
            // printf("while\n min %s \nbck %s\n", min, min_back);
            char *pm = min;
            char *pmb = min_back;
            int i;
            for(i = 0; i < size; i++) {
                if(*pm == 0) pm = S;
                if(*pmb == 0) pmb = S;
                // printf("%c %c\n", *pm, *pmb);
                if(*pm > *pmb) {
                    min = min_back;
                    break;
                } else if(*pm < *pmb) {
                    break;
                }
                ++pm;
                ++pmb;
            }
            
            ++min_back ;
            min_back = _find_min(min_back, strlen(min_back));
        }

        // printf("min is %s\n", min);

        strcat(ret, min);
        if(min != S) {
            strncat(ret, S, min - S);
        }
    } else {
        int alphabet[26] = {0};
        char *p = S;
        while(*p) {
            ++alphabet[*p - 'a'];
            ++p;
        }
        int i;
        p = ret;
        for(i = 0; i < 26; i++) {
            int j;
            for(j = 0; j < alphabet[i]; j++) {
                *p++ = 'a' + i;
            }
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    // char tmp[128] = "zdxscdwadcez";

    char tmp[] = "xitavoyjqiupzadbdyymyvuteolyeerecnuptghlzsynozeuuvteryojyokpufanyrqqmtgxhyycltlnusyeyyqygwupcaagtkuq";
    char d[] = "aagtkuqxitavoyjqiupzadbdyymyvuteolyeerecnuptghlzsynozeuuvteryojyokpufanyrqqmtgxhyycltlnusyeyyqygwupc";
    int len = 1;

    char *ret;
    ret = orderlyQueue(tmp, 1);
    printf("ret is %s\ndes is %s\n", ret, d);

    int re = strcmp(d, ret);
    printf("re is %d\n", re);
    free(ret);

    // ret = orderlyQueue(tmp, );
    // printf("ret is %s\n", ret);
    // free(ret);


    // printf("ss %.02x %c tmp %s\n", *ss, *ss, tmp);

    return 0;
}