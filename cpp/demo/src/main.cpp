#include <iostream>
#include <string>
#include <cstring>

using namespace std;

#if 1
static bool end_with(string body, const char *suffix)
{
    if(!suffix) 
        return true;
    size_t len = strlen(suffix);
    if(len <= 0) 
        return true;
    return body.find(suffix, body.size() - len) != string::npos;
}

int main(int argc, char **argv)
{
    // string s("sss.h");
    bool ret = !end_with("sss.h", ".h");
    cout << "ret " << ret << endl;

    return 0;
}

#else

void print(std::string::size_type n, std::string const &s)
{
    if (n == std::string::npos) {
        std::cout << "not found\n";
    } else {
        std::cout << "found: " << s.substr(n) << '\n';
    }
}
 
int main()
{
    std::string::size_type n;
    std::string const s = "This is a string";
 
    // 从 string 开始搜索
    n = s.find("is");
    print(n, s);
 
    // 从位置 5 开始搜索
    n = s.find("is", 5);
    print(n, s);
 
    // 寻找单个字符
    n = s.find('a');
    print(n, s);
 
    // 寻找单个字符
    n = s.find('q');
    print(n, s);
}
#endif
