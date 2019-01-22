#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>  




static int __init hello_init(void)
{
    printk(KERN_INFO "Hello!!!\n");
    return 0;
}

static void __exit hello_cleanup(void)
{
  printk(KERN_INFO "Bye.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
