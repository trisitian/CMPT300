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

static void printFiles(int options){
    DIR *curr;
    struct dirent *dp;

    // open directory
    curr = opendir(".");
        if((curr == NULL)){
            perror("Error opening directory \n");
            exit(1);
        }

    if(options == 0){ // no args 
        while((dp = readdir(curr)) != NULL){
            printf("%s \n", dp->d_name);
        }

    }else if(options ==1){ // -i
        while((dp = readdir(curr)) != NULL){
            printf("\t%ld \t%s \n", dp->d_ino, dp->d_name); // her example starts with a leading tab
        }

    }else if(options == 2){ // -l
        struct stat info;
        while((dp = readdir(curr))!= NULL){
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
                printf("\t%ld", info.st_nlink);
                printf("\t%s", user->pw_name);
                printf("\t%s", group->gr_name);
                printf("\t%ld", info.st_size);
                //printf("\t%s", info.st_atim); // not sure how to use this param
                printf("\t%s\n",dp->d_name);
            }
        }

    }else if(options ==3){
        printf("-R called");

    }else if(options ==4){
        char *temp;
        int search = 0;
        while(search <= files.size){
            temp = files.FileList[search];
            while((dp = readdir(curr))!= NULL){
                // compare each file
            }
        }
    }
    closedir(curr);
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
    char *temp;
    int length;
    
    //get flags
    for(int i = 1; i < argc; i++){ // start at one as argv[0] is ./myls
        //parse flags and file list
        temp = argv[i];
        if(temp[0]== '-'){ // flag
            length = strlen(temp); // to parse each argument if combining options
            for(int j = 1; j < length; j++){
                switch(temp[j]){
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
                        printf("ERROR: %c in argument %s is not recognized.\nValid arguments are: -i, -l, -R (Case Sensitive)\n", temp[j], temp); 
                        exit(1);
                }
            }

        }else{ // files to add to file list
            files.FileList[files.size] = temp;
            files.size++;
        }
    }

    if(files.size == 0 && flagCount == 0){ // call script on just current folder
        printFiles(0);

    // ***************ELSE IF CAUSING NOT PRINTING STACKED ARGUMENTS*********************************
    }else if(flagBoolList[0]){
        printFiles(1);

    }else if(flagBoolList[1]){
        printFiles(2);

    }else if(flagBoolList[3]){
        printFiles(3);

    }else if(files.size > 0){
        printFiles(4);

    }else{
        printf("Unknown error.\n");
        exit(1);
    }
    return 0;
}