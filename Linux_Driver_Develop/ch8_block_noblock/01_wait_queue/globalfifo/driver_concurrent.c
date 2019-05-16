#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/sched.h>

#include <linux/wait.h>

/**
 * ! signal pending head
 * ! 没有pending，cpu会持续占用很高，而且无法相应kill
 */
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#include <linux/sched/signal.h>
#endif

#define GLOBALFIFO_SIZE      0X1000
#define MEM_CLEAR           0X1
#define GLOBALFIFO_MAJOR     230


#ifdef MULTI_DEV
#define DEVICE_NUM          10
#endif

static int globalfifo_major = GLOBALFIFO_MAJOR;
module_param(globalfifo_major, int, S_IRUGO);

struct globalfifo_dev {
    struct cdev cdev;
    unsigned int current_len;
    unsigned char mem[GLOBALFIFO_SIZE];
    struct mutex mutex;
    wait_queue_head_t r_wait;
    wait_queue_head_t w_wait;
};

struct globalfifo_dev *globalfifo_devp;

static int globalfifo_open(struct inode *inode, struct file *filp)
{
    filp->private_data = globalfifo_devp;
    return 0;
}

static int globalfifo_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static long globalfifo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) 
{
    struct globalfifo_dev *dev = filp->private_data;

    switch(cmd) {
        case MEM_CLEAR: {
            mutex_lock(&dev->mutex);
            memset(dev->mem, 0, GLOBALFIFO_SIZE);
            mutex_unlock(&dev->mutex);
            printk(KERN_INFO "globalfifo is set to zero\n");
            break;
        }
        default:
            return -EINVAL;
    }
    return 0;
}

static ssize_t globalfifo_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct globalfifo_dev *dev = filp->private_data;
    DECLARE_WAITQUEUE(wait, current);

    mutex_lock(&dev->mutex);
    add_wait_queue(&dev->r_wait, &wait);

    while(dev->current_len == 0) {
        if(filp->f_flags & O_NONBLOCK) {
            ret = -EAGAIN;
            goto out;
        }
        __set_current_state(TASK_INTERRUPTIBLE);
        mutex_unlock(&dev->mutex);

        schedule();

        if(signal_pending(current)) {
            ret = -ERESTARTSYS;
            goto out2;
        }

        mutex_lock(&dev->mutex);
    }

    if(count > dev->current_len) 
        count = dev->current_len;

    if(copy_to_user(buf, dev->mem + p, count)) {
        ret = -EFAULT;
        goto out;
    } else {
        memcpy(dev->mem, dev->mem + count, dev->current_len - count);
        dev->current_len -= count;
        // *ppos += count;

        wake_up_interruptible(&dev->w_wait);

        ret = count;

        printk(KERN_INFO "read %u byte(s) from %lu\n", count, p);
    }
    out:
    mutex_unlock(&dev->mutex);
    out2:
    remove_wait_queue(&dev->r_wait, &wait);
    set_current_state(TASK_RUNNING);

    return ret;
}

static ssize_t globalfifo_write(struct file *filp, const char __user *buf, size_t size , loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct globalfifo_dev *dev = filp->private_data;

    DECLARE_WAITQUEUE(wait, current);

    mutex_lock(&dev->mutex);
    add_wait_queue(&dev->w_wait, &wait);

    while(dev->current_len == GLOBALFIFO_SIZE) {
        if(filp->f_flags & O_NONBLOCK) {
            ret = -EAGAIN;
            goto out;
        }
        __set_current_state(TASK_INTERRUPTIBLE);
        
        mutex_lock(&dev->mutex);

        schedule();

        if(signal_pending(current)) {
            ret = -ERESTARTSYS;
            goto out2;
        }

        mutex_lock(&dev->mutex);
    }

    if(count > GLOBALFIFO_SIZE - dev->current_len) {
        count = GLOBALFIFO_SIZE - dev->current_len;
    }

    if(copy_from_user(dev->mem + p, buf, count)) {
        ret = -EFAULT;
        goto out;
    } else {
        dev->current_len += count;
        ret = count;

        wake_up_interruptible(&dev->r_wait);

        printk(KERN_INFO "written %u byte(s) from %lu\n", count, p);
    }
    
    out:
    mutex_unlock(&dev->mutex);
    out2:
    remove_wait_queue(&dev->w_wait, &wait);
    set_current_state(TASK_RUNNING);

    return ret;
}

static loff_t globalfifo_llseek(struct file *filp, loff_t offset, int orig)
{
    loff_t ret = 0;
    switch(orig) {
        case 0: {
            if(offset < 0) {
                ret = -EINVAL;
                break;
            }
            if((unsigned int) offset > GLOBALFIFO_SIZE) {
                ret = -EINVAL;
                break;
            }
            filp->f_pos = (unsigned int) offset;
            ret = filp->f_pos;
            break;
        }
        case 1: {
            if((filp->f_pos + offset) > GLOBALFIFO_SIZE) {
                ret = -EINVAL;
                break;
            }
            if((filp->f_pos + offset) < 0) {
                ret = -EINVAL;
                break;
            }
            filp->f_pos += offset;
            ret = filp->f_pos;
            break;
        }
        default: {
            ret = -EINVAL;
            break;
        }
    }
    return ret;
}


static const struct file_operations globalfifo_fops = {
    .owner = THIS_MODULE,
    .llseek = globalfifo_llseek,
    .read = globalfifo_read,
    .write = globalfifo_write,
    .unlocked_ioctl = globalfifo_ioctl,
    .open = globalfifo_open,
    .release = globalfifo_release,
};

static void globalfifo_setup_cdev(struct globalfifo_dev *dev, int index)
{
    int err, devno = MKDEV(globalfifo_major, index);

    cdev_init(&dev->cdev, &globalfifo_fops);
    dev->cdev.owner = THIS_MODULE;
    err = cdev_add(&dev->cdev, devno, 1);
    if(err) {
        printk(KERN_NOTICE "Error %d adding globalfifo %d\n", err, index);
    }
}

static int __init globalfifo_init(void)
{
    int ret;
    int i;
    dev_t devno = MKDEV(globalfifo_major, 0);
    printk("--------%s--------\n", __func__);

    if(globalfifo_major)
        ret = register_chrdev_region(devno, 1, "globalfifo");
    else {
        ret = alloc_chrdev_region(&devno, 0, 1, "globalfifo");
        globalfifo_major = MAJOR(devno);
    }
    if(ret < 0) 
        return ret;

    globalfifo_devp = kzalloc(sizeof(struct globalfifo_dev), GFP_KERNEL);
    if(!globalfifo_devp) {
        ret = -ENOMEM;
        goto fail_malloc;
    }

    // * mutex init
    mutex_init(&globalfifo_devp->mutex);
    globalfifo_setup_cdev(globalfifo_devp, 0);

    // * waitqueue init
    init_waitqueue_head(&globalfifo_devp->r_wait);
    init_waitqueue_head(&globalfifo_devp->w_wait);
    return 0;
    fail_malloc:
    unregister_chrdev_region(devno, 1);
    return ret;
}
module_init(globalfifo_init);

static void __exit globalfifo_exit(void)
{
    int i;
    printk("--------%s--------\n", __func__);
    cdev_del(&globalfifo_devp->cdev);
    kfree(globalfifo_devp);
    unregister_chrdev_region(MKDEV(globalfifo_major, 0), 1);
}
module_exit(globalfifo_exit);

MODULE_AUTHOR("ZIZY");
MODULE_LICENSE("GPL v2");

/**
 * !! mknod /dev/globalfifo c 230 0
 * 
 */
