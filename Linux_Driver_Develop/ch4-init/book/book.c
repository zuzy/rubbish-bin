#include <linux/init.h>
#include <linux/module.h>

static int hello_data __initdata = 1;

static int __init hello_init(void)
{
    pr_info("hello world %d\n", hello_data);
    return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
    printk(KERN_INFO, "hello exit\n");
    pr_info("hello world exit\n");
}
module_exit(hello_exit);

MODULE_AUTHOR("ZIZY");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A simple hello world module");
MODULE_ALIAS("A simple module");