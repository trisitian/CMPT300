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
struct dirent **direntList;
int numOfFiles = 0;

// void combineDirentLists(struct dirent **direntListB, int listBSize){
//     struct dirent **newDirentList = malloc(sizeof(struct dirent **) * (listBSize + numOfFiles + 1));
//     int a = 0, b = 0;

//     for (int i = 0; i < (listBSize + numOfFiles); i++){
//         if (a < numOfFiles && b < listBSize){
//             if (alphasort(&direntList[a], &direntListB[b]) < 0){
//                 newDirentList[i] = direntList[a];
//                 a++;
//             } else {
//                 newDirentList[i] = direntListB[b];
//                 b++;
//             }
//         } else if (a >= numOfFiles) {
//             newDirentList[i] = direntListB[b];
//                 b++;
//         } else {
//             newDirentList[i] = direntList[a];
//             a++;
//         }
//     }
//     direntList = newDirentList;
//     free(newDirentList);
// }

void readFiles(char* dir, struct dirent **dirList){
    
    numOfFiles = scandir(dir, &direntList, NULL, alphasort);
    // printf("%i", numOfFiles);
    if (numOfFiles == -1){
        printf("Error indexing directory %s\n", dir);
        exit(1);
    }

    if(flagBoolList[2]){ // -R
        struct dirent **newDirentList;
        struct dirent *dp;

        for(int i = 0; i < numOfFiles; i++){
            dp = direntList[i];
            if (dp->d_type == DT_DIR){
                if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
                    char *newDir = malloc(sizeof(char*) * 255);

                    // use folder name as the directory to be accessed
                    if (strcmp(dir, ".") == 0){
                        strcpy(newDir, "./");
                    } else {
                        strcpy(newDir, dir);
                        strcat(newDir, "/");
                    }
                    strcat(newDir, dp->d_name);

                    readFiles(newDir, newDirentList); // recursively access subfolder
                    free(newDir);
                }
            }
            // combineDirentLists(newDirentList);   
        }
    }
}

void printFiles(struct Files files){
    char *dir = "";
    // DIR *curr;
    char *temp;
    struct dirent *dp;
    struct stat info;
    bool localfileCall = false;

    // for each custom path it will loop back
    // executing at least once for '.'
    do{
        // open directory
        if (files.size == 0){
            dir = ".";

        } else {
            temp = files.FileList[files.size -1];
            stat(temp, &info);
            if(S_ISDIR(info.st_mode)){
                dir = files.FileList[files.size - 1];
            }else{
                localfileCall = true;
                dir = ".";
            }
        }

        if(strcmp(dir, "") == 0){
                printf("Error opening directory %s\n", files.FileList[files.size - 1]);
                exit(1);
            }

        // read directory
        if (!localfileCall){
            readFiles(dir, direntList);
        }

        for(int i = 0; i < numOfFiles; i++){
            dp = direntList[i];
            
            if(localfileCall){ // read file
                if(strcmp(temp, dp->d_name) == 0){
                    localfileCall = false;
                    printf("%s \n", temp);
                    break;
                }else{
                    continue;
                }
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
            printf("%s\n", dp->d_name);
        }
        if(localfileCall){
            printf("%s is not in the current directory\n", temp);
            localfileCall = false;
        }
        // closedir(curr);
        files.size--;
    } while (files.size > 0);
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