struct el
{
    struct list_head lp;
    long key;
    spinlock_t mutex;
    int data;
};
DEFINE_RWLOCK(listmutex);
LIST_HEAD(head);

int search(long key, int *result)
{
    struct el *p;

#ifdef RW_LOCK
    /**
     * @brief rw_lock
     * 
     */
    read_lock(&listmutex);
    list_for_each_entry(p, &head, lp) {
        if(p->key == key) {
            *result = p->data;
            read_unlock(&listmutex);
            return 1;
        }
    }
    read_unlock(&listmutex);
#else
    /**
     * @brief rcu instead of rwlock
     * 
     */
    rcu_read_lock();
    list_for_each_entry(p, &head, lp) {
        if(p->key == key) {
            *result = p->data;
            rcu_read_unlock();
            return 1;
        }
    }
    rcu_read_unlock();
#endif
    return 0;
}

int delete(long key)
{
    struct el *p;
#ifdef RW_LOCK
    write_lock(&listmutex);
    list_for_each_entry(p, &head, lp) {
        if(p->key == key) {
            list_del(&p->lp);
            write_unlock(&listmutex);
            kfree(p);
            return 1;
        }
    }
    write_unlock(&listmutex);
#else
    spin_lock(&listmutex);
    list_for_each_entry(p, &head, lp) {
        if(p->key == key) {
            list_del_rcu(&p->lp); // ! ? list_del ?
            spin_unlock(&listmutex);
            synchronize_rcu();
            kfree(p);
            return 1;
        }
    }
    spin_unlock(&listmutex);
#endif
    return 0;
}
/**
 * @brief RCU 另一种思路的读写锁, 读没有锁, 访问共享资源会先复制一个副本, 然后对副本进行修改.
 * ! RCU可以看做是读写锁的高性能版本, 相比读写锁, RCU既允许多个读执行单元访问被保护的数据, 又允许多个读执行单元和多个写执行单元同事访问被保护的数据.
 * ! 但是RCU不能代替读写锁, 如果写比较多时, 对读执行单元的性能提高不能弥补写执行单元同步导致的损失!
 * ! 使用RCU时, 写执行单元之间的同步开销会比较大, 它需要延迟数据结构的释放, 复制被修改的数据结构, 它夜必须使用某种锁机制同步并发的其他写执行单元的修改操作.
 * 
 */