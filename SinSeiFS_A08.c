#define FUSE_USE_VERSION 28
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

static const char *dirpath = "/home/clarence/Documents";

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

void vigen(char str[1000], char newStr[1000], bool encr) {
  // Skip if directory
  if (!strcmp(str, ".") || !strcmp(str, "..")) {
    strcpy(newStr, str);
    return;
  };

  char key[] = "SISOP";
  int msgLen = strlen(str), keyLen = strlen(key), i, j;
  char newKey[msgLen];

  // generating new key
  for (i = 0, j = 0; i < msgLen; ++i, ++j) {
    if (j == keyLen) j = 0;

    newKey[i] = key[j];
  }
  newKey[i] = '\0';

  int flag = 0;
  i = 0;
  while (str[i] != '\0') {
    char temp;

    // exclude extension
    if (str[i] == '.') {
      flag = 1;
    }
    if (flag == 1) {
      newStr[i] = str[i];
      i++;
      continue;
    }

    if (encr) {
      if (!((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97))) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
          if (newKey[i] >= 'a' && newKey[i] <= 'z') {
            temp = newKey[i] - 'a' + 'A';
            newStr[i] = ((str[i] + temp) % 26) + 'A';
          } else {
            newStr[i] = ((str[i] + newKey[i]) % 26) + 'A';
          }
        }
        if (str[i] >= 'a' && str[i] <= 'z') {
          temp = str[i] - 'a' + 'A';
          if (newKey[i] >= 'a' && newKey[i] <= 'z') {
            char tempNK = newKey[i] - 'a' + 'A';
            newStr[i] = ((temp + tempNK) % 26) + 'A';
          } else {
            newStr[i] = ((temp + newKey[i]) % 26) + 'A';
          }
          newStr[i] = newStr[i] - 'A' + 'a';
        }
      }
    } else {
      if ((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97)) {
        if (!((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97))) {
          if (str[i] >= 'A' && str[i] <= 'Z') {
            if (newKey[i] >= 'a' && newKey[i] <= 'z') {
              temp = newKey[i] - 'a' + 'A';
              newStr[i] = (((str[i] + temp) + 26) % 26) + 'A';
            } else {
              newStr[i] = (((str[i] + newKey[i]) + 26) % 26) + 'A';
            }
          }
          if (str[i] >= 'a' && str[i] <= 'z') {
            temp = str[i] - 'a' + 'A';
            if (newKey[i] >= 'a' && newKey[i] <= 'z') {
              char tempNK = newKey[i] - 'a' + 'A';
              newStr[i] = (((temp + tempNK) + 26) % 26) + 'A';
            } else {
              newStr[i] = (((temp + newKey[i]) + 26) % 26) + 'A';
            }
            newStr[i] = newStr[i] - 'A' + 'a';
          }
        }
      }
    }

    i++;
  }
  newStr[i] = '\0';
}

void loglvlWarning(const char *log, const char *path) {
  FILE *fp;
  fp = fopen("/home/clarence/SinSeiFS.log", "a");
  fputs("WARNING::", fp);
  char timestamp[1000];
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  sprintf(timestamp, "%02d%02d%04d-%02d:%02d:%02d:", tm.tm_mday, tm.tm_mon + 1,
          tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
  fputs(timestamp, fp);
  fputs(log, fp);
  fputs("::", fp);
  fputs(path, fp);
  fputs("\n", fp);
  fclose(fp);
}

void loglvlInfo(const char *log, const char *path) {
  FILE *fp;
  fp = fopen("/home/clarence/SinSeiFS.log", "a");
  fputs("INFO::", fp);
  char timestamp[1000];
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  sprintf(timestamp, "%02d%02d%04d-%02d:%02d:%02d:", tm.tm_mday, tm.tm_mon + 1,
          tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
  fputs(timestamp, fp);
  fputs(log, fp);
  fputs("::", fp);
  fputs(path, fp);
  fputs("\n", fp);
  fclose(fp);
}

void loglvlInfo2(const char *log, const char *source, const char *destination) {
  FILE *fp;
  fp = fopen("/home/clarence/SinSeiFS.log", "a");
  fputs("INFO::", fp);
  char timestamp[1000];
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  sprintf(timestamp, "%02d%02d%04d-%02d:%02d:%02d:", tm.tm_mday, tm.tm_mon + 1,
          tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
  fputs(timestamp, fp);
  fputs(log, fp);
  fputs("::", fp);
  fputs(source, fp);
  fputs("::", fp);
  fputs(destination, fp);
  fputs("\n", fp);
  fclose(fp);
}

static int xmp_getattr(const char *path, struct stat *stbuf) {
  int res;
  char fpath[1000];

  printf("\n🚀 GETATTR\n");
  char temp[1000];
  strcpy(temp, path);

  sprintf(fpath, "%s%s", dirpath, path);

  int hasToEncrypt = 0;

  // ? Current Path -> directory sekarang
  // ? pathEnc -> path yang harus didecrypt biar bisa dibaca sama si attr
  char currPath[1000] = "", pathEnc[1000] = "";

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
    rot13Denc(pathEnc, decrypted2);
    atbash(decrypted2, decrypted);
    sprintf(fpathToSend, "%s%s%s", dirpath, currPath, decrypted);
  } else {
    sprintf(fpathToSend, "%s%s%s", dirpath, currPath, pathEnc);
  }

  printf("fpathToSend: %s", fpathToSend);

  res = lstat(fpathToSend, stbuf);
  if (res == -1) return -errno;

  loglvlInfo("GETATTR", path);

  return 0;
}

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

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi) {
  char fpath[1000];
  char temp[1000];
  strcpy(temp, path);

  sprintf(fpath, "%s%s", dirpath, path);

  int hasToEncrypt = 0;

  // ? Current Path -> directory sekarang
  // ? pathEnc -> path yang harus didecrypt biar bisa dibaca sama si attr
  char currPath[1000] = "", pathEnc[1000] = "";

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

  int res = 0;
  int fd = 0;

  (void)fi;

  fd = open(fpathToSend, O_RDONLY);

  if (fd == -1) return -errno;

  res = pread(fd, buf, size, offset);

  if (res == -1) res = -errno;

  close(fd);

  loglvlInfo("READ", path);

  return res;
}

static int xmp_rename(const char *from, const char *to) {
  char fpathFrom[1000];
  char fpathTo[1000];

  if (strcmp(from, "/") == 0) {
    from = dirpath;
    sprintf(fpathFrom, "%s", from);
  } else {
    sprintf(fpathFrom, "%s%s", dirpath, from);
  }

  if (strcmp(to, "/") == 0) {
    to = dirpath;
    sprintf(fpathTo, "%s", to);
  } else {
    sprintf(fpathTo, "%s%s", dirpath, to);
  }

  int res;

  res = rename(fpathFrom, fpathTo);

  if (res == -1) return -errno;

  loglvlInfo2("RENAME", fpathFrom, fpathTo);

  // createLogInfo2("RENAME", from, to);
  // bool en1 = false, en2 = false, en3 = false, en4 = false;
  // char *token = strtok(from, "/");
  // while (token != NULL) {
  //   if (strncmp(token, "AtoZ_", 5) == 0) {
  //     en1 = true;
  //     break;
  //   } else if (strncmp(token, "RX_[Nama]", 9) == 0) {
  //     en2 = true;
  //     break;
  //   }
  //   token = strtok(NULL, "/");
  // }

  // char *token1 = strtok(to, "/");
  // while (token1 != NULL) {
  //   if (hasToEncrypt == 1) {
  //     en3 = true;
  //     break;
  //   } else if (hasToEncrypt == 2) {
  //     en4 = true;
  //     break;
  //   }
  //   token1 = strtok(NULL, "/");
  // }

  // char fpathToSend[2000], decrypted[1000], decrypted2[1000];
  // if (!en1 && en3) {
  //   createLogInfo2("ENCRYPT1", from, to);
  // } else if (en1 && !en3) {
  //   createLogInfo2("DECRYPT1", from, to);
  // } else if (!en2 && en4) {
  //   createLogInfo2("ENCRYPT2", from, to);
  //   initEncrypt2(fpathTo);
  // } else if (en2 && !en4) {
  //   createLogInfo2("DECRYPT2", from, to);
  //   initDecrypt2(fpathTo);
  // }

  return 0;
}

static int xmp_mkdir(const char *path, mode_t mode) {
  char fpath[1000];
  if (strcmp(path, "/") == 0) {
    path = dirpath;
    sprintf(fpath, "%s", path);
  } else {
    sprintf(fpath, "%s%s", dirpath, path);
  }
  int res;
  res = mkdir(fpath, mode);

  if (res == -1) return -errno;

  loglvlInfo("MKDIR", path);

  return 0;
}

static int xmp_create(const char *path, mode_t mode,
                      struct fuse_file_info *fi) {
  char fpath[1000];
  if (strcmp(path, "/") == 0) {
    path = dirpath;
    sprintf(fpath, "%s", path);
  } else {
    sprintf(fpath, "%s%s", dirpath, path);
  }

  (void)fi;

  int res;
  res = creat(fpath, mode);
  if (res == -1) return -errno;

  close(res);
  loglvlInfo("CREATE", path);

  return 0;
}

static int xmp_rmdir(const char *path) {
  char fpath[1000];
  if (strcmp(path, "/") == 0) {
    path = dirpath;
    sprintf(fpath, "%s", path);
  } else {
    sprintf(fpath, "%s%s", dirpath, path);
  }

  int res;

  res = rmdir(fpath);
  if (res == -1) return -errno;

  loglvlWarning("RMDIR", path);
  return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi) {
  char fpath[1000];
  if (strcmp(path, "/") == 0) {
    path = dirpath;
    sprintf(fpath, "%s", path);
  } else {
    sprintf(fpath, "%s%s", dirpath, path);
  }

  int fd;
  int res;

  (void)fi;
  fd = open(fpath, O_WRONLY);
  if (fd == -1) return -errno;

  res = pwrite(fd, buf, size, offset);
  if (res == -1) res = -errno;

  close(fd);
  loglvlInfo("WRITE", path);
  return res;
}

static int xmp_link(const char *from, const char *to) {
  char fpathFrom[1000];
  char fpathTo[1000];
  if (strcmp(from, "/") == 0) {
    from = dirpath;
    sprintf(fpathFrom, "%s", from);
  } else {
    sprintf(fpathFrom, "%s%s", dirpath, from);
  }

  if (strcmp(to, "/") == 0) {
    to = dirpath;
    sprintf(fpathTo, "%s", to);
  } else {
    sprintf(fpathTo, "%s%s", dirpath, to);
  }

  int res;

  res = link(fpathFrom, fpathTo);
  if (res == -1) return -errno;

  loglvlInfo2("LINK", from, to);
  return 0;
}

static int xmp_unlink(const char *path) {
  char fpath[1000];
  if (strcmp(path, "/") == 0) {
    path = dirpath;
    sprintf(fpath, "%s", path);
  } else {
    sprintf(fpath, "%s%s", dirpath, path);
  }

  int res;

  res = unlink(fpath);
  if (res == -1) return -errno;

  loglvlWarning("UNLINK", path);
  return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .rename = xmp_rename,
    .mkdir = xmp_mkdir,
    .create = xmp_create,
    .link = xmp_link,
    .rmdir = xmp_rmdir,
    .write = xmp_write,
    .unlink = xmp_unlink,
};

int main(int argc, char *argv[]) {
  umask(0);

  return fuse_main(argc, argv, &xmp_oper, NULL);
}