#include <stdio.h>
#include "tbox/tbox.h"


static tb_int_t assert_demo()
{
    // 打印输出，需要自己加换行符
    tb_printf("hello world!\n");

    // 只在debug下打印输出一行
    tb_trace_d("hello world debug");

    // 在debug/release下打印输出一行
    tb_trace_i("hello world release");

    // debug下进行断言检测
    tb_assert(1 == 1);
    

    // 空等待
    getchar();

    return 0;
}


static tb_int_t tb_demo_thread_func(tb_cpointer_t priv)
{
    // self
    tb_size_t self = tb_thread_self();
    
    // trace
    tb_trace_i("thread[%lx: %s]: init", self, priv);

    // exit 
    // tb_thread_return(-1);
    tb_long_t t = tb_mclock();
    // tb_thread_suspend();

    while(1) {
        tb_trace_i("thread last %ld ms", tb_mclock() - t);
        tb_sleep(1);
    }

    // trace
    tb_trace_i("thread[%lx: %s]: exit", self, priv);

    // getchar();

    // ok
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_thread_main(tb_int_t argc, tb_char_t** argv)
{
    // init thread
    tb_thread_ref_t thread = tb_thread_init(tb_null, tb_demo_thread_func, "hello", 0);
    // if (thread)
    // {
    //     // wait thread
    //     tb_int_t retval = 0;
    //     if (tb_thread_wait(thread, -1, &retval) > 0)
    //     {
    //         // trace
    //         tb_trace_i("wait: ok, retval: %d", retval);
    //     }
    
    //     // exit thread
    //     tb_thread_exit(thread);
    // }
    tb_trace_i("!!!");
    tb_sleep(1);
    tb_trace_i("suspend");
    tb_thread_suspend(thread);

    tb_sleep(3);
    tb_trace_i("resume");
    tb_thread_resume(thread);

    // tb_thread_exit(thread);

    tb_int_t retval = 0;
    tb_thread_wait(thread, 6000, &retval);
    tb_trace_i("wait timeout %d", retval);
    // wait
    // getchar();
    return 0;
}


int main(int argc, char** argv)
{
    printf("hello world!\n");
    tb_init(tb_null, tb_null);
    assert_demo();
    tb_demo_platform_thread_main(argc, argv);
    // tb_trace_i("assert");
    // // tb_assert_abort(1 != 2);
    // tb_assert_leave(1 != 2);
    tb_exit();
    return 0;
}
