#ifndef __CH2NUM_H__
#define __CH2NUM_H__
/**
 * @brief  transform number in Chinese to int;
 * @note   cut down the num in Chinese of string input (utf8 format); Only number <0~999> support
 * @param  *input: target string with Chinese Num
 * @retval result num;
 */
int chinese_num(char *input);

#endif