# SoalShift_modul4_E13
Soal Shift Modul 4 Praktikum Sistem Operasi

1. Semua nama file dan folder harus terenkripsi. Enkripsi yang Atta inginkan sangat sederhana, yaitu Caesar cipher. Namun, Kusuma mengatakan enkripsi tersebut sangat mudah dipecahkan. Dia menyarankan untuk character list diekspansi,tidak hanya alfabet, dan diacak. Berikut character list yang dipakai:

   qE1~ YMUR2\"\`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0

   Misalkan ada file bernama “halo” di dalam folder “INI_FOLDER”, dan key yang dipakai adalah 17, maka:
“INI_FOLDER/halo” saat belum di-mount maka akan bernama “n,nsbZ]wio/QBE#”, saat telah di-mount maka akan otomatis terdekripsi kembali menjadi “INI_FOLDER/halo”.
Perhatian: Karakter ‘/’ adalah karakter ilegal dalam penamaan file atau folder dalam *NIX, maka dari itu dapat diabaikan

   Jawab:
   
   Pertama, akan disiapkan file systemnya sendiri yang harus menjalankan 3 fungsi dasar yang harus diimplementasikan, yaitu: `getattr`, `readdir`, dan `read`. Bedanya di ketiga fungsi ini diterapkan enkripsi dan dekripsi menggunakan `Caesar Cipher`. Jadi hal pertama yang dilakukan adalah membuat fungsi untuk enkripsi dan dekripsi seperti berikut.
   ```
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
   ```
   Setelah fungsi siap berikutnya tinggal memodifikasi tiga fungsi tersebut, sehingga file/folder apapun yang akan ditampilkan pertama akan dienkripsi terlebih dahulu, baru kemudian didekripsi di fungsi `readdir` agar nanti tetap menampilkan hasil sesudah dekripsi pada folder yang di-mount. Seperti berikut.
   ```
    static int xmp_getattr(const char *path, struct stat *stbuf)
    {
      int res;
      char fpath[1000];
      sprintf(fpath,"%s%s",dirpath,path);

      char judul[1000];
      sprintf(judul, "%s", path);
      enkrip(judul);
      char judulLengkap[1000];
      memset(judulLengkap, '\0', 1000);
      strcpy(judulLengkap, dirpath);
      //strcat(judulLengkap, "/");
      strcat(judulLengkap, judul);
      res = lstat(judulLengkap, stbuf);

      int flagread;
            char user[100], group[100], waktu[100];
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
            //printf("ini file, nama sama grup : %s %s %s\n",path,user,group);
            //printf("ini waktu %s\n",waktu);

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
          remove(path);

              }
      }

      if (res == -1)
        return -errno;

      return 0;
    }

    static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
    {
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
   ```
   2. Semua file video yang tersimpan secara terpecah-pecah (splitted) harus secara otomatis tergabung (joined) dan diletakkan dalam folder “Videos”
Urutan operasi dari kebutuhan ini adalah:
Tepat saat sebelum file system di-mount
Secara otomatis folder “Videos” terbuat di root directory file system
Misal ada sekumpulan file pecahan video bernama “computer.mkv.000”, “computer.mkv.001”, “computer.mkv.002”, dst. Maka secara otomatis file pecahan tersebut akan di-join menjadi file video “computer.mkv”
Untuk mempermudah kalian, dipastikan hanya video file saja yang terpecah menjadi beberapa file. File pecahan tersebut dijamin terletak di root folder fuse
Karena mungkin file video sangat banyak sehingga mungkin saja saat menggabungkan file video, file system akan membutuhkan waktu yang lama untuk sukses ter-mount. Maka pastikan saat akan menggabungkan file pecahan video, file system akan membuat 1 thread/proses(fork) baru yang dikhususkan untuk menggabungkan file video tersebut
Pindahkan seluruh file video yang sudah ter-join ke dalam folder “Videos”
Jangan tampilkan file pecahan di direktori manapun
Tepat saat file system akan di-unmount
Hapus semua file video yang berada di folder “Videos”, tapi jangan hapus file pecahan yang terdapat di root directory file system
Hapus folder “Videos”

   3. Sebelum diterapkannya file system ini, Atta pernah diserang oleh hacker LAPTOP_RUSAK yang menanamkan user bernama “chipset” dan “ic_controller” serta group “rusak” yang tidak bisa dihapus. Karena paranoid, Atta menerapkan aturan pada file system ini untuk menghapus “file bahaya” yang memiliki spesifikasi:
Owner Name     : ‘chipset’ atau ‘ic_controller’
Group Name    : ‘rusak’
Tidak dapat dibaca
Jika ditemukan file dengan spesifikasi tersebut ketika membuka direktori, Atta akan menyimpan nama file, group ID, owner ID, dan waktu terakhir diakses dalam file “filemiris.txt” (format waktu bebas, namun harus memiliki jam menit detik dan tanggal) lalu menghapus “file bahaya” tersebut untuk mencegah serangan lanjutan dari LAPTOP_RUSAK.

   Jawab :
   
   Pada soal ini kita memodifikasi fungsi readdir, dimana disana kita mengambil owner, group owner dan apakah file itu bisa dibuka atau tidak dari setiap file dalam direktori ini. lalu jika ditemukan demikian maka akan disimpan dalam file bernama filemiris.txt berupa nama file, owner, group dan waktu terakhir. lalu terakhir dihapus.
   
   ```
   
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
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
   ```


   4. Pada folder YOUTUBER, setiap membuat folder permission foldernya akan otomatis menjadi 750. Juga ketika membuat file permissionnya akan otomatis menjadi 640 dan ekstensi filenya akan bertambah “.iz1”. File berekstensi “.iz1” tidak bisa diubah permissionnya dan memunculkan error bertuliskan “File ekstensi iz1 tidak boleh diubah permissionnya.”
   
   Jawab:
   
   Di sini, artinya kita harus menambahkan fungsi `mkdir` untuk menangani pembuatan folder pada FUSE dan `mknod` untuk menangani pembuatan file pada FUSE. Jadi pertama akan didefinisikan dulu fungsi untuk `mkdir` dan `mknod`, lalu diberi kondisi jika judul/direktori adalah kata "YOUTUBER" (tentunya kata tersebut nanti sudah dienkripsi ketika diproses), maka akan mengubah permission khusus pada saat membuat folder atau membuat file. Seperti berikut.


```   
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
   ```
   Tidak lupa pada `struct fuse_operations` juga didefinisikan atribut yang menjadi pointer ke fungsi `mkdir` dan `mknod`, seperti berikut.
   ```
   static struct fuse_operations xmp_oper = {
	 .getattr	= xmp_getattr,
	 .readdir	= xmp_readdir,
	 .read		= xmp_read,
	 .mkdir = xmp_mkdir,
   .chmod = xmp_chmod,
	 .mknod = xmp_mknod
   };
   ```
   
   
   5. Ketika mengedit suatu file dan melakukan save, maka akan terbuat folder baru bernama Backup kemudian hasil dari save tersebut akan disimpan pada backup dengan nama namafile_[timestamp].ekstensi. Dan ketika file asli dihapus, maka akan dibuat folder bernama RecycleBin, kemudian file yang dihapus beserta semua backup dari file yang dihapus tersebut (jika ada) di zip dengan nama namafile_deleted_[timestamp].zip dan ditaruh ke dalam folder RecycleBin (file asli dan backup terhapus). Dengan format [timestamp] adalah yyyy-MM-dd_HH:mm:ss
   
   Jawab :
   
   Pada soal ini kita memodifikasi 2 fungsi. yang pertama yaitu write, di write kita menambahkan sintaks untuk mengambil nama file, dan ekstensi yang telah diedit, juga mengambil waktu terakhir dimodifikasi dan membuat file baru di folder backup dengan format seperti di soal.
   
   
   ```
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

```
   
   Kita harus memodifikasi fungsi yang kedua yaitu pada fungsi unlink. dimana disana kita menambahkan sintaks untuk mengambil nama file dan ekstensi, lalu kita mengambil timestamp terakhir lalu kita zip semua file asli, dan backup, dan zip itu dimasukkan ke folder recycle bin, lalu file aslinya kita hapus.
   
   ```
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

   ```
   

lalu terakhir untuk mempermudah pengerjaan, dibuat fungsi untuk mengambil kejadian pertama dari sebuah character dari string, kalau strchr dari depan, fungsi ini mengambil dari belakang.


```
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
```
