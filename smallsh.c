/* Program Name: smallsh.c
** Author: Adam Oberg
** Description: In this assignment you will write smallsh your own shell in C.
smallsh will implement a subset of features of well-known shells, such as bash.


This program will...

  X Provide a prompt for running commands
  X Handle blank lines and comments, which are lines beginning with the # character
  --Provide expansion for the variable $$
  --Execute 3 commands exit, cd, and status via code built into the shell
  --Execute other commands by creating new processes using a function from the exec family of functions
  --Support input and output redirection
  --Support running commands in foreground and background processes
  --Implement custom handlers for 2 signals, SIGINT and SIGTSTP
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXCHAR 2048 //Max number of characters to be supported.
#define MAXARG 512  //Max number of arguments to be supported.
#define PIDCHAR '$'

int pid;

struct cmd_line
{
  char* command; //Initial command from the user
  char** argList;//List of extra arguments from the user.
  int numArg; // Ammount of arguments
  int bgProcess; //TF value 1 TRUE or 0 FALSE || If '&' char is detected properly then is TRUE
};

void exitProgram(int failure){
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

  if(failure == 1){
    exit(EXIT_FAILURE);
  }
  else{
    exit(EXIT_SUCCESS);

  }
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
    while(currChar != '\n'){
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
    if(strcmp(userInput, "\n") == 0){
      cmdInput->command = NULL;
      cmdInput->argList = NULL;
      cmdInput->numArg = 0;
      cmdInput->bgProcess = 0;
    return cmdInput;
    }


    char *token = strtok_r(userInput, " " , &saveptr); //Take the first word in the line
    cmdInput->command = calloc(strlen(token) + 1, sizeof(char)); //Allocate space in the struct
    strcpy(cmdInput->command, token);//Coppy the token into the struct

    printf("Command is: %s\n", cmdInput->command); //Debug
    fflush(stdout);

    i = 0;
    cmdInput->argList = calloc(strlen(token) * MAXARG , sizeof(char));

    while(strcmp(saveptr, "\0" ) != 0){

      cmdInput->argList[i] = calloc(strlen(token) + 1, sizeof(char)); //Allocate space in the struct
      token = strtok_r(NULL ," ", &saveptr); //Take each arg  of the the line and token it
      strcpy(cmdInput->argList[i], token);//Coppy the token into the struct
      printf("Argument %d: %s\n", i, cmdInput->argList[i]); //Debug
      fflush(stdout);
      i++; //Increase i
    }




    return cmdInput;
}

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
    exitProgram(1);
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
  sleep(1);
  printf("\n");
  fflush(stdout);
  printf("==Success==\nYour CMD line argument is: %s\n", userInput);
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
    parse_CMD(cmdLine);
  }while (strcmp(cmdLine,"___EOF___") != 0);
    exitProgram(0);


    //
    // execv(newargv[0], newargv);
    /* exec returns only on error */
    // perror("execv");
    // exit(EXIT_FAILURE);







  return 0;
}
