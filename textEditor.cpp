#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
#include <stack>

const int BUFFER_SIZE = 100;
const int MAX_FILENAME_LENGTH = 21;

class LineNode {
public:
    char *text;
    int capacity;
    LineNode *next;

    LineNode(int initialCapacity = BUFFER_SIZE) : text(nullptr), capacity(initialCapacity), next(nullptr) {
        text = new(std::nothrow) char[capacity];
        if (text) {
            text[0] = '\0';
        } else {
            std::cerr << "Memory allocation failed for text." << std::endl;
            return;
        }
    }

    // copy constructor
    LineNode(const LineNode &other) : capacity(other.capacity), next(nullptr) {
        text = new(std::nothrow) char[capacity];
        if (text) {
            strcpy(text, other.text);
        } else {
            std::cerr << "Memory allocation failed for text." << std::endl;
        }

        if (other.next) {
            next = new LineNode(*other.next);
        }
    }

    ~LineNode() {
        delete[] text;
        text = nullptr;
    }

    friend std::ostream &operator<<(std::ostream &out, const LineNode &lineNode) {
        out << lineNode.text;
        return out;
    }
};

class TextManager {
public:
    TextManager() : head(nullptr), currentLine(nullptr), clipboard(nullptr), cursor(0, 0) {}

    ~TextManager() {
        freeMemory();
        freeClipboard();
    }

    void appendText() { // always appends to the end regardless of cursor position
        saveState();
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
                moveCursor(cursor.first, strlen(currentLine->text));
                break;
            }
        }
    }

    void addLine() {
        saveState();

        LineNode *newLine = new LineNode();
        if (!currentLine) {
            currentLine = newLine;
            head = currentLine;
        } else {
            newLine->next = currentLine->next;
            currentLine->next = newLine;
            currentLine = newLine;
        }
        std::cout << "New line is started." << std::endl;
        moveCursor(cursor.first + 1, 0);
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
        outFile.close();
        std::cout << "Operation on file " << filename << " completed successfully." << std::endl;
    }

    void loadFromFile() {
        char filename[MAX_FILENAME_LENGTH];
        getUserInputString("Enter the file name (up to 20 characters):", filename, MAX_FILENAME_LENGTH);

        FILE *inFile = fopen(filename, "r");
        if (!inFile) {
            std::cerr << "Failed to open file " << filename << ". Please, make sure it exists." << std::endl;
            return;
        }

        freeMemory();

        char buffer[BUFFER_SIZE];
        currentLine = nullptr;
        while (fgets(buffer, BUFFER_SIZE, inFile)) {
            buffer[strcspn(buffer, "\n")] = '\0';

            LineNode *newLine = new LineNode();
            if (!ensureCapacity(newLine, strlen(buffer))) {
                fclose(inFile);
                return;
            }
            strcpy(newLine->text, buffer);

            if (!head) {
                head = newLine;
            } else currentLine->next = newLine;
            currentLine = newLine;
        }
        moveCursor(0, 0);
        fclose(inFile);
        std::cout << "Operation on file " << filename << " completed successfully." << std::endl;
    }

    void printText() const {
        if (!head) {
            std::cout << "The text is empty yet. Please, enter something first." << std::endl;
            return;
        }

        LineNode *curLine = head;
        int lineIndex = 0;
        std::cout << "Your current text is:" << std::endl;
        while (curLine) {
            if (lineIndex == cursor.first) {
                for (int i = 0; i < cursor.second; ++i)
                    std::cout << curLine->text[i];
                std::cout << "|"; // set the cursor before char
                std::cout << (curLine->text + cursor.second) << std::endl;
            } else std::cout << curLine->text << std::endl;
            curLine = curLine->next;
            ++lineIndex;
        }
    }

    void insertSubstring() {
        saveState();

        if (!currentLine) {
            std::cout << "No current line to insert into." << std::endl;
            return;
        }

        char substring[31];
        getUserInputString("Enter the substring to insert (up to 30 symbols):", substring, 31);

        int substringLen = strlen(substring);
        int charIndex = cursor.second;
        int curTextLen = strlen(currentLine->text);


        if (charIndex > curTextLen) {
            std::cout << "Cursor position out of bounds." << std::endl;
            return;
        }

        if (!ensureCapacity(currentLine, substringLen)) return;

        memmove(currentLine->text + charIndex + substringLen, currentLine->text + charIndex,
                curTextLen - charIndex + 1);
        memcpy(currentLine->text + charIndex, substring, substringLen);

        std::cout << "Substring \"" << substring << "\" was inserted successfully at cursor position." << std::endl;
        moveCursor(cursor.first, charIndex + substringLen);
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
        saveState();

        if (!currentLine) {
            std::cout << "No current line to insert into." << std::endl;
            return;
        }

        int numChars = getUserInputInt("Enter the number of symbols to delete: ");
        int charIndex = cursor.second;

        int curTextLen = strlen(currentLine->text);
        if (charIndex + numChars > curTextLen) {
            std::cout
                    << "The number of characters to delete exceeds the length of the line."
                    << std::endl;
            return;
        }

        memmove(currentLine->text + charIndex, currentLine->text + charIndex + numChars, curTextLen - numChars + 1); // null-terminator

        std::cout << "Deleted " << numChars << " characters from cursor position." << std::endl;
        moveCursor(cursor.first, charIndex);
    }

    void replaceSubstring() {
        saveState();

        if (!currentLine) {
            std::cout << "No current line to replace in." << std::endl;
            return;
        }

        char substring[31];
        getUserInputString("Enter the substring to insert (up to 30 symbols):", substring, 31);

        int substringLen = strlen(substring);
        int charIndex = cursor.second;
        int curTextLen = strlen(currentLine->text);

        if ((curTextLen - charIndex < substringLen) &&
            !ensureCapacity(currentLine, substringLen - (curTextLen - charIndex))) return;

        int newLength = charIndex + substringLen;
        if (newLength >= curTextLen)
            currentLine->text[newLength] = '\0';

        memcpy(currentLine->text + charIndex, substring, substringLen);

        std::cout << "Substring \"" << substring << "\" was inserted with replacement successfully at cursor position."
                  << std::endl;
        moveCursor(cursor.first, charIndex + substringLen);
    }

    void copyText() {
        if (!currentLine) {
            std::cout << "No current line to copy from." << std::endl;
            return;
        }

        int numChars = getUserInputInt("Enter the number of symbols to copy: ");
        int charIndex = cursor.second;
        int curTextLen = strlen(currentLine->text);

        if (charIndex + numChars > curTextLen) {
            std::cout
                    << "The number of characters to copy exceeds the length of the line."
                    << std::endl;
            return;
        }

        delete[] clipboard;
        clipboard = new char[numChars + 1];
        strncpy(clipboard, currentLine->text + charIndex, numChars);
        clipboard[numChars] = '\0';

        std::cout << "Copied text: " << clipboard << std::endl;
        moveCursor(cursor.first, charIndex + numChars);
    }

    void pasteText() {
        saveState();

        if (!clipboard) {
            std::cout << "Clipboard is empty. Copy some text first." << std::endl;
            return;
        }

        if (!currentLine) {
            std::cout << "No current line to paste into." << std::endl;
            return;
        }

        int clipboardLen = strlen(clipboard);
        int charIndex = cursor.second;
        int curTextLen = strlen(currentLine->text);

        if (!ensureCapacity(currentLine, clipboardLen)) return;

        memmove(currentLine->text + charIndex + clipboardLen, currentLine->text + charIndex,
                curTextLen - charIndex + 1);
        memcpy(currentLine->text + charIndex, clipboard, clipboardLen);
        currentLine->text[curTextLen + clipboardLen] = '\0';

        std::cout << "Pasted text: " << clipboard << std::endl;
        moveCursor(cursor.first, charIndex + clipboardLen);
    }

    void cutText() {
        saveState();

        if (!currentLine) {
            std::cout << "No current line to cut from." << std::endl;
            return;
        }

        int numChars = getUserInputInt("Enter the number of symbols to cut: ");
        int charIndex = cursor.second;
        int curTextLen = strlen(currentLine->text);

        if (charIndex + numChars > curTextLen) {
            std::cout << "The number of characters to cut exceeds the length of the line." << std::endl;
            return;
        }

        delete[] clipboard;
        clipboard = new char[numChars + 1];
        strncpy(clipboard, currentLine->text + charIndex, numChars);
        clipboard[numChars] = '\0';

        memmove(currentLine->text + charIndex, currentLine->text + charIndex + numChars,
                curTextLen - numChars - charIndex + 1);

        std::cout << "Cut text: " <<  clipboard << std::endl;
        moveCursor(cursor.first, charIndex);
    }

    void undo() {
        if (undoStack.empty()) {
            std::cout << "No actions to undo." << std::endl;
            return;
        }
        saveCurrentStateForRedo();
        auto state = undoStack.top();
        undoStack.pop();
        restoreState(state);
        std::cout << "Undo operation completed successfully." << std::endl;
    }

    void redo() {
        if (redoStack.empty()) {
            std::cout << "No actions to redo." << std::endl;
            return;
        }
        saveCurrentStateForUndo();
        auto state = redoStack.top();
        redoStack.pop();
        restoreState(state);
        std::cout << "Redo operation completed successfully." << std::endl;
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
                  << "8. Exit.\n"
                  << "9. Insert text with replacement at a specified position.\n"
                  << "10. Copy text.\n"
                  << "11. Paste text.\n"
                  << "12. Cut text.\n"
                  << "13. Undo last action.\n"
                  << "14. Redo last undone action.\n"
                  << "15. Exit.\n";
    }

    void setCursorPosition() {
        int lineIndex = getUserInputInt("Enter the line index: ") - 1;
        int charIndex = getUserInputInt("Enter the character index: ") - 1;

        if (lineIndex < 0 || charIndex < 0) {
            std::cout << "Line and character indices must be greater than 0." << std::endl;
            return;
        }

        LineNode *curLine = head;
        int currentLineIndex = 0;

        while (curLine && currentLineIndex < lineIndex) {
            curLine = curLine->next;
            ++currentLineIndex;
        }

        if (!curLine) {
            std::cout << "Line index out of bounds." << std::endl;
            return;
        }

        int curTextLen = strlen(curLine->text);
        if (charIndex > curTextLen) {
            std::cout << "Character index out of bounds." << std::endl;
            return;
        }
        moveCursor(lineIndex, charIndex);
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
                redo();
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
    std::stack<std::pair<LineNode *, std::pair<int, int>>> undoStack;
    std::stack<std::pair<LineNode *, std::pair<int, int>>> redoStack;
    std::pair<int, int> cursor;
    char *clipboard;

    void saveState() {
        undoStack.push({cloneList(head), cursor});
        while (!redoStack.empty()) {
            freeList(redoStack.top().first);
            redoStack.pop();
        }
    }

    void saveCurrentStateForUndo() {
        LineNode *state = cloneList(head);
        undoStack.push({state, cursor});
    }

    void saveCurrentStateForRedo() {
        LineNode *state = cloneList(head);
        redoStack.push({state, cursor});
    }

    LineNode *cloneList(LineNode *head) {
        if (!head) return nullptr;
        return new LineNode(*head);
    }

    void restoreState(const std::pair<LineNode *, std::pair<int, int>> &state) {
        freeList(head);
        head = cloneList(state.first);
        cursor = state.second;

        currentLine = head;
        int lineIndex = 0;
        while (currentLine && lineIndex < cursor.first) {
            currentLine = currentLine->next;
            lineIndex++;
        } //restore the state of the text editor to a previous state that's stored in a stack
    }

    void freeList(LineNode *head) {
        LineNode *current = head;
        while (current) {
            LineNode *next = current->next;
            delete current;
            current = next;
        }
    }

    void freeMemory() {
        freeList(head);
        head = nullptr;
        currentLine = nullptr;
    }

    void freeClipboard() {
        delete[] clipboard;
        clipboard = nullptr;
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
            if (command >= 1 && command <= 15) return true;
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

    void moveCursor(int lineIndex, int charIndex) {
        cursor.first = lineIndex;
        cursor.second = charIndex;

        currentLine = head;
        int currentLineIndex = 0;
        while (currentLine && currentLineIndex < lineIndex) {
            currentLine = currentLine->next;
            currentLineIndex++;
        }

        displayCursor();
    }

    void displayCursor() const {
        std::cout << "Cursor is at line " << cursor.first + 1 << ", position " << cursor.second + 1 << "." << std::endl;
    }
};

int main() {
    TextManager textManager;

    std::cout << "Welcome! Enter 'm' to see available commands or 'c' to set cursor position." << std::endl;
    char commandLine[BUFFER_SIZE];
    int command;
    do {
        std::cout << "\nPlease, choose your command: " << std::endl;
        std::cin.getline(commandLine, BUFFER_SIZE);

        if (std::cin.fail()) {
            textManager.publicClearInputBuffer("Input exceeds buffer size. Please enter a shorter command.");
            continue;
        }

        if (strcmp(commandLine, "m") == 0) {
            textManager.printMenu();
        } else if (strcmp(commandLine, "c") == 0) {
            textManager.setCursorPosition();
        } else if (textManager.publicIsValidCommand(commandLine, command)) {
            textManager.processCommand(command);
        } else {
            std::cout << "Invalid command! Please, enter a number from 1 to 15." << std::endl;
            continue;
        }
    } while (command != 15);
    return 0;
}
