/*
 * initfs.h
 *
 * TFFS_mount TFFS_mkfs implementation.
 * head file.
 *
 * knightray@gmail.com
 * 10/28 2008
 */

#ifndef _INIT_FS_H
#define _INIT_FS_H

#include "comdef.h"
#include "pubstruct.h"

/*
 * Function prototype
 */
int32
_validate_bs(
	IN	boot_sector_t * pbs
);

void
_parse_boot_sector(
    IN  boot_sector_t * pbs,
	OUT	tffs_t * tffs
);


#endif
