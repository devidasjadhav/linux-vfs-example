#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>  

MODULE_LICENSE("GPLV2");     
MODULE_AUTHOR("Devidas"); 
MODULE_DESCRIPTION("SysFS get set"); 
MODULE_VERSION("0.1"); 


int sysfs_test_set(const char *val, const struct kernel_param *kp){
	printk(KERN_INFO "SET:%s\n",val);
	return 0;
}
int sysfs_test_get(char *buffer, const struct kernel_param *kp){
	strcpy(buffer,"Hello");
	return 5;
}


const struct kernel_param_ops sysfs_test_ops = 
{
    .set = &sysfs_test_set, // Use our setter ...
    .get = &sysfs_test_get
};
//unsigned short param = 0xff;
char param[20]= {0};
module_param_cb(test,&sysfs_test_ops,&param,S_IRUGO | S_IWUSR);


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
