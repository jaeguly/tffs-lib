#! /bin/bash

if [ $# != 2 ]; then
        echo "Usage: mkfs.sh <file name> <size>"
        exit 0
fi

# produce the destination file.
dd if=/dev/zero of=./$1 bs=1k count=$2

# format the destination file.
/sbin/mkfs.vfat -F 32 -v ./$1

