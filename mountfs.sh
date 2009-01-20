#! /bin/bash

if [ "$(id -u)" != "0" ]; then
	echo "This script must be run as root"
	exit 0
fi

if [ $# != 2 ]; then
	echo "mountfs <file name> <dest directory>"
	exit 0
fi

LOOPDEV=`/sbin/losetup -f`

/sbin/losetup $LOOPDEV ./$1
echo $LOOPDEV "installed..."
mount -t vfat $LOOPDEV $2
echo $2 "mount ok..."
