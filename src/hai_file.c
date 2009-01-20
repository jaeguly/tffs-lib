/*
 * hai_file.c
 *
 * Hardware Abstraction Interface implemented by file I/O under Linux.
 * implementation file.
 *
 * knightray@gmail.com
 * 10/27 2008
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "hai_file.h"
#include "pubstruct.h"

tdev_handle_t
HAI_initdevice(
	IN	byte * dev,
	IN	int16 sector_size)
{
	int fd;
	tdev_t * pdev;

	if ((fd = open(dev, O_RDWR)) < 0)
		return (tdev_handle_t)NULL;

	pdev = (tdev_t *)malloc(sizeof(tdev_t));
	pdev->fd = fd;
	pdev->sector_size = sector_size;

	return (tdev_handle_t)pdev;
}

int32
HAI_readsector(
	IN	tdev_handle_t hdev,
	IN	int32 addr,
	OUT	ubyte * ptr)
{
	tdev_t * pdev = (tdev_t *)hdev;

	if (!ptr || !pdev || pdev->fd <= 0)
		return ERR_HAI_INVALID_PARAMETER;

	if (lseek(pdev->fd, addr * pdev->sector_size, SEEK_SET) < 0)
		return ERR_HAI_READ;

	if (read(pdev->fd, ptr, pdev->sector_size) < 0)
		return ERR_HAI_READ;

	return HAI_OK;
}

int32
HAI_writesector(
	IN	tdev_handle_t hdev,
	IN	int32 addr,
	IN	ubyte * ptr)
{
	tdev_t * pdev = (tdev_t *)hdev;

	if (!ptr || !pdev || pdev->fd <= 0)
		return ERR_HAI_INVALID_PARAMETER;
	
	if (lseek(pdev->fd, addr * pdev->sector_size, SEEK_SET) < 0)
		return ERR_HAI_WRITE;

	if (write(pdev->fd, ptr, pdev->sector_size) < 0)
		return ERR_HAI_WRITE;

	fsync(pdev->fd);
	return HAI_OK;
}

int32
HAI_closedevice(
	IN	tdev_handle_t hdev)
{
	tdev_t * pdev = (tdev_t *)hdev;

	if (!pdev || pdev->fd <= 0)
		return ERR_HAI_INVALID_PARAMETER;
	
	if (close(pdev->fd) < 0)
		return ERR_HAI_CLOSE;

	Free(pdev);

	return HAI_OK;
}

int32
HAI_getdevinfo(
	IN	tdev_handle_t hdev,
	OUT	tdev_info_t * devinfo)
{
	tdev_t * pdev = (tdev_t *)hdev;

	if (!devinfo || !pdev || pdev->fd <= 0)
		return ERR_HAI_INVALID_PARAMETER;

	devinfo->free_sectors = 1000;
	return HAI_OK;
}

