# FAT File System Implementation

## Overview
This project implements a File Allocation Table (FAT) based file system with support for files and directories. The system provides basic file operations like create, read, write, delete, and directory navigation.

## Project Structure

### Directory Structure
```
.
|-- Makefile
|-- include
|   |-- command_line.h
|   |-- directory.h
|   |-- disk_operations.h
|   |-- fat.h
|   |-- file.h
|   |-- folder.h
|   |-- meta_data.h
|   `-- utils.h
|-- infinite_disk.bin
|-- meta_data.bin
`-- src
    |-- command_line.c
    |-- directory.c
    |-- disk_operations.c
    |-- fat.c
    |-- file.c
    |-- folder.c
    |-- main.c
    |-- meta_data.c
    `-- utils.c
```

### File Descriptions
- **Makefile**: Build configuration for the project
- **infinite_disk.bin**: Binary file representing the disk storage
- **meta_data.bin**: Binary file storing file system metadata
- **include/**: Header files for all modules
- **src/**: Implementation files for all modules

## File System Architecture

### Metadata Structure
The file system metadata contains all configuration parameters and limits:

```c
typedef struct meta_data {
    uint64_t DISK_SIZE : 40;                    // 2^40 = 1 TB
    uint32_t CLUSTER_SIZE : 20;                 // 2^20 = 1 MB
    uint32_t NO_OF_CLUSTERS_FOR_FILES : 20;     // 2^20 = 1 MB
    uint16_t MAX_FILE_SIZE_IN_CLUSTERS : 10;    // 1K clusters
    uint32_t MAX_FILE_SIZE : 30;                // 1K * 1MB = 1GB
    uint8_t MAX_FILE_NAME_IN_BYTES;             // 256B
    uint16_t MAX_FOLDERS : 11;                  // 2K folders
    uint32_t MAX_FILES : 20;                    // 1M files
    uint32_t MAX_FULL_SIZE_FILES : 19;          // 512K files
    uint32_t NO_OF_DIR_ENTRIES : 21;            // 2M entries
    uint16_t DIR_ENTRY_SIZE_IN_BYTES : 12;      // 285 bytes
    uint64_t DIR_SIZE : 33;                     // 570MB
    uint64_t DIR_SIZE_IN_CLSTRS;                // Cluster count for directory
    uint32_t FAT_ENTRIES : 20;                  // 1M entries
    uint32_t FAT_SIZE;                          // 2MB
    uint32_t FAT_SIZE_IN_CLSTRS;                // Cluster count for FAT
    uint32_t FIRST_FREE_CLSTR : 20;             // First free cluster index
    uint8_t FREE_CLSTR_REG_IDX;                 // Registry index
    uint32_t CLUSTER_OFFSET_IN_BYTES;           // Byte offset to clusters
    uint32_t CLUSTER_OFFSET_IN_CLSTRS;          // Cluster offset
    uint64_t TOTAL_SIZE;                        // Total disk size
    uint32_t TOTAL_CLSTRS;                      // Total cluster count
} meta_data;
```

### Directory Entry Structure
Each directory entry uses the following format:

```
| Field         | Size (bits) | Description                          |
|---------------|-------------|--------------------------------------|
| idx           | 21          | Entry index                          |
| parentIdx     | 21          | Parent folder index                  |
| name          | 2048        | File name (64 chars)                 |
| ext           | 32          | File extension (3 chars)             |
| size          | 30          | File size in bytes                   |
| firstBlk      | 20          | First block index                    |
| accessBits    | 9           | RWX permissions (owner-group-other)  |
| deleted       | 1           | Deletion flag                        |
| isFile        | 1           | File/Folder flag                     |
| isValid       | 1           | Valid entry flag                     |
| CreationTime  | 32          | Creation timestamp                   |
| AccessTime    | 32          | Last access timestamp                |
| ModifyTime    | 32          | Last modification timestamp          |
```

Total size per directory entry: 285 bytes

### Disk Layout
The disk is organized in the following sequence:

1. **First Free Block Register**: 20 bits
2. **FAT Table**: Contains cluster allocation information
3. **Directory**: Contains all file/directory metadata
4. **Data Clusters**: Actual file data storage

## Features

### Supported Commands
- **cd [folder]**: Change current directory
- **cd ..**: Move to parent directory
- **ls**: List contents of current directory
- **ls [folder]**: List contents of specified folder
- **cat [file]**: Display file contents
- **touch [file]**: Create new file
- **vim [file]**: Edit file contents
- **mkdir [folder]**: Create new directory
- **rmdir [folder]**: Remove directory
- **rm [file]**: Remove file
- **exit**: Exit the file system

### Key Operations
- File creation and deletion
- Directory creation and deletion
- File reading and writing
- Directory navigation
- FAT table management
- Metadata persistence

## Implementation Details

### Default Configuration
- Disk size: 64MB
- Cluster size: 1KB
- Maximum file size: 128KB (128 clusters)
- Maximum file name length: 64 bytes
- Maximum folders: 1024

### Data Structures
- **FAT Table**: Tracks cluster allocation and chaining
- **Directory**: Manages all file system entries
- **Metadata**: Stores file system configuration

## Building and Running

### Build
Run `make` to compile the project.

### Usage
Execute the compiled binary to start the file system shell:
```
$ ./fatfs
```

## Example Session
```
current dir : root >> mkdir folder1
current dir : root >> cd folder1
current dir : folder1 >> touch file1.txt
current dir : folder1 >> vim file1.txt
[Enter file contents...]
current dir : folder1 >> cat file1.txt
[File contents displayed...]
current dir : folder1 >> cd ..
current dir : root >> ls
folder1/
current dir : root >> exit
```

## Technical Notes
- The file system uses a 20-bit FAT table for cluster management
- Directory entries are fixed-size (285 bytes) for efficient storage
- First cluster starts at offset 19133443 bytes (after metadata)
- Maximum theoretical file size is 128KB in default configuration

## Limitations
- No user authentication/authorization
- Basic permission system only
- Fixed maximum sizes for files and directories
- No symbolic links or hard links

## Future Enhancements
- Implement file compression
- Add user permissions
- Support larger file sizes
- Add journaling for crash recovery
- Implement file encryption