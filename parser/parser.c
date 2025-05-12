#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>

/**
 * This function is an EDID parser that can take an input of the raw EDID data
*/

#define EDID_LENGTH 128

void parse_edid(const unsigned char *edid){
    if (!check_header(edid)) {
        printf("Invalid EDID header.\n");
        return;
    }

    printf("Valid EDID header.\n");
    parse_manufacturer_id(edid);
    parse_edid_version(edid);
    parse_product_code(edid);
    parse_serial_number(edid);
}

/*
* This method checks if the header is a valid header or not
*/
int check_header(const unsigned char *edid){
    if (edid[0] != 0x00 || edid[1] != 0xFF || edid[2] != 0xFF || edid[3] != 0xFF || edid[4] != 0xFF || edid[5] != 0xFF || 
        edid[6] != 0xFF || edid[7] != 0x00) {
        return 0;  // Invalid header
    }
    return 1;  // Valid header
}

/*
* This method prints the version and revision
*/
void parse_edid_version(const unsigned char *edid) {
    uint8_t version = edid[0x12];  // EDID version byte
    uint8_t revision = edid[0x13];  // EDID revision byte
    printf("EDID Version: %d.%d\n", version, revision);
}

/*
* This method prints the manufacturer id
*/
void parse_manufacturer_id(const unsigned char *edid) {
    uint16_t manufacturer = (edid[8] << 8) | edid[9]; // Combine bytes 8 and 9

    char manufacturer_id[4];

    manufacturer_id[0] = ((manufacturer >> 10) & 0x1F) + 'A' - 1; //ASCII
    manufacturer_id[1] = ((manufacturer >> 5) & 0x1F) + 'A' - 1;
    manufacturer_id[2] = (manufacturer & 0x1F) + 'A' - 1;
    manufacturer_id[3] = '\0';

    printf("Manufacturer ID: %s\n", manufacturer_id);
}

/*
* This method prints the product code
*/
void parse_product_code(const unsigned char *edid) {
    // Combine the two bytes
    uint16_t product_code = edid[10] | (edid[11] << 8);
    printf("Product Code: %u (0x%04X)\n", product_code, product_code);
}

/*
* This method prints the serial number 
*/
void parse_serial_number(const unsigned char *edid) {
    // Combine 4 bytes
    uint32_t serial = edid[12] | (edid[13] << 8) | (edid[14] << 16) | (edid[15] << 24);
    printf("Serial Number: %u (0x%08X)\n", serial, serial);
}


int main() {
    /*FILE *file = fopen("edid_dump.bin", "rb"); //testing using the example edid
    if (!file){
        perror("Failed to open");
        return 1;
    }

    unsigned char edid[128];
    size_t bytesRead = fread(edid, 1, sizeof(edid), file);
    fclose(file);

    if (bytesRead != 128) {
        fprintf(stderr, "Error: Expected 128 bytes but read %zu\n", bytesRead);
        return 1;
    }*/
    unsigned char edid[128] = {
        0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
        0x10, 0xAC, 0x79, 0x42, 0x4C, 0x47, 0x5A, 0x42,
        0x0F, 0x22, 0x01, 0x04, 0xB5, 0x3C, 0x22, 0x78,
        0x3A, 0xDF, 0x15, 0xAD, 0x50, 0x44, 0xAD, 0x25,
        0x0F, 0x50, 0x54, 0xA5, 0x4B, 0x00, 0xD1, 0x00,
        0xD1, 0xC0, 0xB3, 0x00, 0xA9, 0x40, 0x81, 0x80,
        0x81, 0x00, 0x71, 0x4F, 0xE1, 0xC0, 0x4D, 0xD0,
        0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20,
        0x35, 0x00, 0x55, 0x50, 0x21, 0x00, 0x00, 0x1A,
        0x00, 0x00, 0x00, 0xFF, 0x00, 0x32, 0x33, 0x5A,
        0x53, 0x4A, 0x30, 0x34, 0x0A, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x44,
        0x45, 0x4C, 0x4C, 0x20, 0x55, 0x32, 0x37, 0x32,
        0x33, 0x51, 0x45, 0x0A, 0x00, 0x00, 0x00, 0xFD,
        0x00, 0x17, 0x56, 0x0F, 0x8C, 0x36, 0x01, 0x0A,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x2A
    };
    parse_edid(edid);

}
