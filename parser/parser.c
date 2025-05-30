#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

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

//FILE *out = NULL;
char edid_out[1500];

// Declaring all functions
int check_header(const unsigned char *edid);
void parse_manufacturer_id(const unsigned char *edid, char *output, int *offset);
void parse_product_code(const unsigned char *edid, char *output, int *offset);
void parse_serial_number(const unsigned char *edid, char *output, int *offset);
void parse_manufacture_date(const unsigned char *edid, char *output, int *offset);
void parse_edid_version(const unsigned char *edid, char *output, int *offset);
void parse_video_input(const unsigned char *edid, char *output, int *offset);
void parse_screen_size(const unsigned char *edid, char *output, int *offset);
void parse_display_gamma(const unsigned char *edid, char *output, int *offset);
void parse_supported_features(const unsigned char *edid, char *output, int *offset);
void parse_colour_characteristics(const unsigned char *edid, char *output, int *offset);
void parse_established_timings(const unsigned char *edid, char *output, int *offset);
void parse_standard_timings(const unsigned char *edid, char *output, int *offset);
unsigned char *string_to_hex(const char *hex_string);

/**
 * Parses the EDID data
 *
 * @param edid Pointer to the EDID hex string
 */
void parse_edid_string(const char *hex_string, char *output)
{
    int offset = 0;
    unsigned char *edid = string_to_hex(hex_string);
    /*out = fopen("output.txt", "w");
    if (!out)
    {
        perror("Failed to open output file");
        return;
    }*/

    if (!check_header(edid))
    {
        DGB_PRINTF("Invalid EDID header.\n");
        /*if (out)
            fprintf(out, "Invalid EDID header\n");*/
        int written = sprintf(output + offset, "Invalid EDID header\n");
        offset += written;
        return;
    }

    DGB_PRINTF("Valid EDID header.\n");
    int written = sprintf(output + offset, "Valid EDID header\n");
    offset += written;
    /*if (out)
        fprintf(out, "Valid EDID header\n");*/
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
}

/**
 * Parses the EDID data
 *
 * @param edid Pointer to the 128-byte EDID data array
 */
void parse_edid_array(const unsigned char *edid, char *output){
    int offset = 0;
    /*out = fopen("output.txt", "w");
    if (!out)
    {
        perror("Failed to open output file");
        return;
    }*/
    if (!check_header(edid))
    {
        DGB_PRINTF("Invalid EDID header.\n");
        /*if (out)
            fprintf(out, "Invalid EDID header\n");*/
        int written = sprintf(output + offset, "Invalid EDID header\n");
        offset += written;
        return;
    }

    DGB_PRINTF("Valid EDID header.\n");
    int written = sprintf(output + offset, "Valid EDID header\n");
    offset += written;
    /*if (out)
        fprintf(out, "Valid EDID header\n");*/
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
}

/**
 * Converts the hex string to hex format
 *
 * @param edid Pointer to the 128-byte EDID hex string
 */
unsigned char *string_to_hex(const char *hex_string)
{
    static unsigned char edid[128];
    int index = 0;

    while (*hex_string && index < 128)
    {
        while (*hex_string && isspace(*hex_string))
        {
            hex_string++;
        }

        if (*hex_string == '\0')
            break;

        unsigned int byte;
        if (sscanf(hex_string, "%2x", &byte) == 1)
        {
            edid[index++] = (unsigned char)byte;
            hex_string += 2;
        }
        else
        {
            break;
        }
    }

    return edid;
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
    /*if (out)
        fprintf(out, "EDID Version: %d.%d\n", version, revision);*/
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
    /*if (out)
        fprintf(out, "Manufacturer ID: %s\n", manufacturer_id);*/
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
    /*if (out)
        fprintf(out, "Product Code: %u (0x%04X)\n", product_code, product_code);*/
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
    /*if (out)
        fprintf(out, "Serial Number: %u (0x%08X)\n", serial, serial);*/
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
    /*if (out)
        fprintf(out, "Manufacture Date: Year %u, Week %u\n", year, week);*/
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
        /*if (out)
            fprintf(out, "Video Input Type: Digital\n");*/

        uint8_t bit = (input >> 4) & 0x07;
        uint8_t interface = input & 0x0F;

        const char *bits[] = {"Undefined", "6", "8", "10", "12", "14", "16", "Reserved"};

        const char *interfaces[] = {"Undefined", "DVI", "HDMIa", "HDMIb", "MDDI", "DisplayPort"};

        DGB_PRINTF("   Bits per colour: %s\n", bits[bit]);
        written = sprintf(output + *offset, "   Bits per colour: %s\n", bits[bit]);
        *offset += written;
        /*if (out)
            fprintf(out, "   Bits per colour: %s\n", bits[bit]);*/
        if (interface < 6)
        {
            DGB_PRINTF("   Interface: %s\n", interfaces[interface]);
            int written = sprintf(output + *offset, "   Interface: %s\n", interfaces[interface]);
            *offset += written;
            /*if (out)
                fprintf(out, "   Interface: %s\n", interfaces[interface]);*/
        }
        else
        {
            DGB_PRINTF("   Interface: Reserved or Unknown\n");
            int written = sprintf(output + *offset, "   Interface: Reserved or Unknown\n");
            *offset += written;
            /*if (out)
                fprintf(out, "   Interface: Reserved or Unknown\n");*/
        }
    }
    else
    {
        DGB_PRINTF("Video Input Type: Analog\n");
        int written = sprintf(output + *offset, "Video Input Type: Analog\n");
        *offset += written;
        /*if (out)
            fprintf(out, "Video Input Type: Analog\n");*/
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
            /*if (out)
                fprintf(out, "Video setup: Blank-to-Black setup or pedestal\n");*/
        }
        else
        {
            DGB_PRINTF("Video setup: Blank level = Black level\n");
            written = sprintf(output + *offset, "Video setup: Blank level = Black level\n");
            *offset += written;
            /*if (out)
                fprintf(out, "Video setup: Blank level = Black level\n");*/
        }

        DGB_PRINTF("   Sync Types Supported:\n");
        written = sprintf(output + *offset, "   Sync Types Supported:\n");
        *offset += written;
        /*if (out)
            fprintf(out, "   Sync Types Supported:\n");*/
        if (sync & 0x08)
        {
            DGB_PRINTF("   -Separate Sync H & V Signals\n");
            written = sprintf(output + *offset, "   -Separate Sync H & V Signals\n");
            *offset += written;
            /*if (out)
                fprintf(out, "   -Separate Sync H & V Signals\n");*/
        }
        if (sync & 0x04)
        {
            DGB_PRINTF("   -Composite Sync H & V Signals\n");
            written = sprintf(output + *offset, "   -Composite Sync H & V Signals\n");
            *offset += written;
            /*if (out)
                fprintf(out, "   -Composite Sync H & V Signals\n");*/
        }
        if (sync & 0x02)
        {
            DGB_PRINTF("   -Composite Sync Signal on Green Video\n");
            written = sprintf(output + *offset, "   -Composite Sync Signal on Green Video\n");
            *offset += written;
            /*if (out)
                fprintf(out, "   -Composite Sync Signal on Green Video\n");*/
        }
        if (sync & 0x01)
        {
            DGB_PRINTF("   -Serration on Vertical Sync\n");
            written = sprintf(output + *offset, "   -Serration on Vertical Sync\n");
            *offset += written;
            /*if (out)
                fprintf(out, "   -Serration on Vertical Sync\n");*/
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
    /*if (out)
        fprintf(out, "Screen Size: %d cm x %d cm\n", horizontal_size, vertical_size);*/
}

/*
 * Parses and prints the display gamma
 *
 * @param edid Pointer to the 128-byte EDID data array
 */
void parse_display_gamma(const unsigned char *edid, char *output, int *offset)
{
    uint8_t gamma_encoded = edid[23];

    float gamma = (gamma_encoded + 100) / 100.0f;
    DGB_PRINTF("Display Gamma: %.2f\n", gamma);
    int written = sprintf(output + *offset, "Display Gamma: %.2f\n", gamma);
    *offset += written;
    /*if (out)
        fprintf(out, "Display Gamma: %.2f\n", gamma);*/
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
    /*if (out)
        fprintf(out, "Supported Features:\n");*/

    // Power management
    if (power & 0x04)
    {
        DGB_PRINTF(" - Standby Supported\n");
        written = sprintf(output + *offset, " - Standby Supported\n");
        *offset += written;
        /*if (out)
            fprintf(out, " - Standby Supported\n");*/
    }
    if (features & 0x02)
    {
        DGB_PRINTF(" - Suspend Supported\n");
        written = sprintf(output + *offset, " - Suspend Supported\n");
        *offset += written;
        /*if (out)
            fprintf(out, " - Suspend Supported\n");*/
    }
    if (features & 0x01)
    {
        DGB_PRINTF(" - Active-Off Supported\n");
        written = sprintf(output + *offset, " - Active-Off Supported\n");
        *offset += written;
        /*if (out)
            fprintf(out, " - Active-Off Supported\n");*/
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
        /*if (out)
            fprintf(out, " - Display Type: %s\n", display_types_digital[display_type]);*/
    }
    else
    {
        DGB_PRINTF(" - Display Type: %s\n", display_types_analog[display_type]);
        written = sprintf(output + *offset, " - Display Type: %s\n", display_types_analog[display_type]);
        *offset += written;
        /*if (out)
            fprintf(out, " - Display Type: %s\n", display_types_analog[display_type]);*/
    }

    // sRGB color space
    if (features & 0x04)
    {
        DGB_PRINTF(" - sRGB Color Space Default\n");
        written = sprintf(output + *offset, " - sRGB Color Space Default\n");
        *offset += written;
        /*if (out)
            fprintf(out, " - sRGB Color Space Default\n");*/
    }

    // Preferred timing mode
    if (features & 0x02)
    {
        DGB_PRINTF(" - Preferred Timing Mode\n");
        written = sprintf(output + *offset, " - Preferred Timing Mode\n");
        *offset += written;
        /*if (out)
            fprintf(out, " - Preferred Timing Mode\n");*/
    }

    // Continuous timings
    if (features & 0x01)
    {
        DGB_PRINTF(" - Continuous Timing Support\n");
        written = sprintf(output + *offset, " - Continuous Timing Support\n");
        *offset += written;
        /*if (out)
            fprintf(out, " - Continuous Timing Support\n");*/
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

    uint16_t red_x = ((edid[27] << 2) | ((red_green_lo >> 6) & 0x03));
    uint16_t red_y = (edid[28] << 2) | ((red_green_lo >> 4) & 0x03);
    uint16_t green_x = (edid[29] << 2) | ((red_green_lo >> 2) & 0x03);
    uint16_t green_y = ((edid[30]) << 2) | ((red_green_lo & 0x03));

    uint16_t blue_x = (edid[31] << 2) | ((blue_white_lo >> 6) & 0x03);
    uint16_t blue_y = (edid[32] << 2) | ((blue_white_lo >> 4) & 0x03);
    uint16_t white_x = (edid[33] << 2) | ((blue_white_lo >> 2) & 0x03);
    uint16_t white_y = (edid[34] << 2) | (blue_white_lo & 0x03);

    DGB_PRINTF("Color Characteristics (Chromaticity Coordinates):\n");
    int written = sprintf(output + *offset, "Color Characteristics (Chromaticity Coordinates):\n");
    *offset += written;
    /*if (out)
        fprintf(out, "Color Characteristics (Chromaticity Coordinates):\n");*/
    DGB_PRINTF("  Red   : (X = %.4f, Y = %.4f)\n", red_x / 1024.0, red_y / 1024.0);
    written = sprintf(output + *offset, "  Red   : (X = %.4f, Y = %.4f)\n", red_x / 1024.0, red_y / 1024.0);
    *offset += written;
    /*if (out)
        fprintf(out, "  Red   : (X = %.4f, Y = %.4f)\n", red_x / 1024.0, red_y / 1024.0);*/
    DGB_PRINTF("  Green : (X = %.4f, Y = %.4f)\n", green_x / 1024.0, green_y / 1024.0);
    written = sprintf(output + *offset, "  Green : (X = %.4f, Y = %.4f)\n", green_x / 1024.0, green_y / 1024.0);
    *offset += written;
    /*if (out)
        fprintf(out, "  Green : (X = %.4f, Y = %.4f)\n", green_x / 1024.0, green_y / 1024.0);*/
    DGB_PRINTF("  Blue  : (X = %.4f, Y = %.4f)\n", blue_x / 1024.0, blue_y / 1024.0);
    written = sprintf(output + *offset, "  Blue  : (X = %.4f, Y = %.4f)\n", blue_x / 1024.0, blue_y / 1024.0);
    *offset += written;
    /*if (out)
        fprintf(out, "  Blue  : (X = %.4f, Y = %.4f)\n", blue_x / 1024.0, blue_y / 1024.0);*/
    DGB_PRINTF("  White : (X = %.4f, Y = %.4f)\n", white_x / 1024.0, white_y / 1024.0);
    written = sprintf(output + *offset, "  White : (X = %.4f, Y = %.4f)\n", white_x / 1024.0, white_y / 1024.0);
    *offset += written;
    /*if (out)
        fprintf(out, "  White : (X = %.4f, Y = %.4f)\n", white_x / 1024.0, white_y / 1024.0);*/
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
    /*if (out)
        fprintf(out, "Established Timings:\n");*/

    uint8_t mask = 0x80;

    for (int i = 0; i < 8; i++)
    {
        if (timing1 & mask)
        {
            DGB_PRINTF(" - %s\n", established_timings[i]);
            written = sprintf(output + *offset, " - %s\n", established_timings[i]);
            *offset += written;
            /*if (out)
                fprintf(out, " - %s\n", established_timings[i]);*/
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
            /*if (out)
                fprintf(out, " - %s\n", established_timings[8 + i]);*/
        }
        mask >>= 1;
    }

    // manufacturer timing
    if (timing3 != 0x00)
    {
        DGB_PRINTF(" - Manufacturer reserved timings: 0x%02X\n", timing3);
        written = sprintf(output + *offset, " - Manufacturer reserved timings: 0x%02X\n", timing3);
        *offset += written;
        /*if (out)
            fprintf(out, " - Manufacturer reserved timings: 0x%02X\n", timing3);*/
    }
}

/**
 * Parses and prints the standard display timings encoded in the EDID.
 *
 * @param edid Pointer to the 128-byte EDID data array.
 */
void parse_standard_timings(const unsigned char *edid, char *output, int *offset)
{
    /*if (out)
        fprintf(out, "Standard Timings:\n");*/
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
        /*if (out)
            fprintf(out, " - %d x %d (%s) @ %dHz\n", horizontal_resolution, vertical_resolution, aspect_ratios[aspect], vertical_frequency);*/
    }
}

int main()
{
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

    const char *input1 = "00 FF FF FF FF FF FF 00 10 AC 79 42 4C 47 5A 42 0F 22 01 04 B5 3C 22 78 3A DF 15 AD 50 44 AD 25 0F 50 54 A5 4B 00 D1 00 D1 C0 B3 00 A9 40 81 80 81 00 71 4F E1 C0 4D D0 00 A0 F0 70 3E 80 30 20 35 00 55 50 21 00 00 1A 00 00 00 FF 00 32 33 5A 53 4A 30 34 0A 20 20 20 20 20 00 00 00 FC 00 44 45 4C 4C 20 55 32 37 32 33 51 45 0A 00 00 00 FD 00 17 56 0F 8C 36 01 0A 20 20 20 20 20 20 01 2A     ";

    const char *input2 = "00 FF FF FF FF FF FF 00 09 D1 25 80 45 54 00 00 16 1E 01 04 B5 46 28 78 3E 87 D1 A8 55 4D 9F 25 0E 50 54 A5 6B 80 81 80 81 C0 81 00 A9 C0 B3 00 D1 C0 01 01 01 01 4D D0 00 A0 F0 70 3E 80 30 20 35 00 C4 8F 21 00 00 1A 00 00 00 FF 00 58 35 4C 30 30 32 34 38 30 31 39 0A 20 00 00 00 FD 00 32 4C 1E 8C 3C 00 0A 20 20 20 20 20 20 00 00 00 FC 00 42 65 6E 51 20 50 44 33 32 30 30 55 0A 01 D2";

    const char *input3 = "00 FF FF FF FF FF FF 00 1E 6D 09 5B 24 3F 05 00 09 1E 01 04 B5 3C 22 78 9E 30 35 A7 55 4E A3 26 0F 50 54 21 08 00 71 40 81 80 81 C0 A9 C0 D1 C0 81 00 01 01 01 01 4D D0 00 A0 F0 70 3E 80 30 20 65 0C 58 54 21 00 00 1A 28 68 00 A0 F0 70 3E 80 08 90 65 0C 58 54 21 00 00 1A 00 00 00 FD 00 38 3D 1E 87 38 00 0A 20 20 20 20 20 20 00 00 00 FC 00 4C 47 20 55 6C 74 72 61 20 48 44 0A 20 01 2C";
    
    const char *input4 = "00 FF FF FF FF FF FF 00 4C 2D 4D 0C 4A 53 4D 30 0C 1D 01 04 B5 3D 23 78 3A 5F B1 A2 57 4F A2 28 0F 50 54 BF EF 80 71 4F 81 00 81 C0 81 80 A9 C0 B3 00 95 00 01 01 4D D0 00 A0 F0 70 3E 80 30 20 35 00 5F 59 21 00 00 1A 00 00 00 FD 00 38 4B 1E 87 3C 00 0A 20 20 20 20 20 20 00 00 00 FC 00 55 32 38 45 35 39 30 0A 20 20 20 20 20 00 00 00 FF 00 48 54 50 4D 33 30 31 38 39 39 0A 20 20 01 65";
    
    const char *input5 = "00 FF FF FF FF FF FF 00 10 AC 79 42 4C 47 5A 42 0F 22 01 04 B5 3C 22 78 3A DF 15 AD 50 44 AD 25 0F 50 54 A5 4B 00 D1 00 D1 C0 B3 00 A9 40 81 80 81 00 71 4F E1 C0 4D D0 00 A0 F0 70 3E 80 30 20 35 00 55 50 21 00 00 1A 00 00 00 FF 00 32 33 5A 53 4A 30 34 0A 20 20 20 20 20 00 00 00 FC 00 44 45 4C 4C 20 55 32 37 32 33 51 45 0A 00 00 00 FD 00 17 56 0F 8C 36 01 0A 20 20 20 20 20 20 01 2A";
    
    parse_edid_string(input1, edid_out);
    printf("----- Edid parsed in memory ----- \n %s \n --------------------", edid_out);
    /*if (out)
        fprintf(out, edid_out);

    fclose(out);*/
    write_to_file_once("output.txt", edid_out);
}
