#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

/*void printMenu();
bool isValidCommand(char *line, int &command);
void clearInputBuffer();
void processCommand(int command);*/

typedef struct lineNode {
    char *text;
    int textSize;
    struct lineNode *next;
} Line;

lineNode *head = NULL; // move to main??

lineNode *createLine(int BUFFER_SIZE) {
    lineNode *line = (lineNode*)calloc(1, sizeof(lineNode));
    if (line == NULL) {
        printf("Failed. Unable to allocate memory for line.\n");
        return NULL;
    }

    line->text = (char*)calloc(BUFFER_SIZE, sizeof(char));
    if (line->text == NULL) {
        printf("Failed. Unable to allocate memory for line text.\n");
        free(line);
        return NULL;
    }
    line->textSize = BUFFER_SIZE;
    line->next = NULL;
    return line;
}

void appendText(lineNode *line, int BUFFER_SIZE) {
    printf("Please, enter some text you would like to append (no more than 99 characters): \n");
    char input[BUFFER_SIZE];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    int inputLength = strlen(input);
    int currentTextLength = strlen(line->text);

    if (line->textSize < inputLength + currentTextLength + 1) {
        line->textSize += BUFFER_SIZE;
        char *temp = (char*)realloc(line->text, line->textSize); //free and null
        if (temp == NULL) {
            printf("Failed to allocate memory for line text.\n");
            return;
        }
        line->text = temp;
    }
    strcat(line->text, input);
}

lineNode* addLine(lineNode *line, int BUFFER_SIZE) {
    lineNode *newLine = createLine(BUFFER_SIZE);
    newLine->next = line->next;
    line->next = newLine;
    printf("New line is started!\n");
    return newLine;
}

// add validation
void printText() {
    Line *curLine = head;
    while (curLine != NULL) {
        printf("%s\n", curLine->text);
        curLine = curLine->next;
    }
}

void printMenu() {
    printf("Possible commands:\n"
           "1. Append text symbols to the end\n"
           "2. Start a new line\n"
           "3. Use files for saving the information\n"
           "4. Use files for loading the information\n"
           "5. Print the current text to the console\n"
           "5. Insert the text by line and symbol index\n"
           "6. Search\n"
           "8. Exit\n\n");
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
        if (command >= 1 && command <= 8) {
            return true;
        }
    }
    return false;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void processCommand(int command, lineNode **currentLine, int BUFFER_SIZE) {
    switch (command) {
        case 1:
            appendText(*currentLine, BUFFER_SIZE);
            break;
        case 2:
            *currentLine = addLine(*currentLine, BUFFER_SIZE);
            break;
        case 3:
            printf("Method to use files to saving the information\n");
            break;
        case 4:
            printf("Method to use files to loading the information\n");
            break;
        case 5:
            printText();
            break;
        case 6:
            printf("Method to insert the text by line and symbol index\n");
            break;
        case 7:
            printf("Method to search\n");
            break;
        case 8: // consider cases when finishing abruptly
            printf("Okay, bye!\n");
            break;
        default:
            printf("Unexpected command received. Please enter a number from 1 to 8.\n");
            break;
    }
}

int main() {
    char commandLine[3];
    int command;
    const int BUFFER_SIZE = 100;

    head = createLine(BUFFER_SIZE);
    if (head == NULL)
        return 1;

    printf("Welcome! Enter 'm' to see available commands.\n");
    Line *currentLine = head;
    do {
        printf("Please, choose your command: \n");
        fgets(commandLine, sizeof(commandLine), stdin);

        if (commandLine[strlen(commandLine) - 1] != '\n')
            clearInputBuffer();
        else commandLine[strlen(commandLine) - 1] = '\0';

        if (strcmp(commandLine, "m") == 0)
            printMenu();
        else if (isValidCommand(commandLine, command))
            processCommand(command, &currentLine, BUFFER_SIZE);
        else printf("Invalid command! Please, enter a number from 1 to 8.\n");
    } while (command != 8);
    return 0;
}
