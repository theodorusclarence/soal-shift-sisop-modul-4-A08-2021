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
