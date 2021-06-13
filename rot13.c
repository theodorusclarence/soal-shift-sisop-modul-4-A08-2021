#include <stdio.h>

int main() {
    int len = 17;
    char *str = "abcdefghIJKLMN123";
    char newStr[100];
    for(int i = 0; i < len; i++) {
        if (!((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97))) {
            if (str[i] >= 'A' && str[i] <= 'Z') {
                if(str[i] - 13 < 'A') {
                    newStr[i] = 'Z' - (12 - (str[i] - 'A'));
                }
                else {
                    newStr[i] = str[i];
                }
            }
            if (str[i] >= 'a' && str[i] <= 'z') {
                if(str[i] - 13 < 'a') {
                newStr[i] = 'z' - (12 - (str[i] - 'a'));
            }
            
            else {
                newStr[i] = str[i];
            }
            }
            if ((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97)) {
        newStr[i] = str[i];
    }
        }
    }
}