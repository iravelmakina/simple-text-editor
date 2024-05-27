#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

// struct to represent nodes of linked list to store lines with text in arrays
typedef struct lineNode {
    char *text;
    int textSize;
    struct lineNode *next;
} lineNode;

// global head pointer
lineNode *head = NULL;

// func to clear buffer if input exceeds chars number allowed (overflow)
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

// func to check whether string is a valid integer
bool isInteger(char *line) {
    while (*line) {
        if (!isdigit(*line)) {
            return false;
        }
        line++;
    }
    return true;
}

// func to free memory allocated for linked list with text after executing program or while loading new text from file
void freeMemory(lineNode **localHead) {
    lineNode *current;
    while (*localHead != NULL) {
        current = *localHead;
        *localHead = (*localHead)->next;
        free(current->text);
        current->text = NULL;
        free(current);
        current = NULL;
    }
    *localHead = NULL;
};

// func to create a new lineNode and allocate memory for text
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
}

// func to initialize current line if it is NULL, create and set head if needed
lineNode *initializeCurrentLine(lineNode *curLine, int BUFFER_SIZE) {
    if (curLine == NULL) {
        curLine = createLine(BUFFER_SIZE);
        if (curLine == NULL) {
            printf("Failed to allocate memory for new line.\n");
            return NULL;
        }
        if (head == NULL) {
            head = curLine;
        }
    }
    return curLine;
}

// func to ensure current line has enough capacity to append new text
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
}

// func to append text to current line, ensuring enough capacity
void appendText(lineNode **curLine, int BUFFER_SIZE) {
    *curLine = initializeCurrentLine(*curLine, BUFFER_SIZE);
    if (*curLine == NULL)
        return;

    printf("Please, enter some text you would like to append. \n");
    char input[BUFFER_SIZE];

    while (fgets(input, BUFFER_SIZE, stdin)) {
        input[strcspn(input, "\n")] = '\0';
        int inputLength = strlen(input);

        if (inputLength == 0) {
            printf("Invalid input. Please enter a non-empty string: \n");
            continue;
        }

        if (!ensureCapacity(*curLine, inputLength, BUFFER_SIZE))
            return;

        strcat((*curLine)->text, input);

        if (inputLength < BUFFER_SIZE - 1) {
            printf("Text was appended successfully.\n");
            break;
        }
    }
}

// func to add a new line
lineNode* addLine(lineNode *curLine, int BUFFER_SIZE) {
    curLine = initializeCurrentLine(curLine, BUFFER_SIZE);
    if (curLine == NULL)
        return NULL;

    lineNode *newLine = createLine(BUFFER_SIZE);
    if (newLine == NULL)
        return NULL;

    newLine->next = curLine->next;
    curLine->next = newLine;
    printf("New line is started.\n");
    return newLine;
}

// func to get a valid string with restricted allowed size from user
char* getUserInputString(const char* prompt, int maxSize) {
    char* input = NULL;
    while (true) {
        printf("%s\n", prompt);
        input = (char*)calloc(maxSize + 2, sizeof(char));
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
        } else {
            input[strcspn(input, "\n")] = '\0';
            if (strlen(input) == 0) {
                printf("Invalid input. Please enter a non-empty string.\n");
                free(input);
            } else break;
        }
    }
    return input;
}

// func to get a valid integer input from user
int getUserInputInt(const char* prompt) {
    char buffer[10];
    int value;
    while (true) {
        printf("%s\n", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (buffer[strlen(buffer) - 1] != '\n') {
                clearInputBuffer();
            } else {
                buffer[strlen(buffer) - 1] = '\0';
            }
            if (strlen(buffer) == 0)
                printf("Invalid input. Please enter a non-empty integer.\n");
            else if (isInteger(buffer)) {
                value = atoi(buffer);
                break;
            } else {
                printf("Invalid input. Please enter a valid integer.\n");
            }
        }
    }
    return value;
}

// func to open file with certain mode and return file pointer
FILE* openFile(const char* prompt, const char* mode, char** filename) {
    *filename = getUserInputString(prompt, 20);
    if (*filename == NULL)
        return NULL;

    FILE* file = fopen(*filename, mode);
    if (file == NULL) {
        printf("Failed to open file %s. Please, make sure it exists.\n", *filename);
        free(*filename);
        *filename = NULL;
    }

    return file;
}

// func to close the file and free the filename
void closeFile(FILE* file, char* filename) {
    fclose(file);
    printf("Operation on file %s completed successfully.\n", filename);
    free(filename);
}

// func to save linked list text to file
void saveToFile() {
    char* filename;
    FILE* file = openFile("Enter the file name (up to 20 characters):", "w", &filename);
    if (file == NULL)
        return;

    lineNode *curLine = head;
    if (curLine == NULL) {
        printf("The text is empty yet. Please, enter something first.\n");
        return;
    }

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
}

// func to load text from file to linked list
void loadFromFile(lineNode** localHead, lineNode **currentLine, int BUFFER_SIZE) {
    char* filename;
    FILE* file = openFile("Enter the file name (up to 20 characters):", "r", &filename);
    if (file == NULL)
        return;

    if (*localHead != NULL) {
        freeMemory(localHead);
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
            *localHead = newLine;

        } else {
            curLine->next = newLine;
        }
        curLine = newLine;
    }
    *currentLine = curLine;
    closeFile(file, filename);
}

// func to print all the text saved in linked list
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
}

// func to insert certain substring to linked list using line index and position in line
void insertSubstring(lineNode *localHead, int BUFFER_SIZE) {
    if (localHead == NULL) {
        printf("The text is empty. Please, enter something first.\n");
        return;
    }

    int lineIndex = getUserInputInt("Enter the line index: ") - 1;
    int charIndex = getUserInputInt("Enter the character index: ") - 1;
    char* substring = getUserInputString("Enter the substring to insert (up to 30 symbols):", 30);

    if (substring == NULL)
        return;

    lineNode *curLine = localHead;
    int currentLine = 0;

    while (curLine != NULL && currentLine < lineIndex) {
        curLine = curLine->next;
        currentLine++;
    }

    if (curLine == NULL) {
        printf("Line index out of bounds. \n");
        free(substring);
        return;
    }

    int substringLength = strlen(substring);
    int currentTextLength = strlen(curLine->text);

    if (charIndex > currentTextLength) {
        printf("Character index out of bounds. \n");
        free(substring);
        return;
    }

    if (!ensureCapacity(curLine, substringLength, BUFFER_SIZE)) {
        free(substring);
        return;
    }

    memmove(curLine->text + charIndex + substringLength, curLine->text + charIndex, currentTextLength - charIndex + 1);
    memcpy(curLine->text + charIndex, substring, substringLength);

    printf("Substring %s was inserted successfully.\n", substring);
    free(substring);
}

// func to search certain substring in the linked list
void searchSubstring(lineNode *localHead) {
    if (localHead == NULL) {
        printf("The text is empty. Please, enter something first.\n");
        return;
    }

    char* substring = getUserInputString("Enter the substring to search for (up to 30 symbols):", 30);
    if (substring == NULL)
        return;

    lineNode *curLine = localHead;
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
}

void printMenu() {
    printf("Possible commands:\n"
           "1. Append text symbols to the end\n"
           "2. Start a new line\n"
           "3. Use files for saving the information\n"
           "4. Use files for loading the information\n"
           "5. Print the current text to the console\n"
           "6. Insert the text by line and symbol index\n"
           "7. Search\n"
           "8. Exit\n\n");
}

// func to check whether user input is a valid integer associated with command
bool isValidCommand(char *line, int &command) {
    if (isInteger(line)) {
        command = atoi(line);
        if (command >= 1 && command <= 8) {
            return true;
        }
    }
    return false;
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
            saveToFile();
            break;
        case 4:
            loadFromFile(&head, currentLine, BUFFER_SIZE);
            break;
        case 5:
            printText();
            break;
        case 6:
            insertSubstring(head, BUFFER_SIZE);
            break;
        case 7:
            searchSubstring(head);
            break;
        case 8:
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
