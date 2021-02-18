#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

typedef struct{
    char name[255];
    char value[255];
}EnvVar;

typedef struct{
    char *name;
    struct tm time;
    int code;
}Command;

//function to remove the newline character from fgets().
void remove_newline(char* input) {
    for (int i = 0; i < 255; i++) {
        if (input[i] == '\n') {
            input[i] = '\0'; 
            break;
        }
    }
}

/*
prints the string with the proper colour formatting
colour can be redefined by user. Default is red.
*/
void shell_print(int colour, const char* STRING){
    printf("\033[0;%im%s\033[0;37m", colour, STRING);
}

//code for the theme command. Will output instructions if input is invalid.
int change_theme(const char* NEW_COLOUR, int old_colour){
    if (strcmp(NEW_COLOUR, "red") == 0) {
        return 31;
    } else if (strcmp(NEW_COLOUR, "green") == 0) { 
        return 32;
    } else if (strcmp(NEW_COLOUR, "yellow") == 0) {
        return 33;
    } else if (strcmp(NEW_COLOUR, "blue") == 0) {
        return 34;
    } else if (strcmp(NEW_COLOUR, "purple") == 0) {
        return 35;
    } else if (strcmp(NEW_COLOUR, "cyan") == 0) {
        return 36;
    } else if (strcmp(NEW_COLOUR, "white") == 0) {
        return 37;
    } else if (strcmp(NEW_COLOUR, "help") == 0) {
        shell_print(old_colour, "\nAvailable themes are:\n\n");
        shell_print(31, "red\n");
        shell_print(32, "green\n");
        shell_print(33, "yellow\n");
        shell_print(34, "blue\n");
        shell_print(35, "purple\n");
        shell_print(36, "cyan\n");
        shell_print(37, "white\n");
        shell_print(old_colour, "\nEnter \"theme <colour>\" to change theme.\n\n");
        return old_colour;
    } else {
        shell_print(old_colour, "Unknown theme. Type \"theme help\" for more info.\n");
        return old_colour;
    }
}

int main(){
    char input[255]; // user input string, for a max of 255 characters
    char *arguments[15]; // pointer array that points to arguments; max 15 arguments
    const char delimeter[2] = " ";
    char command[255];
    EnvVar EnvVariables[255]; // enviroment variables array, currently set to 255
    Command command_log[255]; // log array
    int enviromentCounter = 0; // currently setting a global counter, we could readjust so that it initializes with NULL values and find the first null value to edit

    int shell_colour = 37; //default red theme colour value

    while(strcmp(input, "exit") != 0){ 
        
        shell_print(shell_colour, "cshell$ ");
        fgets(input, sizeof(input), stdin);
        remove_newline(input);

        int argument_counter = 0; // for indexing & storing number of arguments
        arguments[argument_counter] = strtok(input, delimeter); // get first argument
        while(arguments[argument_counter] != NULL ){
            arguments[++argument_counter] = strtok(NULL, delimeter); // get rest of the arguments
        }
        //*****BUG: cannot handle tab characters & issue with double
        
        /**check arguments[0]:
         * If is starts with $ create a new enviroment variable
         * If its log print the Command array
         * If it is print take arguments[1] to arguments [n] and print each value
         * If it starts with theme take argument[1] and change the prompt colour
         * otherwise call fork() on argument[0] and pass arguments[1] through arguments[n]
         * */
        
        strcpy(command, arguments[0]); // copy first argument as command

        if (command[0] == '$'){
            char *name = strtok(command, "="); // parse name and value, push into array EnvVariables
            memmove(name, name+1, strlen(name)); // remove '$'
            char *value = strtok(NULL, "=");
            
            EnvVar temp;
            strcpy(temp.name, name);
            strcpy(temp.value, value);

            EnvVariables[enviromentCounter++] = temp;
        } else if (strcmp(command, "print") == 0) {
            char *token;
            for (int i = 1; i < argument_counter; i++) {
                token = arguments[i];
                if (token[0] == '$') { // look through enviroment variables NEED TO ADD EDGE CASE FOR IF THE ENV VARIBALE DOES NOT EXIST / IF THERE ARE NO ENV VARIABLES
                    memmove(token, token+1, strlen(token));
                    for (int j = 0; j < enviromentCounter; j++) { // for some reason only prints the most recent value
                        if (strcmp(EnvVariables[j].name, token) == 0) {
                            printf("%s ", EnvVariables[j].value);
                        }
                    }
                } else {
                    printf("%s ", arguments[i]);
                }
            }
            printf("\n");
        } else if (strcmp(command, "theme") == 0){ // change theme
            if (arguments[1] != NULL) {
                shell_colour = change_theme(arguments[1], shell_colour);
            } else {
                shell_colour = change_theme(" ", shell_colour);
            }

        } else if (strcmp(command, "log") == 0) { // output log


        } else if (strcmp(command, "exit") == 0) { // exit shell
            shell_print(shell_colour, "Goodbye!\n");
            return 0;
        } else { // not built-in, pass into shell
            int fork_value = fork();
            if (fork_value < 0) {
                shell_print(shell_colour, "Failed to pass command into shell.");
                continue;
            } else if (fork_value == 0) {

            } else {
                wait(NULL);
            }






            printf("Current command is %s %d\n", command);
        }

    }
    shell_print(shell_colour, "Goodbye!\n");
    return 0;
}