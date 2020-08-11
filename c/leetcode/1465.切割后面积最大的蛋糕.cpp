#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    int maxArea(int h, int w, vector<int>& horizontalCuts, vector<int>& verticalCuts) 
    {
        horizontalCuts.push_back(0);
        horizontalCuts.push_back(h);
        verticalCuts.push_back(0);
        verticalCuts.push_back(w);
        long long dis;
        sort(horizontalCuts.begin(), horizontalCuts.end());
        long long hmax = 0;
        for(auto hi  = horizontalCuts.begin() + 1; hi != horizontalCuts.end(); ++hi) {
            dis = *hi - *(hi - 1);
            hmax = dis > hmax ? dis : hmax;
        }
        sort(verticalCuts.begin(), verticalCuts.end());
        long long smax = 0;
        for(auto si  = verticalCuts.begin() + 1; si != verticalCuts.end(); ++si) {
            dis = *si - *(si - 1);
            smax = dis > smax ? dis : smax;
        }
        long long m = 10e9 + 7;
        long long ret = hmax * smax;
        return ret % m;
    }
};
