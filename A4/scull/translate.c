#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/cdev.h>

#include <linux/moduleparam.h>

#include <linux/device.h>
#include <linux/kernel.h>
#include <llinux/uaccess.h>

#define DEVICE_NAME "trans"
#define CLASS_NAME "transclass"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Janaina, Vadim");

int rot = SHIFT;
module_param(rot, int, 0644);

static int trans_majorNr;
static char msg[40] = {0};
static size_of_msg;
static int count_opens = 0;
static struct class * transcharClass = NULL;
static struct device * transcharDevice = NULL;

int
trans_open(struct inode * inode, struct file * filp) {
    PDEBUG("open device\n");

    struct trans_data * trans_data;

    trans_data = container_of(inode->i_cdev, struct trans_data, cdev);

    filp->private_data = trans_data;

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

int
trans_release(struct inode * inode, struct file * filp){

    return 0;
}

ssize_t
trans_read(struct file * filp, char __user * buf, size_t count, loff_t * f_pos){
    PDEBUG("reading from device\n");

    struct trans_data * trans_data;

    trans_data = (struct trans_data *) filp->private_data;

    mutex_lock(&mutex);
    if (copy_to_user()) {

    }

    return 0;
}

ssize_t
trans_write(struct file *filp, const char __user * buf, size_t count, loff_t * f_pos){

    return count;
}

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = trans_open,
    .read = trans_read,
    .write = trans_write,
    .release = trans_release,
};


int
__init trans_init(void){
    printk(KERN_INFO "trans: on function %s \n", __FUNCTION__);
    int err, i;
    dev_t dev = 0;

    // Try to dynamically allocate a major number for the device
    err = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    trans_major = MAJOR(dev);
    if (err < 0) {
        PDEBUG("Fail to allocate trans_major\n");
        return err;
   }

    PDEBUG("device name-> %s, major number-> %d\n", DEVICE_NAME, trans_major);

    /*PDEBUG("Initializing the Translate LKM\n");
    int err, i;
    dev_t dev = 0;

    // Try to dynamically allocate a major number for the device
    err = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (err < 0) {
        PDEBUG("Fail to allocate trans_major\n");
        return err;
   }

    trans_major = MAJOR(dev);
    PDEBUG("device name-> %s, major number-> %d\n", DEVICE_NAME, trans_major);

    cdev_init(&devices[i].trans_cdev, &fops);
    cdev_add(&devices[i].trans_cdev, dev);

    mutex_init(&mutex);
    return 0;
*/
}

void
__exit trans_exit (void) {
    cdev_del(trans_cdev);
    unregister_chrdev_region(device_number, 1);
    mutex_unlock(&mutex);
    mutex_destroy(&mutex);

    PDEBUG("module exit\n");
}

module_init(trans_init);
module_exit(trans_exit);
