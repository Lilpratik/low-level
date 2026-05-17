#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {

    char input[1024];

    char *args[100];

    char *args1[100];
    char *args2[100];

    while (1) {

        printf("mysh> ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // remove newline
        input[strcspn(input, "\n")] = 0;

        // ---------------- PIPE HANDLING ----------------

        char *pipe_pos = strchr(input, '|');

        if (pipe_pos != NULL) {

            // split string into left and right command
            *pipe_pos = '\0';

            char *left = input;
            char *right = pipe_pos + 1;

            // tokenize left command
            int j = 0;

            char *token = strtok(left, " ");

            while (token != NULL) {
                args1[j++] = token;
                token = strtok(NULL, " ");
            }

            args1[j] = NULL;

            // tokenize right command
            j = 0;

            token = strtok(right, " ");

            while (token != NULL) {
                args2[j++] = token;
                token = strtok(NULL, " ");
            }

            args2[j] = NULL;

            // create pipe
            int pipefd[2];

            pipe(pipefd);

            // first child
            pid_t pid1 = fork();

            if (pid1 == 0) {

                // stdout -> pipe write end
                dup2(pipefd[1], STDOUT_FILENO);

                close(pipefd[0]);
                close(pipefd[1]);

                execvp(args1[0], args1);

                perror("exec1 failed");
                exit(1);
            }

            // second child
            pid_t pid2 = fork();

            if (pid2 == 0) {

                // stdin <- pipe read end
                dup2(pipefd[0], STDIN_FILENO);

                close(pipefd[1]);
                close(pipefd[0]);

                execvp(args2[0], args2);

                perror("exec2 failed");
                exit(1);
            }

            // parent closes both ends
            close(pipefd[0]);
            close(pipefd[1]);

            wait(NULL);
            wait(NULL);

            continue;
        }

        // ---------------- NORMAL COMMANDS ----------------

        int i = 0;

        char *token = strtok(input, " ");

        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }

        args[i] = NULL;

        // skip empty input
        if (args[0] == NULL) {
            continue;
        }

        // built-in exit
        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        // built-in cd
        if (strcmp(args[0], "cd") == 0) {

            if (args[1] == NULL) {
                printf("missing argument\n");
            } else {
                chdir(args[1]);
            }

            continue;
        }

        // create child process
        pid_t pid = fork();

        if (pid == 0) {

            execvp(args[0], args);

            perror("exec failed");

            exit(1);

        } else {

            wait(NULL);
        }
    }

    return 0;
}
