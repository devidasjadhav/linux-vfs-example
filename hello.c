#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>  
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/anon_inodes.h>

MODULE_LICENSE("GPL");     
MODULE_AUTHOR("Devidas"); 
MODULE_DESCRIPTION("SysFS get set"); 
MODULE_VERSION("0.1"); 

int size_of_message = 1024;
char message[1024];

static ssize_t ftest_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);
 
   if (error_count==0){            // if true then have success
      printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

static ssize_t ftest_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
   size_of_message = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "EBBChar: Received %zu characters from the user\n", len);
   return len;
}
 
static int ftest_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "EBBChar: Device successfully closed\n");
   return 0;
}
static struct file_operations ftest_fops =
{
   .read = ftest_read,
   .write = ftest_write,
   .release = ftest_release,
};


int sysfs_test_set(const char *val, const struct kernel_param *kp){
	printk(KERN_INFO "SET:%s\n",val);
	return 0;
}

char dataBuff[1024];
int sysfs_test_get(char *buffer, const struct kernel_param *kp){
//	strcpy(buffer,"Hello");
	int ret = 0;	
	int fdNo = anon_inode_getfd("test",&ftest_fops, dataBuff ,O_RDWR);
	printk(KERN_INFO "GET:%d\n",fdNo);
	//ret = put_user(fdNo,(int *)buffer);	
	ret = sprintf(buffer ,"%d",fdNo);
	printk(KERN_INFO "GET:%d\n",ret);

	return ret;
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
