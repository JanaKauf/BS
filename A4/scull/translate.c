#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/fcntl. h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <stdbool.h>
#include "translate.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Janaina, Vadim");

static int major_number;
static dev_t device_number;
static struct cdev* trans_cdev;
static DEFINE_MUTEX(mutex);
bool read = false;
bool write = false;
int error;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static int dev_read(struct file *, char *, size_t, loff_t *);
static int dev_write(struct file *, const char *, size_t, loff_t *);


static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init translate_init(void){
    printk(KERN_INFO "trans : Initializing the Translate LKM\n");

    // Try to dynamically allocate a major number for the device
    error = alloc_chrdev_region(&device_number, 0, 1, DEVICE_NAME);
    if (error < 0) {
        PDEBUG("trans : Fail to allocate major_number");
        return error;
   }

    major_number = MAJOR(device_number);
    PDEBUG("trans : device name-> %s, major number-> %d\n", DEVICE_NAME, major_number);

    trans_cdev = cdev_alloc();
    trans_cdev->ops = &fops;
    trans_cdev->owner = THIS_MODULE;

    error = cdev_add(trans_cdev, device_number);
    if (error < 0) {
        PDEBUG("trans : could not add to kernel\n");
        return error;
    }

    mutex_init(&mutex);
    return 0;

}

static void __exit translate_exit (void) {
    cdev_del(trans_cdev);
    unregister_chrdev_region(device_number, 1);
    mutex_unlock(&mutex);
    mutex_destroy(&mutex);

    PDEBUG("trans : module exit\n");
}

static int dev_open(struct inode * inode, struct file *filp) {
    PDEBUG("trans : open device");
    mutex_lock(&mutex);
    if (filp->f_mode & FMODE_READ) {
        if(read) {
            mutex_unlock(&mutex);
            return -EBUSY;
        }
        read = true;
    }

    if (filp->f_mode & FMODE_WRITE) {
        if(write) {
            mutex_unlock(&mutex);
            return -EBUSY;
        }
        write = true;
    }

    mutex_unlock(&mutex);

    return 0;

}

static int dev_read(struct file *, char *, size_t, loff_t *) {

}

static int dev_write(struct file *, const char *, size_t, loff_t *) {

}

static int dev_release(struct inode *, struct file *) {

}


module_init(translate_init);
module_exit(translate_exit);
