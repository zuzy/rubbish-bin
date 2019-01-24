#ifndef __HALO_UTF8_H__
#define __HALO_UTF8_H__

#include <string.h>
#include <stdint.h>

#define HC_HTTP_DEBUG 0
#if HC_HTTP_DEBUG
    #define print_utf8(format, arg...)   do { printf("\033[31m[-hc_utf8-]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} \
                                        while (0)
#else
    #define print_utf8(format, arg...)   NULL
#endif

/** 
 * @brief  Cut the string with utf8 format
 * @note   
 * @param  *str: the string to cut; change the string;
 * @param  len: length limited
 * @retval 0->success
 */
int utf8_cut_string(char *str, int len);

/** 
 * @brief  get the utf8 length of string
 * @note   
 * @param  *str: the target string
 * @retval the utf8 length; a Chinese word or someone else equal to one byte length
 */
size_t utf8_strlen(char *str);

/** 
 * @brief  add a x-www couple to string;
 * @note   
 * @param  *des: OUT the string output
 * @param  *key: IN the keyword of x-www
 * @param  *body: IN the body of x-www
 * @param  len: IN the length limited
 * @retval -1 means failed, des will be revert to the state when input; 0 -> success
 */
int add_x_www_couple(char *des, char *key, char *body, int len);

/** 
 * @brief  get the x-www format len of a string;
 * @note   
 * @param  *str: the target string input
 * @retval the result length
 */
size_t sizeof_x_www_format(char *str);

/**
 * @brief  covert Chinese num to num
 * @note   only num 0 <= input <= 999 support
 * @param  *input: Chinese string
 * @param  forcebegin:  if not 0 -> change the first Chinese number
 * @retval return num; error < 0;
 */
int chinese_num(char *input, int forcebegin);

/**
 * @brief  get string before Chinese number
 * @note   
 * @param  *input: 
 * @retval 
 */
char *str_befor_ch_num(char *input);

#endif