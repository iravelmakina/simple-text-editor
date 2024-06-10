#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>

#define BUFFER_SIZE 100
#define MAX_FILENAME_LENGTH 21

class LineNode {
public:
    char *text;
    int capacity;
    LineNode *next;

    LineNode(int initialCapacity = BUFFER_SIZE) : text(nullptr), capacity(initialCapacity), next(nullptr) {
        text = new(std::nothrow) char[capacity];
        if (text) {
            text[0] = '\0';
        } else std::cerr << "Memory allocation failed for text." << std::endl;
    }

    ~LineNode() {
        delete[] text;
    }
};


class TextManager {
public:
    TextManager() : head(nullptr), currentLine(nullptr) {}

    ~TextManager() {
        freeMemory();
    }

    void appendText() {
        if (!currentLine) {
            currentLine = new LineNode();
            if (!head)
                head = currentLine;
        }

        std::cout << "Please, enter some text you would like to append: " << std::endl;
        char input[BUFFER_SIZE];

        while (fgets(input, sizeof(input), stdin)) {
            input[strcspn(input, "\n")] = '\0';

            if (input[0] == '\0') {
                std::cout << "Invalid input. Please enter a non-empty string: " << std::endl;
                continue;
            }

            int inputLen = strlen(input);
            if (!ensureCapacity(currentLine, inputLen)) return;

            strcat(currentLine->text, input);

            if (inputLen < BUFFER_SIZE - 1) {
                std::cout << "Text was appended successfully." << std::endl;
                break;
            }
        }
    }

    void addLine() {
        if (!currentLine) {
            currentLine = new LineNode();
            if (!head)
                head = currentLine;
        } else {
            LineNode *newLine = new LineNode();
            currentLine->next = newLine;
            currentLine = newLine;
        }
    }

    void saveToFile() {
        LineNode *curLine = head;
        if (!curLine) {
            std::cout << "The text is empty yet. Please, enter something first." << std::endl;
            return;
        }

        char filename[MAX_FILENAME_LENGTH];
        getUserInputString("Enter the file name (up to 20 characters):", filename, MAX_FILENAME_LENGTH);

        std::ofstream outFile(filename);
        if (!outFile) {
            std::cerr << "Failed to open file " << filename << ". Please, make sure it exists." << std::endl;
            return;
        }

        while (curLine) {
            outFile << curLine->text << std::endl;
            curLine = curLine->next;
        }
        std::cout << "Operation on file " << filename << " completed successfully." << std::endl;
    }

    void loadFromFile() {
        char filename[MAX_FILENAME_LENGTH];
        getUserInputString("Enter the file name (up to 20 characters):", filename, MAX_FILENAME_LENGTH);

        std::ifstream inFile(filename);
        if (!inFile) {
            std::cerr << "Failed to open file " << filename << ". Please, make sure it exists." << std::endl;
            return;
        }

        freeMemory();

        char buffer[BUFFER_SIZE];
        while (inFile.getline(buffer, BUFFER_SIZE)) {
            LineNode *newLine = new LineNode();
            strcpy(newLine->text, buffer);
            if (!currentLine) {
                head = newLine;
            } else currentLine->next = newLine;
            currentLine = newLine;
        }
        std::cout << "Operation on file " << filename << " completed successfully." << std::endl;
    }

    void printText() const {
        if (!head) {
            std::cout << "The text is empty yet. Please, enter something first." << std::endl;
            return;
        }

        LineNode *curLine = head;
        while (curLine) {
            std::cout << curLine->text << std::endl;
            curLine = curLine->next;
        }
    }

    void insertSubstring() {
        LineNode *curLine = nullptr;
        int lineIndex = 0;
        int charIndex = 0;
        int curTextLen = 0;

        if (!getLineAndIndices(curLine, lineIndex, charIndex, curTextLen)) return;

        char substring[31];
        getUserInputString("Enter the substring to insert (up to 30 symbols):", substring, 31);

        int substringLen = strlen(substring);
        if (!ensureCapacity(curLine, substringLen)) return;

        memmove(curLine->text + charIndex + substringLen, curLine->text + charIndex, curTextLen - charIndex + 1);
        memcpy(curLine->text + charIndex, substring, substringLen);
        std::cout << "Substring \"" << substring << "\" was inserted successfully into line "
                  << lineIndex + 1 << " starting at position " << charIndex + 1 << "." << std::endl;
    }

    void searchSubstring() const {
        if (!head) {
            std::cout << "The text is empty. Please, enter something first." << std::endl;
            return;
        }

        char substring[31];
        getUserInputString("Enter the substring to search for (up to 30 symbols):", substring, 31);

        LineNode *curLine = head;
        int lineNumber = 1;
        bool found = false;
        while (curLine) {
            char *pos = strstr(curLine->text, substring);
            while (pos) {
                int index = pos - curLine->text;
                std::cout << "Found substring '" << substring << "' at line " << lineNumber << ", position "
                          << index + 1 << std::endl;
                found = true;
                pos = strstr(pos + 1, substring);
            }
            curLine = curLine->next;
            ++lineNumber;
        }
        if (!found)
            std::cout << "Substring \"" << substring << "\" not found." << std::endl;
    }

    void printMenu() const {
        std::cout << "Possible commands:\n"
                  << "1. Append text to current line.\n"
                  << "2. Add a new line.\n"
                  << "3. Save the text to a file.\n"
                  << "4. Load the text from a file.\n"
                  << "5. Print the text.\n"
                  << "6. Insert a substring into the text.\n"
                  << "7. Search for a substring in the text.\n"
                  << "8. Exit.\n";
    }

    void publicClearInputBuffer(const std::string& errorMessage) {
        clearInputBuffer(errorMessage);
    }

    bool publicIsValidCommand(char *line, int &command) {
        return isValidCommand(line, command);
    }

    void processCommand(int command) {
        switch (command) {
            case 1:
                appendText();
                break;
            case 2:
                addLine();
                break;
            case 3:
                saveToFile();
                break;
            case 4:
                loadFromFile();
                break;
            case 5:
                printText();
                break;
            case 6:
                insertSubstring();
                break;
            case 7:
                searchSubstring();
                break;
            case 8:
                freeMemory();
                std::cout << "Okay, bye!" << std::endl;
                break;
            default:
                std::cout << "Unexpected command received. Please enter a number from 1 to 8." << std::endl;
                break;
        }
    }

private:
    LineNode *head;
    LineNode *currentLine;

    void freeMemory() {
        LineNode *current = head;
        while (current) {
            LineNode *next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
        currentLine = nullptr;
    }

    void clearInputBuffer(const std:: string& errorMessage) const {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << errorMessage << std::endl;
    }

    bool isInteger(const char *line) {
        int len = strlen(line);
        for (int i = 0; i < len; ++i) {
            if (!isdigit(line[i])) return false;
        }
        return true;
    }

    bool isValidCommand(char *line, int &command) {
        if (isInteger(line)) {
            command = atoi(line);
            if (command >= 1 && command <= 8) return true;
        }
        return false;
    }

    bool ensureCapacity(LineNode *curLine, int additionalLength) {
        if (!curLine || !curLine->text) return false;

        int currentLength = strlen(curLine->text);
        int newLength = currentLength + additionalLength + 1;

        if (curLine->capacity < newLength) {
            char *newBuffer = new(std::nothrow) char[newLength];
            if (!newBuffer) {
                std::cerr << "Failed to allocate memory for line text." << std::endl;
                return false;
            }

            strcpy(newBuffer, curLine->text);
            delete[] curLine->text;
            curLine->text = newBuffer;
            curLine->capacity = newLength;
        }
        return true;
    }

    int getUserInputInt(const char *prompt) {
        char input[BUFFER_SIZE];
        int value;

        while (true) {
            std::cout << prompt << std::endl;
            std::cin.getline(input, BUFFER_SIZE);

            if (std::cin.fail()) {
                clearInputBuffer("Input exceeds buffer size. Please enter a shorter input.");
                continue;
            }

            if (isInteger(input) && input[0] != '\0') {
                value = std::atoi(input);
                break;
            } else std::cout << "Invalid input. Please enter an integer value." << std::endl;
        }
        return value;
    }

    void getUserInputString(const char *prompt, char *output, int maxLength) const {
        while (true) {
            std::cout << prompt << std::endl;
            std::cin.getline(output, maxLength);

            if (std::cin.fail()) {
                clearInputBuffer("Input exceeds buffer size. Please enter a shorter command.");
                continue;
            }

            if (output[0] == '\0')
                std::cout << "Invalid input. Please enter a non-empty string." << std::endl;
            else break;
        }
    }

    bool getLineAndIndices(LineNode *&curLine, int &lineIndex, int &charIndex, int &curTextLen) {
        if (!head) {
            std::cout << "The text is empty. Please, enter something first." << std::endl;
            return false;
        }

        lineIndex = getUserInputInt("Enter the line index: ") - 1;
        charIndex = getUserInputInt("Enter the character index: ") - 1;

        curLine = head;
        int currentLineIndex = 0;

        while (curLine && currentLineIndex < lineIndex) {
            curLine = curLine->next;
            ++currentLineIndex;
        }

        if (!curLine) {
            std::cout << "Line index out of bounds." << std::endl;
            return false;
        }

        curTextLen = strlen(curLine->text);
        if (charIndex > curTextLen) {
            std::cout << "Character index out of bounds." << std::endl;
            return false;
        }
        return true;
    }
};


int main() {
    TextManager textManager;

    std::cout << "Welcome! Enter 'm' to see available commands." << std::endl;
    char commandLine[BUFFER_SIZE];
    int command;
    do {
        std::cout << "Please, choose your command: " << std::endl;
        std::cin.getline(commandLine, BUFFER_SIZE);

        if (std::cin.fail()) {
            textManager.publicClearInputBuffer("Input exceeds buffer size. Please enter a shorter command.");
            continue;
        }

        if (strcmp(commandLine, "m") == 0) {
            textManager.printMenu();
        } else if (textManager.publicIsValidCommand(commandLine, command)) {
            textManager.processCommand(command);
        } else {
            std::cout << "Invalid command! Please, enter a number from 1 to 8." << std::endl;
            continue;
        }
    } while (command != 8);
    return 0;
}