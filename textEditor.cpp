// split into files
// make freed pointers be null
// clearing the console
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

void printMenu();
bool isValidCommand(char *line, int &command);
void clearInputBuffer();

int main() {
    char line[20];
    int command;
    printf("Welcome! Enter 'm' to see available commands.\n");
    do {
        printf("Please, choose your command: \n");
        fgets(line, sizeof(line), stdin);

        if (line[strlen(line) - 1] != '\n') {
            clearInputBuffer();
        } else {
            line[strlen(line) - 1] = '\0';
        }

        if (strcmp(line, "m") == 0)
            printMenu();
        else if (isValidCommand(line, command))
            printf("ok");
             // processCommand();
        else printf("Invalid command! Please, enter a number from 1 to 9.\n");
    } while (command != 9);
    return 0;
}

void printMenu() {
    printf("Possible commands:\n"
           "1. Append text symbols to the end\n"
           "2. Start a new line\n"
           "3. Use files for loading/saving the information\n"
           "4. Print the current text to console\n"
           "5. Insert the text by line and symbol index\n"
           "6. Search\n"
           "7. Clear the console\n"
           "8. Menu\n"
           "9. Exit\n\n");
}

bool isInteger(char *line) {
    while (*line) {
        if (!isdigit(*line)) {
            return false;
        }
        line++;
    }
    return true;
}

bool isValidCommand(char *line, int &command) {
    if (isInteger(line)) {
        command = atoi(line);
        if (command >= 1 && command <= 9) {
            printf("number");
            return true;
        }
    }
    return false;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}



