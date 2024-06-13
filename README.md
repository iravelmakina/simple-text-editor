# Simple Text Editor: The Basics of Procedural, Structural and Object-oriented Programming

## Purpose of Work
Understand the basics of procedural, structural and object-oriented programming by having hands-on experience in using these paradigms.

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


8. **Delete Command**
    ```
    Choose the command:
    > 8
    Choose line, index and number of symbols: 0 0 7
    Choose the command:
    > 5
    my text editor!
    ```

9. **Undo Command**
    ```
    Choose the command:
    > 9
    Choose the command:
    > 5
    Hello, my text editor!
    ```

10. **Redo Command**
    ```
    Choose the command:
    > 10
    Choose the command:
    > 5
    my text editor!
    ```

11. **Cut Command**
    ```
    Choose the command:
    > 11 (cut command)
    Choose line and index and number of symbols: 0 0 3
    Choose the command:
    > 5
    text editor!
    ```

12. **Paste Command**
    ```
    Choose the command:
    > 12 (paste command)
    Choose line and index: 0 0
    Choose the command:
    > 5
    my text editor!
    ```

13. **Copy Command**
    ```
    Choose the command:
    > 13 (copy command)
    Choose line and index and number of symbols: 0 0 15
    Choose the command:
    > 12 (paste command)
    Choose line and index: 0 0
    Choose the command:
    > 5
    my text editor!my text editor!
    ```

14. **Insert with Replacement Command**
    ```
    Choose the command:
    > 14
    Choose line and index: 0 0
    Write text: Hello, world!
    Choose the command:
    > 5
    Hello, world!or!my text editor!
    ```

15. **(Optional) Implement cursor-based logic**


## How to Use
1. Clone the repository.
2. Compile the source code using a C++ compiler.
3. Run the executable.
4. Follow the on-screen instructions to use the text editor functionalities.
