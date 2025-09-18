#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "parser.h"
/**
 * This function is an EDID parser that can take an input of the raw EDID data
 */

#define EDID_LENGTH 128

#define DEBUG_PRINT_ENABLED 0
#if DEBUG_PRINT_ENABLED
#define DGB_PRINTF printf
#else
#define DGB_PRINTF(...) ((void)0)
#endif

#define DEBUG_PRINT_ALL_ENABLED 1
#if DEBUG_PRINT_ALL_ENABLED
#define DGB_PRINTF_ALL printf
#else
#define DGB_PRINTF_ALL(...) ((void)0)
#endif

#define FILE_OUTPUT_ENABLED 0

void write_to_file_once(const char *filename, const char *data) {
#if FILE_OUTPUT_ENABLED
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to open output file");
        return;
    }
    fprintf(fp, "%s", data);
    fclose(fp);
#else
    (void)filename;
    (void)data;
#endif
}

char edid_output[1500];


/**
 * Parses the EDID data
 *
 * @param edid Pointer to the 128-byte EDID data array
 */
void parse_edid_array(const unsigned char *edid, char *output){
    int offset = 0;
    
    if (!check_header(edid))
    {
        DGB_PRINTF("Invalid EDID header.\n");
        
        int written = sprintf(output + offset, "Invalid EDID header\n");
        offset += written;
        return;
    }

    DGB_PRINTF("Valid EDID header.\n");
    int written = sprintf(output + offset, "Valid EDID header\n");
    offset += written;
    
    parse_manufacturer_id(edid, output, &offset);
    parse_product_code(edid, output, &offset);
    parse_serial_number(edid, output, &offset);
    parse_manufacture_date(edid, output, &offset);
    parse_edid_version(edid, output, &offset);
    parse_video_input(edid, output, &offset);
    parse_screen_size(edid, output, &offset);
    parse_display_gamma(edid, output, &offset);
    parse_supported_features(edid, output, &offset);
    parse_colour_characteristics(edid, output, &offset);
    parse_established_timings(edid, output, &offset);
    parse_standard_timings(edid, output, &offset);

    if (verify_edid_checksum(edid)) {
        DGB_PRINTF("Checksum is valid.\n");
        written = sprintf(output + offset, "Checksum is valid\n");
        offset += written;
    } else {
        DGB_PRINTF("Checksum is invalid.\n");
        written = sprintf(output + offset, "Checksum is invalid\n");
        offset += written;
    }

    DGB_PRINTF_ALL("%s", edid_output);
}

/**
 * Checks whether the EDID header is valid.
 *
 * @param edid Pointer to the 128-byte EDID data array
 * @return 1 if header is valid, 0 otherwise.
 */
int check_header(const unsigned char *edid)
{
    if (edid[0] != 0x00 || edid[1] != 0xFF || edid[2] != 0xFF || edid[3] != 0xFF || edid[4] != 0xFF || edid[5] != 0xFF ||
        edid[6] != 0xFF || edid[7] != 0x00)
    {
        return 0; // Invalid header
    }
    return 1; // Valid header
}

/**
 * Parses and prints the EDID version and revision.
 *
 * @param edid Pointer to the 128-byte EDID data array.
 */
void parse_edid_version(const unsigned char *edid, char *output, int *offset)
{
    uint8_t version = edid[0x12];  // EDID version byte
    uint8_t revision = edid[0x13]; // EDID revision byte
    DGB_PRINTF("EDID Version: %d.%d\n", version, revision);
    int written = sprintf(output + *offset, "EDID Version: %d.%d\n", version, revision);
    *offset += written;

}

/**
 * Parses and prints the manufacturer ID
 *
 * @param edid Pointer to the 128-byte EDID data array
 */
void parse_manufacturer_id(const unsigned char *edid, char *output, int *offset)
{
    uint16_t manufacturer = (edid[8] << 8) | edid[9]; // Combine bytes 8 and 9

    char manufacturer_id[4];

    manufacturer_id[0] = ((manufacturer >> 10) & 0x1F) + 'A' - 1; // ASCII
    manufacturer_id[1] = ((manufacturer >> 5) & 0x1F) + 'A' - 1;
    manufacturer_id[2] = (manufacturer & 0x1F) + 'A' - 1;
    manufacturer_id[3] = '\0';

    DGB_PRINTF("Manufacturer ID: %s\n", manufacturer_id);
    int written = sprintf(output + *offset, "Manufacturer ID: %s\n", manufacturer_id);
    *offset += written;
}

/**
 * Parses and prints the product code from the EDID.
 *
 * @param edid Pointer to the 128-byte EDID data array
 */
void parse_product_code(const unsigned char *edid, char *output, int *offset)
{
    // Combine the two bytes
    uint16_t product_code = edid[10] | (edid[11] << 8);
    DGB_PRINTF("Product Code: %u (0x%04X)\n", product_code, product_code);
    int written = sprintf(output + *offset, "Product Code: %u (0x%04X)\n", product_code, product_code);
    *offset += written;
}

/**
 * Parses and prints the serial number from the EDID.
 *
 * @param edid Pointer to the 128-byte EDID data array
 */
void parse_serial_number(const unsigned char *edid, char *output, int *offset)
{
    // Combine 4 bytes
    uint32_t serial = edid[12] | (edid[13] << 8) | (edid[14] << 16) | (edid[15] << 24);
    int written = sprintf(output + *offset, "Serial Number: %u (0x%08X)\n", serial, serial);
    *offset += written;
    DGB_PRINTF("Serial Number: %u (0x%08X)\n", serial, serial);
}

/**
 * Parses and prints the week and year of manufacture
 *
 * @param edid Pointer to the 128-byte EDID data array.
 */
void parse_manufacture_date(const unsigned char *edid, char *output, int *offset)
{
    uint8_t week = edid[16];
    uint8_t year_offset = edid[17];
    uint16_t year = 1990 + year_offset;

    DGB_PRINTF("Manufacture Date: Year %u, Week %u\n", year, week);
    int written = sprintf(output + *offset, "Manufacture Date: Year %u, Week %u\n", year, week);
    *offset += written;
}

/**
 * Prints the video input type and other details details
 *
 * @param edid Pointer to the 128-byte EDID data array
 */
void parse_video_input(const unsigned char *edid, char *output, int *offset)
{
    uint8_t input = edid[20];

    if (input & 0x80)
    {
        DGB_PRINTF("Video Input Type: Digital\n");
        int written = sprintf(output + *offset, "Video Input Type: Digital\n");
        *offset += written;

        uint8_t bit = (input >> 4) & 0x07;
        uint8_t interface = input & 0x0F;

        const char *bits[] = {"Undefined", "6", "8", "10", "12", "14", "16", "Reserved"};

        const char *interfaces[] = {"Undefined", "DVI", "HDMIa", "HDMIb", "MDDI", "DisplayPort"};

        DGB_PRINTF("   Bits per colour: %s\n", bits[bit]);
        written = sprintf(output + *offset, "   Bits per colour: %s\n", bits[bit]);
        *offset += written;

        if (interface < 6)
        {
            DGB_PRINTF("   Interface: %s\n", interfaces[interface]);
            int written = sprintf(output + *offset, "   Interface: %s\n", interfaces[interface]);
            *offset += written;
        }
        else
        {
            DGB_PRINTF("   Interface: Reserved or Unknown\n");
            int written = sprintf(output + *offset, "   Interface: Reserved or Unknown\n");
            *offset += written;
        }
    }
    else
    {
        DGB_PRINTF("Video Input Type: Analog\n");
        int written = sprintf(output + *offset, "Video Input Type: Analog\n");
        *offset += written;

        uint8_t level = (input >> 5) & 0x03;
        uint8_t setup = (input >> 4) & 0x01;
        uint8_t sync = input & 0x0F;

        const char *video_levels[] = {
            "0.700, 0.300 (1.0 V p-p)",
            "0.714, 0.286 (1.0 V p-p)",
            "1.000, 0.286 (1.0 V p-p)",
            "0.700, 0.000 (0.7 V p-p)",
        };

        DGB_PRINTF("   Signal Level: %s\n", video_levels[level]);
        written = sprintf(output + *offset, "   Signal Level: %s\n", video_levels[level]);
        *offset += written;

        if (setup)
        {
            DGB_PRINTF("Video setup: Blank-to-Black setup or pedestal\n");
            written = sprintf(output + *offset, "Video setup: Blank-to-Black setup or pedestal\n");
            *offset += written;
        }
        else
        {
            DGB_PRINTF("Video setup: Blank level = Black level\n");
            written = sprintf(output + *offset, "Video setup: Blank level = Black level\n");
            *offset += written;
        }

        DGB_PRINTF("   Sync Types Supported:\n");
        written = sprintf(output + *offset, "   Sync Types Supported:\n");
        *offset += written;

        if (sync & 0x08)
        {
            DGB_PRINTF("   -Separate Sync H & V Signals\n");
            written = sprintf(output + *offset, "   -Separate Sync H & V Signals\n");
            *offset += written;
        }
        if (sync & 0x04)
        {
            DGB_PRINTF("   -Composite Sync H & V Signals\n");
            written = sprintf(output + *offset, "   -Composite Sync H & V Signals\n");
            *offset += written;
        }
        if (sync & 0x02)
        {
            DGB_PRINTF("   -Composite Sync Signal on Green Video\n");
            written = sprintf(output + *offset, "   -Composite Sync Signal on Green Video\n");
            *offset += written;
        }
        if (sync & 0x01)
        {
            DGB_PRINTF("   -Serration on Vertical Sync\n");
            written = sprintf(output + *offset, "   -Serration on Vertical Sync\n");
            *offset += written;
        }
    }
}

/*
 * Prints the screen size in centimetres in the format of horizontal size x vertical size
 *
 * @param edid Pointer to the 128-byte EDID data array
 */
void parse_screen_size(const unsigned char *edid, char *output, int *offset)
{
    uint8_t horizontal_size = edid[21];
    uint8_t vertical_size = edid[22];
    DGB_PRINTF("Screen Size: %d cm x %d cm\n", horizontal_size, vertical_size);
    int written = sprintf(output + *offset, "Screen Size: %d cm x %d cm\n", horizontal_size, vertical_size);
    *offset += written;
}

/*
 * Parses and prints the display gamma
 *
 * @param edid Pointer to the 128-byte EDID data array
 */
void parse_display_gamma(const unsigned char *edid, char *output, int *offset) {
    uint8_t gamma_encoded = edid[23];
    float gamma = (gamma_encoded + 100) / 100.0f;

    char gamma_str[10];
    float_to_string(gamma, gamma_str);
    DGB_PRINTF("Display Gamma: %s\n", gamma_str);

    int written = sprintf(output + *offset, "Display Gamma: %s \n", gamma_str);
    *offset += written;
}

/**
 * Parses and prints supported display features
 *
 * @param edid Pointer to the 128-byte EDID data array 
*/
void parse_supported_features(const unsigned char *edid, char *output, int *offset)
{
    uint8_t features = edid[24];
    uint8_t input = edid[20];

    uint8_t power = (features >> 5) & 0x03;

    DGB_PRINTF("Supported Features:\n");
    int written = sprintf(output + *offset, "Supported Features:\n");
    *offset += written;

    if (power & 0x04)
    {
        DGB_PRINTF(" - Standby Supported\n");
        written = sprintf(output + *offset, " - Standby Supported\n");
        *offset += written;

    }
    if (features & 0x02)
    {
        DGB_PRINTF(" - Suspend Supported\n");
        written = sprintf(output + *offset, " - Suspend Supported\n");
        *offset += written;

    }
    if (features & 0x01)
    {
        DGB_PRINTF(" - Active-Off Supported\n");
        written = sprintf(output + *offset, " - Active-Off Supported\n");
        *offset += written;

    }

    // Display type
    uint8_t display_type = (features >> 3) & 0x03;
    const char *display_types_digital[] = {
        "RGB 4:4:4", "RGB 4:4:4 & YCrCb 4:4:4", "RGB 4:4:4 & YCrCb 4:2:2", "RGB 4:4:4 & YCrCb 4:4:4 & YCrCb 4:2:2"};
    const char *display_types_analog[] = {
        "Monochrome or Grayscale", "RGB Color", "Non-RGB Color", "Undefined"};
    if (input & 0x80)
    {
        DGB_PRINTF(" - Display Type: %s\n", display_types_digital[display_type]);
        written = sprintf(output + *offset, " - Display Type: %s\n", display_types_digital[display_type]);
        *offset += written;
    }
    else
    {
        DGB_PRINTF(" - Display Type: %s\n", display_types_analog[display_type]);
        written = sprintf(output + *offset, " - Display Type: %s\n", display_types_analog[display_type]);
        *offset += written;
    }

    // sRGB color space
    if (features & 0x04)
    {
        DGB_PRINTF(" - sRGB Color Space Default\n");
        written = sprintf(output + *offset, " - sRGB Color Space Default\n");
        *offset += written;
    }

    // Preferred timing mode
    if (features & 0x02)
    {
        DGB_PRINTF(" - Preferred Timing Mode\n");
        written = sprintf(output + *offset, " - Preferred Timing Mode\n");
        *offset += written;
    }

    // Continuous timings
    if (features & 0x01)
    {
        DGB_PRINTF(" - Continuous Timing Support\n");
        written = sprintf(output + *offset, " - Continuous Timing Support\n");
        *offset += written;
    }
}

/**
 * Parses and prints the chromaticity coordinates (color characteristics)
 *
 * @param edid Pointer to the 128-byte EDID data array.
 */
void parse_colour_characteristics(const unsigned char *edid, char *output, int *offset)
{
    uint8_t red_green_lo = edid[25];
    uint8_t blue_white_lo = edid[26];

    uint16_t red_x   = (edid[27] << 2) | ((red_green_lo >> 6) & 0x03);
    uint16_t red_y   = (edid[28] << 2) | ((red_green_lo >> 4) & 0x03);
    uint16_t green_x = (edid[29] << 2) | ((red_green_lo >> 2) & 0x03);
    uint16_t green_y = (edid[30] << 2) | (red_green_lo & 0x03);

    uint16_t blue_x  = (edid[31] << 2) | ((blue_white_lo >> 6) & 0x03);
    uint16_t blue_y  = (edid[32] << 2) | ((blue_white_lo >> 4) & 0x03);
    uint16_t white_x = (edid[33] << 2) | ((blue_white_lo >> 2) & 0x03);
    uint16_t white_y = (edid[34] << 2) | (blue_white_lo & 0x03);

    float red_x_f   = red_x / 1024.0f;
    char red_x_s[10];
    float_to_string(red_x_f, red_x_s);
    float red_y_f   = red_y / 1024.0f;
    char red_y_s[10];
    float_to_string(red_y_f, red_y_s);
    float green_x_f = green_x / 1024.0f;
    char green_x_s[10];
    float_to_string(green_x_f, green_x_s);
    float green_y_f = green_y / 1024.0f;
    char green_y_s[10];
    float_to_string(green_y_f, green_y_s);
    float blue_x_f  = blue_x / 1024.0f;
    char blue_x_s[10];
    float_to_string(blue_x_f, blue_x_s);
    float blue_y_f  = blue_y / 1024.0f;
    char blue_y_s[10];
    float_to_string(blue_y_f, blue_y_s);
    float white_x_f = white_x / 1024.0f;
    char white_x_s[10];
    float_to_string(white_x_f, white_x_s);
    float white_y_f = white_y / 1024.0f;
    char white_y_s[10];
    float_to_string(white_y_f, white_y_s);

    DGB_PRINTF("Color Characteristics (Chromaticity Coordinates):\n");
    int written = sprintf(output + *offset, "Color Characteristics (Chromaticity Coordinates):\n");
    *offset += written;

    DGB_PRINTF("  Red   : (X = %s, Y = %s)\n", red_x_s, red_y_s);
    written = sprintf(output + *offset, "  Red   : (X = %s, Y = %s)\n", red_x_s, red_y_s);
    *offset += written;

    DGB_PRINTF("  Green : (X = %s, Y = %s)\n", green_x_s, green_y_s);
    written = sprintf(output + *offset, "  Green : (X = %s, Y = %s)\n", green_x_s, green_y_s);
    *offset += written;

    DGB_PRINTF("  Blue  : (X = %s, Y = %s)\n", blue_x_s, blue_y_s);
    written = sprintf(output + *offset, "  Blue  : (X = %s, Y = %s)\n", blue_x_s, blue_y_s);
    *offset += written;

    DGB_PRINTF("  White : (X = %s, Y = %s)\n", white_x_s, white_y_s);
    written = sprintf(output + *offset, "  White : (X = %s, Y = %s)\n", white_x_s, white_y_s);
    *offset += written;
}


/**
 * Parses and prints all established display timings supported by the monitor.
 *
 * @param edid Pointer to the 128-byte EDID data array.
 */
void parse_established_timings(const unsigned char *edid, char *output, int *offset)
{
    uint8_t timing1 = edid[35];
    uint8_t timing2 = edid[36];
    uint8_t timing3 = edid[37];

    const char *established_timings[] = {
        "720x400 @ 70Hz", "720x400 @ 88Hz", "640x480 @ 60Hz", "640x480 @ 67Hz", "640x480 @ 72Hz", "640x480 @ 75Hz", "800x600 @ 56Hz", "800x600 @ 60Hz",

        "800x600 @ 72Hz", "800x600 @ 75Hz", "832x624 @ 75Hz", "1024x768 @ 87Hz (interlaced)", "1024x768 @ 60Hz", "1024x768 @ 70Hz", "1024x768 @ 75Hz", "1280x1024 @ 75Hz"};

    DGB_PRINTF("Established Timings:\n");
    int written = sprintf(output + *offset, "Established Timings:\n");
    *offset += written;

    uint8_t mask = 0x80;

    for (int i = 0; i < 8; i++)
    {
        if (timing1 & mask)
        {
            DGB_PRINTF(" - %s\n", established_timings[i]);
            written = sprintf(output + *offset, " - %s\n", established_timings[i]);
            *offset += written;
        }
        mask >>= 1;
    }

    mask = 0x80;

    for (int i = 0; i < 8; i++)
    {
        if (timing2 & mask)
        {
            DGB_PRINTF(" - %s\n", established_timings[8 + i]);
            written = sprintf(output + *offset, " - %s\n", established_timings[8 + i]);
            *offset += written;
        }
        mask >>= 1;
    }

    // manufacturer timing
    if (timing3 != 0x00)
    {
        DGB_PRINTF(" - Manufacturer reserved timings: 0x%02X\n", timing3);
        written = sprintf(output + *offset, " - Manufacturer reserved timings: 0x%02X\n", timing3);
        *offset += written;
    }
}

/**
 * Parses and prints the standard display timings encoded in the EDID.
 *
 * @param edid Pointer to the 128-byte EDID data array.
 */
void parse_standard_timings(const unsigned char *edid, char *output, int *offset)
{
    DGB_PRINTF("Standard Timings:\n");
    int written = sprintf(output + *offset, "Standard Timings:\n");
    *offset += written;

    for (int i = 0; i < 8; i++)
    {
        uint8_t byte1 = edid[38 + i * 2];
        uint8_t byte2 = edid[39 + i * 2];

        if (byte1 == 0x01 && byte2 == 0x01)
        {
            continue;
        }

        int horizontal_resolution = (byte1 + 31) * 8;
        uint8_t aspect = (byte2 >> 6) & 0x03;
        int vertical_frequency = (byte2 & 0x3F) + 60;

        const char *aspect_ratios[] = {"16:10", "4:3", "5:4", "16:9"};

        int vertical_resolution;
        switch (aspect)
        {
        case 0:
            vertical_resolution = horizontal_resolution * 10 / 16;
            break; // 16:10
        case 1:
            vertical_resolution = horizontal_resolution * 3 / 4;
            break; // 4:3
        case 2:
            vertical_resolution = horizontal_resolution * 4 / 5;
            break; // 5:4
        case 3:
            vertical_resolution = horizontal_resolution * 9 / 16;
            break; // 16:9
        default:
            vertical_resolution = 0;
        }

        DGB_PRINTF(" - %d x %d (%s) @ %dHz\n", horizontal_resolution, vertical_resolution, aspect_ratios[aspect], vertical_frequency);
        written = sprintf(output + *offset, " - %d x %d (%s) @ %dHz\n", horizontal_resolution, vertical_resolution, aspect_ratios[aspect], vertical_frequency);
        *offset += written;
    }
}

void write_parsed_edid_data(unsigned int tx_id, const char *parsed_output)
{
    /*unsigned int parsed_base_address = calculate_address(
        FPGA_STS_EDID_MEM_BASE_ADDRESS,
        PARSED_EDID_0_REGISTER + (tx_id * 0x180),
        FPGA_STS_EDID_MEM_BASE_OFFSET);

    size_t len = strlen(parsed_output);

    for (size_t i = 0; i < len; i += 4)
    {
        uint32_t value = 0;

        if (i < len) value |= (uint8_t)parsed_output[i];
        if (i + 1 < len) value |= ((uint8_t)parsed_output[i + 1]) << 8;
        if (i + 2 < len) value |= ((uint8_t)parsed_output[i + 2]) << 16;
        if (i + 3 < len) value |= ((uint8_t)parsed_output[i + 3]) << 24;

        unsigned int current_address = parsed_base_address + i;
        IOWR(current_address, 0x0, value);
    }

    DGB_PRINTF("Parsed EDID written to memory starting at 0x%x\n", parsed_base_address);*/
    DGB_PRINTF("Parsed output: %s", parsed_output);
}

void float_to_string(float value, char* float_string) {
    int int_part = (int)value;
    int frac_part = (int)((value - int_part) * 10000);

    sprintf(float_string, "%d.%04d", int_part, frac_part);
}

int verify_edid_checksum(const unsigned char *edid)
{
    uint8_t sum = 0;
    for (int i = 0; i < EDID_LENGTH; ++i) {
        sum += edid[i];
    }

    if (sum % 256 == 0) {
        return 1; // Valid
    } else {
        return 0; // Invalid
    }
}
