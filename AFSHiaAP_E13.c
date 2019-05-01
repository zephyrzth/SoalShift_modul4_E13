#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/anargya/shift4";

static const char *kode = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";

void enkrip(char judul[])
{
	for(int i = 0; i < strlen(judul); i++){
		char temp = judul[i];
		for(int j = 0; j < strlen(kode); j++){
			if(kode[j] == temp){
				int posisi = (j + 17)%94;
				judul[i] = kode[posisi];
			}
		}
	}
}

void dekrip(char judul[])
{
	for(int i = 0; i < strlen(judul); i++){
		char temp = judul[i];
		for(int j = 0; j < strlen(kode); j++){
			if(kode[j] == temp){
				int posisi = j - 17;
				while(posisi < 0){
					posisi += 93;
				}
				judul[i] = kode[posisi];
			}		
		}	
	}
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	printf("GET ATTR\n");
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	
	char judul[1000];
	sprintf(judul, "%s", path);
	enkrip(judul);
	printf("Judul enkrip: %s\n", judul);
	char judulLengkap[1000];
	memset(judulLengkap, '\0', 1000);
	strcpy(judulLengkap, dirpath);
	//strcat(judulLengkap, "/");
	strcat(judulLengkap, judul);
	printf("Judul lengkap: %s\n", judulLengkap);
	res = lstat(judulLengkap, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
				off_t offset, struct fuse_file_info *fi)
{
	printf("Readdir\n");
	char fpath[1000];
	char judul[1000];
	char judul2[1000];
	char judulLengkap[1000];
	
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path);
		sprintf(judul, "%s", path);
		enkrip(judul);
		memset(judulLengkap, '\0', 1000);
		strcpy(judulLengkap, dirpath);
		//strcat(judulLengkap, "/");
		strcat(judulLengkap, judul);
		printf("Judul Lengkap readdir: %s\n", judulLengkap);
	}
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(judulLengkap);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		
		memset(judul2, '\0', 1000);
		sprintf(judul2, "%s", de->d_name);
		//strcpy(judul2, de->d_name);
		dekrip(judul2);
		
		res = (filler(buf, judul2, &st, 0));
		if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	printf("Read\n");
	char fpath[1000];
	char judul[1000];
	char judulLengkap[1000];
	
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,path);
		sprintf(judul, "%s", path);
		//strcpy(judul, path);
		enkrip(judul);
		memset(judulLengkap, '\0', 1000);
		strcpy(judulLengkap, dirpath);
		//strcat(judulLengkap, "/");
		strcat(judulLengkap, judul);
		printf("Judul lengkap read: %s\n", judulLengkap);
	}
	int res = 0;
	int fd = 0 ;

	(void) fi;
	fd = open(judulLengkap, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;
	char judul[1000];
	char judulLengkap[1000];
	sprintf(judul, "%s", path);
	enkrip(judul);
	sprintf(judulLengkap, "%s", dirpath);
	strcat(judulLengkap, judul);
	
	if(strstr(judul, "@ZA>AXio")){
		mode = 0640;
		strcat(judulLengkap, "`[S%");
	}
	
	if (S_ISREG(mode)) {
		res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(path, mode);
	else
		res = mknod(path, mode, rdev);
	if (res == -1)
		return -errno;
		
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char judul[1000];
	char judulLengkap[1000];
	sprintf(judul, "%s", path);
	enkrip(judul);
	sprintf(judulLengkap, "%s", dirpath);
	strcat(judulLengkap, judul);
	
	if(strstr(judul, "@ZA>AXio"))
		mode = 0750;
	
	res = mkdir(judulLengkap, mode);
	
	if(res == 1)
		return -errno;
	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.mkdir = xmp_mkdir,
	.mknod = xmp_mknod
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
