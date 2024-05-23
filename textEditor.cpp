#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

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

void appendText(lineNode *line, int BUFFER_SIZE) { // handle >99 chars
    printf("Please, enter some text you would like to append (no more than 99 characters): \n");
    char input[BUFFER_SIZE];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    int inputLength = strlen(input);
    int currentTextLength = strlen(line->text);

    if (line->textSize < inputLength + currentTextLength + 1) {
        line->textSize += BUFFER_SIZE;
        char *temp = (char*)realloc(line->text, line->textSize);
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

    Line *curLine = head;
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

// add validation
void printText() {
    Line *curLine = head;
    while (curLine != NULL) {
        printf("%s\n", curLine->text);
        curLine = curLine->next;
    }
}

void searchSubstring(const char* substring, char* name) {
    FILE* file; // move to separate func
    file = fopen(name, "r");
    if (file == NULL) {
        printf("Failed to open file %s for searching.\n", name);
        return;
    }
    char line[256];
    int lineNumber = 1;
    bool found = false;
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        char* foundPos = strstr(line, substring);
        while (foundPos) {
            int position = foundPos - line + 1; // +1 to make position 1-indexed
            printf("Found substring \"%s\" at line %d, position %d\n", substring, lineNumber, position);
            found = true;
            foundPos = strstr(foundPos + 1, substring);
        }
        lineNumber++;
    }
    fclose(file);
    if (!found)
        printf("Substring \"%s\" not found in file %s.\n", substring, name);
}

void handleSearch() {
    char* name = getFilename();
    if (name == NULL)
        return;

    char substring[31];
    printf("Enter the substring to search for (up to 30 symbols): \n");
    fgets(substring, sizeof(substring), stdin);
    substring[strcspn(substring, "\n")] = '\0';
    searchSubstring(substring, name);
    free(name);
}

void freeMemory(Line **head) {
    Line *current;
    while (*head != NULL) {
        current = *head;
        *head = (*head)->next;
        free(current->text);
        current->text = NULL;
        free(current);
        current = NULL;
    }
};

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
