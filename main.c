#include <ncurses.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "games/coinflip.c"

// TODO: File organsiation. This stuff should definitely be split up

#define DATA_FILE "/.cash"

typedef struct {
    uint64_t money;
    time_t dailyRewardClaimed;
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
    NO_ERROR = 0, // Don't rename to OK because ncurses uses it
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
    if (exitCode == EXEC_FAILED || exitCode == NO_ERROR) {
        return exitCode;
    }

    return ERROR;
}

// TODO: Move to separate file like with coinflip
// Gambling program execution
ErrorCode exec_gamble(char** argv, PersistentData* data)
{
    pid_t pid = fork();

    if (!pid) { // child
        argv++;
        execvp(argv[0], argv);
        exit(EXEC_FAILED);
    }
    
    // Test gambling using coinflip for now
    int result = coinflip();
    if (result == 0) {
        // TODO: we shouldn't be allowing the user to gamble in the first place if they don't have enough money
        if (data->money < 10L) {
            data->money = 0;
        } else {
            data->money -= 10;
        }
        fprintf(stderr, "You lost $10.\n");
    } else {
        data->money += 10;
        printf("You won $10.\n");
    }

    save_persistent_data(data);

    return result;

    ErrorCode status = child_status(pid);

    if (status == EXEC_FAILED) {
        fprintf(stderr, "Failed to execute command.\n");
    } else if (status == ERROR) {
        fprintf(stderr, "Command failed!\n");
        // TODO: we shouldn't be allowing the user to gamble in the first place if they don't have enough money
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

typedef struct {
    const char* text;
    int width; // For highlighted background
    int y;
    int x; // x coord of the first character
} Button;

// TODO: This should be dynamically loaded from games file
const char* const MENU_BUTTONS[] = { "Daily Reward", "Coin Flip", "Quit" };
const size_t MENU_BUTTONS_COUNT = sizeof(MENU_BUTTONS) / sizeof(char*);

/**
 * Create a dynamically allocated array of buttons that are displayed right underneath each other.
 *
 * texts: The content of the buttons.
 * count: The number of buttons.
 * y: The y coordinate of the first button.
 * x: The x coordinate of the first button.
 */
Button* create_vertical_buttons(const char* const* texts, int count, int y, int x)
{
    Button* buttons = malloc(count * sizeof(Button));
    int maxWidth = 0;

    for (int i = 0; i < count; i++) {
        buttons[i].text = texts[i];
        buttons[i].y = y + i;
        buttons[i].x = x;
        int length = strlen(texts[i]);
        if (length > maxWidth) {
            maxWidth = length;
        }
    }

    for (int i = 0; i < count; i++) {
        buttons[i].width = maxWidth;
    }

    return buttons;
}

void draw_button(const Button* button, bool selected)
{
    if (selected) {
        attron(A_REVERSE); // Highlight background
    }

    // Print button text with right padding
    mvprintw(button->y, button->x, " %-*s ", button->width, button->text);

    if (selected) {
        attroff(A_REVERSE); // Turn off highlight
    }
}

bool key_up(int key)
{
    return key == KEY_UP || key == 'k';
}

bool key_down(int key)
{
    return key == KEY_DOWN || key == 'j';
}

bool key_select(int key)
{
    return key == '\n' || key == KEY_ENTER || key == ' ';
}

int main(int argc, char** argv)
{
    PersistentData data = get_persistent_data();
    printf("You have $%lu.\n", data.money);

    if (argc > 1) {
        return exec_gamble(argv, &data);
    }

    // Normal gambling stuff here
    // NOTE: at some point exec_gamble should be using ncurses as well
    initscr();
    curs_set(0); // Hide cursor
    noecho(); // Don't show user input
    keypad(stdscr, true); // Enable arrow keys

    size_t selected = 0;

    Button* buttons = create_vertical_buttons(MENU_BUTTONS, MENU_BUTTONS_COUNT, 15, 1);
    for (size_t i = 0; i < MENU_BUTTONS_COUNT; i++) {
        draw_button(&buttons[i], i == selected);
    }

    int key;
    while (true) {
        key = getch();
        if (key_up(key) && selected > 0) {
            draw_button(&buttons[selected], false);
            selected--;
            draw_button(&buttons[selected], true);
        } else if (key_down(key) && selected < MENU_BUTTONS_COUNT - 1) {
            draw_button(&buttons[selected], false);
            selected++;
            draw_button(&buttons[selected], true);
        } else if (key_select(key)) {
            if (selected == 1) {
                exec_gamble(argv, &data);
            } else {
                break;
            }
        }
    }

    free(buttons);
    endwin();
    return 0;
}
