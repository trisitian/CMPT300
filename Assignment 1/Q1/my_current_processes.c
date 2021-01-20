/**
    This project was created by YOUR NAME HERE and Tristian Labanowich
    Student numbers YOUR STUDEND NUMBER and 301422226
    SFU ID YOUR ID and tal8 
    This program displays current running processes with their names and IDS
*/


#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

const char TOP_PATH[] = "/proc/";

void print_processes(struct dirent *DirItem){

  //print PID and tab
  printf("%s\t", DirItem->d_name);

  //grab path to cmdline file 
  char *filePath = malloc(100);
  strcat(filePath, TOP_PATH);
  strcat(filePath, DirItem->d_name);
  strcat(filePath, "/cmdline");
  FILE *cmdlineFile = fopen(filePath, "r");
  if (cmdlineFile == NULL)
    perror("cmdline file not found for a process");
  
  //grab first line of cmdline file, i.e. the process path
  char *processName = malloc(100);
  //NEW BLOCK HERE
  char delim = '/'; // to parse process from file path 
  char *val; // stores value
  fgets(processName, 100, cmdlineFile);
  if((val = strrchr(processName, delim)) != NULL){ // if it is NULL we don't want it as the process does not have a name
    memmove(val, val +1, strlen(val)); // cut out leading /
    printf("%s\n", val);
  }
  // NEW BLOCK ENDS
  //printf("%s\n", processName)
  
  fclose(cmdlineFile);
  free(filePath);
}

void print_directory() {

  struct dirent *DirItem;
  DIR *Directory = opendir(TOP_PATH);

  //check if directory can be opened
  if (Directory == NULL)
      perror("/proc directory not found");

  while((DirItem = readdir(Directory))){
  
      //check if entry is of type 4 (folder) and if name is numerical
      if ((DirItem->d_type == 4) && (isdigit(*(DirItem->d_name)))) 
          print_processes(DirItem);

  }

  closedir(Directory);

}

int main() {
    
    print_directory();

    return 0;
}