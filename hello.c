#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>  
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/anon_inodes.h>

#include <linux/wait.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");     
MODULE_AUTHOR("Devidas"); 
MODULE_DESCRIPTION("SysFS get set"); 
MODULE_VERSION("0.1"); 

struct pid_struct_list {
	struct task_struct * pid_struct;
	struct list_head list;
}listMember;


int size_of_message = 0;
char message[1024]= {'\0'};
int current_DATA = 0;
//DECLARE_WAIT_QUEUE_HEAD(wait_queue_name);
//wait_queue_head_t my_queue;
//init_waitqueue_head(&my_queue);

static ssize_t ftest_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   if(list_empty(&listMember.list))
	size_of_message=0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
     if(size_of_message!=0) {
   error_count = copy_to_user(buffer, message, size_of_message);
  // if (error_count==0){            // if true then have success
      printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
      printk(KERN_INFO "READIF: %d error_count=%d pid=%d\n",size_of_message,error_count,current->pid);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
	struct pid_struct_list *currMember = kmalloc(sizeof(struct pid_struct_list),GFP_KERNEL);
	currMember->pid_struct = current;
	INIT_LIST_HEAD(&currMember->list);
	list_add_tail(&(currMember->list), &(listMember.list));
	printk(KERN_INFO "READ: %d pid=%d\n",size_of_message,current->pid);
//	currMember->pid_struct->state = TASK_UNINTERRUPTIBLE;
	printk(KERN_INFO "READ: Before Schedule %d\n",size_of_message);
	set_current_state(TASK_INTERRUPTIBLE);
	schedule();
	printk(KERN_INFO "READ: Woken Up %d\n",size_of_message);
   	error_count = copy_to_user(buffer, message, size_of_message);
	printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
	return (size_of_message);
   }

}

static ssize_t ftest_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	struct pid_struct_list *currMember,*nextMember;
   sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
   size_of_message = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "EBBChar: Received %zu characters from the user\n", len);
   if(!list_empty(&listMember.list)){
	list_for_each_entry_safe(currMember,nextMember, &listMember.list, list) {
		printk(KERN_INFO "Write: waking %d\n",currMember->pid_struct->pid);
		wake_up_process(currMember->pid_struct);
//		currMember->pid_struct->state = TASK_RUNNING;
//		schedule();
		list_del(&currMember->list);
		kfree(currMember);
		}
	}
//	size_of_message=0;
 
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
    INIT_LIST_HEAD(&listMember.list);
    printk(KERN_INFO "Hello!!!\n");
    return 0;
}

static void __exit hello_cleanup(void)
{
  printk(KERN_INFO "Bye.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
