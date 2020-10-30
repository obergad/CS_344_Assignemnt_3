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
  char** argList;//List of extra arguments from the user.
  int numArg; // Ammount of arguments
  char* inputFile;//File path for the input file
  char* outputFile;//File Path for the output file
  int bgProcess; //TF value 1 TRUE or 0 FALSE || If '&' char is detected properly then is TRUE
};

/*
  Fucntion: parse_CMD
  Description: Pareses the users input into the struct cmd_line
  Input: char* of the userInput
  output: struct* cmd_line

*/
struct cmd_line *parse_CMD(char* userInput){
    struct cmd_line* cmdInput = malloc(sizeof(struct cmdLine));
    char buffer = " ";
    char *saveptr;

    char *token = strtok_r(userInput, buffer, &saveptr); //Take the first word in the line
    cmdInput->command = calloc(strlen(token) + 1, sizeof(char)); //Allocate space in the struct
    strcpy(cmdInput->command, token);//Coppy the token into the struct
    




    return cmdInput;
}

/*
This fucntion is just made to display the creator of the program.
Has no functionality
*/
void start_shell(){
    char *clear = "clear";
    printf("\n******************"
    "************************");
    printf("\n\n\n\t****Small Shell****");
    printf("\n\n\t -By: Adam Oberg-");
    printf("\n\n\n\n*******************"
    "***********************");
    char* username = getenv("USER");
    printf("\nUSER is: @%s", username);
    printf("\n");
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
  printf("\n: "); //Beginging of a newline in the command prompt.
  fflush(stdout);
  nread = getline(&currLine, &max , stdin); //stdin reads from the commandline
  userInput = currLine;

  if(nread > MAXCHAR){
    printf("Error: Max command length has been reached\n");
    fflush(stdout);
    exit(EXIT_FAILURE);
  }

  sleep(1);
  printf("\n");
  fflush(stdout);
  printf("**Success**\nYour CMD line argument is: %s\n", userInput);
  fflush(stdout);
  return userInput;
}

int main(int argc, char const *argv[]) {
    char* cmdLine = NULL;
    start_shell();
    while (strcmp(cmdLine,"___EOF___") == 0) {
      strcpy(cmdLine,cmd_prompt());
    }
    int i = 0;
    printf("Exiting");
    fflush(stdout);
    while (i != 3) {
      printf(".");
      sleep(1);             //This is for the meme
      fflush(stdout);
      i++;
    }

    //
    // char *newargv[] = { "/bin/ls", "-al", NULL };
    // execv(newargv[0], newargv);
    /* exec returns only on error */
    // perror("execv");
    // exit(EXIT_FAILURE);







  return 0;
}
