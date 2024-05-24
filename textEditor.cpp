#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

struct lineNode;
void clearInputBuffer();
void freeMemory(lineNode **head);

typedef struct lineNode {
    char *text;
    int textSize;
    struct lineNode *next;
} lineNode;

lineNode *head = NULL; // global head pointer

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
    line->text[0] = '\0';
    line->textSize = BUFFER_SIZE;
    line->next = NULL;
    return line;
} // OK

bool initializeCurrentLine(lineNode **curLine, int BUFFER_SIZE) {
    if (*curLine == NULL) {
        *curLine = createLine(BUFFER_SIZE);
        if (*curLine == NULL) {
            printf("Failed to allocate memory for new line.\n");
            return false;
        }
        head = *curLine;
    }
    return true;
} // check using in addline and loadfromfile

bool ensureCapacity(lineNode *curLine, int additionalLength, int BUFFER_SIZE) {
    int currentTextLength = strlen(curLine->text);
    if (curLine->textSize < currentTextLength + additionalLength + 1) {
        curLine->textSize += BUFFER_SIZE;
        char *temp = (char*)realloc(curLine->text, curLine->textSize);
        if (temp == NULL) {
            printf("Failed to allocate memory for line text.\n");
            return false;
        }
        curLine->text = temp;
    }
    return true;
} // OK

void appendText(lineNode **curLine, int BUFFER_SIZE) {
    if (!initializeCurrentLine(curLine, BUFFER_SIZE))
        return;

    printf("Please, enter some text you would like to append. \n");
    char input[BUFFER_SIZE];

    while (fgets(input, BUFFER_SIZE, stdin)) {
        input[strcspn(input, "\n")] = '\0';
        int inputLength = strlen(input);

        if (!ensureCapacity(*curLine, inputLength, BUFFER_SIZE))
            return;

        strcat((*curLine)->text, input);

        if (inputLength < BUFFER_SIZE - 1)
            break;
    }
} // OK

lineNode* addLine(lineNode *curLine, int BUFFER_SIZE) {
    lineNode *newLine = createLine(BUFFER_SIZE);
    if (newLine == NULL)
        return NULL;

    if (curLine != NULL) {
        newLine->next = curLine->next;
        curLine->next = newLine;
    } else head = newLine;
    printf("New line is started.\n");
    return newLine;
} // OK

char* getUserInput(const char* prompt, int maxSize) {
    printf("%s\n", prompt);
    char* input = (char*)calloc(maxSize + 2, sizeof(char));
    if (input == NULL) {
        printf("Failed to allocate memory for input.\n");
        return NULL;
    }

    if (fgets(input, maxSize + 2, stdin) == NULL) {
        printf("Failed to read input.\n");
        free(input);
        return NULL;
    }

    if (input[strlen(input) - 1] != '\n') {
        printf("Input exceeds %d characters. Try again.\n", maxSize);
        clearInputBuffer();
        free(input);
        return NULL;
    } else {
        input[strcspn(input, "\n")] = '\0';
    }

    return input;
} // OK

FILE* openFile(const char* prompt, const char* mode, char** filename) {
    *filename = getUserInput(prompt, 10);
    if (*filename == NULL)
        return NULL;

    FILE* file = fopen(*filename, mode);
    if (file == NULL) {
        printf("Failed to open file %s.\n", *filename);
        free(*filename);
        *filename = NULL;
    }

    return file;
} // OK

void closeFile(FILE* file, char* filename) {
    fclose(file);
    printf("Operation on file %s completed successfully.\n", filename);
    free(filename);
}

void saveToFile(lineNode* head) {
    char* filename;
    FILE* file = openFile("Enter the file name (up to 10 characters):", "w", &filename);
    if (file == NULL)
        return;

    lineNode *curLine = head;
    while (curLine != NULL) {
        if (fputs(curLine->text, file) == EOF) {
            printf("Failed to write to file %s.\n", filename);
            fclose(file);
            return;
        }
        fputs("\n", file);
        curLine = curLine->next;
    }
    closeFile(file, filename);
} // OK

void loadFromFile(lineNode** head, int BUFFER_SIZE) {
    char* filename;
    FILE* file = openFile("Enter the file name (up to 10 characters):", "r", &filename);
    if (file == NULL)
        return;

    if (*head != NULL) {
        freeMemory(head);
    }

    char buffer[BUFFER_SIZE];
    lineNode* curLine = NULL;
    while (fgets(buffer, BUFFER_SIZE, file)) {
        buffer[strcspn(buffer, "\n")] = '\0';

        lineNode *newLine = createLine(BUFFER_SIZE);
        if (newLine == NULL) {
            fclose(file);
            free(filename);
            return;
        }
        strncpy(newLine->text, buffer, BUFFER_SIZE - 1);
        newLine->text[BUFFER_SIZE - 1] = '\0';
        if (curLine == NULL) {
            *head = newLine;

        } else {
            curLine->next = newLine;
        }
        curLine = newLine;
    }
    closeFile(file, filename);


} // fix

void printText() {
    if (head == NULL) {
        printf("The text is empty yet. Please, enter something first.\n");
        return;
    }

    lineNode *curLine = head;
    while (curLine != NULL) {
        printf("%s\n", curLine->text);
        curLine = curLine->next;
    }
} // OK

void searchSubstring(lineNode *head) {
    if (head == NULL) {
        printf("The text is empty. Please, enter something first.\n");
        return;
    }

    char* substring = getUserInput("Enter the substring to search for (up to 30 symbols):", 30);
    if (substring == NULL)
        return;

    lineNode *curLine = head;
    int lineNumber = 1;
    bool found = false;
    while (curLine != NULL) {
        char* foundPos = strstr(curLine->text, substring);
        while (foundPos) {
            int position = foundPos - curLine->text + 1;
            printf("Found substring %s at line %d, position %d\n", substring, lineNumber, position);
            found = true;
            foundPos = strstr(foundPos + 1, substring);
        }
        curLine = curLine->next;
        lineNumber++;
    }
    if (!found)
        printf("Substring %s not found.\n", substring);
    free(substring);
} // refactor

void freeMemory(lineNode **head) {
    lineNode *current;
    while (*head != NULL) {
        current = *head;
        *head = (*head)->next;
        free(current->text);
        current->text = NULL;
        free(current);
        current = NULL;
    }
    *head = NULL;
};

//------------------------------------------------------------------------------------------------------------------------------------------
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
            appendText(currentLine, BUFFER_SIZE);
            break;
        case 2:
            *currentLine = addLine(*currentLine, BUFFER_SIZE);
            break;
        case 3:
            saveToFile(head);
            break;
        case 4:
            loadFromFile(&head, BUFFER_SIZE);
            break;
        case 5:
            printText();
            break;
        case 6:
            printf("Method to insert the text by line and symbol index\n");
            break;
        case 7:
            searchSubstring(head);
            break;
        case 8: // consider cases when finishing abruptly
            freeMemory(&head);
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

    lineNode *currentLine = NULL;

    printf("Welcome! Enter 'm' to see available commands.\n");
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
