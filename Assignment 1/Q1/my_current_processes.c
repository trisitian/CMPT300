#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

const char topPath[] = "/proc/";

void printProcesses(struct dirent *dirItem){

  //print PID and tab
  printf("%s\t", dirItem->d_name);

  //grab path to cmdline file 
  char *filePath = malloc(100);
  strcat(filePath, topPath);
  strcat(filePath, dirItem->d_name);
  strcat(filePath, "/cmdline");

  FILE *cmdlineFile = fopen(filePath, "r");
  if (cmdlineFile == NULL)
    perror("cmdline file not found for a process");
  
  //grab first line of cmdline file, i.e. the process name
  char *processName = malloc(100);
  fgets(processName, 100, cmdlineFile);
  printf("%s\n", processName);
  
  fclose(cmdlineFile);
  free(filePath);
}

int printDirectory() {

  struct dirent *dirItem;
  DIR *directory = opendir(topPath);

  //check if directory can be opened
  if (directory == NULL) {
      perror("/proc directory not found");
      return -1;
  }

  while((dirItem = readdir(directory))){
  
      //check if entry is of type 4 (folder) and if name is numerical
      if ((dirItem->d_type == 4) && (isdigit(*(dirItem->d_name)))) 
          printProcesses(dirItem);

  }

  closedir(directory);
  return 0;
}

int main() {
    
    printDirectory();

    return 0;
}