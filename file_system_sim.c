// Names: Devin Diaz, Bruno Barbosa, Klaudio Vulka

/*
    A simulation of a basic file system that supports the following commands:

        format - clears filetable, disk, and freemap data structures for a clean slate of use in the system
        ls - list all files available on system
        man - lists instructions for what you can do with the system
        exit - exits the filesystem
        create <filename> - create a file and store in system
        read <filename> - read already created file in system
        write <filename> - write to an already created file in system
        delete <filename> - delete already existing file in system
    
        Files names can be 32 bytes, 512 bytes per file content, 100 total blocks available.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_FILE_DATA_BYTE_SIZE 512
#define MAX_FILE_NAME_BYTE_SIZE 32
#define MAX_OPERATION_BYTE_SIZE 16
#define TOTAL_DISK_BLOCKS 100
#define RESERVED_BLOCKS 10
#define INPUT_BUFFER_SIZE 2048
#define PROMPT_ICON '>'
#define MAX_CMD_ARGS 3

// denoted -1, 0, 2... 
typedef enum {
    FILE_NOT_FOUND = -1,
    SUCCESS = 0,
    EXIT_REQUESTED,
    SYSTEM_NOT_EMPTY,
    FILE_EXISTS,
    ERR_NO_SPACE,
    ERR_INVALID_FILE_NAME,
    ERR_INVALID_INPUT
} StatusCodes;

// fields simulating file system data structures
char fileTable[TOTAL_DISK_BLOCKS][MAX_FILE_NAME_BYTE_SIZE];
char disk[TOTAL_DISK_BLOCKS][MAX_FILE_DATA_BYTE_SIZE];
char freeMap[TOTAL_DISK_BLOCKS];

// shows entire free map blocks and allocations
void displayFreeMap(char arr[], int n) {
    printf("Printing Free Map Allocations...\n");
    for(int i = 0; i < n; i++) {
        if(i != 0 && i % 3 == 0) {
            printf("\n");
        }
        printf("Block %d: %d, ", i, (int)arr[i]);
    }
    printf("\n");
}

// shows entire file table contents
void displayFileTableData(char arr[][MAX_FILE_NAME_BYTE_SIZE]) {
    printf("Printing All File Name Content...\n");
    for(int i = 0; i < TOTAL_DISK_BLOCKS; i++) {
        printf("Block %d: %s\n", i, arr[i]);
    }
}

// shows content of files stored in disk
void displayDiskFileData(char arr[][MAX_FILE_DATA_BYTE_SIZE]) {
    printf("Printing All File Data Content...\n");
    for(int i = 0; i < TOTAL_DISK_BLOCKS; i++) {
        printf("Block %d: %s\n", i, arr[i]);
    }
}

// initiallizes filesystem, reserved blocks for system files
int formatDisk() {
    printf("System processing...\n");
    printf("Formatting Disk...\n");
    memset(freeMap, 0, sizeof(freeMap));

    // reserve first 10 blocks in disk for free map.
    for(int i = 0; i < RESERVED_BLOCKS; i++) {
        freeMap[i] = 1;
    }

    // clean slate for disk and file table 
    memset(disk, '\0', sizeof(disk));
    memset(fileTable, '\0', sizeof(fileTable));

    printf("Disk Formatted. Ready For Use.\n\n");
    return SUCCESS;
}

// checks if there's any files on the system
int systemEmpty() {
    for(int i = RESERVED_BLOCKS; i < TOTAL_DISK_BLOCKS; i++) {
        if(freeMap [i] == 1) {
            return SYSTEM_NOT_EMPTY;
        }
    }
    return SUCCESS;
}

// finds next available block on disk via free map and returns block index
int findNextDiskBlock() {
    for(int i = RESERVED_BLOCKS; i < TOTAL_DISK_BLOCKS; i++) {
        if(freeMap[i] == 0) {
            return i;
        }
    }
    fputs("No more space in system. Please delete or reformat.", stderr);
    return ERR_NO_SPACE;
}

// returns index of existing file in file table 
int searchIndexExistingFileInTable(const char *filename) {
    for(int i = RESERVED_BLOCKS; i < TOTAL_DISK_BLOCKS; i++) {
        if(strcmp(filename, fileTable[i]) == 0) {
            return i;
        }
    }
    return FILE_NOT_FOUND;
}

// convers string literal to all lowercase
int strToLower(char *str, size_t size) {
    for(int i = 0; i < strlen(str); i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
    return SUCCESS;
}

// retrieves user input from keyboard
int receiveUserInput(char *buffer, size_t size) {    
    if(fgets(buffer, size, stdin) == NULL) {
        fputs("ERR: Invalid input.\n", stderr);
        return ERR_INVALID_INPUT;
    }

    buffer[strcspn(buffer, "\n")] = '\0';
    return SUCCESS;
}

// workflow of searching file via filename from user input, index is returned if file exists in filetable
int retreiveFileIndex(const char *filename) {
    int fileIndex = searchIndexExistingFileInTable(filename);

    if (fileIndex == FILE_NOT_FOUND) {
        fputs("ERR: File does not exist.\n", stderr);
        return FILE_NOT_FOUND;
    }
    
    return fileIndex;
}

// allows users to create a file in filesystem simulation
int createFile(const char *filename) {

    if(searchIndexExistingFileInTable(filename) != FILE_NOT_FOUND) {
        fputs("ERR: Filename already exists.\n", stderr);
        return FILE_EXISTS;
    }

    int availableDiskBlockIndex = findNextDiskBlock();

    if(availableDiskBlockIndex == ERR_NO_SPACE) {
        fputs("ERR: No space on disk.\n", stderr);
        return ERR_NO_SPACE;
    }

    freeMap[availableDiskBlockIndex] = 1;
    strcpy(fileTable[availableDiskBlockIndex], filename);
    memset(disk[availableDiskBlockIndex], '\0', MAX_FILE_DATA_BYTE_SIZE);

    printf("File %s created successfully at block %d.\n", filename, availableDiskBlockIndex);
    return SUCCESS;
}

// shows file content from a block in disk via index
void displayFileContent(int fileBlockIndex) {
    for(int i = 0; i < MAX_FILE_DATA_BYTE_SIZE; i++) {
        printf("%c", disk[fileBlockIndex][i]);
    }
    printf("\n");
}

// shows content of existing file
int readFile(const char *filename) {
    int fileIndex = retreiveFileIndex(filename);

    if(fileIndex < 0) {
        return FILE_NOT_FOUND;
    }

    displayFileContent(fileIndex);
    return SUCCESS;
}

// allows users to write data to an already created file
int writeToFile(const char *filename) {
    int fileIndex = retreiveFileIndex(filename);

    if(fileIndex < 0) {
        return FILE_NOT_FOUND;
    }

    char fileContentBuffer[INPUT_BUFFER_SIZE];

    printf("Enter content: (Type X on it's own to save & exit.)\n");

    while (1) {
        if(receiveUserInput(fileContentBuffer, sizeof(fileContentBuffer)) == ERR_INVALID_INPUT) {
            return ERR_INVALID_INPUT;
        }

        if(strcmp(fileContentBuffer, "X") == 0 || strcmp(fileContentBuffer, "x") == 0) {
            break;
        }

        if(strlen(disk[fileIndex]) + strlen(fileContentBuffer) + 1 >= MAX_FILE_DATA_BYTE_SIZE) {
            fputs("Content exceeds block limit.", stderr);
            break;
        }

        strcat(disk[fileIndex], fileContentBuffer);
        strcat(disk[fileIndex], "\n");
        
    }
    return SUCCESS;
}

// deletes existing files off the filesystem
int deleteFile(const char *filename) {
    int fileIndex = retreiveFileIndex(filename);

    if(fileIndex < 0) {
        return FILE_NOT_FOUND;
    }

    char inputBuffer[INPUT_BUFFER_SIZE];
    char fileName[MAX_FILE_NAME_BYTE_SIZE];
    strcpy(fileName, fileTable[fileIndex]);

    while(1) {
        printf("File to be deleted: %s. (Y/n) to confirm: ", fileName);

        if(receiveUserInput(inputBuffer, sizeof(inputBuffer)) == ERR_INVALID_INPUT) {
            return ERR_INVALID_INPUT;
        }

        if(strcmp(inputBuffer, "Y") == 0 || strcmp(inputBuffer, "y") == 0) {
            memset(disk[fileIndex], '\0', sizeof(disk[fileIndex]));
            memset(fileTable[fileIndex], '\0', sizeof(fileTable[fileIndex]));
            freeMap[fileIndex] = 0;            
            return SUCCESS;
        }

        else if(strcmp(inputBuffer, "N") == 0 || strcmp(inputBuffer, "n") == 0) {
            return SUCCESS;
        }

        else {
            printf("Invalid input. Try again.\n");
        }

    }
    return SUCCESS;
}

// lists all files allocated in system
int listFiles() {
    printf("Listing files in system...\n\n");

    if(systemEmpty() != SYSTEM_NOT_EMPTY) {
        printf("No files on system.\n");
        return SUCCESS;
    }

    for(int i = RESERVED_BLOCKS; i < TOTAL_DISK_BLOCKS; i++) {
        if(freeMap[i] == 1) {
            printf("Block %d: %s\n", i, fileTable[i]);
        }
    }
    return SUCCESS;
}

// exits file system sim
int exitFileSystem() {
    printf("Exiting file system simulator.\n");
    printf("=============================\n\n");
    return SUCCESS;
}

// displays options to the user of what they can do with this filesystem sim
void fileSystemMenu() {
    const char menu[] = 
        "\n====================================\n"
        "Welcome to our file system simulator!\n\n"
        "Our system allows the following operations for use:\n"
        "1. CREATE Files. e.g.  > create <filename>\n"
        "2. READ Files. e.g. > read <filename>\n"
        "3. WRITE Files. e.g. > write <filename>\n"
        "4. DELETE Files. e.g. > delete <filename>\n"
        "5. LIST ALL files. e.g. > ls\n"
        "6. EXIT. e.g. > exit\n"
        "7. MANUAL Page. e.g. > man\n"
        "====================================\n\n";

    printf("%s", menu);
}

// logic for selecting and executing operations available in the system
int selectFileSystemOperation(const char *operation, const char *filename) {
    if(strcmp(operation, "format") == 0) {
        formatDisk();
        return SUCCESS;
    }
    else if(strcmp(operation, "ls") == 0) {
        listFiles();
        return SUCCESS;
    }
    else if(strcmp(operation, "man") == 0) {
        fileSystemMenu();
        return SUCCESS;
    }
    else if (strcmp(operation, "exit") == 0) {
        return EXIT_REQUESTED;
    }

    if(strcmp(operation, "create") == 0) {
        if (filename == NULL) {
            fputs("ERR: Missing filename for create.\n", stderr);
            return ERR_INVALID_INPUT;
        }
        return createFile(filename);
    }
    else if(strcmp(operation, "read") == 0) {
        if (filename == NULL) {
            fputs("ERR: Missing filename for read.\n", stderr);
            return ERR_INVALID_INPUT;
        }
        return readFile(filename);
    }
    else if(strcmp(operation, "write") == 0) {
        if (filename == NULL) {
            fputs("ERR: Missing filename for write.\n", stderr);
            return ERR_INVALID_INPUT;
        }
        return writeToFile(filename);
    }
    else if(strcmp(operation, "delete") == 0) {
        if (filename == NULL) {
            fputs("ERR: Missing filename for delete.\n", stderr);
            return ERR_INVALID_INPUT;
        }
        return deleteFile(filename);
    }

    fputs("ERR: Invalid operation.\n", stderr);
    return ERR_INVALID_INPUT;
}

// initaties system, parses arguments, and sends them to operations available. continues workflow
int fileSystemSimulator() {
    fileSystemMenu();
    formatDisk();

    // cmd args
    char inputBuffer[INPUT_BUFFER_SIZE];

    while(1) {
        printf("%c ", PROMPT_ICON);
        
        if(receiveUserInput(inputBuffer, sizeof(inputBuffer)) == ERR_INVALID_INPUT) {
            return ERR_INVALID_INPUT;
        }
        
        char *argsBuffer[MAX_CMD_ARGS];
        memset(argsBuffer, '\0', sizeof(argsBuffer));
        int argsCounter = 0;
        
        char *token = strtok(inputBuffer, " ");

        while(token != NULL) {
            if(argsCounter >= MAX_CMD_ARGS) {
                fputs("Arguments exceed maximum. Try again.", stderr);
                argsCounter = -1;
                break;
            }
            argsBuffer[argsCounter++] = token;
            token = strtok(NULL, " ");
        }

        if(argsCounter == -1) {
            continue;
        }

        char *operation = argsBuffer[0];
        char *filename = NULL;

        if (argsCounter == 2) {
            filename = argsBuffer[1];
        }
        else if (argsCounter > 2) {
            fputs("ERR: Too many arguments.\n", stderr);
            continue;
        }

        strToLower(operation, strlen(operation));

        if (filename != NULL && strlen(filename) >= MAX_FILE_NAME_BYTE_SIZE) {
            fputs("ERR: Filename exceeds sizing capacity.\n", stderr);
            continue;
        }

        int status = selectFileSystemOperation(operation, filename);

        if (status == EXIT_REQUESTED) {
            break;
        }

    }
    return SUCCESS;
}

// starts file system simulator
int runFileSystemSimulator(){
    fileSystemSimulator();
    return SUCCESS;
}

// run program
int main() {
    runFileSystemSimulator();
    return SUCCESS;
}
