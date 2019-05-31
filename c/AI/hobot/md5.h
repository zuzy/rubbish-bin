/*-------------------------------------------------------------------------
 *
 * md5.h
 *	  Interface to md5.c
 *
 * Portions Copyright (c) 1996-2005, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * $Header$
 *
 *-------------------------------------------------------------------------
 */

/*
 *  This file is imported from PostgreSQL 8.1.3.
 *  Modified by Taiki Yamaguchi <yamaguchi@sraoss.co.jp>
 */

#ifndef MD5_H
#define MD5_H

#define MD5_PASSWD_LEN 32

//extern
int md5_hash(const void *buff, unsigned int len, char *hexsum);
//extern
int md5_encrypt(const char *passwd, const char *salt, unsigned int salt_len, char *buf);

#endif
