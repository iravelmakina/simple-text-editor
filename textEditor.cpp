#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
#include <stack>
#include <windows.h>

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


class Text {
private:
    LineNode *head;
    LineNode *tail;

public:
    Text() : head(nullptr), tail(nullptr) {}

    Text(const Text &other) : head(nullptr), tail(nullptr) {
        LineNode *current = other.head;
        while (current) {
            appendLine(current->text);
            current = current->next;
        }
    }

    ~Text() {
        clear();
    }

    void appendLine(const char *line) {
        LineNode *newNode = new LineNode(strlen(line) + 1);
        strcpy(newNode->text, line);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    void clear() {
        LineNode *current = head;
        while (current) {
            LineNode *toDelete = current;
            current = current->next;
            delete toDelete;
        }
        head = tail = nullptr;
    }

    friend std::ostream &operator<<(std::ostream &out, const Text &text) {
        LineNode *current = text.head;
        while (current) {
            out << *current << std::endl;
            current = current->next;
        }
        return out;
    }

    LineNode *getHead() const {
        return head;
    }

    void setHead(LineNode *node) {
        head = node;
    }
};


class Cursor {
private:
    int lineIndex;
    int charIndex;
public:
    Cursor(int line = 0, int ch = 0) : lineIndex(line), charIndex(ch) {}

    int getLine() const { return lineIndex; }

    int getChar() const { return charIndex; }

    void setLine(int line) { lineIndex = line; }

    void setChar(int ch) { charIndex = ch; }

    void move(int line, int ch) {
        lineIndex = line;
        charIndex = ch;
    }

    void display() const {
        std::cout << "Cursor is at line " << lineIndex + 1 << ", position " << charIndex + 1 << "." << std::endl;
    }
};

class CaesarCipher {
public:
    CaesarCipher(const char* dllPath) {
        handle = LoadLibrary(TEXT(dllPath));
        if (!handle || handle == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Library was not found");
        }

        encryption = (encryption_ptr) GetProcAddress(handle, TEXT("encrypt"));
        if (!encryption) {
            FreeLibrary(handle);
            throw std::runtime_error("Function 'encrypt' was not found");
        }

        decryption = (decryption_ptr) GetProcAddress(handle, TEXT("decrypt"));
        if (!decryption) {
            FreeLibrary(handle);
            throw std::runtime_error ("Function 'decrypt' was not found");
        }
    }

    ~CaesarCipher() {
        if (handle) {
            FreeLibrary(handle);
        }
    }

    char* encrypt(const char* message, int shift) {
        return encryption(message, shift);
    }

    char* decrypt(const char* message, int shift) {
        return decryption(message, shift);
    }

    void encryptFile(const char* inputFilePath, const char* outputFilePath, int shift) {
        processFile(inputFilePath, outputFilePath, shift, encryption);
    }

    void decryptFile(const char* inputFilePath, const char* outputFilePath, int shift) {
        processFile(inputFilePath, outputFilePath, shift, decryption);
    }

private:
    HINSTANCE handle;
    typedef char* (*encryption_ptr)(const char*, int);
    typedef char* (*decryption_ptr)(const char*, int);
    encryption_ptr encryption;
    decryption_ptr decryption;

    void processFile(const char* inputFilePath, const char* outputFilePath, int shift, char* (*process)(const char*, int)) {
        std::ifstream inputFile(inputFilePath, std::ios::binary);
        if (!inputFile.is_open()) {
            std::cerr << "Failed to open input file. Please, make sure path exists." << std::endl;
            return;
        }

        inputFile.seekg(0, std::ios::end);
        size_t fileSize = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);

        char* content = new char[fileSize + 1];
        inputFile.read(content, fileSize);
        inputFile.close();
        content[fileSize] = '\0';

        char* processedContent = process(content, shift);
        delete[] content;

        std::ofstream outputFile(outputFilePath, std::ios::binary);
        if (!outputFile.is_open()){
            delete[] processedContent;
            std::cerr << "Failed to open output file. Please, make sure path exists." << std::endl;
            return;
        }

        outputFile.write(processedContent, fileSize);
        outputFile.close();
        delete[] processedContent;
    }
};

class TextManager {
public:
    TextManager() : currentLine(nullptr), clipboard(nullptr), cursor(0, 0) {}

    ~TextManager() {
        freeClipboard();
    }

    void appendText() { // always appends to the end regardless of cursor position
        saveState();
        if (!currentLine) {
            text.appendLine("");
            currentLine = text.getHead();
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
            if (!ensureCapacity(currentLine, inputLen)) {
                return;
            }

            strcat(currentLine->text, input);

            if (inputLen < BUFFER_SIZE - 1) {
                std::cout << "Text was appended successfully." << std::endl;
                moveCursor(cursor.getLine(), strlen(currentLine->text));
                break;
            }
        }
    }

    void addLine() {
        saveState();

        LineNode *newLine = new LineNode();
//        if (!newLine->text) {
//            delete newLine;
//            return;
//        }
        if (!currentLine) {
            text.appendLine("");
            currentLine = text.getHead();
        } else {
            newLine->next = currentLine->next;
            currentLine->next = newLine;
            currentLine = newLine;
        }
        std::cout << "New line is started." << std::endl;
        moveCursor(cursor.getLine() + 1, 0);
    }

    void saveToFile() {
        LineNode *curLine = text.getHead();
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

        text.clear();

        char buffer[BUFFER_SIZE];
        currentLine = nullptr;
        while (fgets(buffer, BUFFER_SIZE, inFile)) {
            buffer[strcspn(buffer, "\n")] = '\0';

            LineNode *newLine = new LineNode(strlen(buffer) + 1);
//            if (!newLine->text) {
//                delete newLine;
//                return;
//            }
            if (!ensureCapacity(newLine, strlen(buffer))) {
                delete newLine;
                fclose(inFile);
                return;
            }
            strcpy(newLine->text, buffer);
            text.appendLine(newLine->text);

            if (!currentLine) {
                currentLine = text.getHead();
            } else {
                currentLine = currentLine->next;
            }
        }
        moveCursor(0, 0);
        fclose(inFile);
        std::cout << "Operation on file " << filename << " completed successfully." << std::endl;
    }

    void printText() const {
        if (!text.getHead()) {
            std::cout << "The text is empty yet. Please, enter something first." << std::endl;
            return;
        }

        LineNode *curLine = text.getHead();
        int lineIndex = 0;
        std::cout << "Your current text is:" << std::endl;
        while (curLine) {
            if (lineIndex == cursor.getLine()) {
                for (int i = 0; i < cursor.getChar(); ++i)
                    std::cout << curLine->text[i];
                std::cout << "|"; // set the cursor before char
                std::cout << (curLine->text + cursor.getChar()) << std::endl;
            } else {
                std::cout << curLine->text << std::endl;
            }
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
        int charIndex = cursor.getChar();
        int curTextLen = strlen(currentLine->text);


        if (charIndex > curTextLen) {
            std::cout << "Cursor position out of bounds." << std::endl;
            return;
        }

        if (!ensureCapacity(currentLine, substringLen)) {
            return;
        }

        memmove(currentLine->text + charIndex + substringLen, currentLine->text + charIndex,
                curTextLen - charIndex + 1);
        memcpy(currentLine->text + charIndex, substring, substringLen);

        std::cout << "Substring \"" << substring << "\" was inserted successfully at cursor position." << std::endl;
        moveCursor(cursor.getLine(), charIndex + substringLen);
    }

    void searchSubstring() const {
        if (!text.getHead()) {
            std::cout << "The text is empty. Please, enter something first." << std::endl;
            return;
        }

        char substring[31];
        getUserInputString("Enter the substring to search for (up to 30 symbols):", substring, 31);

        LineNode *curLine = text.getHead();
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
        if (!found) {
            std::cout << "Substring \"" << substring << "\" not found." << std::endl;
        }
    }

    void deleteSubstring() {
        saveState();

        if (!currentLine) {
            std::cout << "No current line to insert into." << std::endl;
            return;
        }

        int numChars = getUserInputInt("Enter the number of symbols to delete: ");
        int charIndex = cursor.getChar();

        int curTextLen = strlen(currentLine->text);
        if (charIndex + numChars > curTextLen) {
            std::cout
                    << "The number of characters to delete exceeds the length of the line."
                    << std::endl;
            return;
        }

        memmove(currentLine->text + charIndex, currentLine->text + charIndex + numChars,
                curTextLen - numChars + 1); // null-terminator

        std::cout << "Deleted " << numChars << " characters from cursor position." << std::endl;
        moveCursor(cursor.getLine(), charIndex);
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
        int charIndex = cursor.getChar();
        int curTextLen = strlen(currentLine->text);

        if ((curTextLen - charIndex < substringLen) &&
            !ensureCapacity(currentLine, substringLen - (curTextLen - charIndex))) {
            return;
        }

        int newLength = charIndex + substringLen;
        if (newLength >= curTextLen) {
            currentLine->text[newLength] = '\0';
        }

        memcpy(currentLine->text + charIndex, substring, substringLen);

        std::cout << "Substring \"" << substring << "\" was inserted with replacement successfully at cursor position."
                  << std::endl;
        moveCursor(cursor.getLine(), charIndex + substringLen);
    }

    void copyText() {
        if (!currentLine) {
            std::cout << "No current line to copy from." << std::endl;
            return;
        }

        int numChars = getUserInputInt("Enter the number of symbols to copy: ");
        int charIndex = cursor.getChar();
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
        moveCursor(cursor.getLine(), charIndex + numChars);
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
        int charIndex = cursor.getChar();
        int curTextLen = strlen(currentLine->text);

        if (!ensureCapacity(currentLine, clipboardLen)) {
            return;
        }

        memmove(currentLine->text + charIndex + clipboardLen, currentLine->text + charIndex,
                curTextLen - charIndex + 1);
        memcpy(currentLine->text + charIndex, clipboard, clipboardLen);
        currentLine->text[curTextLen + clipboardLen] = '\0';

        std::cout << "Pasted text: " << clipboard << std::endl;
        moveCursor(cursor.getLine(), charIndex + clipboardLen);
    }

    void cutText() {
        saveState();

        if (!currentLine) {
            std::cout << "No current line to cut from." << std::endl;
            return;
        }

        int numChars = getUserInputInt("Enter the number of symbols to cut: ");
        int charIndex = cursor.getChar();
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

        std::cout << "Cut text: " << clipboard << std::endl;
        moveCursor(cursor.getLine(), charIndex);
    }

    void undo() {
        if (undoStack.empty()) {
            std::cout << "No actions to undo." << std::endl;
            return;
        }
        saveCurrentStateForRedo();
        std::pair<LineNode *, Cursor> state = undoStack.top();
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
        std::pair<LineNode *, Cursor> state = redoStack.top();
        redoStack.pop();
        restoreState(state);
        std::cout << "Redo operation completed successfully." << std::endl;
    }

    void setCursorPosition() {
        int lineIndex = getUserInputInt("Enter the line index: ") - 1;
        int charIndex = getUserInputInt("Enter the character index: ") - 1;

        if (lineIndex < 0 || charIndex < 0) {
            std::cout << "Line and character indices must be greater than 0." << std::endl;
            return;
        }

        LineNode *curLine = text.getHead();
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

    void printMenu() const {
        std::cout << "Possible commands:\n"
                  << "1. Append text to current line.\n"
                  << "2. Add a new line.\n"
                  << "3. Save the text to a file.\n"
                  << "4. Load the text from a file.\n"
                  << "5. Print the text.\n"
                  << "6. Insert a substring into the text.\n"
                  << "7. Search for a substring in the text.\n"
                  << "8. Delete substring.\n"
                  << "9. Insert text with replacement at a specified position.\n"
                  << "10. Copy text.\n"
                  << "11. Paste text.\n"
                  << "12. Cut text.\n"
                  << "13. Undo last action.\n"
                  << "14. Redo last undone action.\n";
    }

    void publicClearInputBuffer(const char *errorMessage) {
        clearInputBuffer(errorMessage);
    }

    bool publicIsValidCommand(char *line, int &command) {
        return isValidCommand(line, command);
    }

    void publicProcessCommand(int command) {
        processCommand(command);
    }

private:
    Text text;
    LineNode *currentLine;
    std::stack<std::pair<LineNode *, Cursor>> undoStack;
    std::stack<std::pair<LineNode *, Cursor>> redoStack;
    Cursor cursor;
    char *clipboard;

    void saveState() {
        undoStack.push({cloneList(text.getHead()), cursor});
        while (!redoStack.empty()) {
            freeList(redoStack.top().first);
            redoStack.pop();
        }
    }

    void saveCurrentStateForUndo() {
        LineNode *state = cloneList(text.getHead());
        undoStack.push({state, cursor});
    }

    void saveCurrentStateForRedo() {
        LineNode *state = cloneList(text.getHead());
        redoStack.push({state, cursor});
    }

    LineNode *cloneList(LineNode *head) {
        if (!head) {
            return nullptr;
        }
        return new LineNode(*head);
    }

    void restoreState(const std::pair<LineNode *, Cursor> &state) {
        freeList(text.getHead());
        text.setHead(cloneList(state.first));
        cursor = state.second;

        currentLine = text.getHead();
        int lineIndex = 0;
        while (currentLine && lineIndex < cursor.getLine()) {
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

    void freeClipboard() {
        delete[] clipboard;
        clipboard = nullptr;
    }

    void clearInputBuffer(const char *errorMessage) const {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << errorMessage << std::endl;
    }

    bool isInteger(const char *line) {
        int len = strlen(line);
        for (int i = 0; i < len; ++i) {
            if (!isdigit(line[i])) {
                return false;
            }
        }
        return true;
    }

    bool isValidCommand(char *line, int &command) {
        if (isInteger(line)) {
            command = atoi(line);
            if (command >= 1 && command <= 14) {
                return true;
            }
        }
        return false;
    }

    bool ensureCapacity(LineNode *curLine, int additionalLength) {
        if (!curLine || !curLine->text) {
            return false;
        }

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
            } else {
                std::cout << "Invalid input. Please enter an integer value." << std::endl;
            }
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

            if (output[0] == '\0') {
                std::cout << "Invalid input. Please enter a non-empty string." << std::endl;
            } else {
                break;
            }
        }
    }

    void moveCursor(int lineIndex, int charIndex) {
        cursor.move(lineIndex, charIndex);

        currentLine = text.getHead();
        int currentLineIndex = 0;
        while (currentLine && currentLineIndex < lineIndex) {
            currentLine = currentLine->next;
            currentLineIndex++;
        }

        cursor.display();
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
            default:
                std::cout << "Unexpected command received. Please enter a number from 1 to 14." << std::endl;
                break;
        }
    }
};

int main() {
    TextManager textManager;

    std::cout << "Welcome! Enter 'm' to see available commands, 'c' to set cursor position and 'e' to exit."
              << std::endl;
    char commandLine[BUFFER_SIZE];
    int command;
    while (true) {
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
        } else if (strcmp(commandLine, "e") == 0) {
            std::cout << "Okay, bye!" << std::endl;
            break;
        } else if (textManager.publicIsValidCommand(commandLine, command)) {
            textManager.publicProcessCommand(command);
        } else {
            std::cout << "Invalid command! Please, enter a number from 1 to 14." << std::endl;
        }
    }
    return 0;
}
