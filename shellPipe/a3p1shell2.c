//Compile with g++

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
 
#include <sys/types.h>
#include <sys/wait.h>
 
// The array below will hold the arguments: args[0] is the command
static char* args[512];
pid_t pid;
int command_pipe[2];
 
#define READ  0
#define WRITE 1

static void split(char* cmd);
static int command(int input, int first, int second);
static char* skipwhite(char* s);
static int run(char* cmd, int input, int first, int second);
static void cleanup(int n);
static char line[1024];
static int n = 0; // number of calls to 'command'
 
int main()
{
	while (1) {
		// Print the prompt
		printf("$$ ");
		fflush(NULL);
 
		// Read a command line
		if (!fgets(line, 1024, stdin)) 
			return 0;
 
		int input = 0;
		int first = 1;
 
		char* cmd = line;
		char* next = strchr(cmd, '|'); // Find first '|' if present
 
		// check if a pipe is present and execute this if block
		if (next!=NULL){
			*next = '\0';
			input = run(cmd,input,first,0);
			cmd = next+1;
			next = strchr(cmd, '|'); // Find next '|' if present
			first = 0;
			if(next!=NULL){
				*next = '\0';
				input = run(cmd,input,first,1);
				cmd = next+1;
			}
		}else{
			first=0;
		}
		input = run(cmd, input, first, 0);
		cleanup(n);
		n = 0;
	}
	return 0;
}
 
static void split(char* cmd);
 
static int run(char* cmd, int input, int first, int second)
{
	split(cmd);
	if (args[0] != NULL) {
		if (strcmp(args[0], "exit") == 0) 
			exit(0);
		n += 1;
		return command(input, first, second);
	}
	return 0;
}
 
static char* skipwhite(char* s)
{
	while (isspace(*s)) ++s;
	return s;
}
 
static void split(char* cmd)
{
	cmd = skipwhite(cmd);
	char* next = strchr(cmd, ' ');
	int i = 0;
 
	while(next != NULL) {
		next[0] = '\0';
		args[i] = cmd;
		++i;
		cmd = skipwhite(next + 1);
		next = strchr(cmd, ' ');
	}
 
	if (cmd[0] != '\0') {
		args[i] = cmd;
		next = strchr(cmd, '\n');
		next[0] = '\0';
		++i; 
	}
 
	args[i] = NULL;
}

static int command(int input, int first, int second)
{
	int pipettes[2];
 
	pipe( pipettes );	
	pid = fork();
 
	if (pid == 0) {
		if (first == 1 && second == 0 && input == 0) {
			// First command
			dup2( pipettes[WRITE], STDOUT_FILENO );
		} else if (first == 0 && second == 1 && input != 0) {
			// Second command
			dup2(input, STDIN_FILENO);
			dup2(pipettes[WRITE], STDOUT_FILENO);
		} else {
			// Third command
			dup2( input, STDIN_FILENO );
		}
 
		if (execvp( args[0], args) == -1)
			_exit(EXIT_FAILURE); // If child fails
	}
 
	if (input != 0) 
		close(input);
 
	// Nothing more needs to be written
	close(pipettes[WRITE]);
 
	// If it's the third command, nothing more needs to be read
	if (second == 0 && first == 0)
		close(pipettes[READ]);
 
	return pipettes[READ];
}
 
static void cleanup(int n)
{
	int i;
	for (i = 0; i < n; ++i) 
		wait(NULL); 
}