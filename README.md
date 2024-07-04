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

3-4. **Use files to load/save information**
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

5. **Print the current text to console**
    ```
    Choose the command:
    > 5
    Hello, text editor!
    ```

6. **Insert text by line and symbol index**
    ```
    Choose the command:
    > 6
    Choose line and index: 0 6
    Enter text to insert: my
    Choose the command:
    > 5
    Hello, my text editor!
    ```

7. **Search (text can be found more than once)**
    ```
    Choose the command:
    > 7
    Enter text to search: Hello
    Text is present in this position: 0 0
    ```
    
8. **Delete Command**
    ```
    Choose the command:
    > 8
    Choose line, index and number of symbols: 0 0 7
    Choose the command:
    > 5
    my text editor!
    ```
    
9. **Insert with Replacement Command**
    ```
    Choose the command:
    > 14
    Choose line and index: 0 0
    Write text: Hello, world!
    Choose the command:
    > 5
    Hello, world!or!my text editor!
    ```

10. **Copy Command**
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

11. **Paste Command**
    ```
    Choose the command:
    > 12 (paste command)
    Choose line and index: 0 0
    Choose the command:
    > 5
    my text editor!
    ```

12. **Cut Command**
    ```
    Choose the command:
    > 11 (cut command)
    Choose line and index and number of symbols: 0 0 3
    Choose the command:
    > 5
    text editor!
    ```

13. **Encrypt File Command**
    ```
    Choose the command:
    > 13
    Enter input file path: MyFile.txt
    Enter output file path: EncryptedFile.txt
    Enter encryption key: 3
    File encrypted successfully.
    ```

14. **Decrypt File Command**
    ```
    Choose the command:
    > 14
    Enter input file path: EncryptedFile.txt
    Enter output file path: DecryptedFile.txt
    Enter decryption key: 3
    File decrypted successfully.
    ```

15. **(Optional) Implement cursor-based logic**


## How to Use
1. Clone the repository.
2. Compile the source code using a C++ compiler.
3. Ensure the necessary DLL (`caesar.dll` for encryption/decryption) is available and properly configured according to your system (Windows/Unix).
4. Run the executable.
5. Follow the on-screen instructions to use the text editor functionalities.
