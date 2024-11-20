#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define DATA_FILE "/.cash"

typedef struct {
    uint64_t money;
} PersistentData;

FILE* open_data_file(const char* mode)
{
    const char* home = getenv("HOME");
    char* path = malloc(strlen(home) + strlen(DATA_FILE) + 1);
    strcpy(path, home);
    strcat(path, DATA_FILE);

    FILE* file = fopen(path, mode);
    free(path);
    return file;
}

PersistentData get_persistent_data(void)
{
    PersistentData data = { 0 };
    FILE* file = open_data_file("r");

    if (file) {
        fread(&data, sizeof(PersistentData), 1, file);
        fclose(file);
    }

    return data;
}

void save_persistent_data(PersistentData* data)
{
    FILE* file = open_data_file("w");

    if (file) {
        fwrite(data, sizeof(PersistentData), 1, file);
        fclose(file);
    } else {
        fprintf(stderr, "Failed to save data.\n");
    }
}

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

// Gambling program execution
ErrorCode exec_gamble(char** argv, PersistentData* data)
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
        if (data->money < 10L) {
            data->money = 0;
        } else {
            data->money -= 10;
        }
        fprintf(stderr, "You lost $10.\n");
    } else {
        printf("Command succeeded!\n");
        data->money += 10;
        printf("You won $10.\n");
    }

    save_persistent_data(data);

    return status;
}

int main(int argc, char** argv)
{
    PersistentData data = get_persistent_data();
    printf("You have $%lu.\n", data.money);

    if (argc > 1) {
        return exec_gamble(argv, &data);
    }

    // Normal gambling stuff here
    return 0;
}
