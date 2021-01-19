#include <stdio.h>
#include <dirent.h>
#include <string.h>

void 

int listdir(const char *path) 
{
  struct dirent *entry;
  DIR *dp;

  dp = opendir(path);
  if (dp == NULL) 
  {
    perror("opendir");
    return -1;
  }

  while((entry = readdir(dp)))
    puts(entry->d_name);

  closedir(dp);
  return 0;
}

int main() {

    
    listdir("/proc");

    return 0;
}