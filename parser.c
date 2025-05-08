#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>

/**
 * This function is an EDID parser that can take an input of the raw EDID data
*/

 #define EDID_LENGTH 128

void parse_edid(const unsigned char *edid){

}

int check_header(const unsigned char *edid){
    if (edid[0] != 0x00 || edid[1] != 0xFF || edid[2] != 0xFF || edid[3] != 0xFF || edid[4] != 0xFF || edid[5] != 0xFF || 
        edid[6] != 0xFF || edid[7] != 0x00) {
        return 0;  // Invalid header
    }
    return 1;  // Valid header
}

