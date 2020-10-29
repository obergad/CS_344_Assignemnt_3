/* Program Name: smallsh.c
** Author: Adam Oberg
** Description: In this assignment you will write smallsh your own shell in C.
smallsh will implement a subset of features of well-known shells, such as bash.


This program will...

  --Provide a prompt for running commands
  --Handle blank lines and comments, which are lines beginning with the # character
  --Provide expansion for the variable $$
  --Execute 3 commands exit, cd, and status via code built into the shell
  --Execute other commands by creating new processes using a function from the exec family of functions
  --Support input and output redirection
  --Support running commands in foreground and background processes
  --Implement custom handlers for 2 signals, SIGINT and SIGTSTP
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

#define MAXCHAR 2048 //Max number of characters to be supported.
#define MAXARG 512  //Max number of arguments to be supported.


struct cmd_line
{
  char* command; //Initial command from the user
  char** argList;//List of extra arguments from the user
  int argNum; //Number of arguments
  char* inputFile;//File path for the input file
  char* outputFile;//File Path for the output file
  int bgProcess; //TF value 1 TRUE or 0 FALSE
};

struct cmd_line *parse_CMD(char* userInput){
    struct cmd_line* cmdInput;
    cmdInput->bgProcess = 0;

    return cmdInput;
}

/*
This fucntion is just made to display the creator of the program.
Has no functionality
*/
void start_shell(){
    char *clear = "clear";
    system(clear);
    printf("\n\n\n\n******************"
    "************************");
    printf("\n\n\n\t****Small Shell****");
    printf("\n\n\t -By: Adam Oberg-");
    printf("\n\n\n\n*******************"
    "***********************");
    char* username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    sleep(1);
    system(clear);
}
/*
  Function: cmd_prompt
  Description: simply prompts the command prompt for the user and takes in a value
  Paramaters: NULL
  Return: A string that is the input of the user.
*/
char* cmd_prompt(){
  char* userInput;
  int successPrompt = 0; //Bool value stores 1 Success or 0 Fail
  while (successPrompt == 0) {
    userInput = NULL;
    printf(": ");
    scanf("%s", userInput);
    successPrompt = 1;
    printf("**Success**\nYour CMD line argument is: %s\n", userInput);
  }
  return userInput;
}

int main(int argc, char const *argv[]) {
  start_shell();
  cmd_prompt();
  return 0;
}
