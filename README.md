The TFFS project aims to create a FAT file system module for embedded system. All of code is writen by ANSI C, so it is easy to port for your architecture. You can support your own specific hardware by implimentating your own Hardware Abstraction Interface.

## Feature of Tiny-FAT file system ##
  * Supports FAT12, FAT16 and FAT32.
  * Supports both 8.3 format file name and Long File Name(LFN).
  * Supports cache system to improve access speed.
  * Hardware independent design, easy to port to specific hardware platform.
  * Works under Linux, iTRION and None-OS environment(currently only works under Linux).

## Get latest version ##
You can get the latest version of TFFS-lib by checking out from google code.
```
svn checkout http://tffs-lib.googlecode.com/svn/trunk/ tffs-lib-read-only
```

## Build tffs-lib ##
1. Enter into the tffs-lib directory and make the library.
```
make
```
2. _tshell_ is a test tool and a demo application to show how to use the tffs-lib. You can build it by the following command.
```
make tsh
```
3. tffs is a test application including some test cases. You can build it by using the following command.
```
make tffs
```

## Test under linux ##
1. First of all, you have to create a FAT file system image used by _tsh_. It can be done by the script "_mkfs.sh_". The following command create a FAT file system image file with the size of 32M.
```
mkfs.sh fat32.img 32768
```
2. Now, you can use the tsh to test the function of tffs-lib.
```
[zhangck@subnode1 tffs-lib-read-only]$ ./tsh fat32.img
=================Boot sector====================
oem_name: mkdosfs
byts_per_sec: 512
resvd_sec_cnt: 32
sec_per_clus: 1
================================================
count_of_clusters = 64496
tffs->fat_type:3
tffs->sec_fat:32
tffs->sec_root_dir:1040
tffs->sec_first_data:1040
tiny fat file system mount OK.
/:>ls

Total 0 files.
/:>help
tsh commands:
  ls:           Lists the contents of a directory.
  pwd:          Print current directory.
  cd:           Change the current directory to dir.
  mkdir:        Make directories.
  rm:           Remove directories or files.
  cat:          Show the file contents.
  write:        Write contents to the file.
  help:         Show help.
  quit:         Quit tiny shell.
/:>mkdir tdir1
/:>ls
       0 byte   2009/01/20 - 16:24  tdir1

Total 1 files.
/:>mkdir tdir2
/:>ls
       0 byte   2009/01/20 - 16:24  tdir1
       0 byte   2009/01/20 - 16:24  tdir2

Total 2 files.
/:>write test1.txt
this is a test line.
this is a test line2.

============================================================
wrote 44 bytes to file test1.txt ok.
/:>ls
       0 byte   2009/01/20 - 16:24  tdir1
       0 byte   2009/01/20 - 16:24  tdir2
      44 byte   2009/01/20 - 16:24  test1.txt

Total 3 files.
/:>cat test1.txt
this is a test line.
this is a test line2.

============================================================
[test1.txt] total size 44 bytes
/:>write testlongfilenamefile.fileextion
this is a test line 01.
this is a test line 02.
this is a test line 03.
this is a test line 04.

============================================================
wrote 96 bytes to file testlongfilenamefile.fileextion ok.
/:>ls
       0 byte   2009/01/20 - 16:24  tdir1
       0 byte   2009/01/20 - 16:24  tdir2
      44 byte   2009/01/20 - 16:24  test1.txt
      96 byte   2009/01/20 - 16:25  testlongfilenamefile.fileextion

Total 4 files.
/:>cd tdir1
/tdir1/:>ls
       0 byte   2009/01/20 - 16:24  .
       0 byte   2009/01/20 - 16:24  ..

Total 2 files.
/tdir1/:>mkdir dir1
/tdir1/:>mkdir dir2
/tdir1/:>mkdir dir3
/tdir1/:>mkdir dir4
/tdir1/:>mkdir dir5
/tdir1/:>ls
       0 byte   2009/01/20 - 16:24  .
       0 byte   2009/01/20 - 16:24  ..
       0 byte   2009/01/20 - 16:26  dir1
       0 byte   2009/01/20 - 16:26  dir2
       0 byte   2009/01/20 - 16:26  dir3
       0 byte   2009/01/20 - 16:26  dir4
       0 byte   2009/01/20 - 16:26  dir5

Total 7 files.
/tdir1/:>write t.txt
this is a test line 01.
this is a test line 02.

============================================================
wrote 52 bytes to file t.txt ok.
/tdir1/:>ls
       0 byte   2009/01/20 - 16:24  .
       0 byte   2009/01/20 - 16:24  ..
       0 byte   2009/01/20 - 16:26  dir1
       0 byte   2009/01/20 - 16:26  dir2
       0 byte   2009/01/20 - 16:26  dir3
       0 byte   2009/01/20 - 16:26  dir4
       0 byte   2009/01/20 - 16:26  dir5
      52 byte   2009/01/20 - 16:26  t.txt

Total 8 files.
/tdir1/:>rm t.txt
/tdir1/:>ls
       0 byte   2009/01/20 - 16:24  .
       0 byte   2009/01/20 - 16:24  ..
       0 byte   2009/01/20 - 16:26  dir1
       0 byte   2009/01/20 - 16:26  dir2
       0 byte   2009/01/20 - 16:26  dir3
       0 byte   2009/01/20 - 16:26  dir4
       0 byte   2009/01/20 - 16:26  dir5

Total 7 files.
/tdir1/:>rm -r dir1
/tdir1/:>ls
       0 byte   2009/01/20 - 16:24  .
       0 byte   2009/01/20 - 16:24  ..
       0 byte   2009/01/20 - 16:26  dir2
       0 byte   2009/01/20 - 16:26  dir3
       0 byte   2009/01/20 - 16:26  dir4
       0 byte   2009/01/20 - 16:26  dir5

Total 6 files.
/tdir1/:>mkdir dir6
/tdir1/:>ls
       0 byte   2009/01/20 - 16:24  .
       0 byte   2009/01/20 - 16:24  ..
       0 byte   2009/01/20 - 16:27  dir6
       0 byte   2009/01/20 - 16:26  dir2
       0 byte   2009/01/20 - 16:26  dir3
       0 byte   2009/01/20 - 16:26  dir4
       0 byte   2009/01/20 - 16:26  dir5

Total 7 files.
/tdir1/:>mkdir dir7
/tdir1/:>ls
       0 byte   2009/01/20 - 16:24  .
       0 byte   2009/01/20 - 16:24  ..
       0 byte   2009/01/20 - 16:27  dir6
       0 byte   2009/01/20 - 16:26  dir2
       0 byte   2009/01/20 - 16:26  dir3
       0 byte   2009/01/20 - 16:26  dir4
       0 byte   2009/01/20 - 16:26  dir5
       0 byte   2009/01/20 - 16:27  dir7

Total 8 files.
/tdir1/:>cd dir2
/tdir1/dir2/:>ls
       0 byte   2009/01/20 - 16:26  .
       0 byte   2009/01/20 - 16:26  ..

Total 2 files.
/tdir1/dir2/:>mkdir dir3
/tdir1/dir2/:>cd dir3
/tdir1/dir2/dir3/:>mkdir dir4
/tdir1/dir2/dir3/:>cd dir4
/tdir1/dir2/dir3/dir4/:>mkdir dir5
/tdir1/dir2/dir3/dir4/:>cd dir5
/tdir1/dir2/dir3/dir4/dir5/:>mkdir dir6
/tdir1/dir2/dir3/dir4/dir5/:>cd dir6
/tdir1/dir2/dir3/dir4/dir5/dir6/:>pwd
/tdir1/dir2/dir3/dir4/dir5/dir6/
/tdir1/dir2/dir3/dir4/dir5/dir6/:>quit
[zhangck@subnode1 tffs-lib-read-only]$
```

3. You can also use the fat system module of linux to validate the result of tffs-lib by using the following command.
```
[root@subnode1 tffs-lib-read-only]# mkdir /mnt/fatimg
[root@subnode1 tffs-lib-read-only]# ./mountfs.sh fat32.img /mnt/fatimg/
/dev/loop1 installed...
/mnt/fatimg/ mount ok...
[root@subnode1 tffs-lib-read-only]# cd /mnt/fatimg/
[root@subnode1 fatimg]# ls
tdir1  tdir2  test1.txt  testlongfilenamefile.fileextion
[root@subnode1 fatimg]# ll
drwxr-xr-x 8 root root 512  1月 20 16:24 tdir1
drwxr-xr-x 2 root root 512  1月 20 16:24 tdir2
-rwxr-xr-x 1 root root  44  1月 20 16:24 test1.txt
-rwxr-xr-x 1 root root  96  1月 20 16:25 testlongfilenamefile.fileextion
```

## Enjoy it ##

