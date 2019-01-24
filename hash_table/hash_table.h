/**
 * @file hash_table.h
 * @author zizy (nuaanjzizy@163.com)
 * @brief hash && lru struct
 * @version 0.1
 * @date 2018-11-14
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "list.h"

#define MAX_HASH_SIZE	32
#define REQUIE(condition,tag) if(condition)goto tag
#define __I
#define __O

/**
 * @brief init && buid a hash table
 * 
 * @return void* 
 */
void *hash_init();

void hash_add(void __I *hash, 
                char __I *key, 
                char __I *payload, 
                size_t __I len);

void hash_del(void __I *hash, char __I *key);

char *hash_search(void __I *hash, char __I *key, int __O *len);

int hash_size(void __I *hash);

void hash_pop(void __I *hash, int __I len);
