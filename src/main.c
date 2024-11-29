#include "../include/utils.h"
#include "../include/meta_data.h"
#include "../include/directory.h"
#include "../include/fat.h"
#include "../include/folder.h"
#include "../include/file.h"
#include "../include/disk_operations.h"
#include "../include/command_line.h"

/*
Dir Enteries
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~ idx : 12 M enteries --- max val = 2^32 = 4294967296 --- len of directory
~ in bits : 32 bits
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~ parentIdx : idx of the parent folder (any val from all the idxs)
~ in bits : 32 bits
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~ name (of the file): max_chars = 64 
~ in bits : 64*8 = 512-bits
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~ ext : (extention of the file) max_chars = 3
~ in bits : 3*8 = 24 bits
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~ size (of file in bytes) : max_size = 128 KB = 2^17 
~ in bits : 17 + 3 (extra) = 20 bits 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~ firstBlk : 64K enteries = 2^16  
~ in bits : 16 bits 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~ accessBits : rwx-rxw-rwx 
~ in bits : 3+3+3 = 9 bits 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~ deleted : to see if the file is valid or not (doesn't dlt permantly)
~ in bits : 1 - bit (0 or 1) 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~ isFile : an attribute to check if it is a file or folder
~ in bits : 1 - bit (0 or 1)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~ max - date : 31-12-3999 round of to 32-16-4096 = (2^5)-(2^4)-(2^12) 
~ in bits : 5+4+12 = 21 bits
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~ max - time : 24:59:59 round of to 32:64:64 = (2^5):(2^6):(2^6)
~ in bits : 5+6+6 = 17 bits
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
?~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~        TOTAL SIZE OF ONE ENTERY IN DIR 
~   = 21 + 21 + 2048 + 32 + 30 + 20 + 9 + 1 + 1 + 1 + (32*3)
~   = 2280 bits = 285 bytes ~ 288 bytes (size of struct of dir_entry)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~           TOTAL SIZE OF DIRECORY
~   = SIZE OF ONE ENTRY * NO OF ENTERIES
~   = 730 * 65536 = 730*64 k = 46720 k bits = 5840 KB
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

&________________________________________________________________________________________________________________________________________________________________________________________________________________
&|   idx    |   parentIdx    |    name    |     ext     |    size    |   firstBlk   |    accessbit     |   deleted     |   isfile    |   isValid    |    CreationTime    |     AccessTime    |     ModifyTime    |
&|__21-bits_|_____21-bits____|__2048-bits_|____32-bits__|___30-bits__|____20-bits___|__(r-w-x)_9-bits__|____1-bit______|____1-bit____|____1-bit_____|_______32-bits______|______32-bits______|______32-bits______|          



! SEQUENCE OF DATA IN FILE (disk)

! 20 - bits : first free block
! FAT table 
! DIRECTORY
! 1st Block
! 2nd Block 
! ... so on


*/


extern meta_data md;


int main_formate() {
    uint64_t disk_size = 67108864;              // 64 MB
    uint32_t cluster_size = 1024;               // 1 K
    uint16_t max_file_size_in_clstr = 128;      // 128 blocks
    uint8_t max_file_name_in_bytes = 64;        // 64 bytes
    uint16_t max_folders = 1024;                // 1 K folders
    const char* disk_name = "infinite_disk.bin";

    FILE* disk = fopen(disk_name, "rb+");
    printf("here!");
    fflush(stdout);
    if (formate_disk(disk, disk_name, disk_size, cluster_size, max_file_size_in_clstr, max_file_name_in_bytes, max_folders)) {
        printf("\nDISK FORMATED SUCCESSFULLY !\n");
    }
    fclose(disk);
    
    return 0;
}

int main() {

    uint64_t disk_size = 67108864;              // 64 MB
    uint32_t cluster_size = 1024;               // 1 K
    uint16_t max_file_size_in_clstr = 128;      // 128 blocks
    uint8_t max_file_name_in_bytes = 64;        // 64 bytes
    uint16_t max_folders = 1024;                // 1 K folders

    uint32_t current_folder_idx = 0;
    uint32_t parent_folder = 0; 
    char current_folder_name[256] = "root";

    const char* disk_name = "infinite_disk.bin";

    if (compute_meta_data(&md, disk_size, cluster_size, max_file_size_in_clstr, max_file_name_in_bytes, max_folders)) {
        
        print_meta_data(&md);
        write_meta_data_in_file("meta_data.bin", &md);
        create_disk(disk_name, md.TOTAL_SIZE);

        FILE* disk = fopen(disk_name, "rb+");
        directory directory = initialize_directory(md.NO_OF_DIR_ENTRIES);
        FAT fat = initialize_fat(md.FAT_ENTRIES);
        uint32_t first_free = 0;

        create_folder(&directory, "folder1", 0);
        create_folder(&directory, "folder2", 0);
        create_folder(&directory, "folder3", 1);
        create_folder(&directory, "folder4", 2);

        create_file(&directory, "file.txt", 0);

        write_fat(disk, &fat);
        read_fat(disk, &fat);
        print_fat(&fat);
        print_directory(&directory);
        write_directory(disk, &directory);
        read_directory(disk, &directory);
        printf("%u", directory.no_of_entries);
        print_directory(&directory);


        while (1) {

            printf("\ncurrent dir : %s\n", current_folder_name);
            current_folder_name[0] = '\0';

            char* f_name;
            int op_id = input_command(&f_name);
            
            if (op_id == CD_FOLDER) {
                uint32_t idx = find_entry_by_name(&directory, f_name, parent_folder);
                parent_folder = current_folder_idx;
                current_folder_idx = idx;
                strncpy(current_folder_name, f_name, strlen(f_name));
            } 
            else if (op_id == CD_PARENT) {
                current_folder_idx = parent_folder;
                parent_folder = get_parent_by_idx(&directory, current_folder_idx);
                char* name = get_name_by_idx(&directory, current_folder_idx);
                strncpy(current_folder_name, name, strlen(name));
            } 
            else if (op_id == LS_SELF) {
                print_children(&directory, current_folder_idx);
            } 
            else if (op_id == LS_FOLDER) {
                uint32_t idx = find_entry_by_name(&directory, f_name, parent_folder);
                print_children(&directory, idx);
            } 
            else if (op_id == CAT) {
                uint32_t idx = find_entry_by_name(&directory, f_name, current_folder_idx);
                char* contents;
                uint32_t size;
                read_from_file(disk, &directory, &fat, idx, &contents, &size);
                printf("\n%s", contents);
            } 
            else if (op_id == TOUCH) {
                if (!create_file(&directory, f_name, current_folder_idx)) OPERATION_UNSUCCESSFUL();
            } 
            else if (op_id == VIM) {
                uint32_t idx = find_entry_by_name(&directory, f_name, current_folder_idx);
                char* content = "this is safanfl nlsn\nnkabkbvjhbekwb mnbskfwlb skwbev fw";
                // uint32_t size = input_file_content(&content);
                write_in_file(disk, &directory, &fat, &first_free, idx, &content, strlen(content));
            } 
            else if (op_id == MKDIR) {
                if (!create_folder(&directory, f_name, current_folder_idx)) OPERATION_UNSUCCESSFUL();
            } 
            else if (op_id == RMDIR) {
                uint32_t idx = find_entry_by_name(&directory, f_name, current_folder_idx);
                delete_entry(&directory, idx);
            } 
            else if (op_id == RM) {
                uint32_t idx = find_entry_by_name(&directory, f_name, current_folder_idx);
                delete_entry(&directory, idx);
            } 
            else {
                perror("\nINVALID OPERATION !\n");
                return 0;
            }

        }

        fclose(disk);
    }
  

    return 0;
}



struct _20_bit {
    uint32_t i : 20; // 20-bit field
};

void write_20_bits(FILE* file, uint32_t value) {
    unsigned char buffer[3]; // 3 bytes are sufficient for 20 bits
    buffer[0] = value & 0xFF;          // Extract the least significant 8 bits
    buffer[1] = (value >> 8) & 0xFF;   // Extract the next 8 bits
    buffer[2] = (value >> 16) & 0x0F;  // Extract the upper 4 bits (20 bits total)

    fwrite(buffer, 1, 3, file); // Write exactly 3 bytes
}

int main1() {
    struct _20_bit it;
    it.i = 0xABCDE; // Example 20-bit value

    // Check the size of the struct
    printf("Size of struct _20_bit: %zu bytes\n", sizeof(struct _20_bit));  
    fflush(stdout);

    // Open the binary file
    FILE* file = fopen("./temp.bin", "wb");
    if (!file) {
        printf("FILE issue !");
        return 1;
    }

    // Write the 20-bit value
    write_20_bits(file, it.i);

    fclose(file);
    printf("20-bit data written successfully.\n");
    return 0;
}
