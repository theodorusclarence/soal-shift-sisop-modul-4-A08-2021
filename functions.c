#include <stdio.h>
#include <string.h>

void atbash(char str[1000], char newStr[1000]) {
  int i;

  i = 0;
  while (str[i] != '\0') {
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

int main() {
  char str[1000] = "a1/23";
  char newStr[1000];

  atbash(str, newStr);

  printf("%s\n", newStr);
}