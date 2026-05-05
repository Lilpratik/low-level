#include <stdio.h> // for printf, fgets
#include <stdlib.h> // for exit
#include <unistd.h> // fork, exec
#include <sys/wait.h> // wait
#include <string.h> // strtok 

int main() {
    char input[1024]; // buffer to store user input
    char *args[100];

    while (1) {
	// infinite loop

	// 1. Print prompt
	printf("mysh> ");

	// 2. Read input from user
	if (fgets(input, sizeof(input), stdin) == NULL) {
	    break; // exit if error or EOF
	}

	// remove new line from the input
	input[strcspn(input, "\n")] = 0;

	// split input into tokens
	int i = 0;
	char *token = strtok(input, " ");
	while (token != NULL) {
	    args[i++] = token;
	    token = strtok(NULL, " ");
	}
	args[i] = NULL;

	// fork process
	pid_t pid = fork();

	if (pid == 0) {
	    // child process
	    execvp(args[0], args);
	    perror("exec failed"); // only runs if exec fails
	} else {
	    // parent process
	    wait(NULL);
	}
	
    }
    
    return 0;
}
