/**
 * @brief normal spin_lock
 * 
 */
spinlock_t xxx_lock;
int xxx_count = 0;
static int xxx_open(struct inode *inode, sturct file *filp)
{
    ...
    spinlock(&xxx_lock);
    if(xxx_count) {
        spin_unlock(&xxx_lock);
        return -EBUSY;
    }
    xxx_count++;
    spin_unlock(&xxx_lock);
    return 0;
}

static int xxx_release(struct inode *inode, sturct file *filp)
{
    ...
    spinlock(&xxx_lock);
    xxx_count--;
    spin_unlock(&xxx_lock);

    return 0;
}


/**
 * @brief rwlock, read-write spin lock
 * 
 */

rwlock_t my_rwlock;
rwlock_init(&my_rwlock);
