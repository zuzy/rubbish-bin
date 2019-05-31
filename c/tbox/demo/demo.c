#include "tbox/tbox.h" 

int main(int argc, char** argv)
{
    // 初始化整个tbox库环境，这个是必须的，但是只需要在程序最开始执行一下
    if (!tb_init(tb_null, tb_null)) return 0;

    // 打印输出，需要自己加换行符
    tb_printf("hello world!\n");

    // 只在debug下打印输出一行
    tb_trace_d("hello world");

    // 在debug/release下打印输出一行
    tb_trace_i("hello world");

    // debug下进行断言检测
    tb_assert(1 == 1);
    tb_assert_abort(1 != 2);

    // 空等待
    getchar();

    // 退出整个tbox环境，这步会释放tbox占用的所有资源，如果是debug版本，还会在终端输出内存泄露检测信息等等。
    tb_exit();
    return 0;
}