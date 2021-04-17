#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
// questions asked if these were able to be used on piazza and we got the go ahead, these libraries are for stat()
#include <sys/types.h> 
#include <sys/stat.h>
#include <glob.h>
#include <time.h>

// to keep track of flags used
bool flagBoolList[3] = {false, false, false}; // [0]: 'i', [1]: 'l', [2]: 'R'
int flagCount = 0;

struct Files{
    char *FileList[100]; // 100 files limit
    int size; // amount of files passed
};


char dates[12][4] = {"Jan", "Feb", "Mar", "Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
char *allFiles[10000];
int allFilesCounter = 0;
void printFiles(struct Files files){
    DIR *curr;
    char *temp;
    struct dirent *dp;
    struct stat info;
    bool localfileCall = false;

    // for each custom path it will loop back
    // executing at least once for '.'
    do{
        // open directory
        if (files.size == 0){
            curr = opendir(".");

        } else {
            temp = files.FileList[files.size -1];
            stat(temp, &info);
            if(S_ISDIR(info.st_mode)){
                curr = opendir(files.FileList[files.size - 1]);
            }else{
                localfileCall = true;
                curr = opendir(".");
            }
        }

        if((curr == NULL)){
                printf("Error opening directory %s\n", files.FileList[files.size - 1]);
                exit(1);
            }

        // read directory
        while((dp = readdir(curr)) != NULL){
            if(localfileCall){ // read file
                if(strcmp(temp, dp->d_name) == 0){
                    localfileCall = false;
                    allFiles[allFilesCounter] = temp;
                    allFilesCounter++;
                    //printf("%s \n", temp);
                    break;
                }else{
                    continue;
                }
            }

            if(dp->d_type == DT_DIR && flagBoolList[2]){ // -R
                // skip . and .. directories 
                if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
                    struct Files newFiles;
                    newFiles.FileList[0] = malloc(sizeof(char) * 255);
                    
                    // use folder name as the directory to be accessed
                    strcpy(newFiles.FileList[0], "");
                    if (files.size == 0){
                        strcat(newFiles.FileList[0], "./");
                    } else {
                        strcat(newFiles.FileList[0], files.FileList[files.size - 1]);
                        strcat(newFiles.FileList[0], "/");
                    }
                    strcat(newFiles.FileList[0], dp->d_name);
                    newFiles.size = 1;

                    printFiles(newFiles); // recursively access subfolder
                    free(newFiles.FileList[0]);
                }
            }
            
            //printf("%s\n", dp->d_name);
            allFiles[allFilesCounter] = dp->d_name;
            allFilesCounter++;
        }
        if(localfileCall){
            printf("%s is not in the current directory\n", temp);
            localfileCall = false;
        }
        closedir(curr);
        files.size--;
    } while (files.size > 0);
    //DO SORT HERE
    for(int i = 0; i < allFilesCounter; i++){
        printf("%s\n", allFiles[i]);
    }



    if(flagBoolList[0]){ // -i
        printf("%ld  ", dp->d_ino);
    }

     if(flagBoolList[1]){ // -l
        stat(dp->d_name, &info);
        struct passwd *user = getpwuid(info.st_uid);
        struct group *group = getgrgid(info.st_gid);
        struct tm *date = localtime(&info.st_atim.tv_sec);
        printf( (S_ISDIR(info.st_mode)) ? "d" : "-");
        printf( (info.st_mode & S_IRUSR) ? "r" : "-");
        printf( (info.st_mode & S_IWUSR) ? "w" : "-");
        printf( (info.st_mode & S_IXUSR) ? "x" : "-");
        printf( (info.st_mode & S_IRGRP) ? "r" : "-");
        printf( (info.st_mode & S_IWGRP) ? "w" : "-");
        printf( (info.st_mode & S_IXGRP) ? "x" : "-");
        printf( (info.st_mode & S_IROTH) ? "r" : "-");
        printf( (info.st_mode & S_IWOTH) ? "w" : "-");
        printf( (info.st_mode & S_IXOTH) ? "x" : "-");
        printf("  %ld", info.st_nlink);
        printf("  %s", user->pw_name);
        printf("  %s", group->gr_name);
        printf("  %ld  ", info.st_size);
        printf("\t%s %d %d %d:%d\t", dates[date->tm_mon -1], date->tm_mday, date->tm_year+1900, date->tm_hour, date->tm_min); // not sure how to use this param
                
    }
}

void flagCheck(int location, char flagChar){
    if (flagBoolList[location]){
        // keep running with bad flags, notify user
        printf("Warning: Argument %c was repeatedly entered\n\n", flagChar);  
    } else {
        flagBoolList[location] = true;
        flagCount++;
    }
}

int main(int argc, char**argv){
    struct Files files;
    files.size = 0;
    char *argument;
    int length;
    
    for(int i = 1; i < argc; i++){ // start at one as argv[0] is ./myls
        argument = argv[i];

        if(argument[0]== '-'){ // flags
            // parse argument if contains multiple flags
            length = strlen(argument); 
            for(int j = 1; j < length; j++){
                switch(argument[j]){
                    case 'i':
                        flagCheck(0, 'i');
                        break;
                    case 'l':
                        flagCheck(1, 'l');
                        break;
                    case 'R':
                        flagCheck(2, 'R');
                        break;
                    default:
                        // keep running with bad flags, notify user  
                        printf("ERROR: %c in argument %s is not recognized.\nValid arguments are: -i, -l, -R (Case Sensitive)\n", argument[j], argument); 
                        exit(1);
                }
            }

        }else{ // files
            files.FileList[files.size] = argument;
            files.size++;
        }
    }

    printFiles(files);

    return 0;
}