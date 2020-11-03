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
#include <readline/readline.h>
#include <readline/history.h>

#define MAXCHAR 2048 //Max number of characters to be supported.
#define MAXARG 512  //Max number of arguments to be supported.
#define PIDCHAR '$'

int pid;
int childProcess_id;

struct cmd_line
{
  char* arg;
  char** argList;//List of extra arguments from the user.
  int numArg; // Ammount of arguments
  int bgProcess; //TF value 1 TRUE or 0 FALSE || If '&' char is detected properly then is TRUE
  char *inputName;
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
    printf("running parse_CMD\n");
    if(strcmp(userInput, "\n") == 0 ||strcmp(userInput, "\0") == 0 ){
      cmdInput->argList = NULL;
      cmdInput->numArg = 0;
      cmdInput->bgProcess = 0;
      printf("string was null\n");

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


    const char space[2] = " ";


    // //Take the first word in the line
    // //Allocate space in the struct
    // cmdInput->argList[0] = calloc(strlen(token) + 1, sizeof(char));
    // //Coppy the token into the struct
    // strcpy(cmdInput->argList[0], token);
    // fflush(stdout);
    i = 0;
    char *token = strtok(userInput, space);
    cmdInput->argList = calloc(strlen(token) * MAXARG , sizeof(char));
    cmdInput->arg = calloc(strlen(token), sizeof(char));
    while(strcmp(token, "\0" ) != 0){

        // Check for > to find file input
      if(strcmp(token, ">") == 0){
        printf("Found >\n");
        token = strtok(NULL,  " ");
        strcpy(cmdInput->inputName, token);
      }
      // Check for > to find file output
      else if(strcmp(token, "<") == 0){
        printf("Found <\n");
        token = strtok(NULL,  " ");
        strcpy(cmdInput->outputName, token);
      }
      else{
        //Allocate space in the struct
        cmdInput->argList[i] = calloc(strlen(token) + 1, sizeof(char));
        //Take each arg  of the the line and token it
        token = strtok(NULL, space);
        printf("Current token at space %i: %s\n", i, token);
        //Coppy the token into the struct
        strcpy(cmdInput->argList[i], token);
        //add to the total arg array
        cmdInput->arg = strcat(cmdInput->arg, token);
        if (cmdInput->argList[i] == '&') {
          cmdInput->bgProcess = 1;
        }
      }
      cmdInput->numArg = i;
      i++;
    }


    printf("cmdInput->argList[i]: at parse: %s\n", cmdInput->argList[0] );
    return cmdInput;
}

/*
    run_status()
    run_status will print the satus of the last exited child
    input: child status
    output: void
*/
void run_status(){

}
/*
    forks the current procrss and runs the command as a child
*/
int run_Command(struct cmd_line *cmdInput){
    char currDir[MAXCHAR];
    size_t size = MAXCHAR;
    char arg[MAXCHAR];
    int childStatus;
    printf("run_Command cmdInput->argList[0] = %s \n",  cmdInput->argList[0]);
    if(strcmp(cmdInput->argList[0], "\n") == 0 || strcmp(cmdInput->argList[0], "\0") == 0 ){
    return 0;
    }
    char *newargv[] = {cmdInput->argList[0], cmdInput->arg, NULL};

    //--------------------------------------------------------
    // Echo
    if (strcmp(cmdInput->argList[0], "echo") == 0 ) {
      printf(":%s\n", cmdInput->arg);
      fflush(stdout);
      return 1;
    }
    else if (strcmp(cmdInput->argList[0], "echo\0") == 0 && cmdInput->numArg == 0) {
      printf(":\n");
      fflush(stdout);
      return 1;
    }
    // Echo
    else if(strcmp(cmdInput->argList[0], "exit") == 0 ){
      exitProgram();
    }
    //--------------------------------------------------------
    //CD
    else if(strcmp(cmdInput->argList[0], "cd\n") == 0) {
      chdir(getenv("HOME"));
      getcwd(currDir, size);
      printf("You are now in: %s: \n",currDir);
      waitpid(childStatus, childProcess_id, 0 );
      return 1;
    }
    else if(strcmp(cmdInput->argList[0], "cd") == 0){
      printf("Executing cd with argument '%s'\n", cmdInput->arg);
      chdir(cmdInput->argList[0]);
      getcwd(currDir, size);
      printf("You are now in: %s: \n",currDir);
      waitpid(childStatus, childProcess_id, 0 );
      return 1;
    }
    //CD
    // //--------------------------------------------------------
    // // < && >
    // for(size_t j = 0; j < cmdInput->numArg; j++){
    //   printf("argList at %i: %s\n", j , cmdInput->argList[j] );
    //   if(strcmp(cmdInput->argList[j], ">") == 0){
    //     printf("Detected '>'\n");
    //   }
    //   else if(strcmp(cmdInput->argList[j], "<") == 0){
    //     printf("Detected '<'\n");
    //   }
    // }
    // // < && >
    // //--------------------------------------------------------





    else{
      return 1;
      char *newargv[] = {cmdInput->argList[0] , cmdInput->argList[0], NULL};

    }
    pid_t spawnPid = fork();
    switch(spawnPid){
      case -1:
         perror("fork()\n");
         exit(1);
         break;
      case 0:
         // In the child process
         printf("CHILD(%d) running command\n", getpid());
         // Replace the current program with "/bin/ls"
         execvp(newargv[0], newargv);
         // exec only returns if there is an error
         perror("execve");
         exit(2);
         break;
      default:
         // In the parent process
         // Wait for child's termination
         waitpid(childStatus, childProcess_id, 0 );
         spawnPid = waitpid(spawnPid, &childStatus, 0);
         printf("PARENT(%d): child(%d) terminated. Exiting\n", getpid(), spawnPid);
         exit(0);
         break;
       }
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
  else{
  printf("\n");
  fflush(stdout);
  return userInput;
  }
  return userInput;
}

int main(int argc, char const *argv[]) {
    char* cmdLine;
    start_shell();
    printf("PID: %d\n", pid);
    do{
    cmdLine =  calloc(MAXCHAR + 1, sizeof(char));
    strcpy(cmdLine,cmd_prompt());
    printf("cmdline: %s\n", cmdLine);
    if (strcmp(cmdLine, "\n") == 0 || strcmp(cmdLine, "\0") == 0 ) {
      printf("empty string\n");
      continue;
    }
    else{
      run_Command(parse_CMD(cmdLine));
    }
    }while (strcmp(cmdLine,"___EOF___") != 0);
    exitProgram();


    //
    // execv(newargv[0], newargv);
    /* exec returns only on error */
    // perror("execv");
    // exit(EXIT_FAILURE);







  return 0;
}
