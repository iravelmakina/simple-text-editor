# Simple Text Editor: The Basics of Procedural and Structural Programming

## Purpose of Work
Understand the basics of procedural and structural programming by having hands-on experience in using these paradigms. Learn how memory management works inside the process, understand the virtual memory concept and typical issues with memory allocation and deallocation, and learn the basic data types.

## Task
Develop a console-based text editor with the following functionalities:

1. **Append text symbols to the end**
    ```
    Choose the command:
    > 1
    Enter text to append: Hello, text editor!
    ```

2. **Start a new line**
    ```
    Choose the command:
    > 2
    New line is started
    ```

3. **Use files to load/save information**
    ```
    Choose the command:
    > 3
    Enter the file name for saving: MyFile.txt
    Text has been saved successfully
    Choose the command:
    > 4
    Enter the file name for loading: MyFile.txt
    Text has been loaded successfully
    ```

4. **Print the current text to console**
    ```
    Choose the command:
    > 5
    Hello, text editor!
    ```

5. **Insert text by line and symbol index**
    ```
    Choose the command:
    > 6
    Choose line and index: 0 6
    Enter text to insert: my
    Choose the command:
    > 5
    Hello, my text editor!
    ```

6. **Search (text can be found more than once)**
    ```
    Choose the command:
    > 7
    Enter text to search: Hello
    Text is present in this position: 0 0
    ```

7. **(Optional) Clearing the console**

    In this case, you should clean the console before each command starts.

## Task Milestones
1. **Implement input command parser**
    - Print help information, read the command from the user, and respond with `The command is not implemented` if the command is not available.

2. **Implement text memory storage**
    - Consider the following format for text memory storage:
        ```
        M y t e x t \0
        ```
      where `\0` represents the end of the string. Use dynamic arrays or linked lists for efficient memory management.

3. **Implement text memory printing to console**
    - Iterate through the data structure and print each symbol to the console.

4. **Implement the logic to save text to file**
    - Save text to a file using the following example:
        ```c
        #include <stdio.h>
        int main() {
            FILE* file;
            file = fopen("myfile.txt", "w");
            if (file != NULL) {
                fputs("Hello, files world!", file);
                fclose(file);
            }
            return 0;
        }
        ```

5. **Implement the logic to load text from file**
    - Load text from a file using the following example:
        ```c
        #include <stdio.h>
        int main() {
            FILE* file;
            char mystring[100];
            file = fopen("MyFile.txt", "r");
            if (file == NULL) {
                printf("Error opening file");
            } else {
                if (fgets(mystring, 100, file) != NULL) {
                    printf("%s", mystring);
                }
                fclose(file);
            }
            return 0;
        }
        ```

6. **Implement substring search mechanism**
    - Iterate through the text document and compare each symbol with the given substring. Find all matched substrings, not just the first or last one.

7. **Implement substring insertion mechanism**
    - Insert a substring using line and symbol indexing (the insertion position). Consider memory reallocation and moving, for example:
        ```
        m a p \0
        ```
      Insert `hash` at line 0, starting from the 0th element:
        ```
        h a s h m a p \0
        ```

8. **Implement the console clearing mechanism**

## How to Use
1. Clone the repository.
2. Compile the source code using a C++ compiler.
3. Run the executable.
4. Follow the on-screen instructions to use the text editor functionalities.
