#ifndef PARSER_H
#define PARSER_H

#define PARSED_EDID_0_REGISTER 0x400

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
void parse_edid_string(const char *hex_string, char *output);
void parse_edid_array(const unsigned char *edid, char *output);
void write_parsed_edid_data(unsigned int tx_id, const char *parsed_output);
void write_to_file_once(const char *filename, const char *data);
void float_to_string(float value, char* float_string);
int verify_edid_checksum(const unsigned char *edid);

#endif