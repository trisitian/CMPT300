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
  
  //grab first line of cmdline file, i.e. the process name
  char *processName = malloc(100);
  fgets(processName, 100, cmdlineFile);
  printf("%s\n", processName);
  
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