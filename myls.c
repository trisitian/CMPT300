#include <stdio.h>
#include <string.h>
#include <stdbool.h>


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
struct flags{
    char flagList[5]; // pretty sure you can only have 3 flags max but have 5 just in case 
    int size; // current amount of flags, will be used when applying options
};


int main(int argc, char**argv){
    struct flags args;
    args.size = 0;
    char *temp;
    int length;
    bool used[3] = {false, false, false}; // to keep track of flags used
    //get flags
    for(int i =0; i <argc; i++){
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
        }
        
    }
}