#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include "color.h"



void cls()
{
	system("clear");
	printf("%s", COLOR_RESET);
}
/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("nMSh");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(char **args)
{
  int i;
  printf("%snMUD Shell (NMSh)\n%s", COLOR_YELLOW, COLOR_RESET);
  printf("%sType program names and arguments, and hit enter.\n%s", COLOR_GREEN, COLOR_RESET);
  printf("%sThe following are built in:\n%s", COLOR_GREEN, COLOR_RESET);

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args)
{
  return 0;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *lsh_read_line(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "nMSh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "nMSh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "nMSh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "nMSh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("#> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
#define name1 name0
#define pass1 passwd

//-----------------------------------------------------------------------------------------------------------------//
//-------------------------------------------MAIN()-CODE-----------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------------------//
int main(int argc, char **argv[])
{
	char input[10353];
	time_t mytime;
	char login[16], password[16];
	mytime = time(NULL);
	//clear the screen
	cls();
	//Print welcome messages to the screen
	printf ("%sWelcome to nMUD!%s\n", COLOR_BRIGHTRED, COLOR_RESET);


   printf ("%sCreated by Nick Griebel, in the%s %sU%s.S.%sA%s\n", COLOR_RED, COLOR_RESET, COLOR_BRIGHTRED, COLOR_RESET, COLOR_BLUE, COLOR_RESET);

	printf ("%s", COLOR_GREEN);
	printf (ctime(&mytime));
	printf ("\n%s", COLOR_RESET);
	recio:
	//receive users input
	printf ("\n%s", COLOR_GREY);
	printf ("Please select a login option.%s\n", COLOR_RESET);
	printf ("%s1> %sExisting User%s\n", COLOR_GREY, COLOR_CYAN, COLOR_RESET);
	printf ("%s2> %sNew User%s\n", COLOR_GREY);
	printf ("$$$> ");
	scanf  ("%s", input);
	int n;
		char name0[16];
		char passwd[16];
		char * name;
   		char * pass;
		
		
	
	if (*input == '1')
			loginloop:
		{
		
			 
		
			 			
				if (n <= 3)

   			 {        
       				 printf ("Login now\n");
			   	 printf ("%sUSERNAME>> %s", COLOR_YELLOW, COLOR_GREY);
				 scanf  ("%s", name0);
				
				 
				 printf ("%s%sPASSWORD>> %s%s", COLOR_RESET, COLOR_YELLOW, LOCKCURSOR, PWMASK);
				
				 scanf  ("%s", passwd);
				 printf ("%s", RESETMASK);
				 	
	
        			 
   			 }
			 else
			 {
		           printf ("%sSorry, you exceeded your maximum allowed login attempts. You will be terminated. %s\n", COLOR_BRIGHTRED, COLOR_RESET);
			   printf ("%sGoodbye. %s\n", COLOR_BRIGHTRED, COLOR_RESET);
		   	 exit(1);			
		         }
		
   			   

   		 if(strcmp(name0,name1) == 0 && strcmp(passwd,pass1) == 0)
			{
       			printf("Correct username and password%s\n", COLOR_RESET);
		 goto loginsuccess;
   			}
		 else
			{
       			printf("Wrong username or password%s\n", COLOR_RESET);
			++n;
		
		 goto loginloop;
			}
	        
	//reset console color after program has run its course. 
printf("%s", COLOR_RESET);	
return 0;
		
		loginsuccess:
			
			printf ("%sWelcome back to nMUD!%s\n", COLOR_GREY, COLOR_RESET);
			lsh_loop();
		return EXIT_SUCCESS;
	}
	else if (*input == '2')	
	{
		printf ("%sUsername & Password limited to 16 characters each. Characters include a-z, A-Z, and 0-9.%s\n", COLOR_BRIGHTRED, COLOR_RESET);
		printf ("Enter New Username: ");
		scanf  ("%s", name0);
		printf ("Enter New Password: ");
		scanf  ("%s", passwd);
		
		goto loginloop;
	}
	else
	{
		printf ("%sError: Unrecognized input: '%s'%s", COLOR_BRIGHTRED, input, COLOR_RESET);
	        goto recio;

	}
}

