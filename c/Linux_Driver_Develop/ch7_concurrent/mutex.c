struct mutex my_mutex;
mutex_init(&my_mutex);

void mutex_lock(struct mutex *lock);
int mutex_lock_interruptible(struct mutex *lock);
int mutex_trylock(struct mutex *lock);

void mutex_unlock(struct mutex *lock);

/**
 * @brief 互斥锁 是进程级的, 用于多个进程之间对资源的互斥, 虽然也是在内核中, 但是该内核执行路径是以进程的身份, 代表进程来争夺资源.
 * @brief 自旋锁 相较互斥锁更为底层, 互斥锁的实现依赖自旋锁.
 * ! 1. 当锁不能被获取到时, 使用互斥体的开销是进程上下午的切换时间, 使用自旋锁的开销是等待获取自旋锁. 若临界区比较小, 适合使用自旋锁.
 * ! 2. 互斥体锁保护的临界区可以包含引起阻塞的代码, 而自旋锁绝对要避免用来保护包含这样代码的临界区.
 *      ! 阻塞意味着进程的切换, 如果进程被切换出去, 另一个进程企图获取本自旋锁, 死锁就会发生.
 * ! 3. 互斥体存在于进程上下文, 因此, 如果被保护的共享资源需要在中断或者软中断情况下使用, 则在互斥体和自旋锁之间选择自旋锁.
 * 
 */