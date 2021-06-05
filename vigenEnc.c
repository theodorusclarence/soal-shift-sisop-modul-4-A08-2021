#include<stdio.h>
#include<string.h>
 
int main(){
    char msg[] = "abcdABCD123xyzXYZ";
    char key[] = "HELLO";
    char temp;
    int msgLen = strlen(msg), keyLen = strlen(key), i, j;
 
    char newKey[msgLen], encryptedMsg[msgLen], decryptedMsg[msgLen];
 
    //generating new key
    for(i = 0, j = 0; i < msgLen; ++i, ++j){
        if(j == keyLen)
            j = 0;
 
        newKey[i] = key[j];
    }
 
    newKey[i] = '\0';
    
    //encryption
    for(i = 0; i < msgLen; ++i) {
        if (msg[i] >= 'A' && msg[i] <= 'Z') {
            if(newKey[i] >= 'a' && newKey[i] <= 'z') {
                temp = newKey[i] - 'a' + 'A';
                encryptedMsg[i] = ((msg[i] + temp) % 26) + 'A';
            }
            else {
                encryptedMsg[i] = ((msg[i] + newKey[i]) % 26) + 'A';
            }
            // printf("%d, %d, %d %d\n", encryptedMsg[i], msg[i]
            // , newKey[i], (msg[i] + newKey[i]) % 26);
        }
        if (msg[i] >= 'a' && msg[i] <= 'z') {
            temp = msg[i] - 'a' + 'A';
            if(newKey[i] >= 'a' && newKey[i] <= 'z') {
                char tempNK = newKey[i] - 'a' + 'A';
                encryptedMsg[i] = ((temp + tempNK) % 26) + 'A';
            }
            else {
                encryptedMsg[i] = ((temp + newKey[i]) % 26) + 'A';
            }
            encryptedMsg[i] = encryptedMsg[i] - 'A' + 'a';
        }
        if ((msg[i] >= 0 && msg[i] < 65) || (msg[i] > 90 && msg[i] < 97)) {
            encryptedMsg[i] = msg[i];
        }
        // encryptedMsg[i] = ((msg[i] + newKey[i]) % 26) + 'A';
    }
    encryptedMsg[i] = '\0';
 
    //decryption
    for(i = 0; i < msgLen; ++i)
        decryptedMsg[i] = (((encryptedMsg[i] - newKey[i]) + 26) % 26) + 'A';
 
    decryptedMsg[i] = '\0';
 
    printf("Original Message: %s", msg);
    printf("\nKey: %s", key);
    printf("\nNew Generated Key: %s", newKey);
    printf("\nEncrypted Message: %s", encryptedMsg);
    printf("\nDecrypted Message: %s", decryptedMsg);
 
return 0;
}