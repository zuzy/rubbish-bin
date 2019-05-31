#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *_build_(char *des) 
{
    char *a[] = {".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--.."};
    char *ret = calloc(strlen(des) + 1, 4);
    char *p = des;
    while(*p) {
        strcat(ret, a[*p++ - 'a']);
        
    }
    return ret;
}

int uniqueMorseRepresentations(char** words, int wordsSize) {
    if(wordsSize <= 1) return wordsSize;
    
    char **m_words = calloc(wordsSize, sizeof(char*));
    int ret = 1;

    char **m_ptr = m_words;
    *m_ptr = _build_(words[0]);
    printf("1 -> %s\n", *m_ptr);
    int i = 1;
    for(; i < wordsSize; i++) {
        m_ptr = m_words;
        char *tmp = _build_(words[i]);
        printf("%d -> %s\n", i, tmp);
        int find = 0;
        while(*m_ptr) {
            if(strcmp(*m_ptr, tmp)) {
                ++m_ptr;
            } else {
                printf("find \n");
                find = 1;
                break;
            }
        }
        if(find) {
            free(tmp);
        } else {
            ++ret;
            *m_ptr = tmp;
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    char *input[] = {"gin", "zen", "gig", "msg", "wer"};
    int ret = uniqueMorseRepresentations(input, 5);
    printf("ret %d\n", ret);
    return 0;
}