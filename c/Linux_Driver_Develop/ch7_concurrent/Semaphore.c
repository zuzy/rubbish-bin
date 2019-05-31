
struct semaphore sem;

// 初始化信号量为val
void sema_init(struct semaphore *sem, int val);

// 用于获得信号量, 导致睡眠, 不能在中断上下文中使用.
void down(struct semaphore *sem);
int down_interruptible(struct semaphore *sem);
int down_trylock(struct semaphore *sem);

if(down_interruptible(&sem)) {
    return -ERESTARTSYS;
}

// 释放信号量
void up(struct semaphore *sem);