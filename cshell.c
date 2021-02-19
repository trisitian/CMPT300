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

void remove_newline(char* input) {
    for (int i = 0; i < 255; i++) {
        if (input[i] == '\n') {
            input[i] = '\0'; 
            break;
        }
    }
}

//prints with proper colour formatting.
void shell_print(int colour, const char* STRING){
    printf("\033[0;%im%s\033[0;37m", colour, STRING);
}

//theme command. Outputs instructions if input is invalid.
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

/**
 * Parse line into arguments
 * @returns sizeof arguments array
 * @param arguments, a character array of arguments
 * @param input, the input that the user or script has enetered
 * */
int parseLine(char *arguments[15], char input[255]){
    int argument_counter = 0;
    const char delimeter[2] = " ";
    arguments[argument_counter] = strtok(input, delimeter);
    while(arguments[argument_counter] != NULL ){
        arguments[++argument_counter] = strtok(NULL, delimeter); // get rest of the arguments
    }
    return argument_counter;
}

/**
 * Create new user defined variable (EnvVar)
 * @returns new enviroment counter
 * @param command, the command that was enter
 * @param EnvVariables, the array of enviroment variables
 * @param enviromentCounter, a counter for the current max of envrioment variables
 * 
 * */
int assignENV(char* command, EnvVar EnvVariables[255], int enviromentCounter){
    char *name = strtok(command, "="); // parse name and value, push into array EnvVariables
    memmove(name, name+1, strlen(name)); // remove '$'
    char *value = strtok(NULL, "=");
        
    EnvVar temp;
    strcpy(temp.name, name);
    strcpy(temp.value, value);
    EnvVariables[enviromentCounter++] = temp;

    return (enviromentCounter + 1);
}

/**
 * Print arguments passed by user
 * @param envriomentCounter, the amount of enviroment variables that exist
 * @param EnvVaribales, the array of enviroment variables
 * Does not return anything, instead prints inside the function
 * */
void print_var(char* token, int enviromentCounter, EnvVar EnvVariables[255], int shell_colour){
    memmove(token, token+1, strlen(token));
    for (int j = 0; j < enviromentCounter; j++) {
        if (strcmp(EnvVariables[j].name, token) == 0) {
            shell_print(shell_colour, strcat(EnvVariables[j].value , " "));
        }
    }
}

/**
 * Precheck changing theme
 * @returns, the new shell colour
 * @param shell_colour, the current shell coulour
 * @param arguments,the arguments array
 * */
int change_theme_auto(int shell_colour, char *arguments[15]){
    if (arguments[1] != NULL) {
        return change_theme(arguments[1], shell_colour);
    } else {
        return change_theme(" ", shell_colour);
    }
}

int main(int argc, char** argv){
    char input[255];            // user input string; max 255 char
    char *arguments[15];        // store arguments from input; max 15 arguments
    char command[255];          // store first argument as command; max size 255
    EnvVar EnvVariables[255];   // store user defined variables; max size 255
    Command command_log[255];   // store past actions, timestamps & return var; max size 255

    //**we could readjust so that this initializes with NULL values and find the first null value to edit
    int enviromentCounter = 0;  // currently setting a global counter

    int argument_counter = 0;   // for indexing & storing number of arguments
    int shell_colour = 37;      // default white theme
    int script_mode = 0;
    FILE *script;

    //If you are passing in a script run things automatically
    if(argv[1] != NULL){
        script = fopen(argv[1], "r");
        if(script == NULL){
            printf("\nInvalid file path. File \"%s\" needs to be on the top level.\nRunning in user mode. \n\n", argv[1]);
        }else{
            script_mode = 1;
        }
    }

    while(strcmp(input, "exit") != 0){ 
        
        if (script_mode) {
            if (!fgets(input, sizeof(input), script)){
                fclose(script);
                return 0;
            }
        } else {
            shell_print(shell_colour, "cshell$ ");
            fgets(input, sizeof(input), stdin);
        }

        remove_newline(input);
        argument_counter = parseLine(arguments, input); 

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
            enviromentCounter = assignENV(command, EnvVariables, enviromentCounter);
        } else if (strcmp(command, "print") == 0) {
            char *token;
            for (int i = 1; i < argument_counter; i++) {
                token = arguments[i];
                if (token[0] == '$') { // look through enviroment variables NEED TO ADD EDGE CASE FOR IF THE ENV VARIBALE DOES NOT EXIST / IF THERE ARE NO ENV VARIABLES
                    print_var(token, enviromentCounter, EnvVariables, shell_colour);
                } else {
                    char var[strlen(arguments[i])]; // need to make a copy of string for concatination to work
                    strcpy(var, arguments[i]);
                    shell_print(shell_colour, strcat(var, " "));
                }
            }
            printf("\n");
        } else if (strcmp(command, "theme") == 0){  // change theme
            shell_colour = change_theme_auto(shell_colour, arguments);
        } else if (strcmp(command, "log") == 0) {   // output log

        } else if (strcmp(command, "exit") == 0) {  // exit shell
            shell_print(shell_colour, "Goodbye!\n");
            if (script_mode){
                fclose(script);
            }
            return 0;
        } else {                                    // for non built-in commands
            int fd[2];
            if (pipe(fd) == -1) {
                shell_print(shell_colour, "Failed to pass command into shell. (Failed to create pipe)");
            }

            int fork_value = fork();
            if (fork_value < 0) {
                shell_print(shell_colour, "Failed to pass command into shell. (Failed to create fork)");
                continue;

            } else if (fork_value == 0) {   // Child
                close(fd[0]);
                arguments[argument_counter + 1] = NULL;
                // dup2(fd[1], STDIN_FILENO);
                dup2(fd[1], STDOUT_FILENO);
                dup2(fd[1], STDERR_FILENO);
                int return_value = execvp(command, arguments);
                if (return_value == -1) {
                    write(fd[1], command, strlen(command));
                    write(fd[1], " is not a recognized command.", 29);
                }
                // close(fd[1]);
                exit(0);

            } else {                        // Parent
                close(fd[1]);
                char buff[2];
                printf("\n");
                while (read(fd[0], buff, 1)){
                    shell_print(shell_colour, buff);
                }
                wait(NULL);
                // close(fd[0]);
                printf("\n");
            }
        }
    }
    return 0;
}