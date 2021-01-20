#include <stdio.h>
#include <stdlib.h>

const char OS_INFO_PATH[] = "/etc/os-release";
const char KERNAL_INFO_PATH[] = "/proc/version";

void read_os_info(){
    
    FILE *infoFile = fopen(OS_INFO_PATH, "r");

    if (infoFile == NULL)
        perror("os-release file not found.");

    char infoText;

    printf("OS ");
    while ((infoText = fgetc(infoFile))){
        if (infoText == '\"')
            break;
    }

    while ((infoText = fgetc(infoFile))){
        if (infoText == '\"')
            break;
        printf("%c", infoText);
    }    

    //cleanup
    fclose(infoFile);
}

void read_kernal_info(){

    //open version file
    //outputs error if fails to open
    FILE *infoFile = fopen(KERNAL_INFO_PATH, "r");
    if (infoFile == NULL)
        perror("version file not found.");

    char *infoText = malloc(100);

    for (int i = 0; i < 3; i++){
        fscanf(infoFile, "%s", infoText);
        printf("%s ", infoText);
    }

    //cleanup
    free(infoText);
    fclose(infoFile);
}

int main(){

    read_os_info();

    read_kernal_info();

    return 0;
}