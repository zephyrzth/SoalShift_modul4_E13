#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

static const char *dirpath = "/home/trasv/shift4";
int no3=0;

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000], waktu[100];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	int flagread;
        char user[100], group[100];
        sprintf(fpath,"%s%s",dirpath,path);
        struct passwd *pw = getpwuid(stbuf->st_uid);
        struct group  *gr = getgrgid(stbuf->st_gid);
        if(stbuf->st_mode & S_IRUSR && stbuf->st_mode & S_IRGRP)
                flagread=1;
        else
                flagread=0;
        strcpy(user,pw->pw_name);
        strcpy(group,gr->gr_name);
        strftime(waktu, 50, "%Y-%m-%d %H:%M:%S", localtime(&stbuf->st_mtime));
        printf("ini file, nama sama grup : %s %s %s\n",path,user,group);
        printf("ini waktu %s\n",waktu);

        if(strcmp(user,"chipset")==0 || strcmp(user,"ic_controller")==0 /*&& flagread==0*/)
	{
	if(strcmp(group,"trasv")==0)
  		{
                	FILE *file;
                	char isian[1000], tempat[100];
                	memset(isian, 0, sizeof(isian));
                	sprintf(isian, "%s %s %s %s\n", path, user, group, waktu);
			sprintf(tempat, "%s/filemiris.txt",dirpath);
                	file = fopen(tempat, "a");
                	fprintf(file, "%s", isian);
			fclose(file);

                	printf("jancok %s\n",path);
			remove(path);

        	}
	}

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_access(const char *path, int mask)
{
        int res;

        res = access(path, mask);
        if (res == -1)
                return -errno;

        return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));

		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;
	int fd = 0 ;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	res = mkdir(path, mode);
	//char a[] = "/home/trasv/shift4/YOUTUBE/";
	//strcat(a,res);
	//chmod(res,
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi)
{
        int fd;
        int res;

        (void) fi;
        fd = open(path, O_WRONLY);
        if (fd == -1)
                return -errno;

        res = pwrite(fd, buf, size, offset);
        if (res == -1)
                res = -errno;

        close(fd);
        return res;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) {

    (void) fi;

    int res;
    res = creat(path, mode);
    if(res == -1)
        return -errno;

    close(res);

    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
        int res;

        res = open(path, fi->flags);
        if (res == -1)
                return -errno;

        close(res);
        return 0;
}


static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.mkdir		= xmp_mkdir,
	.access		= xmp_access,
	.write		= xmp_write,
	.create		= xmp_create,
	.open		= xmp_open,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
