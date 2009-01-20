/*
 * main.c
 *
 * Test function and main function under linux.
 * implementation file.
 *
 * knightray@gmail.com
 * 10/27 2008
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hai.h"
#include "tffs.h"
#include "debug.h"
#include "dirent.h"
#include "crtdef.h"
#include "fat.h"
#include "cache.h"

int TFFS_DIR_tester(char * imgname);
int TFFS_FAT_tester(char * imgname);
int TFFS_FILE_tester(char * imgname);
int TFFS_CACHE_tester(char * imgname);
int HAI_tester(char * imgname);

void loop_dir(tdir_handle_t hdir);
void dump_fat(tffs_handle_t htffs);
void function_tester();
void ls_dir(tffs_handle_t htffs, char * dir_name);
int create_file(tffs_handle_t htffs, char * fname, char * mode, int size);
int read_file(tffs_handle_t htffs, char * fname);
int malloc_fat_entry(tfat_t * pfat, int32 clus_num);
void dump_cache(tcache_t * pcache);

#define TASSERT(ret, val)  if(ret != val) { \
                        printf("ASSERT failed at %s():%d, ret = %d\n", __FUNCTION__, __LINE__, ret); \
                        exit(0); \
                    }

int main(int argc, char * argv[])
{
	if (argc < 2) {
		printf("Usage : tffs <image name>\n");
		return -1;
	}
	
	//function_tester();
	//return 0;

	if (TFFS_DIR_tester(argv[1]) < 0) {
		printf("TFFS_tester failed.\n");
		return -1;
	}

	return 0;
}

/*--------------------------------------------------------------------------------------------*/
int
TFFS_CACHE_tester(char * imgname)
{
	tffs_handle_t htffs;
	tcache_t * pcache;
	int ret;
	int16 seci;
	ubyte secbuf[512];
	uint32 sectors[] = {12, 11, 4, 3, 11, 4, 2, 9, 8, 4, 10, 5, 12};

	ret = TFFS_mount(imgname, &htffs);
	TASSERT(ret, TFFS_OK);
	
	pcache = cache_init(((tffs_t *)htffs)->hdev, 6, 512);
	if (pcache == NULL) {
		printf("cache init failed.\n");
		return -1;
	}

	dump_cache(pcache);

	for (seci = 0; seci < sizeof(sectors)/sizeof(sectors[0]); seci++) {
		printf("sector %d access.\n", sectors[seci]);
		if (seci % 2)
			ret = cache_readsector(pcache, sectors[seci], secbuf);
		else
			ret = cache_writesector(pcache, sectors[seci], secbuf);

		TASSERT(ret, CACHE_OK);
		dump_cache(pcache);
	}
	
	ret = cache_destroy(pcache);
	TASSERT(ret, CACHE_OK);
	return 1;
}

void
dump_cache(tcache_t * pcache)
{
	int16 seci;

	printf("cache###seccnt[%d], head[%d], tail[%d]###\n", pcache->seccnt, pcache->head, pcache->tail);
	for (seci = 0; seci < pcache->seccnt; seci++) {
		printf("   [%02d] sec[%d], is_dirty[%d], pre[%d], next[%d]\n", seci, 
			pcache->secbufs[seci].sec,
			pcache->secbufs[seci].is_dirty,
			pcache->secbufs[seci].pre,
			pcache->secbufs[seci].next);
	}
	printf("#########################################\n");
}

/*--------------------------------------------------------------------------------------------*/

int
TFFS_DIR_tester(char * imgname)
{
	tffs_handle_t htffs;
	int ret;

	ret = TFFS_mount(imgname, &htffs);
	TASSERT(ret, TFFS_OK);
	
	dump_fat(htffs);
	ls_dir(htffs, "/");

	ret = TFFS_mkdir(htffs, "/mdir1");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_mkdir(htffs, "/mdir2");
	TASSERT(ret, TFFS_OK);
	
	ret = TFFS_mkdir(htffs, "/mdir3");
	TASSERT(ret, TFFS_OK);

	ls_dir(htffs, "/");
	printf("[1]%s(): create mdir1,mdir2,mdir3 OK!\n", __FUNCTION__);	

	ret = TFFS_mkdir(htffs, "/mdir1/abcdefghijklmnopqrstuvwxyz1234567890");
	TASSERT(ret, TFFS_OK);

	ls_dir(htffs, "/mdir1/");

	ret = TFFS_mkdir(htffs, "/mdir1/abcdefghijklmnopqrstuvwxyz1234567890/mdir111");
	TASSERT(ret, TFFS_OK);
	
	ls_dir(htffs, "/mdir1/abcdefghijklmnopqrstuvwxyz1234567890/");
	ls_dir(htffs, "/mdir1/abcdefghijklmnopqrstuvwxyz1234567890/mdir111/");
	printf("[2]%s(): create long file name directory OK!\n", __FUNCTION__);	

	ret = TFFS_mkdir(htffs, "/mdir4/mdir44");
	TASSERT(ret, ERR_TFFS_INVALID_PATH);

	ret = TFFS_mkdir(htffs, "/mdir3");
	TASSERT(ret, ERR_TFFS_DIR_ALREADY_EXIST);
	printf("[3]%s(): mkdir error return code check OK!\n", __FUNCTION__);

	ret = TFFS_rmdir(htffs, "/mdir1");
	TASSERT(ret, ERR_TFFS_NOT_EMPTY_DIR);

	ret = TFFS_rmdir(htffs, "/mdir4");
	TASSERT(ret, ERR_TFFS_NO_SUCH_FILE);
	printf("[4]%s(): rmdir error return code check OK!\n", __FUNCTION__);

	ret = TFFS_mkdir(htffs, "mdir5");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_chdir(htffs, "/mdir2");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_mkdir(htffs, "mdir21");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_chdir(htffs, "../mdir3");
	TASSERT(ret, TFFS_OK);
	
	ret = TFFS_mkdir(htffs, "mdir31");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_chdir(htffs, "./mdir31/../../mdir2/");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_mkdir(htffs, "mdir21/mdir211");
	TASSERT(ret, TFFS_OK);

	ls_dir(htffs, "/");
	ls_dir(htffs, "/mdir1/");
	ls_dir(htffs, "/mdir2/");
	ls_dir(htffs, "/mdir2/mdir21/");
	ls_dir(htffs, "/mdir3/");
	printf("[5]%s(): chdir test ok!\n", __FUNCTION__);

	ret = TFFS_rmdir(htffs, "/mdir3/mdir31");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmdir(htffs, "/mdir3");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmdir(htffs, "/mdir2/mdir21/mdir211");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmdir(htffs, "/mdir2/mdir21");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmdir(htffs, "/mdir2");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmdir(htffs, "/mdir5");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmdir(htffs, "/mdir1/abcdefghijklmnopqrstuvwxyz1234567890/mdir111");
	TASSERT(ret, TFFS_OK);
	
	ret = TFFS_rmdir(htffs, "/mdir1/abcdefghijklmnopqrstuvwxyz1234567890");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmdir(htffs, "/mdir1");
	TASSERT(ret, TFFS_OK);
	printf("[6]%s(): remove directory OK!\n", __FUNCTION__);
	
	dump_fat(htffs);

	ret = TFFS_umount(htffs);
	TASSERT(ret, TFFS_OK);
	
	printf("%s(): ALL TEST CLEAR!!\n", __FUNCTION__);
	return 0;
}

void
ls_dir(tffs_handle_t htffs, char * dir_name)
{
	tdir_handle_t hdir;
	int32 ret;

	if ((ret = TFFS_opendir(htffs, dir_name, &hdir)) != TFFS_OK) {
		printf("TFFS_opendir failed. [%s] ret = %d\n", dir_name, ret);
		return;
	}

	printf("##[%s]\n", dir_name);
	loop_dir(hdir);

	if ((ret = TFFS_closedir(hdir)) != TFFS_OK) {
		printf("TFFS_closedir failed. ret = %d\n", ret);
		return ;
	}
}

void
loop_dir(tdir_handle_t hdir)
{
	int ret;

	while (1) {
		dirent_t dirent;

		if ((ret = TFFS_readdir(hdir, &dirent)) == TFFS_OK) {
			if (dirent.dir_attr & DIR_ATTR_DIRECTORY) {
				printf("  [%s](%s)\n", dirent.d_name, dirent.d_name_short);
			}
			else {
				printf("  |%s|(%s)\n", dirent.d_name, dirent.d_name_short);
			}
		}
		else if (ret == ERR_TFFS_LAST_DIRENTRY) {
			break;
		}
		else {
			printf("TFFS_readdir failed.\n");
			break;
		}
	}
}

/*--------------------------------------------------------------------------------------------*/
void
dump_dir_entry(tdir_entry_t * pdir_entry)
{
	printf("pdir_entry->dirent_num=%d\n", pdir_entry->dirent_num);
	printf("pdir_entry->long_name=%s\n", pdir_entry->long_name);
	printf("pdir_entry->short_name=%s\n", pdir_entry->short_name);
	
	int i;
	long_dir_entry_t * pldent;

	pldent = (long_dir_entry_t *)pdir_entry->pdirent;
	for (i = pdir_entry->dirent_num - 2; i >= 0; i--) {

		printf("pdirent[%d].ldir_ord = %x\n", i, pldent->ldir_ord);
		printf("pdirent[%d].ldir_attr = %x\n", i, pldent->ldir_attr);
		printf("pdirent[%d].ldir_type = %x\n", i, pldent->ldir_type);
		printf("pdirent[%d].ldir_chksum = %x\n", i, pldent->ldir_chksum);
		printf("pdirent[%d].ldir_fst_clus_lo = %x\n", i, pldent->ldir_fst_clus_lo);
		
		pldent++;
	}
}
void
function_tester()
{
#include "common.h"
	char fname[64];
	tdir_entry_t dir_entry;
	int ret;

	char path[256];
	char * tokens[12];
	int i, n;

	strcpy(path, "dir1/dir11/dir33/xxx.txt");
	divide_path(path, fname);
	printf("%s:%s\n", path, fname);
	n = tokenize(path, '/', tokens);
	for (i = 0; i < n; i++)
		printf("tokens[%d] = %s\n", i, tokens[i]);

	strcpy(path, "dir1");
	divide_path(path, fname);
	printf("%s:%s\n", path, fname);
	n = tokenize(path, '/', tokens);
	for (i = 0; i < n; i++)
		printf("tokens[%d] = %s\n", i, tokens[i]);

	strcpy(path, "/dir11/dir33/xxx/");
	divide_path(path, fname);
	printf("%s:%s\n", path, fname);
	n = tokenize(path, '/', tokens);
	for (i = 0; i < n; i++)
		printf("tokens[%d] = %s\n", i, tokens[i]);
	return;

	sprintf(fname, "hai.h");
	ret = dirent_init(fname, 0, TRUE, &dir_entry);
	printf("line[%d], ret = %d\n", __LINE__, ret);
	dump_dir_entry(&dir_entry);

	sprintf(fname, "abcdefgh.ijk");
	ret = dirent_init(fname, 0, TRUE, &dir_entry);
	printf("line[%d], ret = %d\n", __LINE__, ret);
	dump_dir_entry(&dir_entry);

	sprintf(fname, "abcdefghijklmn");
	ret = dirent_init(fname, 0, TRUE, &dir_entry);
	printf("line[%d], ret = %d\n", __LINE__, ret);
	dump_dir_entry(&dir_entry);

	sprintf(fname, "abcdefghijklmnopqrstvuwxyz.1234567890");
	ret = dirent_init(fname, 0, TRUE, &dir_entry);
	printf("line[%d], ret = %d\n", __LINE__, ret);
	dump_dir_entry(&dir_entry);

	sprintf(fname, "this is some text.txt");
	ret = dirent_init(fname, 0, TRUE, &dir_entry);
	printf("line[%d], ret = %d\n", __LINE__, ret);
	dump_dir_entry(&dir_entry);

	sprintf(fname, ".");
	ret = dirent_init(fname, 0, TRUE, &dir_entry);
	printf("line[%d], ret = %d\n", __LINE__, ret);
	dump_dir_entry(&dir_entry);

	sprintf(fname, "dir");
	ret = dirent_init(fname, 0, TRUE, &dir_entry);
	printf("line[%d], ret = %d\n", __LINE__, ret);
	dump_dir_entry(&dir_entry);

	sprintf(fname, "abcdefgh");
	ret = dirent_init(fname, 0, TRUE, &dir_entry);
	printf("line[%d], ret = %d\n", __LINE__, ret);
	dump_dir_entry(&dir_entry);

}

/*--------------------------------------------------------------------------------------------*/

int
TFFS_FILE_tester(char * imgname)
{
	tffs_handle_t htffs;
	int ret;

	ret = TFFS_mount(imgname, &htffs);
	TASSERT(ret, TFFS_OK);

	dump_fat(htffs);
	ret = create_file(htffs, "/tfile1.txt", "w", 512);
	TASSERT(ret, TFFS_OK);

	ret = create_file(htffs, "/tfile2.txt", "w", 2048);
	TASSERT(ret, TFFS_OK);

	ret = create_file(htffs, "/tfile3.txt", "a", 1280);
	TASSERT(ret, TFFS_OK);

	ret = create_file(htffs, "/tfile1.txt", "a", 128);
	TASSERT(ret, TFFS_OK);

	printf("[1]create 3 files OK!\n");

	ret = TFFS_mkdir(htffs, "/fdir1");
	TASSERT(ret, TFFS_OK);

	ret = create_file(htffs, "/fdir1/tfile11.txt", "w", 512);
	TASSERT(ret, TFFS_OK);

	ret = TFFS_mkdir(htffs, "/fdir1/fdir11");
	TASSERT(ret, TFFS_OK);

	ret = create_file(htffs, "/fdir1/fdir11/tfile111.txt", "w", 512);
	TASSERT(ret, TFFS_OK);

	printf("[2]create files under directory OK!\n");
	
	ret = create_file(htffs, "/fdir1/abcdefghijklmnopqrstuvwxyz1234567890.123456789abc", "w", 1);
	TASSERT(ret, TFFS_OK);

	ret = create_file(htffs, "/fdir1/abcdefghijklmnopqrstuvwxyz1234567890.123456789abc", "a", 1);
	TASSERT(ret, TFFS_OK);

	printf("[3]create file with long file name OK!\n");

	ret = read_file(htffs, "/tfile1.txt");
	TASSERT(ret, TFFS_OK);

	ret = read_file(htffs, "/tfile2.txt");
	TASSERT(ret, TFFS_OK);

	ret = read_file(htffs, "/fdir1/fdir11/tfile111.txt");
	TASSERT(ret, TFFS_OK);

	ret = read_file(htffs, "/fdir1/abcdefghijklmnopqrstuvwxyz1234567890.123456789abc");
	TASSERT(ret, TFFS_OK);

	printf("[4]read file OK!\n");
	dump_fat(htffs);

	ret = TFFS_rmdir(htffs, "/fdir1/fdir11");
	TASSERT(ret, ERR_TFFS_NOT_EMPTY_DIR);

	ret = TFFS_rmfile(htffs, "/fdir1/fdir11/tfile111.txt");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmdir(htffs, "/fdir1/fdir11");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmfile(htffs, "/fdir1/abcdefghijklmnopqrstuvwxyz1234567890.123456789abc");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmfile(htffs, "/fdir1/tfile11.txt");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmdir(htffs, "/fdir1");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmfile(htffs, "/tfile1.txt");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmfile(htffs, "/tfile2.txt");
	TASSERT(ret, TFFS_OK);

	ret = TFFS_rmfile(htffs, "/tfile3.txt");
	TASSERT(ret, TFFS_OK);

	printf("[5]remove file OK!\n");
	dump_fat(htffs);

	ret = TFFS_umount(htffs);
	TASSERT(ret, TFFS_OK);

	printf("%s(): ALL TEST CLEAR!\n", __FUNCTION__);
	return 0;
}

int32
_do_file_read(tfile_handle_t hfile)
{
#define BUF_SIZE	256
#define TOTAL_SIZE	102400

 	ubyte * pbuf;
	ubyte * pallbuf;
	int32 ret;
	int32 total_size;

	pbuf = (ubyte *)malloc(BUF_SIZE + 1);
	pallbuf = (ubyte *)malloc(TOTAL_SIZE + 1);
	total_size = 0;

	while (1) {
		if ((ret = TFFS_fread(hfile, BUF_SIZE, pbuf)) < 0) {
			if (ret == ERR_TFFS_FILE_EOF) {
				ret = TFFS_OK;
				//printf("%s: got the end of the file.\n", __FUNCTION__);
				break;
			}
			else {
				printf("%s: TFFS_read failed. ret = %d\n", __FUNCTION__, ret);
				goto _release;
			}
		}
		memcpy(pallbuf + total_size, pbuf, ret);
		total_size += ret;
		printf("# read content[%d bytes]:\n", ret);
		//pbuf[ret] = '\0';
		//printf("%s\n", pbuf);
	}
	printf("### read all content[%d bytes]:\n", total_size);
	pallbuf[total_size] = '\0';
	//printf("%s", pallbuf);

_release:
	free(pbuf);
	free(pallbuf);
	return ret;
#undef BUF_SIZE
#undef TOTAL_SIZE
}

int32
_do_file_write(tfile_handle_t hfile, int size)
{
 	ubyte * pbuf;
	int32 ret;
	int32 total_size;
	int32 i;

	pbuf = (ubyte *)malloc(size + 1);
	for (i = 0; i < size; i++) {
		if (i % 24 == 0) {
			pbuf[i] = '\n';
			continue;
		}
		pbuf[i] = 'a' + i % 24;
	}
	pbuf[size - 1] = '\n';
	total_size = 0;

	while (1) {
		if ((ret = TFFS_fwrite(hfile, size, pbuf)) < 0) {
			if (ret == ERR_TFFS_FILE_EOF) {
				printf("%s: got the end of the file.\n", __FUNCTION__);
				break;
			}
			else {
				printf("%s: TFFS_write failed. ret = %d\n", __FUNCTION__, ret);
				goto _release;
			}
		}
		//printf("### write content[%d bytes]:\n", ret);

		total_size += ret;
		if (total_size >= size)
			break;
	}
	printf("### write content[%d bytes]:\n", total_size);

_release:
	free(pbuf);
	return ret;
}

int
create_file(tffs_handle_t htffs, char * fname, char * mode, int size)
{
	int32 ret;
	tfile_handle_t hfile;

	if ((ret = TFFS_fopen(htffs, fname, mode, &hfile)) != TFFS_OK)
		return ret;

	if ((ret = _do_file_write(hfile, size)) <= 0)
		return ret;

	if ((ret = TFFS_fclose(hfile)) != TFFS_OK)
		return ret;

	return TFFS_OK;
}

int
read_file(tffs_handle_t htffs, char * fname)
{
	int32 ret;
	tfile_handle_t hfile;

	if ((ret = TFFS_fopen(htffs, fname, "r", &hfile)) != TFFS_OK)
		return ret;

	printf("Read from file [%s]\n", fname);

	if ((ret = _do_file_read(hfile)) <= 0)
		return ret;

	if ((ret = TFFS_fclose(hfile)) != TFFS_OK)
		return ret;

	return TFFS_OK;
}

/*--------------------------------------------------------------------------------------------*/

int
TFFS_FAT_tester(char * imgname)
{
#include "pubstruct.h"

	tffs_handle_t htffs;
	int ret;

	if ((ret = TFFS_mount(imgname, &htffs)) != TFFS_OK) {
		printf("TFFS_mount failed. ret = %d\n", ret);
		return -1;
	}

	dump_fat(htffs);

	tffs_t * ptffs = (tffs_t *)htffs;
	tfat_t * pfat = ptffs->pfat;

	if ((ret = malloc_fat_entry(pfat, 1)) != FAT_OK) {
		printf("malloc_fat_entry failed. ret = %d\n", ret);
		return -1;
	}

	if ((ret = malloc_fat_entry(pfat, 10)) != FAT_OK) {
		printf("malloc_fat_entry failed. ret = %d\n", ret);
		return -1;
	}

	if ((ret = malloc_fat_entry(pfat, 100)) != FAT_OK) {
		printf("malloc_fat_entry failed. ret = %d\n", ret);
		return -1;
	}

	dump_fat(htffs);

	return 0;
}

void
dump_fat(tffs_handle_t htffs)
{
#define FAT_SECTOR_NUM		1

	tffs_t * ptffs = (tffs_t *)htffs;
	ubyte * psector;
	int sec_i;

	psector = (ubyte *)malloc(ptffs->pbs->byts_per_sec);

	for (sec_i = 0; sec_i < FAT_SECTOR_NUM; sec_i++) {
		HAI_readsector(ptffs->hdev, ptffs->sec_fat + sec_i, psector);
		printf("-----------------fat sector %02d-------------------------\n", sec_i);
		print_sector(psector, 0);
		printf("-------------------------------------------------------\n");
	}

	free(psector);
}

int
malloc_fat_entry(tfat_t * pfat, int32 clus_num)
{
	int i;
	int32 ret;
	uint32 * pclus_array;
	
	ASSERT(clus_num >= 1);

	pclus_array = (uint32 *)malloc(sizeof(uint32) * clus_num);

	if ((ret = fat_malloc_clus(pfat, FAT_INVALID_CLUS, &(pclus_array[0]))) != FAT_OK) {
		printf("fat_malloc_clus failed. ret = %d\n", ret);
		return ret;
	}

	for (i = 1; i < clus_num; i++) {
		if ((ret = fat_malloc_clus(pfat, pclus_array[i - 1], &(pclus_array[i]))) != FAT_OK) {
			printf("fat_malloc_clus failed. ret = %d\n", ret);
			return ret;
		}
	}

	printf("## allocated %d fat entries : \n", clus_num);
	for (i = 0; i < clus_num; i++) {
		printf("##     [0x%08x]->\n", pclus_array[i]);
	}

	free(pclus_array);
	return ret;
}

/*--------------------------------------------------------------------------------------------*/

int
HAI_tester(char * imgname)
{
#define SECTOR_SIZE		512

	tdev_handle_t hdev;
	ubyte * secbuf;
	tdev_info_t * devinfo;
	int32 ret;
	
	if ((hdev = HAI_initdevice(imgname, SECTOR_SIZE)) == NULL) {
		printf("HAI_initdevice called error.\n");
		return -1;
	}

	secbuf = (ubyte *)malloc(SECTOR_SIZE);
	if ((ret = HAI_readsector(hdev, 0, secbuf)) != HAI_OK) {
		printf("HAI_readsector() called error. ret = %d\n", ret);
		return -1;
	} 
	print_sector(secbuf, 0);
	free(secbuf);

	devinfo = (tdev_info_t *)malloc(sizeof(tdev_info_t));
	if ((ret = HAI_getdevinfo(hdev, devinfo)) != HAI_OK) {
		printf("HAI_getdevinfo() called error. ret = %d\n", ret);
		free(devinfo);
		return -1;
	}
	printf("devinfo->free_sectors = %d\n", devinfo->free_sectors);
	free(devinfo);
	
	if ((ret = HAI_closedevice(hdev)) != HAI_OK) {
		printf("HAI_closedevice() called error. ret = %d\n", ret);
		free(secbuf);
		return -1;
	}
	return 0;
}

