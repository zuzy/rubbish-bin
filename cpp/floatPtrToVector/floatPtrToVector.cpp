#include <string>
#include <iostream>
#include <vector>

using namespace std;

const int SOME_SIZE = 5;

int main(int argc, char *argv[])
{
    float* pf = new float[SOME_SIZE];
    for(int i = 0; i < SOME_SIZE; ++i ) {
        pf[i] = i * 2;
    }
    // Initialize the newly allocated memory
    std::vector<float> vf{pf, pf + SOME_SIZE};

    for (const auto& _v : vf) {
        cout << _v << endl;
    }


    // for(auto _v = vf.begin(); _v != vf.end(); ++_v) {
    //     cout << _v->first << " " << _v->second << endl;
    // }
    return 0;
}