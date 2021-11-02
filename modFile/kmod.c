#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

//设备驱动常用变量
//static char *buffer_var = NULL;
static char *buffer_var = NULL;
static struct class *devClass; // Global variable for the device class
static struct cdev cdev;
static dev_t condition_dev_no;


struct flagStruct
{
    int len;
    char* flagUser;
};



static struct flagStruct* flagObj;
static char* flag = "flag{You_get_it}";

static long condition_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

static int condition_open(struct inode *i, struct file *f);

static int condition_close(struct inode *i, struct file *f);

static bool chk_range_not_ok(ssize_t v1,ssize_t v2);


static struct file_operations condition_fops =
        {
                .owner = THIS_MODULE,
                .open = condition_open,
                .release = condition_close,
                .unlocked_ioctl = condition_ioctl
        };

// 设备驱动模块加载函数
static int __init condition_init(void)
{
    printk(KERN_INFO "[i] Module condition registered");
    if (alloc_chrdev_region(&condition_dev_no, 0, 1, "condition") < 0)
    {
        return -1;
    }
    if ((devClass = class_create(THIS_MODULE, "chardrv")) == NULL)
    {
        unregister_chrdev_region(condition_dev_no, 1);
        return -1;
    }
    if (device_create(devClass, NULL, condition_dev_no, NULL, "condition") == NULL)
    {
        printk(KERN_INFO "[i] Module condition error");
        class_destroy(devClass);
        unregister_chrdev_region(condition_dev_no, 1);
        return -1;
    }
    cdev_init(&cdev, &condition_fops);
    if (cdev_add(&cdev, condition_dev_no, 1) == -1)
    {
        device_destroy(devClass, condition_dev_no);
        class_destroy(devClass);
        unregister_chrdev_region(condition_dev_no, 1);
        return -1;
    }
    printk(KERN_INFO "[i] <Major, Minor>: <%d, %d>\n", MAJOR(condition_dev_no), MINOR(condition_dev_no));
    return 0;
}

// 设备驱动模块卸载函数
static void __exit condition_exit(void)
{
    // 释放占用的设备号
    unregister_chrdev_region(condition_dev_no, 1);
    cdev_del(&cdev);
}




// ioctl函数命令控制
long condition_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int retval = 0;
    printk(KERN_INFO "Ioctl Get!\n");
    switch (cmd) {
        case 111: //doubleCon  //get flag_addr
            printk("Your flag is at %llx! But I don't think you know it's content\n",flag);
            break;

        case 222: //doubleCon  //print flag
            flagObj =  (struct flagStruct*)arg;
            ssize_t userFlagAddr = flagObj->flagUser;
            ssize_t userFlagObjAddr = (ssize_t)flagObj;

            if(chk_range_not_ok(userFlagAddr,flagObj->len)
                &&chk_range_not_ok(userFlagObjAddr,0)
                &&(flagObj->len == strlen(flag)))
            {
                if(!strncmp(flagObj->flagUser,flag,strlen(flag)))
                    printk("Looks like the flag is not a secret anymore. So here is it %s\n", flag);
                else
                    printk("Wrong!");
                break;
            }
            else
            {
                printk("Wrong!\n");
                break;
            }
        default:
            retval = -1;
            break;
    }    

    return retval;
}


static bool chk_range_not_ok(ssize_t v1,ssize_t v2)
{
    if((v1 + v2) <= 0x7ffffffff000)
        return true;
    else
        return false;
}

static int condition_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "[i] Module condition: open()\n");
    return 0;
}

static int condition_close(struct inode *i, struct file *f)
{
    kfree(buffer_var);
    //buffer_var = NULL;
    printk(KERN_INFO "[i] Module condition: close()\n");
    return 0;
}

module_init(condition_init);
module_exit(condition_exit);

MODULE_LICENSE("GPL");
// MODULE_AUTHOR("blackndoor");
// MODULE_DESCRIPTION("Module vuln overflow");