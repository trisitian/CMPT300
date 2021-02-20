/**This assignment was a collaboration between Jinhan Liu (301430717) and Tristian Labanowich (301422226).
 * This is a custom shell program
 * to run the shell run make then ./cshell
 * to run the shell with a script run make then ./cshell <scriptname>
 * This shell is compatible with all bash commands and can even run itself!
 * to exit the shell call "exit"
**/
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
    char name[255];
    struct tm *time;
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

// prints with proper colour formatting
void shell_print(int colour, const char* STRING){
    printf("\033[0;%im%s\033[0;37m", colour, STRING);
}

// theme command 
// outputs instructions if input is invalid
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
 * @returns                     size of arguments array
 * @param arguments             character array of arguments
 * @param input                 input from user or script
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
 * @returns                     new enviroment counter
 * @param command               command from user
 * @param EnvVariables          array of enviroment variables
 * @param enviromentCounter     counter for current max of envrioment variables
 * @param shell_colour          theme of cshell for printing
 * */
int assignENV(char* command, EnvVar EnvVariables[255], int enviromentCounter, int shell_colour){
    // parse name and value
    char *name = strtok(command, "=");
    memmove(name, name+1, strlen(name));    // remove '$'
    char *value = strtok(NULL, "=");
    
    // edge case: invalid inputs
    if ( value == NULL || name == NULL) {
        shell_print(shell_colour, "Error: did not specify variable name or value.\n");
        return enviromentCounter;
    }

    // if variable exists, change its value
    for (int j = 0; j < enviromentCounter; j++) {
        if (strcmp(EnvVariables[j].name, name) == 0) {
            strcpy(EnvVariables[j].value, value);
            return enviromentCounter;
        }
    }
    // otherwise push new variable into array EnvVariables
    EnvVar temp;
    strcpy(temp.name, name);
    strcpy(temp.value, value);
    EnvVariables[enviromentCounter++] = temp;

    return (enviromentCounter + 1);
}

/**
 * Print arguments passed by user
 * @param token                 argument to be printed
 * @param envriomentCounter     amount of enviroment variables
 * @param EnvVaribales          array of enviroment variables
 * @param shell_colour          theme of cshell for printing
 * returns nothing, prints inside function
 * */
void print_var(char* token, int enviromentCounter, EnvVar EnvVariables[255], int shell_colour){
    memmove(token, token+1, strlen(token));
    for (int j = 0; j < enviromentCounter; j++) {
        if (strcmp(EnvVariables[j].name, token) == 0) {
            shell_print(shell_colour, EnvVariables[j].value);
            printf(" ");
            return;
        }
    }
    //Only runs if enviroment variable is not defined
    shell_print(shell_colour, "Error "); 
    shell_print(shell_colour, token);
    shell_print(shell_colour, " Is not defined\n");
}

/**
 * Precheck changing theme
 * @returns                     new shell colour
 * @param shell_colour          current shell coulour
 * @param arguments             arguments array
 * */
int change_theme_auto(int shell_colour, char *arguments[15]){
    if (arguments[1] != NULL) {
        return change_theme(arguments[1], shell_colour);
    } else {
        return change_theme(" ", shell_colour);
    }
}
/**
 * A function that adds each command entered to the log
 * @param command_log, the log of all commands entered before current command
 * @param commandName, the name of the command entered
 * @param exitCode, the code either predefined in predefined functions or the fork return status
 * @param commandCounter, the amount of commands that were entered before this call
 * */
void addCommand(Command command_log[255], char *commandName, int exitCode, int commandCounter){
    time_t callTime;            // Global variable for when user calls command
    Command temp;               // Globabl temp variable for commands (to be passed to log)

    time(&callTime);
    strcpy(temp.name, commandName);
    temp.time = localtime(&callTime);
    temp.code = exitCode;
    command_log[commandCounter] = temp;
}

int main(int argc, char** argv){
    char input[255];            // input string;                            max 255 char
    char *arguments[15];        // parsed arguments from input;             max 15 arguments
    char command[255];          // first argument;                          max size 255 char
    
    EnvVar EnvVariables[255];   // user defined variables array;            max size 255 items
    Command command_log[255];   // past actions, timestamps & return value; max size 255 items

    //****we could readjust so that this initializes with NULL values and find the first null value to edit
    //****currently setting a global counter
    int enviromentCounter = 0;  // # of user defined variables
    int commandCounter = 0;     // # of commands that have been run (for indexing command_log)
    int argument_counter = 0;   // # of arguments, for indexing
    int shell_colour = 37;      // default white theme
    
    int script_mode = 0;
    FILE *script;

    // check if script mode is used
    if(argv[1] != NULL){
        script = fopen(argv[1], "r");
        if(script == NULL){
            printf("\nInvalid file path. File \"%s\" needs to be on the top level.\nRunning in user mode. \n\n", argv[1]);
        }else{
            script_mode = 1;    // turns on script mode
        }
    }

    while(strcmp(input, "exit") != 0){ 
        if (script_mode) {
            if (!fgets(input, sizeof(input), script)){  // program terminates at EOF
                fclose(script);
                return 0;
            }
        } else {    // interactive mode, get input
            shell_print(shell_colour, "cshell$ ");
            fgets(input, sizeof(input), stdin);
        }

        remove_newline(input);
        argument_counter = parseLine(arguments, input); 

        //*****BUG: issue with back to back spaces
        
        /**check arguments[0]:
         * If is starts with $ create a new enviroment variable
         * If its log print the Command array
         * If it is print take arguments[1] to arguments [n] and print each value
         * If it starts with theme take argument[1] and change the prompt colour
         * otherwise call fork() on argument[0] and pass arguments[1] through arguments[n]
         * */
        
        // edge case: read no character from file
        if (arguments[0] == NULL){
            continue;
        }
        strcpy(command, arguments[0]); // copy first argument as command

        if (command[0] == '$'){
            enviromentCounter = assignENV(command, EnvVariables, enviromentCounter, shell_colour);
            addCommand(command_log,arguments[0], 0, commandCounter); // Prints entire argument as per instructions
            commandCounter++;
        } else if (strcmp(command, "print") == 0) { // print arguments
            char *token;
            for (int i = 1; i < argument_counter; i++) {
                token = arguments[i];
                if (token[0] == '$') { // argument is EnvVar
                    print_var(token, enviromentCounter, EnvVariables, shell_colour);
                } else {
                    // strcat needs new variable to work
                    char var[strlen(arguments[i])]; 
                    strcpy(var, arguments[i]); 
                    shell_print(shell_colour, strcat(var, " "));
                }
            }
            printf("\n");
            addCommand(command_log,"Print", 0, commandCounter); 
            commandCounter++;
            
        } else if (strcmp(command, "theme") == 0){  // change theme
            int trace = shell_colour; // to check if the command returned properly
            shell_colour = change_theme_auto(shell_colour, arguments);
            if(trace == shell_colour){
                addCommand(command_log,"Theme", 1, commandCounter); 
                commandCounter++;
            }else{
                addCommand(command_log,"Theme", 0, commandCounter); 
                commandCounter++;
            }

        } else if (strcmp(command, "log") == 0) {   // output log
            if(commandCounter == 0){
                shell_print(shell_colour,"No commands have been entered yet\n");
            }else{
                for(int i = 0; i < commandCounter; i++){
                    shell_print(shell_colour, asctime(command_log[i].time));
                    shell_print(shell_colour,"  ");
                    shell_print(shell_colour, command_log[i].name);
                    shell_print(shell_colour, " ");
                    char code = command_log[i].code + '0'; // convert int to char
                    shell_print(shell_colour,&code);
                    printf(" \n");
                }
            }
            addCommand(command_log,"log", 0, commandCounter); 
            commandCounter++;

        } else if (strcmp(command, "exit") == 0) {  // exit shell
            shell_print(shell_colour, "Goodbye!\n");
            if (script_mode){
                fclose(script);
            }
            return 0;

        } else {    // for non built-in commands
            // create pipes
            int fd[2];
            if (pipe(fd) == -1) {
                shell_print(shell_colour, "Failed to execute command. (Failed to create pipe)");
                addCommand(command_log,arguments[0], -1, commandCounter); 
                commandCounter++;
            }
            // create fork
            int fork_value = fork();
            int fork_return_value; // for log
            if (fork_value < 0) {
                shell_print(shell_colour, "Failed to execute command. (Failed to create fork)");
                fork_return_value =-1;
            } else if (fork_value == 0) {   // Child
                fork_return_value = 0;
                close(fd[0]);
                arguments[argument_counter + 1] = NULL;
                
                // redirect io
                // dup2(fd[1], STDIN_FILENO);
                dup2(fd[1], STDOUT_FILENO);
                dup2(fd[1], STDERR_FILENO);

                int return_value = execvp(command, arguments);
                if (return_value == -1) {
                    write(fd[1], "\"", 1);
                    write(fd[1], command, strlen(command));
                    write(fd[1], "\" is not a recognized command.", 30);
                    exit(-1);
                }
                exit(0);
            } else {    // Parent
                close(fd[1]);
                wait(NULL);

                char buff[2];   // buffer
                while (read(fd[0], buff, 1)){
                    shell_print(shell_colour, buff);
                }

                printf("\n");
            }
            addCommand(command_log,arguments[0],fork_return_value,commandCounter); 
            commandCounter++;
        }



    }
    return 0;
}