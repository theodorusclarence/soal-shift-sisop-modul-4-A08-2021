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

void loglvlWarning(const char *log, const char *path){
    FILE *fp;
    fp = fopen("/home/yusuf/SinSeiFS.log", "a");
    fputs("WARNING::", fp);
    char timestamp[1000];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(timestamp, "%02d%02d%02d-%02d:%02d:%02d:", (tm.tm_year + 1900)%100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fputs(timestamp, fp);
    fputs(log, fp);
    fputs("::", fp);
    fputs(path, fp);
    fputs("\n", fp);
    fclose(fp);
}

void loglvlInfo(const char *log, const char *path){
    FILE *fp;
    fp = fopen("/home/yusuf/SinSeiFS.log", "a");
    fputs("INFO::", fp);
    char timestamp[1000];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(timestamp, "%02d%02d%02d-%02d:%02d:%02d:", (tm.tm_year + 1900)%100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fputs(timestamp, fp);
    fputs(log, fp);
    fputs("::", fp);
    fputs(path, fp);
    fputs("\n", fp);
    fclose(fp);
}

void loglvlInfo2(const char *log, const char *source, const char *destination){
    FILE *fp;
    fp = fopen("/home/yusuf/SinSeiFS.log", "a");
    fputs("INFO::", fp);
    char timestamp[1000];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(timestamp, "%02d%02d%02d-%02d:%02d:%02d:", (tm.tm_year + 1900)%100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
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
  loglvlInfo("GETATTR",path);
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
  loglvlInfo("READDIR",path);
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
  loglvlInfo("READ",path);
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
  loglvlInfo("MKDIR",path);
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
  loglvlInfo("CREAT",path);
  return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size){
    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }

    int res;
    res = readlink(fpath, buf, size - 1);
    if (res == -1)
        return -errno;

    buf[res] = '\0';

    loglvlInfo("READLINK", path);
    return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev){
    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }

	int res;

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(fpath, mode);
	else
		res = mknod(fpath, mode, rdev);
	if (res == -1)
		return -errno;

    loglvlInfo("MKNOD", path);
	return 0;
}

static int xmp_symlink(const char *from, const char *to){
    char fpathFrom[1000];
    char fpathTo[1000];
    if(strcmp(from,"/") == 0){
        from=dirpath;
        sprintf(fpathFrom,"%s",from);
    } else{
        sprintf(fpathFrom, "%s%s",dirpath,from);
    }

    if(strcmp(to,"/") == 0){
        to=dirpath;
        sprintf(fpathTo,"%s",to);
    } else{
        sprintf(fpathTo, "%s%s",dirpath,to);
    }

    int res;

    res = symlink(fpathFrom, fpathTo);
    if (res == -1)
        return -errno;

    loglvlInfo2("SYMLINK", from, to);
    return 0;
}

static int xmp_unlink(const char *path){
    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }

    int res;

    res = unlink(fpath);
    if (res == -1)
        return -errno;

    loglvlWarning("UNLINK", path);
    return 0;
}

static int xmp_rmdir(const char *path){
    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }

    int res;

    res = rmdir(fpath);
    if (res == -1)
        return -errno;

    loglvlWarning("RMDIR", path);
    return 0;
}

static int xmp_link(const char *from, const char *to){
    char fpathFrom[1000];
    char fpathTo[1000];
    if(strcmp(from,"/") == 0){
        from=dirpath;
        sprintf(fpathFrom,"%s",from);
    } else{
        sprintf(fpathFrom, "%s%s",dirpath,from);
    }

    if(strcmp(to,"/") == 0){
        to=dirpath;
        sprintf(fpathTo,"%s",to);
    } else{
        sprintf(fpathTo, "%s%s",dirpath,to);
    }

    int res;

    res = link(fpathFrom, fpathTo);
    if (res == -1)
        return -errno;

    loglvlInfo2("LINK", from, to);
    return 0;
}

static int xmp_chmod(const char *path, mode_t mode){
    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }
    
    int res;

    res = chmod(fpath, mode);
    if (res == -1)
        return -errno;

    loglvlInfo("CHMOD", path);
    return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid){
    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }

    int res;

    res = lchown(fpath, uid, gid);
    if (res == -1)
        return -errno;

    loglvlInfo("CHOWN", path);
    return 0;
}

static int xmp_truncate(const char *path, off_t size){
    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }

    int res;
    res = truncate(fpath, size);
    if (res == -1)
        return -errno;
    
    loglvlInfo("TRUNCATE", path);
    return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2]){
	char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }

    int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(fpath, tv);
	if (res == -1)
		return -errno;

    loglvlInfo("UTIMENS", path);
	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi){
    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }

    int res;

    res = open(fpath, fi->flags);
    if (res == -1)
        return -errno;

    close(res);

    loglvlInfo("OPEN", path);
    return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
	char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }

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
    loglvlInfo("WRITE", path);
    return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf){
    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }
    
    int res;

    res = statvfs(fpath, stbuf);
    if (res == -1)
        return -errno;

    loglvlInfo("STATFS", path);
    return 0;
}

#ifdef HAVE_SETXATTR
static int xmp_setxattr(const char *path, const char *name, const char *value, size_t size, int flags){
	char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }

    int res = lsetxattr(fpath, name, value, size, flags);
	if (res == -1)
		return -errno;

    loglvlInfo("SETXATTR", path);
	return 0;
}

static int xmp_getxattr(const char *path, const char *name, char *value, size_t size){
    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }
    
    int res = lgetxattr(fpath, name, value, size);
    if (res == -1)
        return -errno;
    
    loglvlInfo("GETXATTR", path);
    return res;
}

static int xmp_listxattr(const char *path, char *list, size_t size){
    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }

    int res = llistxattr(fpath, list, size);
    if (res == -1)
        return -errno;
    
    loglvlInfo("LISTXATTR", path);
    return res;
}

static int xmp_removexattr(const char *path, const char *name){
    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else{
        sprintf(fpath, "%s%s",dirpath,path);
    }

    int res = lremovexattr(fpath, name);
    if (res == -1)
        return -errno;
    
    loglvlInfo("REMOVEXATTR", path);
    return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .rename = xmp_rename,
    .mkdir = xmp_mkdir,
    .create = xmp_create,
    .readlink = xmp_readlink,
    .mknod = xmp_mknod,
    .symlink = xmp_symlink,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir,
    .link = xmp_link,
    .chmod = xmp_chmod,
    .chown = xmp_chown,
    .truncate = xmp_truncate,
    .utimens = xmp_utimens,
    .open = xmp_open,
    .write = xmp_write,
    .statfs= xmp_statfs,
#ifdef HAVE_SETXATTR
    .setxattr       = xmp_setxattr,
    .getxattr       = xmp_getxattr,
    .listxattr      = xmp_listxattr,
    .removexattr    = xmp_removexattr,
#endif
};

int main(int argc, char *argv[]) {
  umask(0);

  return fuse_main(argc, argv, &xmp_oper, NULL);
}