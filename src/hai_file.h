/*
 * hai_file.h
 *
 * Hardware Abstraction Interface implemented by file I/O under Linux.
 * head file.
 *
 * knightray@gmail.com
 * 10/27 2008
 */

#ifndef _HAI_FILE_H
#define _HAI_FILE_H

#include "hai.h"

typedef struct _tdev{
	int fd;
	int16 sector_size;
}tdev_t;

#endif
