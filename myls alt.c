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

char dates[12][4] = {"Jan", "Feb", "Mar", "Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

// to keep track of flags used
bool flagBoolList[3] = {false, false, false}; // [0]: 'i', [1]: 'l', [2]: 'R'
int flagCount = 0;

struct Files{
    char *FileList[100]; // 100 files limit
    int size; // amount of files passed
};

struct dataset{
    char **fileindex; 
    char **filelong;
    char **filename;
    int size;
};

void initDataset(struct dataset data){
    data.fileindex = (char**) malloc(sizeof(char));
    data.filelong  = (char**) malloc(sizeof(char));
    data.filename  = (char**) malloc(sizeof(char));
    data.size = 0;
}

void increase(struct dataset data, int amount){
    if (amount != 1){
        data.fileindex = (char**) realloc(data.fileindex, sizeof(char) * (data.size + amount));
        data.filelong  = (char**) realloc(data.filelong, sizeof(char) * (data.size + amount));
        data.filename  = (char**) realloc(data.filename, sizeof(char) * (data.size + amount));
    }
    else if (data.size % 100 > 98){
        data.fileindex = (char**) realloc(data.fileindex, sizeof(char) * (data.size + 100));
        data.filelong  = (char**) realloc(data.filelong, sizeof(char) * (data.size + 100));
        data.filename  = (char**) realloc(data.filename, sizeof(char) * (data.size + 100));
    }
    data.size += amount;
}

void freeDataset(struct dataset data){
    free(data.fileindex);
    free(data.filelong);
    free(data.filename);
}

struct dataset organizeFiles(struct Files files){
    DIR *curr;
    char *temp;
    bool localfileCall = false;

    struct dirent *dp;
    struct stat info;
    struct dataset data;
    initDataset(data);

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
                    printf("%s \n", temp);
                    break;
                }else{
                    continue;
                }
            }

            if(dp->d_type == DT_DIR && flagBoolList[2]){ // -R
                // skip . and .. directories 
                if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
                    struct Files newFiles;
                    struct dataset newData;
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

                    newData = organizeFiles(newFiles); // recursively access subfolder
                    if (newData.size > 0){
                        increase(data, newData.size);
                        for (int i = 0; i < newData.size; i++){
                            data.fileindex[data.size] = newData.fileindex[i];
                            data.filelong[data.size] = newData.filelong[i];
                            data.filename[data.size] = newData.filename[i];
                            increase(data, 1);
                        }
                    }
                    freeDataset(newData);
                    free(newFiles.FileList[0]);
                }
            }

            if(flagBoolList[0]){ // -i
                data.fileindex[data.size] = (char*) dp->d_ino;
                // printf("%ld  ", dp->d_ino);
            }
        
            if(flagBoolList[1]){ // -l
                data.filelong[data.size] = "";
                stat(dp->d_name, &info);
                struct passwd *user = getpwuid(info.st_uid);
                struct group *group = getgrgid(info.st_gid);
                struct tm *date = localtime(&info.st_atim.tv_sec);
                strcat(data.filelong[data.size], (S_ISDIR(info.st_mode)) ? "d" : "-");
                strcat(data.filelong[data.size], (info.st_mode & S_IRUSR) ? "r" : "-");
                strcat(data.filelong[data.size], (info.st_mode & S_IWUSR) ? "w" : "-");
                strcat(data.filelong[data.size], (info.st_mode & S_IXUSR) ? "x" : "-");
                strcat(data.filelong[data.size], (info.st_mode & S_IRGRP) ? "r" : "-");
                strcat(data.filelong[data.size], (info.st_mode & S_IWGRP) ? "w" : "-");
                strcat(data.filelong[data.size], (info.st_mode & S_IXGRP) ? "x" : "-");
                strcat(data.filelong[data.size], (info.st_mode & S_IROTH) ? "r" : "-");
                strcat(data.filelong[data.size], (info.st_mode & S_IWOTH) ? "w" : "-");
                strcat(data.filelong[data.size], (info.st_mode & S_IXOTH) ? "x" : "-");
                strcat(data.filelong[data.size], "\t");
                strcat(data.filelong[data.size], (char*) info.st_nlink);
                strcat(data.filelong[data.size], "\t");
                strcat(data.filelong[data.size], (char*) user->pw_name);
                strcat(data.filelong[data.size], "\t");
                strcat(data.filelong[data.size], (char*) group->gr_name);
                strcat(data.filelong[data.size], "\t");
                strcat(data.filelong[data.size], (char*) info.st_size);
                strcat(data.filelong[data.size], "\t");
                strcat(data.filelong[data.size], dates[date->tm_mon -1]);
                strcat(data.filelong[data.size], date->tm_mday);
                strcat(data.filelong[data.size], date->tm_year+1900);
                strcat(data.filelong[data.size], date->tm_hour);
                strcat(data.filelong[data.size], date->tm_min);
                strcat(data.filelong[data.size], "\t");
                // printf( (S_ISDIR(info.st_mode)) ? "d" : "-");
                // printf( (info.st_mode & S_IRUSR) ? "r" : "-");
                // printf( (info.st_mode & S_IWUSR) ? "w" : "-");
                // printf( (info.st_mode & S_IXUSR) ? "x" : "-");
                // printf( (info.st_mode & S_IRGRP) ? "r" : "-");
                // printf( (info.st_mode & S_IWGRP) ? "w" : "-");
                // printf( (info.st_mode & S_IXGRP) ? "x" : "-");
                // printf( (info.st_mode & S_IROTH) ? "r" : "-");
                // printf( (info.st_mode & S_IWOTH) ? "w" : "-");
                // printf( (info.st_mode & S_IXOTH) ? "x" : "-");
                // printf("  %ld", info.st_nlink);
                // printf("  %s", user->pw_name);
                // printf("  %s", group->gr_name);
                // printf("  %ld  ", info.st_size);
                // printf("\t%s %d %d %d:%d\t", dates[date->tm_mon -1], date->tm_mday, date->tm_year+1900, date->tm_hour, date->tm_min);
                
            }
            data.filename[data.size] = "";
            strcat(data.filename[data.size], (char*) dp->d_name); 
            // printf("%s\n", dp->d_name);
            increase(data, 1);
        }
        if(localfileCall){
            printf("%s is not in the current directory\n", temp);
            localfileCall = false;
        }
        closedir(curr);
        files.size--;
    } while (files.size > 0);

    return data;
}

void printFiles(struct dataset data){
    for (int i = 0; i < data.size; i++){
        if(flagBoolList[0]){ // -i
            printf("%s  ", data.fileindex[i]);        
        }
        if(flagBoolList[1]){ // -l
            printf("%s  ", data.filelong[i]);        
        }
        printf("%s\n", data.filename[i]);
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
    struct dataset data;
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

    data = organizeFiles(files);
    printFiles(data);

    return 0;
}