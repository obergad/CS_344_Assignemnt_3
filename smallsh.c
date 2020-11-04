/* Program Name: smallsh.c
** Author: Adam Oberg
** Description: In this assignment you will write smallsh your own shell in C.
smallsh will implement a subset of features of well-known shells, such as bash.


This program will...

  XX Provide a prompt for running commands
  XX Handle blank lines and comments, which are lines beginning with the # character
  XX Provide expansion for the variable $$
  --Execute 3 commands exit, cd, and status via code built into the shell
  --Execute other commands by creating new processes using a function from the exec family of functions
  --Support input and output redirection
  --Support running commands in foreground and background processes
  --Implement custom handlers for 2 signals, SIGINT and SIGTSTP
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <signal.h>



#define MAXCHAR 2048 //Max number of characters to be supported.
#define MAXARG 512  //Max number of arguments to be supported.
#define PIDCHAR '$'

int pid;
int childProcess_id = 0;

struct cmd_line
{
  char* arg;
  char** argList;//List of extra arguments from the user.
  int numArg; // Ammount of arguments
  int bgProcess; //TF value 1 TRUE or 0 FALSE || If '&' char is detected properly then is TRUE
  char* inputName;
  char* outputName;
};
/*
This fucntion is just made to display the creator of the program.
Has no functionality
*/
void start_shell(){
    printf("\n******************"
    "************************");
    printf("\n\n\n\t****Small Shell****");
    printf("\n\n\t -By: Adam Oberg-");
    printf("\n\n\n\n*******************"
    "***********************");
    char* username = getenv("USER");
    printf("\nUSER is: @%s", username);
    printf("\n");
    fflush(stdout);
    sleep(1);
}
//Function to exit the program.
void exitProgram(){
  int i = 0;
  printf("Exiting");
  fflush(stdout);
  while (i != 3) {
    printf(".");
    sleep(1);             //This is for the meme
    fflush(stdout);
    i++;
  }
  sleep(1);
  printf("\n");
  fflush(stdout);
  exit(0);

}
/*
  Fucntion: parse_CMD
  Description: Pareses the users input into the struct cmd_line
  Input: char* of the userInput
  output: struct* cmd_line
*/
struct cmd_line *parse_CMD(char* userInput){
    struct cmd_line* cmdInput = malloc(sizeof(struct cmd_line));
    char *saveptr;
    char currChar = NULL;
    int i = 0;
    char* temp = calloc(MAXCHAR + 1, sizeof(char*));
    char pidstr[16];
    int bool = 0;
    cmdInput->inputName = calloc(MAXCHAR + 1, sizeof(char*));
    cmdInput->outputName = calloc(MAXCHAR + 1, sizeof(char*));
    if(strcmp(userInput, "\n") == 0 ||strcmp(userInput, "\0") == 0 ){
      cmdInput->argList = NULL;
      cmdInput->numArg = 0;
      cmdInput->bgProcess = 0;
    return cmdInput;
    }
    //==================================================================
    //Find newline and replace with \0
    	for (i=0; bool != 1 && i<MAXCHAR; i++) {
    		if (userInput[i] == '\n') {
    			userInput[i] = '\0';
    			bool = 1;
    		}
    	}
    //==================================================================
    // Handle $$
    i = 0;
    while(currChar != '\n' || currChar !='\0'){
      currChar = userInput[i];
      if (currChar == '\n' || currChar == '\0') {
          break;
      }
      if(userInput[i] == '$' && userInput[i+1] == '$'){
        pid = getpid();
        sprintf(pidstr, "%i", pid);
        temp = strcat(temp, pidstr);
        strcpy(userInput, temp);
        break;
      }
      temp[i] = currChar;
      i++;
    }
    // Handle $$
    //==================================================================

    i = 1;

    //Get the first string
    cmdInput->argList = malloc(sizeof(char*) * MAXARG + 1);
    char *token = strtok_r(userInput, " " , &saveptr); //Take the first word in the line
    cmdInput->argList[0] = calloc(strlen(token) + 1, sizeof(char)); //Allocate space in the struct
    // strcat (token, " ");
    strcpy(cmdInput->argList[0], token);//Coppy the token into the struct
    // strcat(cmdInput->argList[0], " ");


    cmdInput->arg = calloc(MAXCHAR, sizeof(char));

    while(strcmp(saveptr, "\0" ) != 0) {


          token = strtok_r(NULL ," ", &saveptr); //Take each arg  of the the line and token it
          // strcat(token, " ");
          cmdInput->arg = strcat(cmdInput->arg, token);
          // strcat(cmdInput->argList[i], " ");
          cmdInput->argList[i] = calloc(strlen(token) + 1, sizeof(char)); //Allocate space in the struct
          strcpy(cmdInput->argList[i], token);//Coppy the token into the struct
          fflush(stdout);
        if (strncmp(cmdInput->argList[i], "&", strlen("&")) == 0) {
          cmdInput->bgProcess = 1;
          cmdInput->argList[i] = NULL;

      }
      cmdInput->numArg = i;
      i++;
    }
    cmdInput->argList[i] = NULL;

    i = 0;
    // Check for > to find file input
    while ( i < cmdInput->numArg) {

      if(strncmp( cmdInput->argList[i], "<", strlen("<")) == 0) {
        printf("Found <\n");
        strcpy(cmdInput->inputName, cmdInput->argList[i + 1]);
        cmdInput->argList[i] = NULL;
        cmdInput->argList[i + 1] = NULL;
        i++;

      }
      // Check for > to find file output
      else if(strncmp( cmdInput->argList[i], ">", strlen(">")) == 0){
        printf("Found >\n");
        strcpy(cmdInput->outputName, cmdInput->argList[i + 1]);
        cmdInput->argList[i] = NULL;
        cmdInput->argList[i + 1] = NULL;
        i++;

      }
      i++;
    }


    return cmdInput;
}

/*
    run_status()
    run_status will print the satus of the last exited child
    input: child status
    output: void
*/
void run_status(){
    //code based on code shown in lecture / modules
    if(WIFEXITED(childProcess_id)){
      printf("exited normally with status %d\n", WEXITSTATUS(childProcess_id));
      fflush(stdout);
    }
    else{
      printf("exited not normally due to signal %d\n", WTERMSIG(childProcess_id));
      fflush(stdout);
    }
}
/*
    forks the current procrss and runs the command as a child
*/
int run_Command(struct cmd_line *cmdInput){
    int input;
    int output;
    int result;
    char currDir[MAXCHAR];
    size_t size = MAXCHAR;
    int childStatus;
    if(strcmp(cmdInput->argList[0], "\n") == 0 || strcmp(cmdInput->argList[0], "\0") == 0 ){
    return 0;
    }

    // //--------------------------------------------------------
    // // Echo
    // if (strcmp(cmdInput->argList[0], "echo") == 0 ) {
    //   printf(":%s\n", cmdInput->arg);
    //   fflush(stdout);
    //   return 1;
    // }
    // else if (strcmp(cmdInput->argList[0], "echo\0") == 0 && cmdInput->numArg == 0) {
    //   printf(":\n");
    //   fflush(stdout);
    //   return 1;
    // }
    // // Echo
    // else if(strcmp(cmdInput->argList[0], "exit") == 0 ){
    //   exitProgram();
    // }
    //--------------------------------------------------------
    //CD
    if(strcmp(cmdInput->argList[0], "cd\n") == 0) {
      chdir(getenv("HOME"));
      getcwd(currDir, size);
      printf("You are now in: %s: \n",currDir);
      waitpid(childStatus, &childProcess_id, 0 );
      return 1;
    }
    else if(strcmp(cmdInput->argList[0], "cd") == 0){
      printf("Executing cd with argument '%s'\n", cmdInput->arg);
      chdir(cmdInput->argList[1]);
      getcwd(currDir, size);
      printf("You are now in: %s: \n",currDir);
      waitpid(childStatus, &childProcess_id, 0 );
      return 1;
    }
    else if(strcmp(cmdInput->argList[0], "exit") == 0){
      exitProgram();
    }
    //CD
    // //--------------------------------------------------------
    else if(strncmp(cmdInput->argList[0], "status", strlen("status")) == 0){
      run_status();
    }
    else{
      pid_t spawnPid = fork();
      switch(spawnPid){
        case -1:
           perror("fork()\n");
           exit(1);
           break;
        case 0:
           // In the child process
           // Handle input, code is basically straight from https://repl.it/@cs344/54redirectc#main.c
          if (strcmp(cmdInput->inputName, "")) {
            // open the file
            printf("cmdInput->inputName:%s.\n", cmdInput->inputName );
            fflush(stdout);
            input = open(cmdInput->inputName, O_RDONLY, 0644);
            if (input == -1) {
              perror("Unable to open the input file\n");
              exit(1);
            }
            // assign the file
            result = dup2(input, STDIN_FILENO);
            if (result == -1) {
              perror("Unable to assign the input file\n");
              exit(2);
            }
            // trigger its close
            fcntl(input, F_SETFD, FD_CLOEXEC);
          }

          // Handle output, code is basically straight from https://repl.it/@cs344/54redirectc#main.c
          if (strcmp(cmdInput->outputName, "")) {
            // open the file
            printf("cmdInput->outputName: %s \n", cmdInput->outputName );
            fflush(stdout);
            output = open(cmdInput->outputName, O_WRONLY | O_CREAT | O_TRUNC , 0644);
            if (output == -1) {
              perror("Unable to open output file\n");
              exit(1);
            }
            // assign the file
            result = dup2(output, STDOUT_FILENO);
            if (result == -1) {
              perror("Unable to assign output file\n");
              exit(2);
            }
            // close that bad boy
            fcntl(output, F_SETFD, FD_CLOEXEC);
          }
          fflush(stdout);

           execvp(cmdInput->argList[0], cmdInput->argList);
           // exec only returns if there is an error
           perror("execvp");
           exit(2);
           break;
        default:
           // In the parent process
           // Wait for child's termination
           waitpid(childStatus, &childProcess_id, 0 );
           spawnPid = waitpid(spawnPid, &childStatus, 0);
           // printf("PARENT(%d): child(%d) terminated. Exiting\n", getpid(), spawnPid);
           break;
         }
    }
    return -1;
}
/*
  Function: cmd_prompt
  Description: simply prompts the command prompt for the user and takes in a value
  Paramaters: NULL
  Return: A string that is the input from the user.
*/
char* cmd_prompt(){
  char* userInput;
  char *currLine = NULL;
  size_t max = MAXCHAR;
  int nread; //Number of bytes read by the getline function.
  userInput = calloc(MAXCHAR + 1, sizeof(char)); //Allocating space for userinput and clear userInput
  printf(":"); //Beginging of a newline in the command prompt.
  fflush(stdout);
  nread = getline(&currLine, &max , stdin); //stdin reads from the commandline
  userInput = currLine;
  if(nread > MAXCHAR){
    printf("Error: Max command length has been reached\n");
    fflush(stdout);
    exitProgram();
  }
  else if(strcmp(userInput, "\n") == 0 ){
    printf("Error: No input has been given.\n");
    fflush(stdout);
    return("\n");
  }
  else if(userInput[0] == '#' ){
    printf("\n");
    fflush(stdout);
    return("\n");   //if its a comment return a null character this is
  }
  else {
    return userInput;
  }
  return userInput;
}

int main(int argc, char const *argv[]) {
    char* cmdLine;
    start_shell();
    printf("PID: %d\n", pid);
    do {
    cmdLine =  calloc(MAXCHAR + 1, sizeof(char));
    strcpy(cmdLine,cmd_prompt());
    if (strcmp(cmdLine, "\n") == 0 || strcmp(cmdLine, "\0") == 0 ) {
      continue;
    }
    else {
      run_Command(parse_CMD(cmdLine));
    }
    } while (strcmp(cmdLine,"___EOF___") != 0);
    exitProgram();


    //
    // execv(newargv[0], newargv);
    /* exec returns only on error */
    // perror("execv");
    // exit(EXIT_FAILURE);







  return 0;
}
