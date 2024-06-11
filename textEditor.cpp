#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
#include <stack>

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
    TextManager() : head(nullptr), currentLine(nullptr), clipboard(nullptr) {}

    ~TextManager() {
        freeMemory();
        while (!undoStack.empty()) {
            freeCommand(undoStack.top());
            undoStack.pop();
        }
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
                std::cout << "Invalid input. Please, enter a non-empty string: " << std::endl;
                continue;
            }

            int inputLen = strlen(input);
            if (!ensureCapacity(currentLine, inputLen)) return;

            Command command;
            command.type = APPEND;
            command.lineIndex = countLines() - 1;
            command.oldText = strdup(currentLine->text);
            command.newText = strdup(input);
            pushCommand(command);

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
        Command command;
        command.type = ADD_LINE;
        command.lineIndex = countLines() - 1;
        command.oldText = nullptr;
        command.newText = nullptr;
        pushCommand(command);

        std::cout << "New line is started." << std::endl;
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

        Command command;
        command.type = INSERT;
        command.lineIndex = lineIndex;
        command.charIndex = charIndex;
        command.oldText = strndup(curLine->text, curTextLen);;
        command.newText = strdup(substring);
        command.oldTextLen = curTextLen;
        command.newTextLen = substringLen;
        pushCommand(command);

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

    void deleteSubstring() {
        LineNode *curLine = nullptr;
        int lineIndex = 0;
        int charIndex = 0;
        int curTextLen = 0;

        if (!getLineAndIndices(curLine, lineIndex, charIndex, curTextLen)) return;

        int numChars = getUserInputInt("Enter the number of symbols to delete: ");

        if (charIndex + numChars > curTextLen) {
            std::cout
                    << "The number of characters to delete exceeds the length of the line."
                    << std::endl;
            return;
        }

        Command command;
        command.type = DELETE;
        command.lineIndex = lineIndex;
        command.charIndex = charIndex;
        command.oldText = strndup(curLine->text + charIndex, numChars);
        command.newText = nullptr;
        command.oldTextLen = numChars;
        pushCommand(command);

        memmove(curLine->text + charIndex, curLine->text + charIndex + numChars, curTextLen - numChars + 1);
        std::cout << "Deleted " << numChars << " characters from line " << lineIndex + 1 << " starting at position "
                  << charIndex + 1 << "." << std::endl;
    }

    void replaceSubstring() {
        LineNode *curLine = nullptr;
        int lineIndex = 0;
        int charIndex = 0;
        int curTextLen = 0;

        if (!getLineAndIndices(curLine, lineIndex, charIndex, curTextLen)) return;

        char substring[31];
        getUserInputString("Enter the substring to insert (up to 30 symbols):", substring, 31);

        int substringLen = strlen(substring);

        if ((curTextLen - charIndex < substringLen) &&
            !ensureCapacity(curLine, substringLen - (curTextLen - charIndex)))
            return;

        Command command;
        command.type = REPLACE;
        command.lineIndex = lineIndex;
        command.charIndex = charIndex;
        command.oldText = strndup(curLine->text + charIndex, substringLen);
        command.newText = strdup(substring);
        command.oldTextLen = strlen(command.oldText);
        command.newTextLen = substringLen;
        pushCommand(command);

        int newLength = charIndex + substringLen;
        if (newLength >= curTextLen)
            curLine->text[newLength] = '\0';

        memcpy(curLine->text + charIndex, substring, substringLen);
        std::cout << "Substring \"" << substring << "\" was inserted with replacement successfully into line "
                  << lineIndex + 1 << " starting at position " << charIndex + 1 << "." << std::endl;
    }

    void copyText() {
        LineNode *curLine = nullptr;
        int lineIndex = 0;
        int charIndex = 0;
        int curTextLen = 0;

        if (!getLineAndIndices(curLine, lineIndex, charIndex, curTextLen)) return;

        int numChars = getUserInputInt("Enter the number of symbols to copy: ");
        if (charIndex + numChars > curTextLen) {
            std::cout
                    << "The number of characters to copy exceeds the length of the line."
                    << std::endl;
            return;
        }

        delete[] clipboard;
        clipboard = new char[numChars + 1];
        strncpy(clipboard, curLine->text + charIndex, numChars);
        clipboard[numChars] = '\0';

        std::cout << "Copied text: " << clipboard << std::endl;
    }

    void pasteText() {
        if (!clipboard) {
            std::cout << "Clipboard is empty. Copy some text first." << std::endl;
            return;
        }

        LineNode *curLine = nullptr;
        int lineIndex = 0;
        int charIndex = 0;
        int curTextLen = 0;

        if (!getLineAndIndices(curLine, lineIndex, charIndex, curTextLen)) return;

        int clipboardLen = strlen(clipboard);
        if (!ensureCapacity(curLine, clipboardLen)) return;

        Command command;
        command.type = PASTE;
        command.lineIndex = lineIndex;
        command.charIndex = charIndex;
        command.oldText = nullptr;
        command.newText = strdup(clipboard);
        command.oldTextLen = 0;
        command.newTextLen = clipboardLen;
        pushCommand(command);

        memmove(curLine->text + charIndex + clipboardLen, curLine->text + charIndex, curTextLen - charIndex + 1);
        memcpy(curLine->text + charIndex, clipboard, clipboardLen);

        std::cout << "Pasted text: " << clipboard << std::endl;
    }

    void cutText() {
        LineNode *curLine = nullptr;
        int lineIndex = 0;
        int charIndex = 0;
        int curTextLen = 0;

        if (!getLineAndIndices(curLine, lineIndex, charIndex, curTextLen)) return;

        int numChars = getUserInputInt("Enter the number of symbols to cut: ");
        if (charIndex + numChars > curTextLen) {
            std::cout << "The number of characters to cut exceeds the length of the line." << std::endl;
            return;
        }

        delete[] clipboard;
        clipboard = new char[numChars + 1];
        strncpy(clipboard, curLine->text + charIndex, numChars);
        clipboard[numChars] = '\0';

        Command command;
        command.type = DELETE;
        command.lineIndex = lineIndex;
        command.charIndex = charIndex;
        command.oldText = strndup(curLine->text + charIndex, numChars);
        command.newText = nullptr;
        command.oldTextLen = numChars;
        command.newTextLen = 0;
        pushCommand(command);

        memmove(curLine->text + charIndex, curLine->text + charIndex + numChars, curTextLen - numChars - charIndex + 1);
        std::cout << "Cut " << numChars << " characters from line " << lineIndex + 1 << ", starting at position " << charIndex + 1 << "." << std::endl;
    }

    void undo() {
        if (undoStack.empty()) {
            std::cout << "No actions to undo." << std::endl;
            return;
        }
        Command command = undoStack.top();
        undoStack.pop();

        LineNode *curLine = head;
        for (int i = 0; i < command.lineIndex; ++i)
            curLine = curLine->next;

        switch (command.type) {
            case APPEND:
                undoAppend(curLine, command);
                break;
            case ADD_LINE:
                undoAddLine(command);
                break;
            case INSERT:
                undoInsert(curLine, command);
                break;
            case DELETE:
                undoDelete(curLine, command);
                break;
            case REPLACE:
                undoReplace(curLine, command);
                break;
            case CUT:
                undoCut(curLine, command);
                break;
            case PASTE:
                undoPaste(curLine, command);
                break;
            default:
                std::cout << "Undo not applicable for this command type." << std::endl;
                return;
        }
        redoStack.push(command);
        std::cout << "Undo performed." << std::endl;
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
                  << "8. Delete a substring from the text.\n"
                  << "9. Insert text with replacement at a specified position.\n"
                  << "10. Copy text.\n"
                  << "11. Paste text.\n"
                  << "12. Cut text.\n"
                  << "13. Undo last action.\n"
                  << "14. Redo last undone action.\n"
                  << "15. Exit.\n";
    }

    void publicClearInputBuffer(const std::string &errorMessage) {
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
                deleteSubstring();
                break;
            case 9:
                replaceSubstring();
                break;
            case 10:
                copyText();
                break;
            case 11:
                pasteText();
                break;
            case 12:
                cutText();
                break;
            case 13:
                undo();
                break;
            case 14:
//                redo();
                break;
            case 15:
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
    char *clipboard;
    enum CommandType {APPEND, ADD_LINE, INSERT, DELETE, REPLACE, CUT, PASTE};
    struct Command {
        CommandType type;
        int lineIndex;
        int charIndex;
        char *oldText;
        char *newText;
        int oldTextLen;
        int newTextLen;
    };
    std::stack<Command> undoStack;
    std::stack<Command> redoStack;

    void pushCommand(Command command) {
        undoStack.push(command);
        while (!redoStack.empty()) {
            freeCommand(redoStack.top());
            redoStack.pop();
        }
    }

    void freeCommand(Command &command) {
        delete[] command.oldText;
        delete[] command.newText;
    }

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

    void deleteLine(int lineIndex) {
        LineNode *curLine = head;
        LineNode *prevLine = nullptr;
        for (int i = 0; i < lineIndex; ++i) {
            if (!curLine) return;
            prevLine = curLine;
            curLine = curLine->next;
        }
        if (!curLine) return;

        if (!prevLine)
            head = curLine->next;
        else prevLine->next = curLine->next;

        delete curLine;
    }

    void clearInputBuffer(const std::string &errorMessage) const {
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
            if (command >= 1 && command <= 15) return true;
        }
        return false;
    }

    int countLines() {
        int counter = 0;
        LineNode *curLine = head;
        while (curLine) {
            ++counter;
            curLine = curLine->next;
        }
        return counter;
    }

    char* strndup(const char* str, size_t n) {
        size_t len = strlen(str);
        if (len < n)
            n = len;
        char* dup = new char[n + 1];
        if (dup) {
            strncpy(dup, str, n);
            dup[n] = '\0';
        }
        return dup;
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

    void undoAppend(LineNode* curLine, Command& command) {
        strcpy(curLine->text, command.oldText);
    }

    void undoAddLine(Command& command) {
        deleteLine(command.lineIndex);
    }

    void undoInsert(LineNode* curLine, Command& command) {
        memmove(curLine->text + command.charIndex,
                curLine->text + command.charIndex + command.newTextLen,
                strlen(curLine->text) - command.charIndex - command.newTextLen + 1);
    }

    void undoDelete(LineNode* curLine, Command& command) {
        if (!ensureCapacity(curLine, command.oldTextLen)) return;
        memmove(curLine->text + command.charIndex + command.oldTextLen,
                curLine->text + command.charIndex,
                strlen(curLine->text) - command.charIndex + 1);
        memcpy(curLine->text + command.charIndex, command.oldText, command.oldTextLen);
    }

    void undoReplace(LineNode* curLine, Command& command) {
        memcpy(curLine->text + command.charIndex, command.oldText, command.oldTextLen);
    }

    void undoCut(LineNode* curLine, Command& command) {
        if (!ensureCapacity(curLine, command.oldTextLen)) return;
        memmove(curLine->text + command.charIndex + command.oldTextLen,
                curLine->text + command.charIndex,
                strlen(curLine->text) - command.charIndex + 1);
        memcpy(curLine->text + command.charIndex, command.oldText, command.oldTextLen);
    }

    void undoPaste(LineNode* curLine, Command& command) {
        memmove(curLine->text + command.charIndex,
                curLine->text + command.charIndex + command.newTextLen,
                strlen(curLine->text) - command.charIndex - command.newTextLen + 1);
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
            std::cout << "Invalid command! Please, enter a number from 1 to 10." << std::endl;
            continue;
        }
    } while (command != 15);
    return 0;
}