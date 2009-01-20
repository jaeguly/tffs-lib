/*
 * crtdef.h
 *
 * C Run-Time Libraries functions definitions
 * head file
 * 
 * knightray@gmail.com
 * 10/28 2008
 */
#ifndef _CRT_DEF_H
#define _CRT_DEF_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define Malloc(size) malloc(size)
#define Free(ptr) free(ptr)
#define Memset(ptr, b, size) memset(ptr, b, size)
#define Memcpy(dst, src, size) memcpy(dst, src, size)
#define Memcmp(dst, src, size) memcmp(dst, src, size)
#define Strcmp(dst, src) strcmp(dst, src)
#define Strcpy(dst, src) strcpy(dst, src)
#define Strlen(str) strlen(str)
#define Toupper(c) toupper(c)
#define Sprintf sprintf

typedef struct _tffs_sys_time{
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
}tffs_sys_time_t;

void Getcurtime(tffs_sys_time_t * ptime);

#endif
