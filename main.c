#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

typedef enum {
    OK = 0,
    ERROR = 1,
    // TODO: probably use a better code
    EXEC_FAILED = 99
} ErrorCode;

ErrorCode child_status(pid_t pid)
{
    int status;
    waitpid(pid, &status, 0);

    if (WIFSIGNALED(status)) {
        return ERROR;
    }

    int exitCode = WEXITSTATUS(status);
    if (exitCode == EXEC_FAILED || exitCode == OK) {
        return exitCode;
    }

    return ERROR;
}

ErrorCode exec_gamble(char** argv)
{
    pid_t pid = fork();

    if (!pid) { // child
        argv++;
        execvp(argv[0], argv);
        exit(EXEC_FAILED);
    }

    ErrorCode status = child_status(pid);

    if (status == EXEC_FAILED) {
        fprintf(stderr, "Failed to execute command.\n");
    } else if (status == ERROR) {
        fprintf(stderr, "Command failed!\n");
        fprintf(stderr, "You lost $10.\n");
    } else {
        printf("Command succeeded!\n");
        printf("You won $10.\n");
    }

    return status;
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        return exec_gamble(argv);
    }

    // Normal gambling stuff here
    return 0;
}
