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

// to keep track of flags used
bool flagBoolList[3] = {false, false, false}; // [0]: 'i', [1]: 'l', [2]: 'R'
int flagCount = 0;

struct Files{
    char *FileList[100]; // 100 files limit
    int size; // amount of files passed
};

struct Files files;

static void printFiles(){
    DIR *curr;
    struct dirent *dp;
    struct stat info;

    // for each custom path it will loop back
    // executing at least once for '.'
    do{
        // open directory
        if (files.size == 0){
            curr = opendir(".");

        } else {
            curr = opendir(files.FileList[files.size - 1]);
        }

        if((curr == NULL)){
                printf("Error opening directory %s", files.FileList[files.size - 1]);
                exit(1);
            }

        // DEPRECATED? Maybe move into -R?
        // if(files.size > 0){
        //     char *temp;
        //     int search = 0;
        //     while(search <= files.size){
        //         temp = files.FileList[search];
        //         while((dp = readdir(curr))!= NULL){
        //             // compare each file
        //     }
        // }
        
        // read directory
        while((dp = readdir(curr)) != NULL){
            if(flagBoolList[0]){ // -i
                printf("%ld  ", dp->d_ino);
            }
        
            if(flagBoolList[1]){ // -l
                if(!stat(dp->d_name, &info)){
                    struct passwd *user = getpwuid(info.st_uid);
                    struct group *group = getgrgid(info.st_gid);
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
                    //printf("\t%s", info.st_atim); // not sure how to use this param
                }
            }

            if(flagBoolList[2]){ // -R
                printf("  -R called, but feature not implemented.  ");
            }
            
            printf("%s\n", dp->d_name);
        }

        closedir(curr);
        files.size--;
    } while (files.size > 0);
}

static void flagCheck(int location, char flagChar){
    if (flagBoolList[location]){
        // keep running with bad flags, notify user
        printf("Warning: Argument %c was repeatedly entered\n\n", flagChar);  
    } else {
        flagBoolList[location] = true;
        flagCount++;
    }
}

int main(int argc, char**argv){
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

    printFiles();

    return 0;
}