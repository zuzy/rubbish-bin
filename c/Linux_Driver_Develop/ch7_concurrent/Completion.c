struct completion my_completion;
init_completion(&my_completion);

// 等待一个完成量被唤醒
void wait_for_completion(struct completion *c);

// 唤醒一个等待的执行单元
void complete(struct completion *c);
// 释放所有等待同一完成量的执行单元
void complete_all(struct completion *c);