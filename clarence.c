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

static int xmp_getattr(const char *path, struct stat *stbuf) {
  int res;
  char fpath[1000];

  printf("\n🚀 GETATTR\n");
  char temp[1000];
  strcpy(temp, path);

  sprintf(fpath, "%s%s", dirpath, path);

  bool hasToEncrypt = false;

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
      hasToEncrypt = true;
    }
    token = strtok(NULL, "/");
  }

  // TODO Send decrypted path, jadi ga bingung si attrnya
  char fpathToSend[2000], decrypted[1000];
  if (hasToEncrypt) {
    atbash(pathEnc, decrypted);
    sprintf(fpathToSend, "%s%s%s", dirpath, currPath, decrypted);
  } else {
    sprintf(fpathToSend, "%s%s%s", dirpath, currPath, pathEnc);
  }

  res = lstat(fpathToSend, stbuf);
  if (res == -1) return -errno;

  return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi) {
  char fpath[1000];
  printf("\n🚀 READDIR\n");

  bool hasToEncrypt = false;
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
      hasToEncrypt = true;
    }
    printf("token: %s\n", token);
    token = strtok(NULL, "/");
  }

  if (strcmp(path, "/") == 0) {
    path = dirpath;
    sprintf(fpath, "%s", path);
  } else {
    sprintf(fpath, "%s%s", dirpath, path);
  }

  int res = 0;

  DIR *dp;
  struct dirent *de;
  (void)offset;
  (void)fi;

  dp = opendir(fpath);

  if (dp == NULL) return -errno;

  while ((de = readdir(dp)) != NULL) {
    struct stat st;

    memset(&st, 0, sizeof(st));

    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;

    // Create variable for new name
    char temp[1000];

    if (hasToEncrypt) {
      atbash(de->d_name, temp);
    } else {
      strcpy(temp, de->d_name);
    }

    res = (filler(buf, temp, &st, 0));

    if (res != 0) break;
  }

  closedir(dp);

  return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi) {
  char fpath[1000];
  char temp[1000];
  strcpy(temp, path);

  sprintf(fpath, "%s%s", dirpath, path);

  bool hasToEncrypt = false;

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
      hasToEncrypt = true;
    }
    token = strtok(NULL, "/");
  }

  // TODO Send decrypted path, jadi ga bingung si attrnya
  char fpathToSend[2000], decrypted[1000];
  if (hasToEncrypt) {
    atbash(pathEnc, decrypted);
    sprintf(fpathToSend, "%s%s%s", dirpath, currPath, decrypted);
  } else {
    sprintf(fpathToSend, "%s%s%s", dirpath, currPath, pathEnc);
  }

  int res = 0;
  int fd = 0;

  (void)fi;

  fd = open(fpathToSend, O_RDONLY);

  if (fd == -1) return -errno;

  res = pread(fd, buf, size, offset);

  if (res == -1) res = -errno;

  close(fd);

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
  return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .rename = xmp_rename,
    .mkdir = xmp_mkdir,
    .create = xmp_create,
};

int main(int argc, char *argv[]) {
  umask(0);

  return fuse_main(argc, argv, &xmp_oper, NULL);
}