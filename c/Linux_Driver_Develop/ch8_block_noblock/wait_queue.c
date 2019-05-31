
static ssize_t xxx_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
    /**
     * @brief Construct a new declare waitqueue object
     * ! 定义等待队列元素, 添加元素到等待队列
     */
    DECLARE_WAITQUEUE(wait, current);
    add_wait_queue(&xxx_wait, wait);

    do {
        avail = device_writable(...);
        if(avail < 0) {
            /**
             * ! 非阻塞
             */
            if(file->f_flags & O_NONBLOCK) {
                ret = -EAGAIN;
                goto out;
            }

            /**
             * ! 改变进程状态, 调度其他进程执行
             */
            __set_current_state(TASK_INTERRUPTIBLE);
            schedule();

            /**
             * ! 如果是被信号唤醒
             */
            if(singal_pending(current)) {
                ret = -ERESTARTSYS;
                goto out;
            }
        }
    } while(avial < 0);

    device_write(...);
    out:
    /**
     * ! 将元素移出 xxx_write 指引的队列
     * ! 设置进程状态为 TASK_running
     */
    remove_wait_queue(&xxx_wait, &wait);
    set_current_state(TASK_RUNNING);
    return ret;
}