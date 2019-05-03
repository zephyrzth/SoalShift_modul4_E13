#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pthread.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

static const char *dirpath = "/home/trasv/shift4";
int key = 0;
char chrlist[100]="qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
pthread_t tid;

char decrypt(char *x){
	for(int i=0;x[i]!= '\0';++i){
		if(x[i]=='/')
			continue;

		for(int j=0;j<94;++j){	
			if(x[i]==chrlist[j]){			
				if(j>+key){
					x[i]=chrlist[j-key];	
					j=94;
				}
				else if(j==key){
					x[i]=chrlist[j-key];	
					j=94;
				}
				else{
					int a = 94-(key-j);
					x[i]=chrlist[a];
					j=94;
					
				}
				
			}
			
		}		
		
	}	
	return *x;
}

char encrypt(char *y){
	for(int i=0;y[i]!= '\0';++i){
		if(y[i]=='/')
			continue;

		for(int j=0;j<94;++j)
		{
			if(y[i]==chrlist[j])
			{	
				if((94-j)>=key)
				{
					y[i]=chrlist[j+key];	
					j=94;
				}
				else if(j==key){
					y[i]=chrlist[j+key];	
					j=94;
				}
				else 
				{	
					int a = key-(94-j);
					y[i]=chrlist[a];
					j=94;	
				}
				
			}
			
		}		
		
	}
	return *y;
}

int getLastCharPos(char *str, char chr){
	char *posChar = NULL;
	char *tempPosChar = strchr(str, chr);

 	while(tempPosChar != NULL){
		posChar = tempPosChar;

 		tempPosChar = strchr(tempPosChar+1, chr);
	}
	if(posChar==NULL)
		return 0;

 	return (int) (posChar-str);
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  	int res;
  	
	char fpath[1000];
	char temp[1000];
	sprintf(temp,"%s",path);
	encrypt(temp);
	sprintf(fpath,"%s%s",dirpath,temp);
	
	res = lstat(fpath, stbuf);
	
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
  char temp[1000];
  	strcpy(temp,path);
  	
  	encrypt(temp);
	sprintf(fpath, "%s%s",dirpath,temp);
	
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		
		memset(temp,'\0',sizeof(temp));
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		
		char usrtarget1[20], grptarget[20], usrtarget2[20];
		char pathskrg[1000];
		gid_t gid;
		uid_t uid;

		strcpy(usrtarget1,"chipset");
		strcpy(usrtarget2,"ic_controller");
		strcpy(grptarget,"rusak");
		strcpy(pathskrg,fpath);

		strcpy(temp,de->d_name);
		strcpy(pathskrg+strlen(pathskrg),temp);
		stat(pathskrg,&st);

		gid = st.st_gid;
		uid = st.st_uid;

		struct group *grp;
		grp = getgrgid(gid);

		char grpname[20];
		strcpy(grpname, grp->gr_name);

		struct passwd *usr;
		usr = getpwuid(uid);

		char usrname[20];
		strcpy(usrname, usr->pw_name);

		if( ((strcmp(usrname,usrtarget1) == 0) || (strcmp(usrname,usrtarget2) == 0)) && (strcmp(grpname,grptarget) == 0) )
		{
			FILE *cek, *target;
			cek = fopen(pathskrg,"r");
			if(cek==NULL){
				target = fopen("/home/trasv/shift4/filemiris.txt","w");
				if(target==NULL){
				struct tm mod = *localtime(&(st.st_atime));
				char isiFIle[1100];
				sprintf(isiFIle, "%s %d %d %d:%d:%d %d-%d-%d", temp,gid,uid,mod.tm_hour,mod.tm_min,mod.tm_sec,mod.tm_mday,mod.tm_mon,mod.tm_year+1900);
				fputs(isiFIle,target);
				fclose(target);
				printf("ini isinya cuk : %s\n\n\n", isiFIle);
				remove(pathskrg);
				}
			}
			fclose(cek);
		}

		if((strcmp(temp,".")!=0) && (strcmp(temp,"..")!=0)) {
			decrypt(temp);
			int lastDotChar = getLastCharPos(temp, '.');
			if (((temp[lastDotChar-3]=='m' && temp[lastDotChar-2]=='k' && temp[lastDotChar-1]=='v') || 
			(temp[lastDotChar-3]=='m' && temp[lastDotChar-2]=='p' && temp[lastDotChar-1]=='4'))){
				continue;	
			}
			res = (filler(buf, temp, &st, 0));
			if(res!=0) break;
		}
	}
	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	
  	char fpath[1000];
  	char temp[1000];
	
	strcpy(temp,path);
	
	encrypt(temp);
	
	if(strcmp(temp,"/") == 0){
		sprintf(fpath,"%s",dirpath);
	}
	else{
		sprintf(fpath, "%s%s",dirpath,temp);
	}
	
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


static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000], temp[1000],temp2[1000];
	
	strcpy(temp, path);	
	encrypt(temp);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,temp);

	int fd;
	int res;

	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	
	strcpy(temp2,path);
	encrypt(temp2);
	sprintf(temp, "%s/%s", dirpath,temp2);

	if(access(temp, R_OK)<0)				//JIKA FILE TIDAK ADA
		return res;


	char pathBackup[1000] = "/home/trasv/shift4/Backup";
	mkdir(pathBackup, 0777);

	char filenameNoExt[1000], ext[100], timestamp[1000], fileBackup[1000];
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(timestamp, "%04d-%02d-%02d_%02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	int slash = getLastCharPos(path, '/');
	int dot = getLastCharPos(path, '.');
	
	if (dot==0) {
		dot = strlen(path);
		ext[0] = '\0';
	}
	else{
		strcpy(ext, path+dot);
	}

	strncpy(filenameNoExt, path+slash+1, dot-(slash+1));
	filenameNoExt[dot-(slash+1)] = '\0';
	
	sprintf(fileBackup,"%s_%s%s", filenameNoExt, timestamp, ext);
	encrypt(fileBackup);
	
	sprintf(temp, "%s%s", dirpath, temp2);
	
	FILE *source = fopen(temp, "r");

	sprintf(temp, "%s/%s", pathBackup, fileBackup);
	
	FILE *target = fopen(temp, "w");

	char ch;
	while ((ch = fgetc(source)) != EOF)
		fprintf(target, "%c", ch);

	fclose(target);
	fclose(source);
	return res;
}

static int xmp_unlink(const char *path)
{
	char fpath[1000], temp[1000];
	strcpy(temp, path);
	encrypt(temp);

	if(strcmp(temp,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,temp);

	int res, isFile, status;

	isFile = access(fpath, F_OK);

	if(isFile<0)				//JIKA BUKAN FILE
		return 0;

	char backup[100] = "Backup";
	char command[1000], timestamp[100], FileZip[1000], namaFile[1000], ext[1000],
			filenameNoExt[1000], namaRecycleBin[100]="RecycleBin";

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(timestamp, "%04d-%02d-%02d_%02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	int slash = getLastCharPos(path, '/');
	int dot = getLastCharPos(path, '.');
	
	if (dot==0)
		dot = strlen(path);
	else{
		strcpy(ext, path+dot);
	}
	//char c[100];
	//strcpy(c,'/');
	//strcpy(namaFile, c);

	strcpy(namaFile, path);
	printf("------------------%s\n",namaFile);
	/*int i,len=strlen(namaFile);
	for(i=1;i<len-1;i++)
	{
		namaFile[i-1]=namaFile[i];
	}
	namaFile[len-1]='\0';*/
	char* namaFile1=namaFile+1;
	strcpy(namaFile,namaFile1);
	strncpy(filenameNoExt, path+slash+1, dot-(slash+1));
	filenameNoExt[dot-(slash+1)] = '\0';

	sprintf(FileZip, "%s_deleted_%s", filenameNoExt, timestamp);
	encrypt(FileZip);
	encrypt(namaFile);
	encrypt(filenameNoExt);
	printf("------------------%s\n",namaFile);
	sprintf(command, "cd %s && mkdir -p '%s' && zip '%s/%s' '%s' '%s/%s'* && rm -f '%s/%s'*", dirpath, namaRecycleBin,namaRecycleBin, FileZip, namaFile, backup, filenameNoExt, backup, filenameNoExt);

	if (fork()==0)
		execlp("bash","bash", "-c", command, NULL);

	while((wait(&status))>0);

	res = unlink(fpath);
	
	if (res == -1)
		return -errno;

	return 0;
}


static int xmp_truncate(const char *path, off_t size)
{
	char fpath[1000], temp[1000];
	strcpy(temp, path);
	encrypt(temp);

	if(strcmp(temp,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,temp);
	
	int res;

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	char fpath[1000], temp[1000];
	strcpy(temp, path);
	encrypt(temp);

	if(strcmp(temp,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,temp);

	int res;

	res = utimensat(0, fpath, ts, AT_SYMLINK_NOFOLLOW);
	
	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.write 		= xmp_write,
	.utimens	= xmp_utimens,
	.truncate 	= xmp_truncate,
	.unlink 	= xmp_unlink,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
