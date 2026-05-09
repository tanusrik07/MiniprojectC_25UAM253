#include <stdio.h>
#include <stdlib.h>

struct clientData {
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

int main() {
    FILE *fPtr;
    struct clientData blankClient = {0, "", "", 0.0};

    if ((fPtr = fopen("credit.dat", "wb")) == NULL) {
        printf("File could not be created.\n");
        exit(-1);
    }

    // Write 100 empty slots
    for (int i = 0; i < 100; i++) {
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    }

    fclose(fPtr);
    printf("credit.dat created successfully with 100 empty slots!\n");
    return 0;
}