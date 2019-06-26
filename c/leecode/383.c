#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool canConstruct(char * ransomNote, char * magazine){
    size_t ln = strlen(ransomNote);
    size_t lm = strlen(magazine);
    if(ln > lm) {
        return false;
    }
    int i = 0;
    for (; i < ln; i++) {
        int j = 0;
        for(; j < lm; j++) {
            if(!magazine[j]) continue;
            if(magazine[j] == ransomNote[i]) {
                magazine[j] = 0;
                goto Next;
            }
        }
        return false;
        Next:
            NULL;
    }
    return true;
}

int main(int argc, char *argv)
{
    return 0;
}