# Shift 4

## 1

### 1a, b, c, e

> Jika sebuah direktori dibuat dengan awalan “AtoZ\_”, maka direktori tersebut akan menjadi direktori ter-encode.

> Jika sebuah direktori di-rename dengan awalan “AtoZ\_”, maka direktori tersebut akan menjadi direktori ter-encode.

> Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.

> Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)

Pada soal ini, yang harus dilakukan adalah dengan membuat fungsi encode terlebih dahulu, karena enkripsi atbash adalah enkripsi yang bisa berlaku 2 arah untuk encode dan decode, maka kita hanya perlu menggunakan 1 fungsi saja.

```c
void atbash(char str[1000], char newStr[1000]) {
  // Skip if directory
  if (!strcmp(str, ".") || !strcmp(str, "..")) {
    strcpy(newStr, str);
    return;
  };

  int i, flag = 0;
  i = 0;
  while (str[i] != '\0') {
    // exclude extension
    if (str[i] == '.') {
      flag = 1;
    }
    if (flag == 1) {
      newStr[i] = str[i];
      i++;
      continue;
    }

    if (!((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97) ||
          (str[i] > 122 && str[i] <= 127))) {
      if (str[i] >= 'A' && str[i] <= 'Z') {
        newStr[i] = 'Z' + 'A' - str[i];
      }
      if (str[i] >= 'a' && str[i] <= 'z') {
        newStr[i] = 'z' + 'a' - str[i];
      }
    }

    if (((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97) ||
         (str[i] > 122 && str[i] <= 127))) {
      newStr[i] = str[i];
    }

    i++;
  }
  newStr[i] = '\0';
}
```

Kemudian, dari fungsi di atas yang bertugas untuk memirror string, maka kita harus menentukan apakah folder tersebut harus kita encode dengan menggunakan `strncmp(token, "AtoZ_", 5) == 0`

Pengecekan di atas dilakukan pada fungsi `xmp_readdir` sehingga kita bisa mengubah nama yang ditampilkan pada file system fuse.

```c
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi) {
  char fpath[1000];
  printf("\n🚀 READDIR\n");

  int hasToEncrypt = 0;
  char currPath[1000] = "", pathEnc[1000] = "";

  char *token = strtok(path, "/");
  while (token != NULL) {
    if (hasToEncrypt) {
      strcat(pathEnc, "/");
      strcat(pathEnc, token);
    } else if (!hasToEncrypt) {
      strcat(currPath, "/");
      strcat(currPath, token);
    }

    // TODO Check if prefixed with atoz_
    if (strncmp(token, "AtoZ_", 5) == 0) {
      hasToEncrypt = 1;
    } else if (strncmp(token, "RX_", 3) == 0) {
      hasToEncrypt = 2;
    }
    // printf("token: %s\n", token);
    token = strtok(NULL, "/");
  }

  if (strcmp(path, "/") == 0) {
    path = dirpath;
    sprintf(fpath, "%s", path);
  } else {
    char pathDec[1000], pathDec2[1000];
    if (hasToEncrypt == 1) {
      atbash(pathEnc, pathDec);
      sprintf(fpath, "%s%s%s", dirpath, path, pathDec);
    } else if (hasToEncrypt == 2) {
      rot13Denc(pathEnc, pathDec2);
      atbash(pathDec2, pathDec);
      sprintf(fpath, "%s%s%s", dirpath, path, pathDec);
    } else {
      sprintf(fpath, "%s%s", dirpath, path);
    }
  }

  int res = 0;

  DIR *dp;
  struct dirent *de;
  (void)offset;
  (void)fi;
  printf("temp send: %s\n", fpath);

  dp = opendir(fpath);

  if (dp == NULL) return -errno;

  while ((de = readdir(dp)) != NULL) {
    struct stat st;

    memset(&st, 0, sizeof(st));

    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;

    // Create variable for new name
    char temp[1000], temp2[1000];

    if (hasToEncrypt == 1) {
      atbash(de->d_name, temp);
    } else if (hasToEncrypt == 2) {
      atbash(de->d_name, temp2);
      rot13Enc(temp2, temp);
    } else {
      strcpy(temp, de->d_name);
    }

    printf("temp send: %s\n", temp);

    res = (filler(buf, temp, &st, 0));

    if (res != 0) break;
  }

  closedir(dp);

  loglvlInfo("READDIR", path);

  return 0;
}
```

Pada fungsi ini kamu melakukan pengecekan dan melakukan pengecekan itu untuk sub directory" lainnya. Karena dibutuhkan untuk bisa bekerja secara rekursif.

Setelah kita mengetahui bahwa folder tersebut harus di encode, maka kita menyimpan `pathEnc` dan memberikan flag untuk menandakan.

Kemudian setelah itu kita membuka directory tersebut, dan mengencode `de->d_name` dengan algoritma atbash

```c
 if (strcmp(path, "/") == 0) {
    path = dirpath;
    sprintf(fpath, "%s", path);
  } else {
    char pathDec[1000], pathDec2[1000];
    if (hasToEncrypt == 1) {
      atbash(pathEnc, pathDec);
      sprintf(fpath, "%s%s%s", dirpath, path, pathDec);
    }
    // ...
    else {
      strcpy(temp, de->d_name);
    }
    // ...
```

Pada percabangan ini, jika path merupakan `/` maka bisa diskip, dan jika path memiliki awalan AtoZ, maka kita harus decode terlebih dahulu menggunakan fungsi atbash, hal ini karena setelah ini kita akan memasukkan path tersebut ke opendir, maka dibutuhkan path yang sebenarnya sehingga file bisa membaca directorynya.

Selain penambahan codingan pada `xmp_readdir`, dilakukan juga penyesuaian untuk mendecode pada saat melakukan `xmp_getattr` dan `xmp_read` karena pada saat fuse ingin menjalankan command `ls -l` atau `cat`, kita membutuhkan nama file yang asli sehingga bisa dicari dan ditampilkan. Maka ada code untuk mendecode namanya kembali.

```c
char *token = strtok(temp, "/");
  while (token != NULL) {
    if (hasToEncrypt) {
      strcat(pathEnc, "/");
      strcat(pathEnc, token);
    } else if (!hasToEncrypt) {
      strcat(currPath, "/");
      strcat(currPath, token);
    }

    // TODO Check if prefixed with atoz_
    if (strncmp(token, "AtoZ_", 5) == 0) {
      hasToEncrypt = 1;
    } else if (strncmp(token, "RX_", 3) == 0) {
      hasToEncrypt = 2;
    }

    token = strtok(NULL, "/");
  }

  // TODO Send decrypted path, jadi ga bingung si attrnya
  char fpathToSend[2000], decrypted[1000], decrypted2[1000];
  if (hasToEncrypt == 1) {
    atbash(pathEnc, decrypted);
    sprintf(fpathToSend, "%s%s%s", dirpath, currPath, decrypted);
  } else if (hasToEncrypt == 2) {
    rot13Denc(pathEnc, decrypted);
    atbash(decrypted, decrypted2);
    sprintf(fpathToSend, "%s%s%s", dirpath, currPath, decrypted2);
  } else {
    sprintf(fpathToSend, "%s%s%s", dirpath, currPath, pathEnc);
  }

  printf("fpathToSend: %s\n", fpathToSend);
```

Logika pada get_attr, dan read kurang lebih sama, yaitu kita kembali mengecek apabila memiliki awalan AtoZ secara rekursif, kemudian mendecode nama folder yang akan dikirimkan.

#### Screenshot

![](https://i.imgur.com/pTyyh1Z.png)
Folder dengan nama yang terencode ketika di fuse, berlaku juga rekursif

![](https://i.imgur.com/UXI71Ph.png)
![](https://i.imgur.com/chCyLkp.png)
`ls -l` dan `cat` berjalan

![](https://i.imgur.com/pOW8pJN.png)
file terename akan berubah ke secara otomatis

![](https://i.imgur.com/6iylirm.png)
file yang baru saja dibuat juga akan mengencode secara otomatis.

## 1d

> Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ\_[Nama Direktori]

Pencatatan dilakukan pada `xmp_mkdir` dan `xmp_rename`
yaitu dengan fungsi

```c
loglvlInfo("MKDIR", path);

loglvlInfo2("RENAME", fpathFrom, fpathTo);
```

Fungsi log akan dijelaskan lebih lanjut pada nomor 4

![](https://i.imgur.com/G5olN4e.png)
Pencatatan mkdir
![](https://i.imgur.com/XYzmimh.png)
Pencatatan rename

# Soal 2
Pada soal ini kita memiliki beberapa macam fungsi encode
## Soal 2 a,b,c
### a
Jika sebuah direktori dibuat dengan awalan `RX_[Nama]`, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai kasus nomor 1 dengan algoritma tambahan ROT13 (Atbash + ROT13). Fungsi encode dari ROT13 berbeda dengan fungsi deencode sehingga harus membuat fungsi yang terpisah. Oleh karena itu yang harus pertama dilakukan adalah membuat fungsi encode dan deencode dari ROT13

```c
void rot13Enc(char str[1000], char newStr[1000]) {
  // Skip if directory
  if (!strcmp(str, ".") || !strcmp(str, "..")) {
    strcpy(newStr, str);
    return;
  };

  int i, flag = 0;
  i = 0;
  while (str[i] != '\0') {
    // exclude extension
    if (str[i] == '.') {
      flag = 1;
    }
    if (flag == 1) {
      newStr[i] = str[i];
      i++;
      continue;
    }

    if (!((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97))) {
      if (str[i] >= 'A' && str[i] <= 'Z') {
        if (str[i] + 13 > 'Z') {
          newStr[i] = str[i] - 13;
        } else {
          newStr[i] = str[i] + 13;
        }
      }
      if (str[i] >= 'a' && str[i] <= 'z') {
        if (str[i] + 13 > 'z') {
          newStr[i] = str[i] - 13;
        } else {
          newStr[i] = str[i] + 13;
        }
      }
    }

    if ((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97)) {
      newStr[i] = str[i];
    }

    i++;
  }
  newStr[i] = '\0';
}

void rot13Denc(char str[1000], char newStr[1000]) {
  // Skip if directory
  if (!strcmp(str, ".") || !strcmp(str, "..")) {
    strcpy(newStr, str);
    return;
  };

  int i, flag = 0;
  i = 0;
  while (str[i] != '\0') {
    // exclude extension
    if (str[i] == '.') {
      flag = 1;
    }
    if (flag == 1) {
      newStr[i] = str[i];
      i++;
      continue;
    }

    if (!((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97))) {
      if (str[i] >= 'A' && str[i] <= 'Z') {
        if (str[i] - 13 < 'A') {
          newStr[i] = str[i] + 13;
        } else {
          newStr[i] = str[i] - 13;
        }
      }
      if (str[i] >= 'a' && str[i] <= 'z') {
        if (str[i] - 13 < 'a') {
          newStr[i] = str[i] + 13;
        } else {
          newStr[i] = str[i] - 13;
        }
      }
    }

    if ((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97)) {
      newStr[i] = str[i];
    }

    i++;
  }
  newStr[i] = '\0';
}
```

Kemudian, dari fungsi di atas yang bertugas untuk menambah ASCII char sebanyak 13 (kalau melebihi batas, diulang dari awal), maka kita harus menentukan apakah folder tersebut harus kita encode dengan menggunakan `strncmp(token, "RX_", 5) == 0`
Pengecekan di atas dilakukan pada fungsi `xmp_readdir` sehingga kita bisa mengubah nama yang ditampilkan pada file system fuse. Penjelasan detail mengenai `xmp_readdir` sudah dipaparkan di nomor 1. Di sini hanya bagaimana enconde/deencode terkait dipanggil.

```c
char *token = strtok(path, "/");
while (token != NULL) {
  ...
  // TODO Check if prefixed with RX_ 
  else if (strncmp(token, "RX_", 3) == 0) {
    hasToEncrypt = 2;
  }
  ...
  } else if (hasToEncrypt == 2) {
    rot13Denc(pathEnc, pathDec2);
    atbash(pathDec2, pathDec);
    sprintf(fpath, "%s%s%s", dirpath, path, pathDec);
  } else {
    sprintf(fpath, "%s%s", dirpath, path);
  }
}

...

while ((de = readdir(dp)) != NULL) {
  ...
  } else if (hasToEncrypt == 2) {
    atbash(de->d_name, temp2);
    rot13Enc(temp2, temp);
  } else {
    strcpy(temp, de->d_name);
  }

  printf("temp send: %s\n", temp);

  res = (filler(buf, temp, &st, 0));

  if (res != 0) break;
}
...
```

## Soal 2d
Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/RX_[Nama Direktori]

Pencatatan dilakukan pada xmp_mkdir dan xmp_rename yaitu dengan fungsi

loglvlInfo("MKDIR", path);

loglvlInfo2("RENAME", fpathFrom, fpathTo);
Fungsi log akan dijelaskan lebih lanjut pada nomor 4

# Soal 4

Dalam membuat log dari segala aktifitas berlangsung , maka bentuk log akan diatur dengan 2 format yaitu INFO dan WARNING. Command yang termasuk dalam log level WARNING yaitu `rmdir` dan `unlink` sedangkan yang lainnya termasuk log level INFO.

### Log Level WARNING
Dalam penulisan log WARNING , yaitu dengan membuat fungsi `loglvlWarning` sehingga untuk melakukan pemanggilan fungsi akan lebih mudah apabila dibuat terpisah antara INFO dan WARNING. 

Pada fungsi ini kita melakukan inisialisasi `fp` sebagai FILE yang dimana sebagai tempat untuk melakukan store log tersebut. Untuk log akan distore ke fp memiliki Command tersebut bertujuan untuk destinasi log yang akan dibuat.
```
void loglvlWarning(const char *log, const char *path) {
  FILE *fp;
  fp = fopen("/home/clarence/SinSeiFS.log", "a");
  fputs("WARNING::", fp);
```
Untuk format penulisan log WARNING, `WARNING::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC]`. Untuk melakukan format tersebut maka kita harus menginisialisasi ke variable dengan command. Sehingga format tanggal dan timestamp didapat dari `struct tm tm = *localtime(&t)` dan hasil format kita store `char timestamp[1000]`
 ```
 char timestamp[1000];
 time_t t = time(NULL);
 struct tm tm = *localtime(&t);
 sprintf(timestamp, "%02d%02d%04d-%02d:%02d:%02d:", tm.tm_mday, tm.tm_mon + 1,
          tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
 ``` 
 pada format year menggunakan `tm.tm_year + 1900` yagn bertujuan untuk menghasilkan tahun `2021` karena format yang didapat dari `localtime` berupa `0121` sehingga harus ditambah `1900` agar tahun dapat sesuai dengan formatnya.

 Kemudian variable timestamp tersebut di puts di log tersebut dengan ditambah `log` yang sebelum nya sudah di store dari fungsi lain yang dimana `log` tersebut berisi jenis command /`[CMD]` yang dilakukan. kemudian diakhiri dengan `path` yang juga sudah di store dari fungsi lain , berisi alamat dari file tersebut.
 ```
  fputs(timestamp, fp);
  fputs(log, fp);
  fputs("::", fp);
  fputs(path, fp);
 ```
contoh dari fungsi lain yang menggunakan `loglvlwarning` yaitu `xmp_rmdir` yaitu dengan cara sebagai berikut
```
static int xmp_rmdir(const char *path){
  ...
  
  loglvlWarning("RMDIR", path);
}
```
Dengan alur sebagai berikut maka log dari command `rmdir` akan distore ke `SinSeiFS.log`. Hasil dari log level warning  dapat dilihat sebagai berikut:

![](https://i.imgur.com/RjM7aK2.png)

### Log Level INFO
 Pada penulisan log INFO ini , yaitu untuk mempermudah disaat memanggil fungsi maka membagi menjadi 2 fungsi yaitu `loglvlInfo` dan `loglvlInfo2`. Prinsip dari log INFO sama dengan prinsip yang dimiliki fungsi log WARNING, yang menjadi pembedanya yaitu dari format dalam melakukan penulisan log

 #### loglvlInfo
 Pada `loglvlinfo` hanya berbeda pada `INFO::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC]` maka command yang diganti yaitu
 ```
 void loglvlInfo(const char *log, const char *path) {
   ...

   fputs("INFO::", fp);
  }
 ```
contoh dari penerapan `loglvlinfo` yaitu pada command `xmp_readdir` yaitu dengan cara memanggil fungsi tersebut di akhir fungsi 
```
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi){
...
                        
loglvlInfo("READDIR", path);
}
```
sehingga dengan cara berikut log dari `readdir` akan masuk ke `SinSeiFs.log`. Hasil dari penulisan `loglvlinfo` dapat dilihat sebagai berikut:

![](https://i.imgur.com/qHhUqJZ.png)

### loglvlInfo2
Sedangkan untuk fungsi ini yang menjadi pembeda yaitu dari variabel yang dipassing ke fungsi ini. sehingga bukannya hanya `path` merupakan alamat yang dipassing ke fungsi , `fpathto` juga dipassing yang dimana berisi destination. sehingga pada format log menjadi 

`INFO::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]` 

yang dimana source dan destination dari fungsi yang terkait ditulis pada log tersebut. perubahan pada fungsi `loglvlInfo2` yaitu
```
void loglvlInfo2(const char *log, const char *source, const char *destination){
...

fputs(log, fp);
fputs("::", fp);
fputs(source, fp);
fputs("::", fp);
fputs(destination, fp);

...
}
```
contoh penerapan `loglvlInfo2' yaitu pada fungsi `xmp_rename` yang dimana harus memanggil fugnsi ini
```
static int xmp_rename(const char *from, const char *to) {
  ...

  loglvlInfo2("RENAME", fpathFrom, fpathTo);

  ...
```
Sehingga log dari `xmp_rename` akan di tulis pada `SinSeiFs.log`. Hasil dari penulisan log dapat dilihat sebagai berikut:

![](https://i.imgur.com/D1ivSEQ.png)

Dengan metode yang sudah dijelaskan maka penulisan log pada `SinSeiFS.log` akan terstruktur dengan baik

## Kendala
#### 1
Pada soal 2b disebutkan bahwa:
<p>Jika sebuah direktori di-rename dengan awalan “RX_[Nama]”, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai dengan kasus nomor 1 dengan algoritma tambahan Vigenere Cipher dengan key “SISOP” (Case-sensitive, Atbash + Vigenere).</p>
Namun pada praktikum ini kami masih belum bisa mengetahui apakah suatu direktori adalah telah dilakukan rename atau tidak. Penentuan tersebut berkaitan dengan fungsi encode apa yang akan diberlakukan.

#### 2
Kami masih belum bisa memecah file-file di direktori asli menjadi file-file kecil sebesar 1024 bytes, sementara jika diakses melalui filesystem rancangan Sin dan Sei akan menjadi normal. 


