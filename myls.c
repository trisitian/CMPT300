#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h> // questions asked if theese were able to be used on piazza and we got the go ahead, theese libraries are for stat()
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>


/**
 *  Deciding to put legacy code up here, feel free to swap it out with current solutions if you think thats better
 *              if(temp[j] == 'i' || temp[j] == 'R' || temp[j] == 'l'){
                    args.flagList[args.size] = temp[j];
                    args.size++;
                }else{
                     printf("ERROR %c, OF ARGUMENT %s IS NOT RECOGNIZED:\n VALID ARGUMETS ARE -i, -l, -R (Case Sensitive)\n", temp[j], temp); // keep running with bad flags, notify user
                }
 * 
 * 
 * */

struct Flags{
    char flagList[5]; // pretty sure you can only have 3 flags max but have 5 just in case 
    int size; // current amount of flags, will be used when applying options
};

struct Files{
    char *FileList[100]; // doubt they are going to call more than 100 files
    int size; // current amount of files passed
};


struct Flags args;
struct Files files;

static void printFiles(int options){
    DIR *curr;
    struct dirent *dp;
    if(options == 0){ // no args 
        curr = opendir(".");
        if((curr == NULL)){
            perror("Error opening directory \n");
            exit(1);
        }
        while((dp = readdir(curr))!= NULL){
            printf("%s \n", dp->d_name);
        }
    }else if(options ==1){ // -i
        curr = opendir(".");
        if((curr == NULL)){
            perror("Error opening directory \n");
            exit(1);
        }
        while((dp = readdir(curr))!= NULL){
            printf("\t%ld \t%s \n", dp->d_ino, dp->d_name); // her example starts with a leading tab
        }
    }else if(options == 2){ // -l
        curr = opendir(".");
        struct stat info;
        if((curr == NULL)){
            perror("Error opening directory \n");
            exit(1);
        }
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
        curr = opendir(".");
        if((curr == NULL)){
            perror("Error opening directory \n");
            exit(1);
        }
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

int main(int argc, char**argv){
    args.size = 0;
    files.size = 0;
    char *temp;
    int length;
    bool used[3] = {false, false, false}; // to keep track of flags used
    //get flags
    for(int i =1; i <argc; i++){ // start at one as argv[0] is ./myls
        //parse flags and file list
        temp = argv[i];
        if(temp[0]== '-'){ // flag
            length = strlen(temp); // to parse each argument if combining options
            for(int j = 1; j < length; j++){
                switch(temp[j]){ // am I high or is this the best way to ensure someone doesn't type '-l -l'
                    case 'i':
                        if(used[0]){
                            printf("ERROR %c, OF ARGUMENT %s Has already been entered\n", temp[j], temp); // keep running with bad flags, notify user 
                        }else{
                            args.flagList[args.size] = temp[j];
                            args.size++;
                            used[0] = true;
                        }
                        break;
                    case 'l':
                        if(used[1]){
                            printf("ERROR %c, OF ARGUMENT %s Has already been entered\n", temp[j], temp); // keep running with bad flags, notify user 
                        }else{
                            args.flagList[args.size] = temp[j];
                            args.size++;
                            used[1] = true;
                        }
                        break;
                    case 'R':
                        if(used[2]){
                            printf("ERROR %c, OF ARGUMENT %s Has already been entered\n", temp[j], temp); // keep running with bad flags, notify user 
                        }else{
                            args.flagList[args.size] = temp[j];
                            args.size++;
                            used[2] = true;
                        }
                        break;
                    default:
                        printf("ERROR %c, OF ARGUMENT %s IS NOT RECOGNIZED:\n VALID ARGUMETS ARE -i, -l, -R (Case Sensitive)\n", temp[j], temp); // keep running with bad flags, notify user  

                }
            }
        }else{ // files to add to file list
            files.FileList[files.size] = temp;
            files.size++;
        }
    }
    if(files.size == 0 && args.size == 0){ // call script on just current folder
        printFiles(0);
    }else if(used[0]){ // clearly needs to be optimized, just trying to get a barebones file working
        printFiles(1);
    }else if(used[1]){
        printFiles(2);
    }else if(used[3]){
        printFiles(3);
    }else if(files.size > 0){
        printFiles(4);
    }else{
        printf("Unknown error\n");
        exit(1);
    }
}