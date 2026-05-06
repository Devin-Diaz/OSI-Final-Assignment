# Simple File System Simulator (C)

## Overview

This project implements a simplified command-line file system in C. The goal is to simulate how an operating system manages disk storage using core data structures such as a free map and file table.

The system supports basic file operations including creating, reading, writing, deleting, and listing files. Each file occupies exactly one disk block, making the system easier to reason about while still demonstrating key filesystem concepts.

---

## Video Link
* https://youtu.be/f0G6GL5IQGg
---

## Team Members and Responsibilities

### Devin Diaz
* Designed and implemented the command-line interface workflow
* Developed user input handling and command parsing logic
* Integrated filesystem operations into the simulator loop
* Implemented overall system coordination and operation selection logic
* Assisted with debugging, validation, and overall program structure

### Klaudio Vulka 
* Designed the core filesystem data structures
* Implemented the primary global data fields
* Developed the `formatDisk()` functionality
* Helped establish the simulated disk initialization process

### Bruno Barbosa
* Implemented the primary filesystem operations:
  * `create`
  * `write`
  * `read`
  * `delete`
  * `ls`
  * Assisted with filesystem functionality and operation logic

## Features

* Format a simulated disk
* Create files (single-block storage)
* Write content to files
* Read file content
* Delete files
* List all files (`ls`)
* Command-line interface with argument parsing
* Input validation and error handling

---

## System Design

### Disk Structure

* Total blocks: 100
* Block size: 512 bytes
* Blocks `0–9`: Reserved for filesystem metadata (Free Map)
* Blocks `10–99`: Available for file storage

---

### Core Data Structures

#### 1. Disk

```c
char disk[TOTAL_DISK_BLOCKS][MAX_FILE_DATA_BYTE_SIZE];
```

Stores the actual content of files.

#### 2. Free Map

```c
char freeMap[TOTAL_DISK_BLOCKS];
```

Tracks allocation status of each block:

* `0` → Free
* `1` → Allocated

#### 3. File Table

```c
char fileTable[TOTAL_DISK_BLOCKS][MAX_FILE_NAME_BYTE_SIZE];
```

Maps filenames to disk blocks:

* `fileTable[i]` stores the filename assigned to block `i`

---

## Supported Commands

| Command             | Description                         |
| ------------------- | ----------------------------------- |
| `format`            | Resets the disk and clears all data |
| `create <filename>` | Creates a new file                  |
| `read <filename>`   | Displays file content               |
| `write <filename>`  | Writes content to a file            |
| `delete <filename>` | Deletes a file                      |
| `ls`                | Lists all files                     |
| `exit`              | Exits the simulator                 |

---

## Example Usage

```bash
> create file1
> write file1
Enter content:
Hello World
X
> read file1
Hello World

> ls
file1

> delete file1
```

---

## How to Compile and Run

### Compile

```bash
gcc file_system_sim.c -o file_system_sim
```

### Run

```bash
./file_system_sim
```

---

## Key C Programming Concepts Used

### 1. Pointers and Arrays

* Use of `char *` for string handling
* Passing arrays to functions via pointers

### 2. Strings in C

* `strcpy`, `strcmp`, `strlen`, `strtok`
* Null-terminated strings (`'\0'`)

### 3. Memory Management

* Static memory allocation using arrays
* Clearing memory with `memset`

### 4. Input Handling

* Safe input using `fgets`
* Buffer management and validation

### 5. Control Flow

* Infinite loops for command interpreter (`while(1)`)
* Conditional branching (`if`, `else if`)

### 6. Modular Design

* Separation of concerns:

  * Input parsing
  * Command selection
  * File operations

### 7. Error Handling

* Return codes (`SUCCESS`, `ERR_*`)
* Defensive programming (checking bounds, null values)

### 8. String Tokenization

```c
strtok()
```

Used to parse user commands into arguments.

---

## Design Decisions

* **Single-block files**: Simplifies storage and avoids fragmentation complexity
* **Free Map as source of truth**: Ensures consistent allocation tracking
* **Command parsing at top level**: Avoids redundant input handling inside operations
* **Temporary input buffers**: Prevents buffer overflow and allows validation before storage

---

## Limitations

* No persistent storage (data resets on program restart)
* No multi-block file support
* No directory hierarchy (single root only)
* Limited file size (one block per file)

---

## Future Improvements

* Multi-block file support
* Persistent disk storage using binary files
* Directory structure (folders)
* Improved command parsing
* File metadata (timestamps, size tracking)

---


