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
