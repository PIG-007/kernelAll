#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
 
//设备驱动常用变量
//static char *buffer_var = NULL;
static char *buffer_var = NULL;
static struct class *devClass; // Global variable for the device class
static struct cdev cdev;
static dev_t stack_dev_no;
 
 
struct Note
{
    int idx;
    int len;
    char* data;
};
 
 
//static char* notelist[1000];
static char* notelist[1000];
static struct Note* noteChunk;
static int count = 0;
 
 
 
static ssize_t stack_read(struct file *filp, char __user *buf,
size_t len, loff_t *f_pos);
 
static ssize_t stack_write(struct file *filp, const char __user *buf,
size_t len, loff_t *f_pos);
 
static long stack_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
 
static int stack_open(struct inode *i, struct file *f);
 
static int stack_close(struct inode *i, struct file *f);
 
 
 
static struct file_operations stack_fops =
        {
                .owner = THIS_MODULE,
                .open = stack_open,
                .release = stack_close,
                .write = stack_write,
                .read = stack_read,
                .unlocked_ioctl = stack_ioctl
        };
 
// 设备驱动模块加载函数
static int __init stack_init(void)
{
    printk(KERN_INFO "\033[1;31;40m[Gift:]notelist:0x%016llx\033[0m",&notelist);
    printk(KERN_INFO "[i] Module stack registered");
    if (alloc_chrdev_region(&stack_dev_no, 0, 1, "stack") < 0)
    {
        return -1;
    }
    if ((devClass = class_create(THIS_MODULE, "chardrv")) == NULL)
    {
        unregister_chrdev_region(stack_dev_no, 1);
        return -1;
    }
    if (device_create(devClass, NULL, stack_dev_no, NULL, "stack") == NULL)
    {
        printk(KERN_INFO "[i] Module stack error");
        class_destroy(devClass);
        unregister_chrdev_region(stack_dev_no, 1);
        return -1;
    }
    cdev_init(&cdev, &stack_fops);
    if (cdev_add(&cdev, stack_dev_no, 1) == -1)
    {
        device_destroy(devClass, stack_dev_no);
        class_destroy(devClass);
        unregister_chrdev_region(stack_dev_no, 1);
        return -1;
    }
 
    printk(KERN_INFO "[i] <Major, Minor>: <%d, %d>\n", MAJOR(stack_dev_no), MINOR(stack_dev_no));
    return 0;
}
 
// 设备驱动模块卸载函数
static void __exit stack_exit(void)
{
    // 释放占用的设备号
    unregister_chrdev_region(stack_dev_no, 1);
    cdev_del(&cdev);
}
 
 
// 读设备
ssize_t stack_read(struct file *filp,  char __user *buf,
size_t len, loff_t *f_pos)
{
    printk(KERN_INFO "Stack_read function" );
    copy_to_user(buf,buffer_var,len);
    return 1;
}
 
// 写设备
ssize_t stack_write(struct file *filp, const char __user *buf,
size_t len, loff_t *f_pos)  //buffer overflow
{
    printk(KERN_INFO "Stack_write function" );
    copy_from_user(buffer_var, buf, len);
    printk("[i] Module stack write: %s\n",buffer_var);
    return len;
}
 
 
 
// ioctl函数命令控制
long stack_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    char* chunk = NULL;
    int retval = 0;
    printk(KERN_INFO "Ioctl Get!\n");
    printk("notelist_addr:0x%llx\n",&notelist[0]);
    switch (cmd) {
 
        case 1://add
            //noteChunk = (char *)kmalloc(sizeof(struct Note),GFP_KERNEL);
            //copy_from_user(noteChunk, arg, sizeof(struct Note));
 
            printk("Kernel Add function!---001\n");
            noteChunk = (struct Note*)arg;
            chunk = (char *)kmalloc(noteChunk->len,GFP_KERNEL);
            printk("chunk_addr:0x%llx\n",chunk);
            if (!chunk)
            {
                printk("Alloca Error\n");
                return 0;
            }
            memcpy(chunk, noteChunk->data,noteChunk->len);
            notelist[count] = chunk;
            chunk = NULL;
            count ++;
            printk("Add Success!\n");
            break;
 
        case 888: //free without clean point and data
            printk("Kernel Free function!---888\n");
            noteChunk = (struct Note*)arg;
            printk("notelist:0x%llx\n",notelist[noteChunk->idx]);
            if (notelist[noteChunk->idx])
            {
                kfree(notelist[noteChunk->idx]);
                //notelist[noteChunk->idx] = NULL;
                printk("Free Success!\n");
            }
            else
            {
                printk("You can't free it!There is no chunk!\n");
            }
            break;
 
        case 3://edit   //UAF and overflow
            printk("Kernel Edit function!---003\n");
            noteChunk = (struct Note*)arg;
            if (notelist[noteChunk->idx])
            {
                memcpy(notelist[noteChunk->idx], noteChunk->data,noteChunk->len);
                printk("Edit Success!\n");
            }
            else
            {
                printk("You can't edit it!There is no chunk!\n");
            }
            break;
 
        case 4://read   //over read
            printk("Kernel Read function!---004\n");
            noteChunk = (struct Note*)arg;
            if(notelist[noteChunk->idx]){
                copy_to_user(noteChunk->data,notelist[noteChunk->idx],noteChunk->len);
                printk("Read Success!\n");
            }
            break;
 
        case 111: //Test add chunk
            printk("Test add chunk!---111\n");
            printk(KERN_INFO "No buffer_var!Malloc now!" );
            buffer_var=(char*)kmalloc(0xa8,GFP_KERNEL);
            printk("buffer_var:0x%llx\n",buffer_var);
            break;
 
        default:
            retval = -1;
            break;
    }   
 
    return retval;
}
 
 
static int stack_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "[i] Module stack: open()\n");
    return 0;
}
 
static int stack_close(struct inode *i, struct file *f)
{
    kfree(buffer_var);
    //buffer_var = NULL;
    printk(KERN_INFO "[i] Module stack: close()\n");
    return 0;
}
 
module_init(stack_init);
module_exit(stack_exit);
 
MODULE_LICENSE("GPL");
// MODULE_AUTHOR("blackndoor");
// MODULE_DESCRIPTION("Module vuln overflow");
