# Simple Text Editor (C++)

A basic console-based text editor implemented in C++ using procedural and structural programming principles. The editor supports fundamental text manipulation operations and file I/O through a simple command-line interface.

---

## Features

- Append, insert, delete, and manipulate text
- Save and load documents
- Search, cut, copy, and paste
- Optional Caesar cipher encryption/decryption via DLL
- Cross-platform CMake support (Windows DLL usage recommended)

> 🔐 **Encryption/Decryption** is powered by a dynamic library (`caesar.dll`) built from:  
> [github.com/iravelmakina/caesar-encryption-algorithm](https://github.com/iravelmakina/caesar-encryption-algorithm)

---

## Example Commands

### 1. Append text
```
Choose the command:
> 1
Enter text to append: Hello, text editor!
```

### 2. Start a new line
```
Choose the command:
> 2
New line is started
```

### 3–4. Save and load from file
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

### 5. Print current text
```
Choose the command:
> 5
Hello, text editor!
```

### 6. Insert text by position
```
Choose the command:
> 6
Choose line and index: 0 6
Enter text to insert: my

Choose the command:
> 5
Hello, my text editor!
```

### 7. Search text
```
Choose the command:
> 7
Enter text to search: Hello
Text is present in this position: 0 0
```

### 8. Delete text
```
Choose the command:
> 8
Choose line, index and number of symbols: 0 0 7

Choose the command:
> 5
my text editor!
```

### 9. Insert with replacement
```
Choose the command:
> 14
Choose line and index: 0 0
Write text: Hello, world!

Choose the command:
> 5
Hello, world!or!my text editor!
```

### 10. Copy text
```
Choose the command:
> 13
Choose line and index and number of symbols: 0 0 15

Choose the command:
> 12
Choose line and index: 0 0

Choose the command:
> 5
my text editor!my text editor!
```

### 11. Paste text
```
Choose the command:
> 12
Choose line and index: 0 0

Choose the command:
> 5
my text editor!
```

### 12. Cut text
```
Choose the command:
> 11
Choose line and index and number of symbols: 0 0 3

Choose the command:
> 5
text editor!
```

### 13. Encrypt file
```
Choose the command:
> 13
Enter input file path: MyFile.txt
Enter output file path: EncryptedFile.txt
Enter encryption key: 3
File encrypted successfully.
```

### 14. Decrypt file
```
Choose the command:
> 14
Enter input file path: EncryptedFile.txt
Enter output file path: DecryptedFile.txt
Enter decryption key: 3
File decrypted successfully.
```

### 15. Ser cursor position
```
Enter the line index: 1
Enter the character index: 6
Cursor is at line 1, position 6.
```

---

## Build Instructions

### Prerequisites

- C++ compiler (GCC, Clang, MSVC)
- CMake 3.10+
- Windows OS (for DLL usage)
- `caesar.dll` from [Caesar Encryption Algorithm](https://github.com/iravelmakina/caesar-encryption-algorithm)

### Steps

```bash
git clone https://github.com/iravelmakina/simple-text-editor.git
cd simple-text-editor
mkdir build && cd build
cmake ..
cmake --build .
```

> Ensure `caesar.dll` is placed in the same folder as the output executable or in a path available to the linker/runtime.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Contributor

- [@iravelmakina](https://github.com/iravelmakina)
