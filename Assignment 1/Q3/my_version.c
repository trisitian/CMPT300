/**
    This project was created by YOUR NAME HERE and Tristian Labanowich
    Student numbers YOUR STUDEND NUMBER and 301422226
    SFU ID YOUR ID and tal8 
    This program displays current running processes with their names and IDS
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char OS_INFO_PATH[] = "/etc/os-release";
const char KERNAL_INFO_PATH[] = "/proc/version";

void read_os_info(){
    
    //open os-release file for reading
    FILE *infoFile = fopen(OS_INFO_PATH, "r");

    if (infoFile == NULL)
        perror("os-release file not found.");

    printf("OS: ");

    char *infoText = malloc(100), *nameLocation;

    //Try to find the line with the OS name by iterating the file 
    //line by line, and try to find the substring
    while ((fgets(infoText, 100, infoFile)) != NULL){
        nameLocation = strstr(infoText, "PRETTY_NAME=");
        if (nameLocation != NULL)
            break;
    }

    //i=13 will point to the beginning of the name, past the first quotation
    //Reason for i<100 because i was allocated to have size of 100
    for (int i = 13; i<100; i++){
        if (infoText[i] == '\"'){
            printf("\n");
            break;
        }
        printf("%c", infoText[i]);
    }

    //cleanup
    free(infoText);
    fclose(infoFile);
}

void read_kernal_info(){

    //open version file
    //outputs error if fails to open
    FILE *infoFile = fopen(KERNAL_INFO_PATH, "r");
    if (infoFile == NULL)
        perror("version file not found.");

    char *infoText = malloc(100);

    //because fscanf ends at a whitespace,
    //the sequence is repeated 3 times to print the info.
    for (int i = 0; i < 3; i++){
        fscanf(infoFile, "%s", infoText);
        printf("%s ", infoText);
    }

    printf("\n");

    //cleanup
    free(infoText);
    fclose(infoFile);
}

int main(){

    read_os_info();

    read_kernal_info();

    return 0;
}