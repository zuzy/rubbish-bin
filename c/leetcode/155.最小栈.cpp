/*
 * @lc app=leetcode.cn id=155 lang=cpp
 *
 * [155] 最小栈
 *
 * https://leetcode-cn.com/problems/min-stack/description/
 *
 * algorithms
 * Easy (52.76%)
 * Likes:    500
 * Dislikes: 0
 * Total Accepted:    111.4K
 * Total Submissions: 206.6K
 * Testcase Example:  '["MinStack","push","push","push","getMin","pop","top","getMin"]\n' +
  '[[],[-2],[0],[-3],[],[],[],[]]'
 *
 * 设计一个支持 push ，pop ，top 操作，并能在常数时间内检索到最小元素的栈。
 * 
 * 
 * push(x) —— 将元素 x 推入栈中。
 * pop() —— 删除栈顶的元素。
 * top() —— 获取栈顶元素。
 * getMin() —— 检索栈中的最小元素。
 * 
 * 
 * 
 * 
 * 示例:
 * 
 * 输入：
 * ["MinStack","push","push","push","getMin","pop","top","getMin"]
 * [[],[-2],[0],[-3],[],[],[],[]]
 * 
 * 输出：
 * [null,null,null,null,-3,null,0,-2]
 * 
 * 解释：
 * MinStack minStack = new MinStack();
 * minStack.push(-2);
 * minStack.push(0);
 * minStack.push(-3);
 * minStack.getMin();   --> 返回 -3.
 * minStack.pop();
 * minStack.top();      --> 返回 0.
 * minStack.getMin();   --> 返回 -2.
 * 
 * 
 * 
 * 
 * 提示：
 * 
 * 
 * pop、top 和 getMin 操作总是在 非空栈 上调用。
 * 
 * 
 */
#include <iostream>
#include <stdio.h>

// @lc code=start
class MinStack {
    struct stack{
        int val;
        stack *next;
        stack *prev;
        stack *snext;
        stack *sprev;

        stack(int v)
        : val(v)
        , next(NULL)
        , prev(NULL)
        , snext(NULL)
        , sprev(NULL)
        {}

    };
    stack d;
    stack *dummy;
public:
    /** initialize your data structure here. */
    MinStack() 
    : d(0)
    {
        dummy = &d;
        dummy->next = dummy->prev = dummy;
        dummy->snext = dummy->sprev = dummy;
    }
    
    void push(int x) 
    {
        stack *tmp = new stack(x);
        tmp->prev = dummy->prev;
        dummy->prev->next = tmp;
        dummy->prev = tmp;
        tmp->next = dummy;

        stack *p = dummy->snext;
        while(p != dummy) {
            if(p->val > tmp->val) break;
            p = p->snext;
        }
        tmp->sprev = p->prev;
        p->sprev->snext = tmp;
        p->sprev = tmp;
        tmp->snext = p;
    }
    
    void pop() {
        
    }
    
    int top() {
        
    }
    
    int getMin() {
        
    }
};

/**
 * Your MinStack object will be instantiated and called as such:
 * MinStack* obj = new MinStack();
 * obj->push(x);
 * obj->pop();
 * int param_3 = obj->top();
 * int param_4 = obj->getMin();
 */
// @lc code=end

