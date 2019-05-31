/**
 * @file hash_table.c
 * @author zizy (nuaanjzizy@163.com)
 * @brief realization of https://mp.weixin.qq.com/s/B5xiVeW22ZumbI9KfrYJSg
 *          a LRU(Last Recently Used) Hash structure
 * @version 0.1
 * @date 2018-11-14
 * 
 * @copyright Copyright (c) 2018
 * 
 */


#include "hash_table.h"

struct hash_node {
    struct list_head list;
    struct list_head lru_list;
    char *key;
    char *value;
    int len;
};

struct hash_table {
    struct list_head head[MAX_HASH_SIZE];
    struct list_head lru_head;
    size_t size;
};

void *hash_init()
{
    struct hash_table *hash = (struct hash_table *)malloc(sizeof(struct hash_table));
    if(hash == NULL) return NULL;
    int i;
    hash->size = 0;
    for(i = 0; i < MAX_HASH_SIZE; ++i) {
        INIT_LIST_HEAD(&hash->head[i]);
    }
    INIT_LIST_HEAD(&hash->lru_head);
    return (void *)hash;
}

static void _hash_add_node(struct hash_table __I *table, struct hash_node __I *node)
{
    REQUIE(table == NULL || node == NULL, Error);
    int index = *node->key % MAX_HASH_SIZE;
    printf("add to index %d\n", index);
    struct hash_node *p;
    list_for_each_entry(p, &table->head[index], list) {
        if(strcmp(p->key, node->key) == 0) {
            p->value = realloc(p->value, node->len);
            p->len = node->len;
            memcpy(p->value, node->value, node->len);
            return;
        }
    }
    ++table->size;
    list_add_tail(&node->list, &table->head[index]);
    list_add_tail(&node->lru_list, &table->lru_head);
    Error: {
        return;
    }
}

void hash_add(void __I *hash, char __I *key, char __I *payload, size_t __I len)
{
    struct hash_table *h = (struct hash_table *)hash;
    struct hash_node *node = NULL;
    REQUIE(hash == NULL || key == NULL || *key == 0 || payload == NULL, Error);
    REQUIE((node = (struct hash_node *)malloc(sizeof(struct hash_node))) == NULL, Error);
    REQUIE((node->key = calloc(1, strlen(key) + 1)) == NULL, Error);
    REQUIE((node->value = calloc(1, len)) == NULL, Error);
    strcpy(node->key, key);
    memcpy(node->value, payload, len);
    node->len = len;
    _hash_add_node(hash, node);
    return;
    
    Error:{
        if(node) {
            if(node->key) free(node->key);
            if(node->value) free(node->value);
        }
    }
}

static void _free_node(struct hash_node __I *node) 
{
    if(node) {
        if(node->key) free(node->key);
        if(node->value) free(node->value);
        free(node);
    }
}

static void _hash_del_node(struct hash_table __I *table, char __I *key)
{
    REQUIE(table == NULL || key == NULL || *key == 0, Error);
    int index = *key % MAX_HASH_SIZE;
    struct hash_node *p, *t;
    list_for_each_entry_safe(p, t, &table->head[index], list) {
        if(strcmp(p->key, key) == 0) {
            list_del(&p->list);
            list_del(&p->lru_list);
            _free_node(p);
            --table->size;
            break;
        }
    }
    Error: {
        return;
    }
}

void hash_del(void __I *hash, char __I *key)
{
    struct hash_table *h = (struct hash_table *)hash;
    _hash_del_node(h, key);
}

void hash_deinit(void *hash)
{
    if(hash == NULL) return;
    struct hash_table *h = (struct hash_table *)hash;
    struct hash_node *p, *t;
    list_for_each_entry_safe(p, t, &h->lru_head, lru_list) {
        list_del(&p->lru_list);
        list_del(&p->list);
        _free_node(p);
    }
}

static void _hash_update_lru(struct hash_table __I *hash, struct hash_node __I *node)
{
    list_move_tail(&node->lru_list, &hash->lru_head);
    // list_del(&node->lru_list);
    // list_add_tail(&node->lru_list, &hash->lru_head);
}

char *hash_search(void __I *hash, char __I *key, int __O *len)
{
    REQUIE(hash == NULL || key == NULL || *key == 0, Error);
    struct hash_table *h = (struct hash_table *)hash;
    int index = *key % MAX_HASH_SIZE;
    struct hash_node *p;
    list_for_each_entry(p, &h->head[index], list) {
        if(strcmp(key, p->key) == 0) {
            *len = p->len;
            char *out = calloc(1, p->len);
            memcpy(out, p->value, p->len);
            _hash_update_lru(h, p);
            return out;
        }
    }
    Error: {
        *len = -1;
        return NULL;
    }
}

int hash_size(void __I *hash)
{
    struct hash_table *h = (struct hash_table *)hash;
    return h->size;
}

void hash_pop(void __I *hash, int __I len)
{
    REQUIE(hash == NULL || len == 0, Error);
    struct hash_table *h = (struct hash_table *)hash;
    int size = hash_size(hash);
    int l = (len < 0 || l > size) ? size : len;
    int i = 0;
    struct hash_node *p, *t;
    list_for_each_entry_safe(p, t, &h->lru_head, lru_list) {
        if(i < l) {
            ++i;
            list_del(&p->list);
            list_del(&p->lru_list);
            _free_node(p);
        } else {
            break;
        }
    }
    Error: {
        return;
    }
}

void hash_table_dump(void *hash)
{
    struct hash_table *h = (struct hash_table *)hash;
    struct hash_node *p;
    int i = 0;
    for(; i < MAX_HASH_SIZE; ++i) {
        list_for_each_entry(p, &h->head[i], list) {
            printf("key: %s\n\tval:(%d) %s\n", p->key, p->len, p->value);
        }        
    }
}

void hash_table_dump_time(void *hash)
{
    struct hash_table *h = (struct hash_table *)hash;
    struct hash_node *p;
    list_for_each_entry(p, &h->lru_head, lru_list) {
        printf("key: %s\n\tval:(%d) %s\n", p->key, p->len, p->value);
    }
}

int main(int argc,char *argv[])
{
    void *hash = hash_init();
    hash_add(hash, "1", "abc", 4);
    hash_add(hash, "2", "def", 4);
    hash_add(hash, "5", "jkl", 4);
    hash_add(hash, "3", "ghi", 4);
    hash_table_dump(hash);
    printf("-------\n");
    hash_table_dump_time(hash);
    int len = 0;
    char *out = hash_search(hash, "2", &len);
    printf("find %s\n", out);
    hash_table_dump(hash);
    printf("-------\n");
    hash_table_dump_time(hash);

    hash_pop(hash, 2);

    // hash_del(hash, "2");
    printf("-------\n");
    hash_table_dump(hash);
    printf("-------\n");
    hash_table_dump_time(hash);
    
    return 0;
}
