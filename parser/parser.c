#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>

/**
 * This function is an EDID parser that can take an input of the raw EDID data
*/

#define EDID_LENGTH 128

FILE *out = NULL;

int check_header(const unsigned char *edid);
void parse_manufacturer_id(const unsigned char *edid);
void parse_product_code(const unsigned char *edid);
void parse_serial_number(const unsigned char *edid);
void parse_manufacture_date(const unsigned char *edid);
void parse_edid_version(const unsigned char *edid);
void parse_video_input(const unsigned char *edid);
void parse_screen_size(const unsigned char *edid);
void parse_display_gamma(const unsigned char *edid);
void parse_supported_features(const unsigned char *edid);
void parse_colour_characteristics(const unsigned char *edid);

void parse_edid(const unsigned char *edid){

    out = fopen("output.txt", "w");
    if (!out) {
        perror("Failed to open output file");
        return;
    }

    if (!check_header(edid)) {
        printf("Invalid EDID header.\n");
        if (out) fprintf(out, "Invalid EDID header\n");
        return;
    }

    printf("Valid EDID header.\n");
    if (out) fprintf(out, "Valid EDID header\n");
    parse_manufacturer_id(edid);
    parse_product_code(edid);
    parse_serial_number(edid);
    parse_manufacture_date(edid);
    parse_edid_version(edid);
    parse_video_input(edid);
    parse_screen_size(edid);
    parse_display_gamma(edid);
    parse_supported_features(edid);
    parse_colour_characteristics(edid);
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
    if (out) fprintf(out, "EDID Version: %d.%d\n", version, revision);
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
    if (out) fprintf(out, "Manufacturer ID: %s\n", manufacturer_id);
}

/*
* This method prints the product code
*/
void parse_product_code(const unsigned char *edid) {
    // Combine the two bytes
    uint16_t product_code = edid[10] | (edid[11] << 8);
    printf("Product Code: %u (0x%04X)\n", product_code, product_code);
    if (out) fprintf(out, "Product Code: %u (0x%04X)\n", product_code, product_code);
}

/*
* This method prints the serial number 
*/
void parse_serial_number(const unsigned char *edid) {
    // Combine 4 bytes
    uint32_t serial = edid[12] | (edid[13] << 8) | (edid[14] << 16) | (edid[15] << 24);
    printf("Serial Number: %u (0x%08X)\n", serial, serial);
    if (out) fprintf(out, "Serial Number: %u (0x%08X)\n", serial, serial);
}

/*
* This method prints the manufacture date
*/
void parse_manufacture_date(const unsigned char *edid) {
    uint8_t week = edid[16];
    uint8_t year_offset = edid[17];
    uint16_t year = 1990 + year_offset;

    printf("Manufacture Date: Year %u, Week %u\n", year, week);
    if (out) fprintf(out, "Manufacture Date: Year %u, Week %u\n", year, week);
}

/*
* This method prints the video input type
*/
void parse_video_input(const unsigned char *edid){
    uint8_t input = edid[20];
    
    if (input & 0x80){
        printf("Video Input Type: Digital\n");
        if (out) fprintf(out, "Video Input Type: Digital\n");

        uint8_t bit = (input >> 4) & 0x07;
        uint8_t interface = input & 0x0F;

        const char *bits[] = {"Undefined", "6", "8", "10", "12", "14", "16", "Reserved"};

        const char *interfaces[] = {"Undefined", "DVI", "HDMIa", "HDMIb", "MDDI", "DisplayPort"};

        printf("   Bits per colour: %s\n", bits[bit]);
        if (out) fprintf(out, "   Bits per colour: %s\n", bits[bit]);
        if (interface < 6){
            printf("   Interface: %s\n", interfaces[interface]);
            if (out) fprintf(out, "   Interface: %s\n", interfaces[interface]);
        }
        else{
            printf("   Interface: Reserved or Unknown\n");
            if (out) fprintf(out, "   Interface: Reserved or Unknown\n");
        }
    }
    else{
        printf("Video Input Type: Analog\n");
        if (out) fprintf(out, "Video Input Type: Analog\n");
        uint8_t level = (input >> 5) & 0x03;
        uint8_t setup = (input >> 4) & 0x01;
        uint8_t sync = input & 0x0F;

        const char *video_levels[] = {
            "0.700, 0.300 (1.0 V p-p)", "0.714, 0.286 (1.0 V p-p)", "1.000, 0.286 (1.0 V p-p)", "0.700, 0.000 (0.7 V p-p)",
        };

        printf("   Signal Level: %s\n", video_levels[level]);

        if (setup){
            printf("Video setup: Blank-to-Black setup or pedestal\n");
            if (out) fprintf(out, "Video setup: Blank-to-Black setup or pedestal\n");
        }
        else{
            printf("Video setup: Blank level = Black level\n");
            if (out) fprintf(out, "Video setup: Blank level = Black level\n");
        }

        printf("   Sync Types Supported:\n");
        if (out) fprintf(out, "   Sync Types Supported:\n");
        if (sync & 0x08){
            printf("   -Separate Sync H & V Signals\n");
            if (out) fprintf(out, "   -Separate Sync H & V Signals\n");
        }
        if (sync & 0x04){
            printf("   -Composite Sync H & V Signals\n");
            if (out) fprintf(out, "   -Composite Sync H & V Signals\n");
        }
        if (sync & 0x02){
            printf("   -Composite Sync Signal on Green Video\n");
            if (out) fprintf(out, "   -Composite Sync Signal on Green Video\n");
        }
        if (sync & 0x01){
            printf("   -Serration on Vertical Sync\n");
            if (out) fprintf(out, "   -Serration on Vertical Sync\n");
        }
    }
}

/*
* This method prints the screen size in the format of horizontal size x vertical size
*/
void parse_screen_size(const unsigned char *edid){
    uint8_t horizontal_size = edid[21]; 
    uint8_t vertical_size = edid[22];
    printf("Screen Size: %d cm x %d cm\n", horizontal_size, vertical_size);
    if (out) fprintf(out, "Screen Size: %d cm x %d cm\n", horizontal_size, vertical_size);
}

/*
* This method prints the display gamma
*/
void parse_display_gamma(const unsigned char *edid) {
    uint8_t gamma_encoded = edid[23];

    float gamma = (gamma_encoded + 100) / 100.0f;
    printf("Display Gamma: %.2f\n", gamma);
    if (out) fprintf(out, "Display Gamma: %.2f\n", gamma);
}

/*
* This method prints the supported features
*/
void parse_supported_features(const unsigned char *edid) {
    uint8_t features = edid[24];
    uint8_t input = edid[20];

    uint8_t power = (features >> 5) & 0x03;

    printf("Supported Features:\n");
    if (out) fprintf(out, "Supported Features:\n");

    // Power management
    if (power & 0x04){
       printf(" - Standby Supported\n"); 
       if (out) fprintf(out, " - Standby Supported\n");
    } 
    if (features & 0x02){
        printf(" - Suspend Supported\n");
        if (out) fprintf(out, " - Suspend Supported\n");
    }
    if (features & 0x01){ 
        printf(" - Active-Off Supported\n");
        if (out) fprintf(out, " - Active-Off Supported\n");
    }

    // Display type
    uint8_t display_type = (features >> 3) & 0x03;
    const char *display_types_digital[] = {
        "RGB 4:4:4", "RGB 4:4:4 & YCrCb 4:4:4", "RGB 4:4:4 & YCrCb 4:2:2", "RGB 4:4:4 & YCrCb 4:4:4 & YCrCb 4:2:2"
    };
    const char *display_types_analog[] = {
        "Monochrome or Grayscale", "RGB Color", "Non-RGB Color", "Undefined"
    };
    if (input & 0x80){
        printf(" - Display Type: %s\n", display_types_digital[display_type]);
        if (out) fprintf(out, " - Display Type: %s\n", display_types_digital[display_type]);
    }
    else{
        printf(" - Display Type: %s\n", display_types_analog[display_type]);
        if (out) fprintf(out,  " - Display Type: %s\n", display_types_analog[display_type]);
    }

    // sRGB color space
    if (features & 0x04){
        printf(" - sRGB Color Space Default\n");
        if (out) fprintf(out,  " - sRGB Color Space Default\n");
    }

    // Preferred timing mode
    if (features & 0x02){
        printf(" - Preferred Timing Mode\n");
        if (out) fprintf(out,  " - Preferred Timing Mode\n");
    }

    // Continuous timings
    if (features & 0x01){
        printf(" - Continuous Timing Support\n");
        if (out) fprintf(out,  " - Continuous Timing Support\n");
    }
}

void parse_colour_characteristics(const unsigned char *edid) {
    
    uint8_t red_green_lo = edid[25];
    uint8_t blue_white_lo = edid[26];

    
    uint16_t red_x = ((edid[27] << 2) | ((red_green_lo >> 6) & 0x03));
    uint16_t red_y = (edid[28] << 2) | ((red_green_lo >> 4) & 0x03);
    uint16_t green_x = (edid[29] << 2) | ((red_green_lo >> 2) & 0x03);
    uint16_t green_y = ((edid[30]) << 2) | ((red_green_lo & 0x03));

    uint16_t blue_x =  (edid[31] << 2) | ((blue_white_lo >> 6) & 0x03);
    uint16_t blue_y = (edid[32] << 2) | ((blue_white_lo >> 4) & 0x03);
    uint16_t white_x = (edid[33] << 2) | ((blue_white_lo >> 2) & 0x03);
    uint16_t white_y = (edid[34] << 2) | (blue_white_lo & 0x03);

    printf("Color Characteristics (Chromaticity Coordinates):\n");
    if (out) fprintf(out,  "Color Characteristics (Chromaticity Coordinates):\n");
    printf("  Red   : (X = %.4f, Y = %.4f)\n", red_x / 1024.0, red_y / 1024.0);
    if (out) fprintf(out,  "  Red   : (X = %.4f, Y = %.4f)\n", red_x / 1024.0, red_y / 1024.0);
    printf("  Green : (X = %.4f, Y = %.4f)\n", green_x / 1024.0, green_y / 1024.0);
    if (out) fprintf(out,  "  Green : (X = %.4f, Y = %.4f)\n", green_x / 1024.0, green_y / 1024.0);
    printf("  Blue  : (X = %.4f, Y = %.4f)\n", blue_x / 1024.0, blue_y / 1024.0);
    if (out) fprintf(out,  "  Blue  : (X = %.4f, Y = %.4f)\n", blue_x / 1024.0, blue_y / 1024.0);
    printf("  White : (X = %.4f, Y = %.4f)\n", white_x / 1024.0, white_y / 1024.0);
    if (out) fprintf(out,  "  White : (X = %.4f, Y = %.4f)\n", white_x / 1024.0, white_y / 1024.0);
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
    fclose(out);

}
