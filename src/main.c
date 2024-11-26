#include "include/utils.h"

// #define DISK_SIZE 67108864                                       // 64 MB 
// #define CLUSTER_SIZE 1024                                        // 1 KB
// #define NO_OF_CLUSTERS DISK_SIZE / CLUSTER_SIZE                  // 64 K clusters
// #define MAX_FILE_SIZE 128 * CLUSTER_SIZE                         // 128-clusters = 128*1k = 128 KB 
// #define MAX_FILE_NAME 64                                         // 64 B
// #define MAX_FULL_SIZE_FILES MAX_FILE_SIZE / DISK_SIZE            // 1/2 K files 
// #define DIRECTORY_ENTERIES 2 * NO_OF_CLUSTERS                    // 2*64 K = 128 K

/*

! disk_size = 64MB

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



! SEQUENCE OF DATA IN FILE

! 20 - bits : first free block
! FAT table 
! DIRECTORY
! 1st Block
! 2nd Block 
! ... so on


*/


meta_data md;


// Function to update the first 20 bits of the file
void update_first_20_bits(const char *filename, uint32_t value) {
    if (value > 0xFFFFF) { // Ensure the value fits in 20 bits
        printf("Error: Value exceeds 20 bits.\n");
        return;
    }

    FILE *file = fopen(filename, "rb+"); // Open file for read/write in binary mode
    if (!file) {
        perror("Error opening file");
        return;
    }

    // Split the 20-bit value into 3 bytes
    uint8_t buffer[3];
    buffer[0] = (value >> 12) & 0xFF;        // High 8 bits
    buffer[1] = (value >> 4) & 0xFF;         // Middle 8 bits
    buffer[2] = (value & 0xF) << 4;          // Low 4 bits

    // Write the 3 bytes to the file
    fseek(file, 0, SEEK_SET); // Go to the beginning of the file
    fwrite(buffer, 1, 3, file);

    fclose(file);
    printf("First 20 bits updated successfully.\n");
}

void create_disk(const char* disk_name, uint64_t disk_size) {
    FILE* file = fopen(disk_name, "wb");
    
    uint8_t value = 0;
    uint8_t buffer[3];
    buffer[0] = (value >> 12) & 0xFF;        // High 8 bits
    buffer[1] = (value >> 4) & 0xFF;         // Middle 8 bits
    buffer[2] = (value & 0xF) << 4;          // Low 4 bits
    
    fseek(file, 0, SEEK_SET); 
    fwrite(buffer, 1, 3, file);


    fseek(file, disk_size , SEEK_SET);
    fputc('\0', file);
    fclose(file);

}

void formate_disk(FILE* disk) {
    
}

int main2() {

    printf("%lu", sizeof(dir_entry));
    printf("%lu", sizeof(dir_entry));

    return 0;
}

int main() {

    uint64_t disk_size = 67108864;              // 64 MB
    uint32_t cluster_size = 1024;               // 1 K
    uint16_t max_file_size_in_clstr = 128;      // 128 blocks
    uint8_t max_file_name_in_bytes = 64;        // 64 bytes
    uint16_t max_folders = 1024;                // 1 K folders

    uint32_t current_folder = 0;

    const char* disk_name = "infinite_disk.bin";
    
    if (compute_meta_data(&md, disk_size, cluster_size, max_file_size_in_clstr, max_file_name_in_bytes, max_folders)) {
        
        print_meta_data(&md);
        write_meta_data_in_file("meta_data.bin", &md);

        create_disk(disk_name, md.TOTAL_SIZE);

        FILE* disk = fopen(disk_name, "rb+");
        directory directory = initialize_directory(md.NO_OF_DIR_ENTRIES);
        create_folder(&directory, "folder1", 0);
        create_folder(&directory, "folder2", 0);
        create_folder(&directory, "folder3", 1);
        create_folder(&directory, "folder4", 2);

        FAT fat = initialize_fat(md.FAT_ENTRIES);
        write_fat(disk, &fat);
        read_fat(disk, &fat);
        print_fat(&fat);
        print_directory(&directory);
        write_directory(disk, &directory);
        read_directory(disk, &directory);
        printf("%u", directory.no_of_entries);
        print_directory(&directory);


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
