#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

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
}

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
}

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
}

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
}

char* getFilename() { // add size validation
    printf("Enter the file name (no more than 10 characters):\n");
    char* name = (char*)malloc(11 * sizeof(char));
    if (name == NULL) {
        printf("Failed to allocate memory for file name.\n");
        return NULL;
    }
    if (fgets(name, 11, stdin) == NULL) {
        printf("Failed to read file name.\n");
        free(name);
        return NULL;
    }

    name[strcspn(name, "\n")] = '\0';
    return name;
}

void writeToFile(char* name, lineNode* head) {
    FILE* file;
    file = fopen(name, "w");
    if (file == NULL) {
        printf("Failed to open file %s for writing.\n", name);
        free(name);
        return;
    }

    lineNode *curLine = head;
    while (curLine != NULL) {
        if (fputs(curLine->text, file) == EOF) {
            printf("Failed to write to file %s.\n", name);
            fclose(file);
            return;
        }
        fputs("\n", file);
        curLine = curLine->next;
    }

    fclose(file);
    printf("Successfully wrote to file %s.\n", name);
}

void saveToFile(lineNode* head) {
    char* name = getFilename();
    if (name == NULL)
        return;

    writeToFile(name, head);
    free(name);
}

void loadFromFile(lineNode** head, int BUFFER_SIZE) { // refactor, read in etc.
    char* name = getFilename();
    if (name == NULL)
        return;

    FILE* file;
    file = fopen(name, "r");
    if (file == NULL) {
        printf("Failed to open file %s for loading.\n", name);
        free(name);
        return;
    }

    char buffer[BUFFER_SIZE];
    lineNode* curLine = *head;
    while (fgets(buffer, BUFFER_SIZE, file)) {
        buffer[strcspn(buffer, "\n")] = '\0';

        lineNode *newLine = createLine(BUFFER_SIZE);
        if (newLine == NULL) {
            fclose(file);
            free(name);
            return;
        }
        strncpy(newLine->text, buffer, BUFFER_SIZE);
        curLine->next = newLine;
        curLine = newLine;
    }
    fclose(file);
    printf("Successfully loaded from file %s.\n", name);
    free(name);
}

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

void searchSubstring(lineNode * head, const char* substring) {
    lineNode *curLine = head;
    int lineNumber = 1;
    bool found = false;

    while (curLine != NULL) {
        char* foundPos = strstr(curLine->text, substring);
        while (foundPos) {
            int position = foundPos - curLine->text + 1;
            printf("Found substring \"%s\" at line %d, position %d\n", substring, lineNumber, position);
            found = true;
            foundPos = strstr(foundPos + 1, substring);
        }
        curLine = curLine->next;
        lineNumber++;
    }
    if (!found)
        printf("Substring \"%s\" not found.\n", substring);
}

void handleSearch() { //add size validation
    char substring[31];
    printf("Enter the substring to search for (up to 30 symbols): \n");
    fgets(substring, sizeof(substring), stdin);
    substring[strcspn(substring, "\n")] = '\0';
    searchSubstring(head, substring);
}

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
            loadFromFile(currentLine, BUFFER_SIZE);
            break;
        case 5:
            printText();
            break;
        case 6:
            printf("Method to insert the text by line and symbol index\n");
            break;
        case 7:
            handleSearch();
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
