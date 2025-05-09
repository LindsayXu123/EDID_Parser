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
}

int check_header(const unsigned char *edid){
    if (edid[0] != 0x00 || edid[1] != 0xFF || edid[2] != 0xFF || edid[3] != 0xFF || edid[4] != 0xFF || edid[5] != 0xFF || 
        edid[6] != 0xFF || edid[7] != 0x00) {
        return 0;  // Invalid header
    }
    return 1;  // Valid header
}

void parse_edid_version(const unsigned char *edid) {
    uint8_t version = edid[0x12];  // EDID version byte
    uint8_t revision = edid[0x13];  // EDID revision byte
    printf("EDID Version: %d.%d\n", version, revision);
}

int main() {
    FILE *file = fopen("edid_dump.bin", "rb"); //testing using the example edid
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
    }

    parse_edid(edid);

}
