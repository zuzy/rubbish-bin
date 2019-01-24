#include <stdbool.h>

// Forward declaration of isBadVersion API.
bool isBadVersion(int version);

int firstBadVersion(int n) {
    int s = 1, e = n, mid;
    if(n == 1) {
        return 1;
    }
    do{
        mid = (s + e) >> 1;
        if(isBadVersion(mid)){
            if(!isBadVersion(mid - 1))
                return mid;
            else 
                e = mid;
        } else {
            if(isBadVersion(mid+ 1))
                return mid + 1;
            else 
                s = mid;
        }
    }while(mid > 1 && mid < n);
    return mid;
}