/**
    This project was created by Tristian Labanowich and Jinhan Liu
    Student numbers 301422226 and 301430717
    SFU ID tal8 and jla769
    This program displays memory utilization of a program
*/

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


void print_mem(){
    int count = 1; // to keep track of each line
    double result = 0; // variables for arithmatic
    double total = 0; 
    FILE *memPtr; // file
    char line_contents[100]; // to read file
    memPtr = fopen("/proc/meminfo", "r"); // open the file in read only
    char *substring; // to parse data
    if(memPtr == NULL){ // exit with error if for some reason proc isn't there, shouldn't happen
        printf("An error occured");
        exit(1);
    }
    fgets(line_contents, sizeof(line_contents), memPtr); // get contents of first line
    substring = strtok(line_contents, ":");
    substring = strtok(NULL, "k");
    total = atoi(substring);
    result = atoi(substring);
    while(fgets(line_contents, sizeof(line_contents), memPtr)){
        if(count == 1 || count ==3 || count == 4 || count == 22){ // get lines that you need, assuming this file structure is universal
        substring = strtok(line_contents, ":"); // parese the string
        substring = strtok(NULL, "k");
            //printf("substring contents: %s\n",substring);
            result -= atoi(substring);
            //printf("current result: %f\n", result);
        }
        count++;
    }
    fclose(memPtr);
    result = (result/total) * 100; // apply formula as specified in instructions
    printf("MEMORY UTILIZATION, %f\n", result); // print result

}

int main(){
    print_mem();
    return 0;
}