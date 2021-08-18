#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <math.h>

#define byte unsigned char
#define true 1
#define false 0

#define DISPLAY_RESET_PIN_OUTPUT   DDRB |= (1 << DDB3)
#define DISPLAY_RESET_PIN_HIGH   PORTB |= (1 << PORTB3)
#define DISPLAY_RESET_PIN_LOW   PORTB &= ~(1 << PORTB3)

#define DISPLAY_DATA_PIN_OUTPUT   DDRB |= (1 << DDB0)
#define DISPLAY_DATA_PIN_HIGH   PORTB |= (1 << PORTB0)
#define DISPLAY_DATA_PIN_LOW   PORTB &= ~(1 << PORTB0)

#define DISPLAY_CLOCK_PIN_OUTPUT   DDRB |= (1 << DDB1)
#define DISPLAY_CLOCK_PIN_HIGH   PORTB |= (1 << PORTB1)
#define DISPLAY_CLOCK_PIN_LOW   PORTB &= ~(1 << PORTB1)

#define DISPLAY_MODE_PIN_OUTPUT   DDRB |= (1 << DDB2)
#define DISPLAY_MODE_PIN_HIGH   PORTB |= (1 << PORTB2)
#define DISPLAY_MODE_PIN_LOW   PORTB &= ~(1 << PORTB2)

#define EEPROM_DATA_PIN_OUTPUT   DDRD |= (1 << DDD1)
#define EEPROM_DATA_PIN_INPUT   DDRD &= ~(1 << DDD1)
#define EEPROM_DATA_PIN_HIGH   PORTD |= (1 << PORTD1)
#define EEPROM_DATA_PIN_LOW   PORTD &= ~(1 << PORTD1)
#define READ_EEPROM_DATA_PIN   (PIND & (1 << PIND1))

#define EEPROM_CLOCK_PIN_OUTPUT   DDRD |= (1 << DDD0)
#define EEPROM_CLOCK_PIN_INPUT   DDRD &= ~(1 << DDD0)
#define EEPROM_CLOCK_PIN_HIGH   PORTD |= (1 << PORTD0)
#define EEPROM_CLOCK_PIN_LOW   PORTD &= ~(1 << PORTD0)

#define KEYBOARD_ROW_0_PIN_OUTPUT   DDRD |= (1 << DDD2)
#define KEYBOARD_ROW_0_PIN_INPUT   DDRD &= ~(1 << DDD2)
#define KEYBOARD_ROW_0_PIN_HIGH   PORTD |= (1 << PORTD2)

#define KEYBOARD_ROW_1_PIN_OUTPUT   DDRD |= (1 << DDD3)
#define KEYBOARD_ROW_1_PIN_INPUT   DDRD &= ~(1 << DDD3)
#define KEYBOARD_ROW_1_PIN_HIGH   PORTD |= (1 << PORTD3)

#define KEYBOARD_ROW_2_PIN_OUTPUT   DDRD |= (1 << DDD4)
#define KEYBOARD_ROW_2_PIN_INPUT   DDRD &= ~(1 << DDD4)
#define KEYBOARD_ROW_2_PIN_HIGH   PORTD |= (1 << PORTD4)

#define KEYBOARD_ROW_3_PIN_OUTPUT   DDRD |= (1 << DDD5)
#define KEYBOARD_ROW_3_PIN_INPUT   DDRD &= ~(1 << DDD5)
#define KEYBOARD_ROW_3_PIN_HIGH   PORTD |= (1 << PORTD5)

#define KEYBOARD_ROW_4_PIN_OUTPUT   DDRD |= (1 << DDD6)
#define KEYBOARD_ROW_4_PIN_INPUT   DDRD &= ~(1 << DDD6)
#define KEYBOARD_ROW_4_PIN_HIGH   PORTD |= (1 << PORTD6)

#define KEYBOARD_LOAD_PIN_OUTPUT   DDRB |= (1 << DDB0)
#define KEYBOARD_LOAD_PIN_HIGH   PORTB |= (1 << PORTB0)
#define KEYBOARD_LOAD_PIN_LOW   PORTB &= ~(1 << PORTB0)

#define KEYBOARD_CLOCK_PIN_OUTPUT   DDRB |= (1 << DDB2)
#define KEYBOARD_CLOCK_PIN_HIGH   PORTB |= (1 << PORTB2)
#define KEYBOARD_CLOCK_PIN_LOW   PORTB &= ~(1 << PORTB2)

#define KEYBOARD_DATA_PIN_INPUT   DDRB &= ~(1 << DDB4)
#define READ_KEYBOARD_DATA_PIN   (PINB & (1 << PINB4))

#define FILE_ENTRY_SIZE 24

const byte DISPLAY_INITIALIZE_DATA_LIST[] PROGMEM = {0x40, 0xA0, 0xC8, 0xA4, 0xA6, 0xA2, 0x2F, 0x27, 0x81, 0x10, 0xFA, 0x90, 0xAF};

const byte FONT_DATA_LIST[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x5F, 0x00, 0x00,
    0x00, 0x07, 0x00, 0x07, 0x00,
    0x14, 0x7F, 0x14, 0x7F, 0x14,
    0x24, 0x2A, 0x7F, 0x2A, 0x12,
    0x63, 0x13, 0x08, 0x64, 0x63,
    0x36, 0x49, 0x4A, 0x3C, 0x48,
    0x00, 0x00, 0x07, 0x00, 0x00,
    0x00, 0x1C, 0x22, 0x41, 0x41,
    0x41, 0x41, 0x22, 0x1C, 0x00,
    0x15, 0x0E, 0x1F, 0x0E, 0x15,
    0x08, 0x08, 0x3E, 0x08, 0x08,
    0x00, 0x00, 0x70, 0x00, 0x00,
    0x08, 0x08, 0x08, 0x08, 0x08,
    0x00, 0x60, 0x60, 0x00, 0x00,
    0x60, 0x10, 0x08, 0x04, 0x03,
    // 0.
    0x3E, 0x7F, 0x49, 0x45, 0x3E,
    0x42, 0x7F, 0x7F, 0x40, 0x40,
    0x71, 0x79, 0x49, 0x49, 0x46,
    0x41, 0x49, 0x49, 0x7F, 0x36,
    0x0F, 0x08, 0x08, 0x7F, 0x7F,
    0x4F, 0x4F, 0x49, 0x49, 0x31,
    0x3E, 0x7F, 0x49, 0x49, 0x31,
    0x01, 0x71, 0x79, 0x05, 0x03,
    0x36, 0x7F, 0x49, 0x49, 0x36,
    0x46, 0x49, 0x49, 0x7F, 0x3E,
    0x00, 0x33, 0x33, 0x00, 0x00,
    0x00, 0x03, 0x73, 0x00, 0x00,
    0x00, 0x08, 0x14, 0x22, 0x00,
    0x14, 0x14, 0x14, 0x14, 0x14,
    0x00, 0x22, 0x14, 0x08, 0x00,
    0x01, 0x01, 0x59, 0x09, 0x06,
    0x3E, 0x41, 0x5D, 0x5D, 0x5E,
    // A.
    0x7E, 0x7F, 0x09, 0x09, 0x7E,
    0x7F, 0x7F, 0x49, 0x49, 0x36,
    0x3E, 0x7F, 0x41, 0x41, 0x41,
    0x7F, 0x7F, 0x41, 0x22, 0x1C,
    0x7F, 0x7F, 0x49, 0x49, 0x41,
    0x7F, 0x7F, 0x09, 0x09, 0x01,
    0x3E, 0x7F, 0x41, 0x49, 0x39,
    0x7F, 0x7F, 0x08, 0x08, 0x7F,
    0x41, 0x7F, 0x7F, 0x41, 0x41,
    0x40, 0x40, 0x41, 0x7F, 0x3F,
    0x7F, 0x7F, 0x08, 0x14, 0x63,
    0x7F, 0x7F, 0x40, 0x40, 0x40,
    0x7F, 0x02, 0x0C, 0x02, 0x7F,
    0x7F, 0x7E, 0x04, 0x08, 0x7F,
    0x3E, 0x7F, 0x41, 0x41, 0x3E,
    0x7F, 0x7F, 0x09, 0x09, 0x06,
    0x1E, 0x3F, 0x61, 0x61, 0x5E,
    0x7F, 0x7F, 0x19, 0x29, 0x46,
    0x46, 0x4F, 0x49, 0x49, 0x31,
    0x01, 0x7F, 0x7F, 0x01, 0x01,
    0x3F, 0x7F, 0x40, 0x40, 0x3F,
    0x07, 0x08, 0x10, 0x3F, 0x7F,
    0x7F, 0x20, 0x18, 0x20, 0x7F,
    0x63, 0x77, 0x08, 0x14, 0x63,
    0x03, 0x7F, 0x78, 0x04, 0x03,
    0x61, 0x51, 0x49, 0x45, 0x43,
    0x00, 0x7F, 0x41, 0x41, 0x41,
    0x03, 0x04, 0x08, 0x10, 0x60,
    0x41, 0x41, 0x41, 0x7F, 0x00,
    0x04, 0x02, 0x01, 0x02, 0x04,
    0x40, 0x40, 0x40, 0x40, 0x40,
    0x00, 0x01, 0x02, 0x04, 0x00,
    // a.
    0x24, 0x54, 0x54, 0x7C, 0x78,
    0x7F, 0x7F, 0x44, 0x44, 0x38,
    0x38, 0x7C, 0x44, 0x44, 0x44,
    0x38, 0x44, 0x44, 0x7F, 0x7F,
    0x38, 0x7C, 0x54, 0x54, 0x58,
    0x04, 0x7E, 0x7F, 0x05, 0x05,
    0x48, 0x54, 0x54, 0x7C, 0x3C,
    0x7F, 0x7F, 0x04, 0x04, 0x78,
    0x00, 0x7B, 0x7B, 0x00, 0x00,
    0x40, 0x40, 0x40, 0x7B, 0x3B,
    0x7F, 0x7F, 0x10, 0x28, 0x44,
    0x00, 0x7F, 0x7F, 0x00, 0x00,
    0x7C, 0x04, 0x7C, 0x04, 0x78,
    0x7C, 0x7C, 0x04, 0x04, 0x78,
    0x38, 0x7C, 0x44, 0x44, 0x38,
    0x7C, 0x7C, 0x14, 0x14, 0x08,
    0x08, 0x14, 0x14, 0x7C, 0x7C,
    0x7C, 0x7C, 0x04, 0x04, 0x04,
    0x48, 0x5C, 0x54, 0x54, 0x24,
    0x04, 0x7F, 0x7F, 0x04, 0x04,
    0x3C, 0x7C, 0x40, 0x40, 0x3C, 
    0x04, 0x08, 0x10, 0x3C, 0x7C, 
    0x3C, 0x40, 0x38, 0x40, 0x3C, 
    0x44, 0x28, 0x10, 0x28, 0x44, 
    0x4C, 0x50, 0x50, 0x7C, 0x3C, 
    0x44, 0x64, 0x54, 0x4C, 0x44, 
    0x00, 0x08, 0x36, 0x41, 0x41, 
    0x00, 0x00, 0x77, 0x00, 0x00, 
    0x41, 0x41, 0x36, 0x08, 0x00, 
    0x18, 0x04, 0x08, 0x10, 0x0C
};

const byte KEYBOARD_DATA_LIST[] PROGMEM = {
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\',
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 0, 10,
	16, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 16,
	0, 27, 0, 0, 32, 0, 0, 0, 0, 17, 18, 19, 20,
	'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 8,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', 0, 10,
	16, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 16,
	0, 27, 0, 0, 32, 0, 0, 0, 0, 17, 18, 19, 20
};

const byte MESSAGE_1[] PROGMEM = "ChipLet";
const byte MESSAGE_2[] PROGMEM = "Done";
const byte MESSAGE_3[] PROGMEM = "Command line:";
const byte MESSAGE_4[] PROGMEM = "File name?";
const byte MESSAGE_5[] PROGMEM = "File size?";
const byte MESSAGE_6[] PROGMEM = "File name:";
const byte MESSAGE_7[] PROGMEM = "File handle:";
const byte MESSAGE_8[] PROGMEM = "File address:";
const byte MESSAGE_9[] PROGMEM = "File size:";
const byte MESSAGE_10[] PROGMEM = "Are you sure you want to delete   this file?";
const byte MESSAGE_11[] PROGMEM = "ChipOS  Version 1.0      by Squirrel Computers";
const byte MESSAGE_12[] PROGMEM = "The file was     created.";
const byte MESSAGE_13[] PROGMEM = "The file was     deleted.";
//const byte MESSAGE_14[] PROGMEM = "Hello\nWorld\nBacon\nEggs\nWaffles\nPancakes\nOmelet\nToast\nOatmeal";
const byte MESSAGE_14[] PROGMEM = "The file was     renamed.";
const byte MESSAGE_15[] PROGMEM = "The file was     resized.";
const byte MESSAGE_16[] PROGMEM = "Eeprom is        disengaged.";

const byte ERROR_MESSAGE_1[] PROGMEM = "Bad command name";
const byte ERROR_MESSAGE_2[] PROGMEM = "Unrecognized     command name";
const byte ERROR_MESSAGE_3[] PROGMEM = "Wrong number of  arguments";
const byte ERROR_MESSAGE_4[] PROGMEM = "Address out of   range";
const byte ERROR_MESSAGE_5[] PROGMEM = "Too many end     statements";
const byte ERROR_MESSAGE_6[] PROGMEM = "File name too    long";
const byte ERROR_MESSAGE_7[] PROGMEM = "File size out of range";
const byte ERROR_MESSAGE_8[] PROGMEM = "Index out of     range";
const byte ERROR_MESSAGE_9[] PROGMEM = "No such file";

const byte HELP_MESSAGE_157[] PROGMEM = "Up and down =\nSelect line\nN and B =\nScroll pages\nEnter =\nEdit line\nI =\nInsert line";
const byte HELP_MESSAGE_158[] PROGMEM = "Delete =\nDelete line\nX =\nCut line\nC =\nCopy line\nV =\nPaste line";

const byte HELP_MESSAGE_1[] PROGMEM = "A program contains commands separated by newlines.\nA command contains a command name and arguments separated by spaces.";
const byte HELP_MESSAGE_2[] PROGMEM = "A command name contains exactly three letters.";
const byte HELP_MESSAGE_3[] PROGMEM = "An argument may be one of the following.\n> Number literal: digits 0 through 9.";
const byte HELP_MESSAGE_4[] PROGMEM = "> Simple variable address: dollar sign followed by a number.\n> Complex variable address: two dollar signs followed by a number.";
const byte HELP_MESSAGE_5[] PROGMEM = "> Null terminated string literal: characters enclosed by quotation marks.";
const byte HELP_MESSAGE_6[] PROGMEM = "> List of number literals: numbers separated by spaces enclosed by parentheses.";
const byte HELP_MESSAGE_7[] PROGMEM = "Longs and floats occupy 4 bytes of space.\nFile handles occupy 1 byte of space.";
const byte HELP_MESSAGE_156[] PROGMEM = "The pixel buffer starts at address 11000 and is 816 bytes long.";

const byte HELP_MESSAGE_8[] PROGMEM = "NWB [byte A] [byte B]\nStore B in A.";
const byte HELP_MESSAGE_9[] PROGMEM = "NWL [long A] [long B]\nStore B in A.";
const byte HELP_MESSAGE_10[] PROGMEM = "NWF [float A] [float B]\nStore B in A.";
const byte HELP_MESSAGE_11[] PROGMEM = "NLB [byte A] [long B]\nConvert B to byte and store in A.";
const byte HELP_MESSAGE_12[] PROGMEM = "NFB [byte A] [float B]\nConvert B to byte and store in A.";
const byte HELP_MESSAGE_13[] PROGMEM = "NBL [long A] [byte B]\nConvert B to long and store in A.";
const byte HELP_MESSAGE_14[] PROGMEM = "NFL [long A] [float B]\nConvert B to long and store in A.";
const byte HELP_MESSAGE_15[] PROGMEM = "NBF [float A] [byte B]\nConvert B to float and store in A.";
const byte HELP_MESSAGE_16[] PROGMEM = "NLF [float A] [long B]\nConvert B to float and store in A.";
const byte HELP_MESSAGE_17[] PROGMEM = "NBS [string A] [byte B]\nConvert B to string and store in A.";
const byte HELP_MESSAGE_18[] PROGMEM = "NLS [string A] [long B]\nConvert B to string and store in A.";
const byte HELP_MESSAGE_19[] PROGMEM = "NFS [string A] [float B]\nConvert B to string and store in A.";

const byte HELP_MESSAGE_20[] PROGMEM = "LWB [byte list A] [long B] [byte list C]\nStore list C with length B in A.";
const byte HELP_MESSAGE_21[] PROGMEM = "LWL [long list A] [long B] [long list C]\nStore list C with length B in A.";
const byte HELP_MESSAGE_22[] PROGMEM = "LWF [float list A] [long B] [float list C]\nStore list C with length B in A.";
const byte HELP_MESSAGE_23[] PROGMEM = "LEB [byte A] [long B] [byte list C] [byte list D]\nDetermine whether list C and D with length B have the same values. (...)";
const byte HELP_MESSAGE_129[] PROGMEM = "LEB (continued)\nStore the result in A.";
const byte HELP_MESSAGE_24[] PROGMEM = "LEL [byte A] [long B] [long list C] [long list D]\nDetermine whether list C and D with length B have the same values. (...)";
const byte HELP_MESSAGE_130[] PROGMEM = "LEL (continued)\nStore the result in A.";
const byte HELP_MESSAGE_25[] PROGMEM = "LFB [long A] [byte B] [long C] [byte list D]\nFind element B in list D with length C. Store the index in A. (...)";
const byte HELP_MESSAGE_131[] PROGMEM = "LFB (continued)\nIf B was not found, store -1 in A.";
const byte HELP_MESSAGE_26[] PROGMEM = "LFL [long A] [long B] [long C] [long list D]\nFind element B in list D with length C. Store the index in A. (...)";
const byte HELP_MESSAGE_132[] PROGMEM = "LFL (continued)\nIf B was not found, store -1 in A.";
const byte HELP_MESSAGE_134[] PROGMEM = "LSB [byte list A] [long B] [byte C]\nStock C in each element of list A with length B.";
const byte HELP_MESSAGE_135[] PROGMEM = "LSL [long list A] [long B] [long C]\nStock C in each element of list A with length B.";
const byte HELP_MESSAGE_136[] PROGMEM = "LSF [float list A] [long B] [float C]\nStock C in each element of list A with length B.";

const byte HELP_MESSAGE_27[] PROGMEM = "SWR [string A] [string B]\nStore B in A.";
const byte HELP_MESSAGE_28[] PROGMEM = "SEQ [byte A] [string B] [string C]\nDetermine whether B and C are equal. Store the result in A.";
const byte HELP_MESSAGE_29[] PROGMEM = "SFN [long A] [string B] [string C]\nFind substring B in string C. Store the index in A.";
const byte HELP_MESSAGE_30[] PROGMEM = "SLB [byte A] [string B]\nStore length of string B in A.";
const byte HELP_MESSAGE_31[] PROGMEM = "SLL [long A] [string B]\nStore length of string B in A.";
const byte HELP_MESSAGE_32[] PROGMEM = "SSB [string A] [string B] [byte C] [byte D]\nStore substring of B from index C inclusive to index D exclusive in A.";
const byte HELP_MESSAGE_33[] PROGMEM = "SSL [string A] [string B] [long C] [long D]\nStore substring of B from index C inclusive to index D exclusive in A.";
const byte HELP_MESSAGE_34[] PROGMEM = "SCN [string A] [string B]\nConcatenate string B to string A.";
const byte HELP_MESSAGE_35[] PROGMEM = "SNB [byte A] [string B]\nConvert string B to byte and store in A.";
const byte HELP_MESSAGE_36[] PROGMEM = "SNL [long A] [string B]\nConvert string B to long and store in A.";
const byte HELP_MESSAGE_37[] PROGMEM = "SNF [float A] [string B]\nConvert string B to float and store in A.";
const byte HELP_MESSAGE_38[] PROGMEM = "SDL [byte A] [string B]\nStore number of elements in space delimited string B in A.";
const byte HELP_MESSAGE_39[] PROGMEM = "SDS [string A] [string B] [byte C]\nStore element from space delimited string B with index C in A.";

const byte HELP_MESSAGE_40[] PROGMEM = "AAB [byte A] [byte B] [byte C]\nAdd B and C and store the result in A.";
const byte HELP_MESSAGE_41[] PROGMEM = "AAL [long A] [long B] [long C]\nAdd B and C and store the result in A.";
const byte HELP_MESSAGE_42[] PROGMEM = "AAF [float A] [float B] [float C]\nAdd B and C and store the result in A.";
const byte HELP_MESSAGE_43[] PROGMEM = "ASB [byte A] [byte B] [byte C]\nSubtract C from B and store the result in A.";
const byte HELP_MESSAGE_44[] PROGMEM = "ASL [long A] [long B] [long C]\nSubtract C from B and store the result in A.";
const byte HELP_MESSAGE_45[] PROGMEM = "ASF [float A] [float B] [float C]\nSubtract C from B and store the result in A.";
const byte HELP_MESSAGE_46[] PROGMEM = "AMB [byte A] [byte B] [byte C]\nMultiply B and C and store the result in A.";
const byte HELP_MESSAGE_47[] PROGMEM = "AML [long A] [long B] [long C]\nMultiply B and C and store the result in A.";
const byte HELP_MESSAGE_48[] PROGMEM = "AMF [float A] [float B] [float C]\nMultiply B and C and store the result in A.";
const byte HELP_MESSAGE_49[] PROGMEM = "ADB [byte A] [byte B] [byte C]\nDivide B by C and store the result in A.";
const byte HELP_MESSAGE_50[] PROGMEM = "ADL [long A] [long B] [long C]\nDivide B by C and store the result in A.";
const byte HELP_MESSAGE_51[] PROGMEM = "ADF [float A] [float B] [float C]\nDivide B by C and store the result in A.";
const byte HELP_MESSAGE_52[] PROGMEM = "ARB [byte A] [byte B] [byte C]\nDivide B by C and store the remainder in A.";
const byte HELP_MESSAGE_53[] PROGMEM = "ARL [long A] [long B] [long C]\nDivide B by C and store the remainder in A.";
const byte HELP_MESSAGE_54[] PROGMEM = "ARF [float A] [float B] [float C]\nDivide B by C and store the remainder in A.";

const byte HELP_MESSAGE_55[] PROGMEM = "MIB [byte A]\nIncrement A.";
const byte HELP_MESSAGE_56[] PROGMEM = "MIL [long A]\nIncrement A.";
const byte HELP_MESSAGE_57[] PROGMEM = "MDB [byte A]\nDecrement A.";
const byte HELP_MESSAGE_58[] PROGMEM = "MDL [long A]\nDecrement A.";
const byte HELP_MESSAGE_59[] PROGMEM = "MRB [byte A] [byte B]\nGenerate random number between 0 inclusive and B exclusive and store in A.";
const byte HELP_MESSAGE_60[] PROGMEM = "MRL [long A] [long B]\nGenerate random number between 0 inclusive and B exclusive and store in A.";
const byte HELP_MESSAGE_61[] PROGMEM = "MSN [float A] [float B]\nCompute the sine of B and store the result in A.";
const byte HELP_MESSAGE_62[] PROGMEM = "MCS [float A] [float B]\nCompute the cosine of B and store the result in A.";
const byte HELP_MESSAGE_63[] PROGMEM = "MTN [float A] [float B]\nCompute the tangent of B and store the result in A.";
const byte HELP_MESSAGE_137[] PROGMEM = "MAS [float A] [float B]\nCompute the inverse sine of B and store the result in A.";
const byte HELP_MESSAGE_138[] PROGMEM = "MAC [float A] [float B]\nCompute the inverse cosine of B and store the result in A.";
const byte HELP_MESSAGE_139[] PROGMEM = "MAT [float A] [float B]\nCompute the inverse tangent of B and store the result in A.";
const byte HELP_MESSAGE_140[] PROGMEM = "MA2 [float A] [float B] [float C]\nCompute angle to the position (B, C) and store the result in A.";
const byte HELP_MESSAGE_141[] PROGMEM = "MPW [float A] [float B] [float C]\nRaise B to the C power and store the result in A.";
const byte HELP_MESSAGE_142[] PROGMEM = "MSR [float A] [float B]\nCompute the square root of B and store the result in A.";
const byte HELP_MESSAGE_143[] PROGMEM = "MLN [float A] [float B]\nCompute the natural log of B and store the result in A.";
const byte HELP_MESSAGE_144[] PROGMEM = "MLG [float A] [float B]\nCompute the log base 10 of B and store the result in A.";
const byte HELP_MESSAGE_145[] PROGMEM = "MAB [float A] [float B]\nCompute the absolute value of B and store the result in A.";

const byte HELP_MESSAGE_64[] PROGMEM = "BNB [byte A] [byte B]\nCompute the bitwise NOT of B and store the result in A.";
const byte HELP_MESSAGE_65[] PROGMEM = "BNL [long A] [long B]\nCompute the bitwise NOT of B and store the result in A.";
const byte HELP_MESSAGE_66[] PROGMEM = "BOB [byte A] [byte B] [byte C]\nCompute the bitwise OR of B and C, and store the result in A.";
const byte HELP_MESSAGE_67[] PROGMEM = "BOL [long A] [long B] [long C]\nCompute the bitwise OR of B and C, and store the result in A.";
const byte HELP_MESSAGE_68[] PROGMEM = "BAB [byte A] [byte B] [byte C]\nCompute the bitwise AND of B and C, and store the result in A.";
const byte HELP_MESSAGE_69[] PROGMEM = "BAL [long A] [long B] [long C]\nCompute the bitwise AND of B and C, and store the result in A.";
const byte HELP_MESSAGE_70[] PROGMEM = "BIB [byte A] [byte B]\nCompute the Boolean inverse of B and store the result in A.";
const byte HELP_MESSAGE_71[] PROGMEM = "BIL [long A] [long B]\nCompute the Boolean inverse of B and store the result in A.";
const byte HELP_MESSAGE_146[] PROGMEM = "BLB [byte A] [byte B] [byte C]\nBit shift B to the left by amount C and store the result in A.";
const byte HELP_MESSAGE_147[] PROGMEM = "BLL [long A] [long B] [byte C]\nBit shift B to the left by amount C and store the result in A.";
const byte HELP_MESSAGE_148[] PROGMEM = "BRB [byte A] [byte B] [byte C]\nBit shift B to the right by amount C and store the result in A.";
const byte HELP_MESSAGE_149[] PROGMEM = "BRL [long A] [long B] [byte C]\nBit shift B to the right by amount C and store the result in A.";

const byte HELP_MESSAGE_72[] PROGMEM = "CEB [byte A] [byte B] [byte C]\nDetermine whether B and C are equal, and store the result in A.";
const byte HELP_MESSAGE_73[] PROGMEM = "CEL [byte A] [long B] [long C]\nDetermine whether B and C are equal, and store the result in A.";
const byte HELP_MESSAGE_74[] PROGMEM = "CGB [byte A] [byte B] [byte C]\nDetermine whether B is greater than C, and store the result in A.";
const byte HELP_MESSAGE_75[] PROGMEM = "CGL [byte A] [long B] [long C]\nDetermine whether B is greater than C, and store the result in A.";
const byte HELP_MESSAGE_76[] PROGMEM = "CGF [byte A] [float B] [float C]\nDetermine whether B is greater than C, and store the result in A.";
const byte HELP_MESSAGE_77[] PROGMEM = "CLB [byte A] [byte B] [byte C]\nDetermine whether B is less than C, and store the result in A.";
const byte HELP_MESSAGE_78[] PROGMEM = "CLL [byte A] [long B] [long C]\nDetermine whether B is less than C, and store the result in A.";
const byte HELP_MESSAGE_79[] PROGMEM = "CLF [byte A] [float B] [float C]\nDetermine whether B is less than C, and store the result in A.";

const byte HELP_MESSAGE_80[] PROGMEM = "FIB [byte A]\nExecute the following branch if value A is not zero.";
const byte HELP_MESSAGE_81[] PROGMEM = "FIL [long A]\nExecute the following branch if value A is not zero.";
const byte HELP_MESSAGE_82[] PROGMEM = "FNB [byte A]\nExecute the following branch if value A is zero.";
const byte HELP_MESSAGE_83[] PROGMEM = "FNL [long A]\nExecute the following branch if value A is zero.";
const byte HELP_MESSAGE_84[] PROGMEM = "FEN\nEnd the preceding branch.";
const byte HELP_MESSAGE_85[] PROGMEM = "FWB [byte A]\nExecute the following branch while value A is not zero.";
const byte HELP_MESSAGE_86[] PROGMEM = "FWL [long A]\nExecute the following branch while value A is not zero.";
const byte HELP_MESSAGE_87[] PROGMEM = "FBR\nTerminate the current while branch.";
const byte HELP_MESSAGE_88[] PROGMEM = "FSB [long A]\nDeclare the following block as a subroutine and store a reference to the subroutine in A.";
const byte HELP_MESSAGE_89[] PROGMEM = "FCL [long A]\nCall subroutine with reference A.";

const byte HELP_MESSAGE_90[] PROGMEM = "KRD [byte A]\nDetermine which key is being pressed and store the result in A.";
const byte HELP_MESSAGE_91[] PROGMEM = "KIP [byte A] [byte B]\nDetermine whether key B is being pressed and store the result in A.";

const byte HELP_MESSAGE_92[] PROGMEM = "TWR [long A]\nSet the current time in milliseconds to A.";
const byte HELP_MESSAGE_93[] PROGMEM = "TRD [long A]\nStore the current time in milliseconds in A.";
const byte HELP_MESSAGE_94[] PROGMEM = "TSB [byte A]\nSleep A milliseconds.";
const byte HELP_MESSAGE_95[] PROGMEM = "TSL [long A]\nSleep A milliseconds.";

const byte HELP_MESSAGE_96[] PROGMEM = "DCL\nClear the display.";
const byte HELP_MESSAGE_97[] PROGMEM = "DPX [byte A] [byte B] [byte C]\nDraw pixels C at horizontal position A and vertical position B.";
const byte HELP_MESSAGE_98[] PROGMEM = "DIM [byte A] [byte B] [long C] [byte list D] [byte E]\nDraw pixels D with length C at horizontal position A (...)";
const byte HELP_MESSAGE_133[] PROGMEM = "DIM (continued)\nand vertical position C with width E.";
const byte HELP_MESSAGE_99[] PROGMEM = "DST [byte A] [byte B] [string C]\nDraw string C at horizontal position A and vertical position B.";
const byte HELP_MESSAGE_100[] PROGMEM = "DNB [byte A] [byte B] [byte C]\nDraw number C at horizontal position A and vertical position B.";
const byte HELP_MESSAGE_101[] PROGMEM = "DNL [byte A] [byte B] [long C]\nDraw number C at horizontal position A and vertical position B.";
const byte HELP_MESSAGE_102[] PROGMEM = "DNF [byte A] [byte B] [float C]\nDraw number C at horizontal position A and vertical position B.";
const byte HELP_MESSAGE_150[] PROGMEM = "DPT [byte A] [byte B] [byte C]\nDraw point in the pixel buffer with color C at position (A, B).";
const byte HELP_MESSAGE_151[] PROGMEM = "DLN [byte A] [byte B] [byte C] [byte D] [byte E]\nDraw line in the pixel buffer with color E from position (A, B) to (C, D).";
const byte HELP_MESSAGE_153[] PROGMEM = "DRC [byte A] [byte B] [byte C] [byte D] [byte E]\nDraw rectangle in the pixel buffer with color E (...)";
const byte HELP_MESSAGE_154[] PROGMEM = "DRC (continued)\nat position (A, B) with width C and height D.";
const byte HELP_MESSAGE_155[] PROGMEM = "DBF\nRedraw the pixel buffer.";

const byte HELP_MESSAGE_103[] PROGMEM = "PST [string A]\nPrompt with starting string A and store the result in A.";
const byte HELP_MESSAGE_104[] PROGMEM = "PSL [byte A] [byte B] [string list C]\nPrompt selection from options C with amount B and store the result in A. (...)";
const byte HELP_MESSAGE_105[] PROGMEM = "PSL (continued)\nThe strings in list C must have start indexes which are multiples of 17.";
const byte HELP_MESSAGE_106[] PROGMEM = "PFL [byte A]\nPrompt file and store the handle in A.";
const byte HELP_MESSAGE_107[] PROGMEM = "PKY [byte A]\nPrompt key and store the result in A.";
const byte HELP_MESSAGE_108[] PROGMEM = "PNB [byte A]\nPrompt number and store the result in A.";
const byte HELP_MESSAGE_109[] PROGMEM = "PNL [long A]\nPrompt number and store the result in A.";
const byte HELP_MESSAGE_110[] PROGMEM = "PNF [float A]\nPrompt number and store the result in A.";

const byte HELP_MESSAGE_111[] PROGMEM = "XSM [byte A] [byte B]\nSet pin A to mode B. If B is zero, the mode is input. If B is one, the mode is output.";
const byte HELP_MESSAGE_113[] PROGMEM = "XDR [byte A] [byte B]\nDigital read pin B and store the result in A.";
const byte HELP_MESSAGE_114[] PROGMEM = "XDW [byte A] [byte B]\nDigital write value B to pin A.";
const byte HELP_MESSAGE_115[] PROGMEM = "XAR [long A] [byte B]\nAnalog read pin B and store the result in A. The result will be between 0 and 1023 inclusive.";

const byte HELP_MESSAGE_117[] PROGMEM = "RNF [byte A]\nStore number of files in A.";
const byte HELP_MESSAGE_118[] PROGMEM = "ROI [file handle A] [byte B]\nFind file with index B, and store handle in A.";
const byte HELP_MESSAGE_119[] PROGMEM = "RON [file handle A] [string B]\nFind file with name B, and store handle in A.";
const byte HELP_MESSAGE_120[] PROGMEM = "RCR [file handle A] [string B] [long C]\nCreate file with name B and size C, and store handle in A.";
const byte HELP_MESSAGE_121[] PROGMEM = "RRD [byte list A] [long B] [file handle C] [long D]\nRead B bytes at index D from file C, and store the data in A.";
const byte HELP_MESSAGE_122[] PROGMEM = "RWR [file handle A] [long B] [long C] [byte list D]\nWrite C bytes from D into file A at index B.";
const byte HELP_MESSAGE_123[] PROGMEM = "RGN [string A] [file handle B]\nStore name of file B in A.";
const byte HELP_MESSAGE_124[] PROGMEM = "RSN [file handle A] [string B]\nSet name of file A to B.";
const byte HELP_MESSAGE_125[] PROGMEM = "RGS [long A] [file handle B]\nStore size of file B in A.";
const byte HELP_MESSAGE_126[] PROGMEM = "RSS [file handle A] [long B]\nSet size of file A to B.";
const byte HELP_MESSAGE_127[] PROGMEM = "RDL [file handle A]\nDelete file A.";
const byte HELP_MESSAGE_128[] PROGMEM = "RRN [file handle A]\nRun file A.";

const byte * const HELP_MESSAGE_LIST_16[] PROGMEM = {HELP_MESSAGE_157, HELP_MESSAGE_158};

const byte * const HELP_MESSAGE_LIST_1[] PROGMEM = {HELP_MESSAGE_1, HELP_MESSAGE_2, HELP_MESSAGE_3, HELP_MESSAGE_4, HELP_MESSAGE_5, HELP_MESSAGE_6, HELP_MESSAGE_7, HELP_MESSAGE_156};

const byte * const HELP_MESSAGE_LIST_2[] PROGMEM = {HELP_MESSAGE_8, HELP_MESSAGE_9, HELP_MESSAGE_10, HELP_MESSAGE_11, HELP_MESSAGE_12, HELP_MESSAGE_13, HELP_MESSAGE_14, HELP_MESSAGE_15, HELP_MESSAGE_16, HELP_MESSAGE_17, HELP_MESSAGE_18, HELP_MESSAGE_19};

const byte * const HELP_MESSAGE_LIST_3[] PROGMEM = {HELP_MESSAGE_20, HELP_MESSAGE_21, HELP_MESSAGE_22, HELP_MESSAGE_23, HELP_MESSAGE_129, HELP_MESSAGE_24, HELP_MESSAGE_130, HELP_MESSAGE_25, HELP_MESSAGE_131, HELP_MESSAGE_26, HELP_MESSAGE_132, HELP_MESSAGE_134, HELP_MESSAGE_135, HELP_MESSAGE_136};

const byte * const HELP_MESSAGE_LIST_4[] PROGMEM = {HELP_MESSAGE_27, HELP_MESSAGE_28, HELP_MESSAGE_29, HELP_MESSAGE_30, HELP_MESSAGE_31, HELP_MESSAGE_32, HELP_MESSAGE_33, HELP_MESSAGE_34, HELP_MESSAGE_35, HELP_MESSAGE_36, HELP_MESSAGE_37, HELP_MESSAGE_38, HELP_MESSAGE_39};

const byte * const HELP_MESSAGE_LIST_5[] PROGMEM = {HELP_MESSAGE_40, HELP_MESSAGE_41, HELP_MESSAGE_42, HELP_MESSAGE_43, HELP_MESSAGE_44, HELP_MESSAGE_45, HELP_MESSAGE_46, HELP_MESSAGE_47, HELP_MESSAGE_48, HELP_MESSAGE_49, HELP_MESSAGE_50, HELP_MESSAGE_51, HELP_MESSAGE_52, HELP_MESSAGE_53, HELP_MESSAGE_54};

const byte * const HELP_MESSAGE_LIST_6[] PROGMEM = {HELP_MESSAGE_55, HELP_MESSAGE_56, HELP_MESSAGE_57, HELP_MESSAGE_58, HELP_MESSAGE_59, HELP_MESSAGE_60, HELP_MESSAGE_61, HELP_MESSAGE_62, HELP_MESSAGE_63, HELP_MESSAGE_137, HELP_MESSAGE_138, HELP_MESSAGE_139, HELP_MESSAGE_140, HELP_MESSAGE_141, HELP_MESSAGE_142, HELP_MESSAGE_143, HELP_MESSAGE_144, HELP_MESSAGE_145};

const byte * const HELP_MESSAGE_LIST_7[] PROGMEM = {HELP_MESSAGE_64, HELP_MESSAGE_65, HELP_MESSAGE_66, HELP_MESSAGE_67, HELP_MESSAGE_68, HELP_MESSAGE_69, HELP_MESSAGE_70, HELP_MESSAGE_71, HELP_MESSAGE_146, HELP_MESSAGE_147, HELP_MESSAGE_148, HELP_MESSAGE_149};

const byte * const HELP_MESSAGE_LIST_8[] PROGMEM = {HELP_MESSAGE_72, HELP_MESSAGE_73, HELP_MESSAGE_74, HELP_MESSAGE_75, HELP_MESSAGE_76, HELP_MESSAGE_77, HELP_MESSAGE_78, HELP_MESSAGE_79};

const byte * const HELP_MESSAGE_LIST_9[] PROGMEM = {HELP_MESSAGE_80, HELP_MESSAGE_81, HELP_MESSAGE_82, HELP_MESSAGE_83, HELP_MESSAGE_84, HELP_MESSAGE_85, HELP_MESSAGE_86, HELP_MESSAGE_87, HELP_MESSAGE_88, HELP_MESSAGE_89};

const byte * const HELP_MESSAGE_LIST_10[] PROGMEM = {HELP_MESSAGE_90, HELP_MESSAGE_91};

const byte * const HELP_MESSAGE_LIST_11[] PROGMEM = {HELP_MESSAGE_92, HELP_MESSAGE_93, HELP_MESSAGE_94, HELP_MESSAGE_95};

const byte * const HELP_MESSAGE_LIST_12[] PROGMEM = {HELP_MESSAGE_96, HELP_MESSAGE_97, HELP_MESSAGE_98, HELP_MESSAGE_133, HELP_MESSAGE_99, HELP_MESSAGE_100, HELP_MESSAGE_101, HELP_MESSAGE_102, HELP_MESSAGE_150, HELP_MESSAGE_151, HELP_MESSAGE_153, HELP_MESSAGE_154, HELP_MESSAGE_155};

const byte * const HELP_MESSAGE_LIST_13[] PROGMEM = {HELP_MESSAGE_103, HELP_MESSAGE_104, HELP_MESSAGE_105, HELP_MESSAGE_106, HELP_MESSAGE_107, HELP_MESSAGE_108, HELP_MESSAGE_109, HELP_MESSAGE_110};

const byte * const HELP_MESSAGE_LIST_14[] PROGMEM = {HELP_MESSAGE_111, HELP_MESSAGE_113, HELP_MESSAGE_114, HELP_MESSAGE_115};

const byte * const HELP_MESSAGE_LIST_15[] PROGMEM = {HELP_MESSAGE_117, HELP_MESSAGE_118, HELP_MESSAGE_119, HELP_MESSAGE_120, HELP_MESSAGE_121, HELP_MESSAGE_122, HELP_MESSAGE_123, HELP_MESSAGE_124, HELP_MESSAGE_125, HELP_MESSAGE_126, HELP_MESSAGE_127, HELP_MESSAGE_128};

const byte MENU_1[] PROGMEM = "Select file\0     Create file\0     Transfer data\0   DPCL help\0       About system\0";
const byte MENU_2[] PROGMEM = "Run file\0        Edit file\0       Get file info\0   Rename file\0     Resize file\0     Delete file\0";
const byte MENU_3[] PROGMEM = "No\0              Yes\0";
const byte MENU_4[] PROGMEM = "Editor\0          Syntax\0          Number\0          List\0            String\0          Arithmetic\0      Mathematical\0    Bitwise/boolean\0 Comparison\0      Flow\0            Key\0             Time\0            Display\0         Prompt\0          Exchange\0        Repository\0";

long findByte(long amount, byte *buffer, byte value)
{
	long output = 0;
	while (output < amount)
	{
		byte tempValue = *buffer;
		if (tempValue == value)
		{
			return output;
		}
		buffer += 1;
		output += 1;
	}
	return -1;
}

byte findWhiteSpace(byte *buffer)
{
	byte output = 0;
	while (output < 255)
	{
		byte tempCharacter = *buffer;
		if (tempCharacter == 32 || tempCharacter == 10 || tempCharacter == 0)
		{
			break;
		}
		buffer += 1;
		output += 1;
	}
	return output;
}

long getTextLength(byte *buffer)
{
	long output = 0;
	while (output < 512)
	{
		byte tempData = *buffer;
		if (tempData == 0)
		{
			break;
		}
		buffer += 1;
		output += 1;
	}
	return output;
}

long getLineLength(byte *buffer)
{
	long output = 0;
	while (output < 512)
	{
		byte tempData = *buffer;
		if (tempData == 0 || tempData == 10)
		{
			break;
		}
		buffer += 1;
		output += 1;
	}
	return output;
}

void copyText(byte *destination, byte *source)
{
	long count = 0;
	while (count < 1000)
	{
		byte tempValue = *source;
		*destination = tempValue;
		if (tempValue == 0)
		{
			break;
		}
		destination += 1;
		source += 1;
		count += 1;
	}
}

void copyData(long amount, byte *destination, byte *source)
{
	while (amount > 0)
	{
		byte tempValue = *source;
		*destination = tempValue;
		destination += 1;
		source += 1;
		amount -= 1;
	}
}

byte equalText(byte *buffer1, byte *buffer2)
{
	long index = 0;
	while (index < 512)
	{
		byte tempValue1 = *buffer1;
		byte tempValue2 = *buffer2;
		if (tempValue1 != tempValue2)
		{
			return false;
		}
		if (tempValue1 == 0)
		{
			break;
		}
		buffer1 += 1;
		buffer2 += 1;
		index += 1;
	}
	return true;
}

byte equalData(long amount, byte *buffer1, byte *buffer2)
{
	while (amount > 0)
	{
		byte tempValue1 = *buffer1;
		byte tempValue2 = *buffer2;
		if (tempValue1 != tempValue2)
		{
			return false;
		}
		buffer1 += 1;
		buffer2 += 1;
		amount -= 1;
	}
	return true;
}

void convertFloatToText(byte *buffer, float number)
{
	byte tempBuffer[20];
	dtostrf(number, 13, 4, (char *)tempBuffer);
	byte *tempPointer = tempBuffer;
	while (*tempPointer == ' ')
	{
		tempPointer += 1;
	}
	copyText(buffer, tempPointer);
}

void sendByteToDisplay(byte data)
{
	byte mask = 128;
	byte count = 0;
	while (count < 8)
	{
		DISPLAY_CLOCK_PIN_LOW;
		byte tempResult = data & mask;
		if (tempResult > 0)
		{
			DISPLAY_DATA_PIN_HIGH;
        } else {
			DISPLAY_DATA_PIN_LOW;
		}
		_delay_us(3);
		DISPLAY_CLOCK_PIN_HIGH;
		_delay_us(3);
		mask /= 2;
		count += 1;
	}
}

void sendCommandToDisplay(byte command)
{
	DISPLAY_MODE_PIN_LOW;
	sendByteToDisplay(command);
}

void sendPixelsToDisplay(byte data)
{
	DISPLAY_MODE_PIN_HIGH;
	sendByteToDisplay(data);
}

void resetDisplay()
{
	DISPLAY_RESET_PIN_LOW;
	_delay_us(500);
	DISPLAY_RESET_PIN_HIGH;
	_delay_us(500);
	byte index = 0;
	while (index < 13)
	{
		byte tempCommand = pgm_read_byte(DISPLAY_INITIALIZE_DATA_LIST + index);
		sendCommandToDisplay(tempCommand);
		index += 1;
	}
}

void setDisplayPos(byte posX, byte posY)
{
	byte column = posX + 30;
	sendCommandToDisplay(0x10 | (column / 16));
	sendCommandToDisplay(0x00 | (column % 16));
	sendCommandToDisplay(0xB0 | posY);
}

void clearDisplay()
{
	byte posY = 0;
	while (posY < 8)
	{
		byte posX = 0;
		setDisplayPos(posX, posY);
		while (posX < 102)
		{
			sendPixelsToDisplay(0x00);
			posX += 1;
		}
		posY += 1;
	}
}

void drawPixels(byte posX, byte posY, byte data)
{
	setDisplayPos(posX, posY);
	sendPixelsToDisplay(data);
}

void sendCharacterToDisplay(byte which)
{
	unsigned int index = (which - 32) * 5;
	byte count = 0;
	while (count < 5)
	{
		byte tempData = pgm_read_byte(FONT_DATA_LIST + index);
		sendPixelsToDisplay(tempData);
		index += 1;
		count += 1;
	}
	sendPixelsToDisplay(0x00);
}

void drawText(byte posX, byte posY, byte *sequence)
{
	setDisplayPos(posX, posY);
	byte count = 0;
	while (count < 250)
	{
		byte which = *sequence;
		if (which == 0)
		{
			break;
		}
		byte isNewlineCharacter = false;
		byte shouldWriteCharacter = true;
		if (which == 10)
		{
			isNewlineCharacter = true;
			shouldWriteCharacter = false;
		}
		if (shouldWriteCharacter)
		{
			sendCharacterToDisplay(which);
		}
		sequence += 1;
		posX += 6;
		if (posX > 101 || isNewlineCharacter)
		{
			posX = 0;
			posY += 1;
			setDisplayPos(posX, posY);
		}
		count += 1;
	}
}

void copyTextFromProgMemToBuffer(byte *destination, const byte *sequence)
{
	byte count = 0;
	while (count < 250)
	{
		byte tempData = pgm_read_byte(sequence);
		*destination = tempData;
		if (tempData == 0)
		{
			break;
		}
		sequence += 1;
		destination += 1;
		count += 1;
	}
}

void copyDataFromProgMemToBuffer(byte *destination, long amount, const byte *sequence)
{
	long index = 0;
	while (index < amount)
	{
		byte tempData = pgm_read_byte(sequence + index);
		*(destination + index) = tempData;
		index += 1;
	}
}

void drawTextFromProgMem(byte startPosX, byte startPosY, const byte *sequence)
{
	byte buffer[150];
	copyTextFromProgMemToBuffer(buffer, sequence);
	drawText(startPosX, startPosY, buffer);
}

void drawCharacter(byte posX, byte posY, byte character)
{
	setDisplayPos(posX, posY);
	sendCharacterToDisplay(character);
}

void drawByteNumber(byte posX, byte posY, byte number)
{
	byte buffer[20];
	itoa(number, (char *)buffer, 10);
	drawText(posX, posY, buffer);
}

void drawLongNumber(byte posX, byte posY, long number)
{
	byte buffer[20];
	ltoa(number, (char *)buffer, 10);
	drawText(posX, posY, buffer);
}

void drawFloatNumber(byte posX, byte posY, float number)
{
	byte buffer[20];
	convertFloatToText(buffer, number);
	drawText(posX, posY, buffer);
}

void drawImage(byte posX, byte posY, long amount, byte *buffer, byte width)
{
	setDisplayPos(posX, posY);
	byte offsetX = 0;
	long count = 0;
	while (count < amount)
	{
		byte tempData = *buffer;
		sendPixelsToDisplay(tempData);
		offsetX += 1;
		if (offsetX > width - 1)
		{
			offsetX = 0;
			posY += 1;
			setDisplayPos(posX, posY);
		}
		buffer += 1;
		count += 1;
	}
}

long receiveRowFromKeyboard(byte which)
{
	switch (which)
	{
		case 0:
			KEYBOARD_ROW_0_PIN_OUTPUT;
			break;
		case 1:
			KEYBOARD_ROW_1_PIN_OUTPUT;
			break;
		case 2:
			KEYBOARD_ROW_2_PIN_OUTPUT;
			break;
		case 3:
			KEYBOARD_ROW_3_PIN_OUTPUT;
			break;
		case 4:
			KEYBOARD_ROW_4_PIN_OUTPUT;
			break;
		default:
			break;
	}
	KEYBOARD_CLOCK_PIN_LOW;
	_delay_us(3);
	KEYBOARD_LOAD_PIN_LOW;
	_delay_us(3);
	KEYBOARD_LOAD_PIN_HIGH;
	_delay_us(3);
	long output = 0;
	long mask = 0x00001000;
	byte count = 0;
	while (count < 13)
	{
		byte tempResult = READ_KEYBOARD_DATA_PIN;
		if (tempResult > 0)
		{
			output |= mask;
		}
		KEYBOARD_CLOCK_PIN_HIGH;
		_delay_us(3);
		KEYBOARD_CLOCK_PIN_LOW;
		_delay_us(3);
		mask >>= 1;
		count += 1;
	}
	switch (which)
	{
		case 0:
			KEYBOARD_ROW_0_PIN_INPUT;
			break;
		case 1:
			KEYBOARD_ROW_1_PIN_INPUT;
			break;
		case 2:
			KEYBOARD_ROW_2_PIN_INPUT;
			break;
		case 3:
			KEYBOARD_ROW_3_PIN_INPUT;
			break;
		case 4:
			KEYBOARD_ROW_4_PIN_INPUT;
			break;
		default:
			break;
	}
	return output;
}

byte escapeKeyHasBeenPressed = false;

byte shiftKeyIsPressed()
{
	long tempRow = receiveRowFromKeyboard(3);
	return ((tempRow & 1) > 0 || (tempRow & (1 << 0x0C)) > 0);
}

byte keyIsPressed(byte which)
{
	byte tempBuffer[sizeof(KEYBOARD_DATA_LIST)];
	copyDataFromProgMemToBuffer(tempBuffer, sizeof(KEYBOARD_DATA_LIST), KEYBOARD_DATA_LIST);
	long index = findByte(sizeof(KEYBOARD_DATA_LIST), tempBuffer, which);
	if (index == -1)
	{
		return false;
	}
	if (which > 32 && ((index > 65) ^ shiftKeyIsPressed()))
	{
		return false;
	}
	index %= 65;
	long tempRow = receiveRowFromKeyboard(index / 13);
	return ((tempRow & (1 << (index % 13))) > 0);
}

byte readKeys()
{
	byte shiftIsPressed = shiftKeyIsPressed();
	long tempRow = 0;
	byte tempPosY = 0;
	while (tempPosY < 5)
	{
		tempRow = receiveRowFromKeyboard(tempPosY);
		if (tempPosY == 3)
		{
			tempRow &= ~(1);
			tempRow &= ~(1 << 0x0C);
		}
		if (tempRow > 0)
		{
			break;
		}
		tempPosY += 1;
	}
	if (tempRow == 0)
	{
		return 0;
	}
	byte tempPosX = 0;
	while (tempPosX < 13)
	{
		if ((tempRow & 1) > 0)
		{
			break;
		}
		tempRow >>= 1;
		tempPosX += 1;
	}
	byte index = tempPosX + tempPosY * 13;
	if (shiftIsPressed)
	{
		index += 65;
	}
	byte output = pgm_read_byte(KEYBOARD_DATA_LIST + index);
	if (output == 27)
	{
		escapeKeyHasBeenPressed = true;
	}
	return output;
}

void checkEscapeKey()
{
	long tempRow = receiveRowFromKeyboard(4);
	if ((tempRow & (1 << 1)) > 0)
	{
		escapeKeyHasBeenPressed = true;
	}
}

byte promptKey()
{
	byte output;
	while (true)
	{
		output = readKeys();
		if (output != 0)
		{
			break;
		}
	}
	while (readKeys() == output)
	{
		
	}
	return output;
}

long promptKeySeed()
{
	long output = 0;
	byte shouldLoop = true;
	while(shouldLoop)
	{
		byte index = 0;
		while (index < 5)
		{
			long tempRow = receiveRowFromKeyboard(index);
			if (index == 3)
			{
				tempRow &= ~(1);
				tempRow &= ~(1 << 0x0C);
			}
			if (tempRow > 0)
			{
				shouldLoop = false;
				break;
			}
			output += 1;
			index += 1;
		}
	}
	byte tempKey1 = readKeys();
	while (true)
	{
		byte tempKey2 = readKeys();
		if (tempKey2 != tempKey1 || tempKey2 == 0)
		{
			break;
		}
	}
	return output;
}

void sendStartConditionToEeprom()
{
	EEPROM_DATA_PIN_OUTPUT;
	EEPROM_DATA_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_HIGH;
	_delay_us(6);
	EEPROM_DATA_PIN_LOW;
	_delay_us(6);
}

void sendStopConditionToEeprom()
{
	EEPROM_DATA_PIN_OUTPUT;
	EEPROM_DATA_PIN_LOW;
	_delay_us(6);
	EEPROM_CLOCK_PIN_HIGH;
	_delay_us(6);
	EEPROM_DATA_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
	// These extra clock cycles are necessary, but
	// I'm not sure why.
	EEPROM_CLOCK_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
}

void sendByteToEeprom(byte data)
{
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
	EEPROM_DATA_PIN_OUTPUT;
	byte mask = 0x80;
	byte count = 0;
	while (count < 8)
	{
		_delay_us(6);
		byte tempResult = data & mask;
		if (tempResult > 0)
		{
			EEPROM_DATA_PIN_HIGH;
		} else {
			EEPROM_DATA_PIN_LOW;
		}
		_delay_us(6);
		EEPROM_CLOCK_PIN_HIGH;
		_delay_us(6);
		mask >>= 1;
		count += 1;
		EEPROM_CLOCK_PIN_LOW;
	}
	EEPROM_DATA_PIN_LOW;
	EEPROM_DATA_PIN_INPUT;
	_delay_us(6);
	EEPROM_CLOCK_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
}

byte receiveByteFromEeprom()
{
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
	EEPROM_DATA_PIN_INPUT;
	byte output = 0;
	byte mask = 0x80;
	byte count = 0;
	while (count < 8)
	{
		EEPROM_CLOCK_PIN_HIGH;
		_delay_us(6);
		byte tempData = READ_EEPROM_DATA_PIN;
		if (tempData > 0)
		{
			output |= mask;
		}
		_delay_us(6);
		EEPROM_CLOCK_PIN_LOW;
		_delay_us(6);
		mask >>= 1;
		count += 1;
	}
	return output;
}

void sendAcknowledgeBit()
{
	EEPROM_DATA_PIN_OUTPUT;
	EEPROM_DATA_PIN_LOW;
	_delay_us(6);
	EEPROM_CLOCK_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
}

void sendNoAcknowledgeBit()
{
	EEPROM_DATA_PIN_OUTPUT;
	EEPROM_DATA_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
}

void sendReadAddressToEeprom(long address)
{
	sendStartConditionToEeprom();
	sendByteToEeprom(0xA0);
	sendByteToEeprom((address & 0x0000FF00) >> 8);
	sendByteToEeprom((address & 0x000000FF));
	sendStartConditionToEeprom();
	sendByteToEeprom(0xA1);
}

void readSequenceFromEeprom(long amount, byte *buffer, long address)
{
	sendReadAddressToEeprom(address);
	long count = 0;
	while (count < amount)
	{
		byte tempData = receiveByteFromEeprom();
		if (count < amount - 1)
		{
			sendAcknowledgeBit();
		} else {
			sendNoAcknowledgeBit();
		}
		*buffer = tempData;
		buffer += 1;
		count += 1;
	}
	sendStopConditionToEeprom();
	/*
	clearDisplay();
	drawText(0, 0, (byte *)"READ");
	drawLongNumber(0, 1, address);
	drawImage(0, 2, count, buffer - count, 100);
	promptKey();
	 */
}

void readLineFromEeprom(byte *buffer, long address)
{
	//clearDisplay();
	//drawLongNumber(0, 0, address);
	sendReadAddressToEeprom(address);
	byte count = 0;
	while (count < 200)
	{
		byte tempData = receiveByteFromEeprom();
		//drawCharacter(count * 6, 1, tempData);
		*buffer = tempData;
		if (tempData == 10 || tempData == 0)
		{
			sendNoAcknowledgeBit();
			break;
		}
		sendAcknowledgeBit();
		buffer += 1;
		count += 1;
	}
	sendStopConditionToEeprom();
	//promptKey();
	/*
	clearDisplay();
	drawText(0, 0, (byte *)"READ LINE");
	drawLongNumber(0, 1, address);
	drawImage(0, 2, count, buffer - count, 100);
	promptKey();
	 */
}

void writePageToEeprom(long address, byte amount, byte *buffer)
{
	sendStartConditionToEeprom();
	sendByteToEeprom(0xA0);
	sendByteToEeprom((address & 0x0000FF00) >> 8);
	sendByteToEeprom((address & 0x000000FF));
	byte count = 0;
	while (count < amount)
	{
		byte tempData = *buffer;
		sendByteToEeprom(tempData);
		buffer += 1;
		count += 1;
	}
	sendStopConditionToEeprom();
	_delay_ms(7);
}

void writeSequenceToEeprom(long address, long amount, byte *buffer)
{
	/*
	clearDisplay();
	drawText(0, 0, (byte *)"WRITE");
	drawLongNumber(0, 1, address);
	drawImage(0, 2, amount, buffer, 100);
	promptKey();
	 */
	long count = 0;
	while (count < amount)
	{
		long nextPageAddress = (address + 128) & 0xFFFFFF80;
		byte tempAmount = nextPageAddress - address;
		if (count + tempAmount > amount)
		{
			tempAmount = amount - count;
		}
		writePageToEeprom(address, tempAmount, buffer);
		address += tempAmount;
		buffer += tempAmount;
		count += tempAmount;
	}
	//readSequenceFromEeprom(amount, buffer - count, address - count);
}

void writeSequenceToEepromFromProgMem(long address, long amount, const byte *sequence)
{
	byte buffer[amount];
	copyDataFromProgMemToBuffer(buffer, amount, sequence);
	writeSequenceToEeprom(address, amount, buffer);
}

void writeLineToEeprom(long address, byte *buffer)
{
	long tempAmount = getLineLength(buffer) + 1;
	writeSequenceToEeprom(address, tempAmount, buffer);
}

byte toUpperCase(byte character)
{
	if (character > 96 && character < 123)
	{
		character -= 32;
	}
	return character;
}

byte programMemory[12288];
byte programIsFinished;
byte programHasError;
byte command[256];
long commandAddress;
byte *argumentLocations[5];
// Type may be 0 for unknown, 1 for byte, or 2 for long.
byte argumentTypes[5];
byte argumentValues[256];
byte branchStack[128];
byte branchStackLevel;
byte branchStackIgnoreLevel;
byte checkEscapeKeyDelay = 0;
long time = 0;
byte nextFileHandleToRun;

byte getArgumentByte(byte which)
{
	byte tempType = argumentTypes[which];
	if (tempType == 2)
	{
		return (byte)(*((long *)(argumentLocations[which])));
	} else {
		return *(argumentLocations[which]);
	}
}

long getArgumentLong(byte which)
{
	byte tempType = argumentTypes[which];
	if (tempType == 1)
	{
		return (long)(*(argumentLocations[which]));
	} else {
		return *((long *)(argumentLocations[which]));
	}
}

float getArgumentFloat(byte which)
{
	byte tempType = argumentTypes[which];
	if (tempType == 1)
	{
		return (float)(*(argumentLocations[which]));
	} else if (tempType == 2)
	{
		return (float)(*((long *)(argumentLocations[which])));
	} else {
		return *((float *)(argumentLocations[which]));
	}
}

byte getArgumentListByte(byte which, long index)
{
	byte tempType = argumentTypes[which];
	if (tempType == 2)
	{
		return (byte)(*((long *)(argumentLocations[which] + index * 4)));
	} else {
		return *(argumentLocations[which] + index);
	}
}

long getArgumentListLong(byte which, long index)
{
	byte tempType = argumentTypes[which];
	if (tempType == 1)
	{
		return (long)(*(argumentLocations[which] + index));
	} else {
		return *((long *)(argumentLocations[which] + index * 4));
	}
}

float getArgumentListFloat(byte which, long index)
{
	byte tempType = argumentTypes[which];
	if (tempType == 1)
	{
		return (float)(*(argumentLocations[which] + index));
	} else if (tempType == 2)
	{
		return (float)(*((long *)(argumentLocations[which] + index * 4)));
	} else {
		return *((float *)(argumentLocations[which] + index * 4));
	}
}

void setDestinationByte(byte value)
{
	*(argumentLocations[0]) = value;
}

void setDestinationLong(long value)
{
	*((long *)(argumentLocations[0])) = value;
}

void setDestinationFloat(float value)
{
	*((float *)(argumentLocations[0])) = value;
}

void setDestinationListByte(long index, byte value)
{
	*(argumentLocations[0] + index) = value;
}

void setDestinationListLong(long index, long value)
{
	*((long *)(argumentLocations[0] + index * 4)) = value;
}

void setDestinationListFloat(long index, float value)
{
	*((float *)(argumentLocations[0] + index * 4)) = value;
}

void reportError(const byte *message)
{
	clearDisplay();
	drawTextFromProgMem(0, 0, message);
	drawTextFromProgMem(0, 3, MESSAGE_3);
	byte *tempPointer = command;
	byte tempCharacter = *tempPointer;
	while (tempCharacter != 10 && tempCharacter != 0)
	{
		tempPointer += 1;
		tempCharacter = *tempPointer;
	}
	*tempPointer = 0;
	drawText(0, 4, command);
	programIsFinished = true;
	programHasError = true;
}

void setFileName(byte handle, byte *buffer)
{
	if (getTextLength(buffer) > 15)
	{
		return;
	}
	long tempAddress = (long)handle * FILE_ENTRY_SIZE;
	long tempLength = getTextLength(buffer);
	writeSequenceToEeprom(tempAddress, tempLength + 1, buffer);
}

void getFileName(byte *buffer, byte handle)
{
	long tempAddress = (long)handle * FILE_ENTRY_SIZE;
	readLineFromEeprom(buffer, tempAddress);
}

void setFileAddress(byte handle, long address)
{
	long tempAddress = (long)handle * FILE_ENTRY_SIZE + 16;
	writeSequenceToEeprom(tempAddress, 4, (byte *)(&address));
}

long getFileAddress(byte handle)
{
	long tempAddress = (long)handle * FILE_ENTRY_SIZE + 16;
	long output;
	readSequenceFromEeprom(4, (byte *)(&output), tempAddress);
	return output;
}

void setFileSize(byte handle, long size)
{
	long tempAddress = (long)handle * FILE_ENTRY_SIZE + 20;
	writeSequenceToEeprom(tempAddress, 4, (byte *)(&size));
}

long getFileSize(byte handle)
{
	long tempAddress = (long)handle * FILE_ENTRY_SIZE + 20;
	long output;
	readSequenceFromEeprom(4, (byte *)(&output), tempAddress);
	return output;
}

long getUnusedFileAddress(long size)
{
	long output = 4096;
	byte hasCollision = true;
	while (hasCollision)
	{
		hasCollision = false;
		byte tempHandle = 0;
		while (tempHandle < 170)
		{
			long tempSize = getFileSize(tempHandle);
			if (tempSize == 0xFFFFFFFF)
			{
				break;
			}
			if (tempSize > 0)
			{
				long tempAddress = getFileAddress(tempHandle);
				if (output + size > tempAddress && output < tempAddress + tempSize)
				{
					output = tempAddress + tempSize;
					hasCollision = true;
				}
			}
			tempHandle += 1;
		}
	}
	return output;
}

byte createFile(byte *buffer, long size)
{
	if (getTextLength(buffer) > 15)
	{
		return 255;
	}
	if (size > (long)30000 || size < 1)
	{
		return 255;
	}
	byte output = 0;
	while (output < 170)
	{
		long tempSize = getFileSize(output);
		if (tempSize == 0x00000000)
		{
			break;
		}
		if (tempSize == 0xFFFFFFFF)
		{
			setFileSize(output + 1, 0xFFFFFFFF);
			break;
		}
		output += 1;
	}
	long tempAddress = getUnusedFileAddress(size);
	setFileName(output, buffer);
	/*
	clearDisplay();
	drawText(0, 2, buffer);
	byte tempBuffer[17];
	getFileName(tempBuffer, 0);
	drawText(0, 3, tempBuffer);
	promptKey();
	 */
	setFileAddress(output, tempAddress);
	setFileSize(output, size);
	byte tempValue = 0;
	writeSequenceToEeprom(tempAddress, 1, &tempValue);
	/*
	clearDisplay();
	drawByteNumber(0, 0, output);
	byte tempBuffer[24];
	readSequenceFromEeprom(24, tempBuffer, (long)output * FILE_ENTRY_SIZE);
	drawImage(0, 2, 24, tempBuffer, 30);
	promptKey();
	 */
	return output;
}

void deleteFile(byte handle)
{
	setFileSize(handle, 0x00000000);
}

void resizeFile(byte handle, long size)
{
	if (size > (long)30000 || size < 1)
	{
		return;
	}
	long lastAddress = getFileAddress(handle);
	long lastSize = getFileSize(handle);
	long nextAddress = getUnusedFileAddress(size);
	setFileAddress(handle, nextAddress);
	setFileSize(handle, size);
	long tempSize;
	if (size < lastSize)
	{
		tempSize = size;
	} else {
		tempSize = lastSize;
	}
	byte tempBuffer[100];
	long index = 0;
	while (index < tempSize - 100)
	{
		readSequenceFromEeprom(100, tempBuffer, lastAddress + index);
		writeSequenceToEeprom(nextAddress + index, 100, tempBuffer);
		index += 100;
	}
	byte tempSize2 = tempSize - index;
	if (tempSize2 > 0)
	{
		readSequenceFromEeprom(tempSize2, tempBuffer, lastAddress + index);
		tempBuffer[tempSize2 - 1] = 0;
		writeSequenceToEeprom(nextAddress + index, tempSize2, tempBuffer);
	}
}

void drawSelectionCursor(byte index, byte pageIndex)
{
	drawCharacter(0, index - pageIndex, '>');
}

void eraseSelectionCursor(byte index, byte pageIndex)
{
	drawCharacter(0, index - pageIndex, ' ');
}

void drawSelections(byte amount, byte *buffer, byte index, byte pageIndex)
{
	clearDisplay();
	byte tempOffset = 0;
	while (tempOffset < 8 && pageIndex + tempOffset < amount)
	{
		long tempIndex = (long)(pageIndex + tempOffset) * 17;
		drawText(6, tempOffset, buffer + tempIndex);
		tempOffset += 1;
	}
	drawSelectionCursor(index, pageIndex);
}

byte promptSelection(byte amount, byte *buffer)
{
	byte output = 0;
	byte pageIndex = 0;
	drawSelections(amount, buffer, output, pageIndex);
	while (1 == 1)
	{
		byte lastIndex = output;
		escapeKeyHasBeenPressed = false;
		byte tempKey = promptKey();
		if (escapeKeyHasBeenPressed)
		{
			return 255;
		}
		if (tempKey == 19)
		{
			output -= 1;
		}
		if (tempKey == 18)
		{
			output += 1;
		}
		if (tempKey == 'b')
		{
			output -= 8;
		}
		if (tempKey == 'n')
		{
			output += 8;
		}
		if (tempKey == 10)
		{
			break;
		}
		if (lastIndex != output)
		{
			if (output > 240)
			{
				output = amount - 1;
			}
			if (output > amount - 1)
			{
				output = 0;
			}
			if (output < pageIndex || output > pageIndex + 7)
			{
				pageIndex = (output / 8) * 8;
				drawSelections(amount, buffer, output, pageIndex);
			} else {
				eraseSelectionCursor(lastIndex, pageIndex);
				drawSelectionCursor(output, pageIndex);
			}
		}
	}
	return output;
}

byte promptSelectionFromProgMem(byte amount, long length, const byte *sequence)
{
	byte tempBuffer[350];
	copyDataFromProgMemToBuffer(tempBuffer, length, sequence);
	return promptSelection(amount, tempBuffer);
}

void drawTextCursor(byte index)
{
	byte tempPosX = (index % 17) * 6;
	byte tempPosY = index / 17;
	drawCharacter(tempPosX, tempPosY, '_');
}

void eraseTextCursor(byte index, byte *buffer)
{
	byte tempValue = *(buffer + index);
	if (tempValue == 0)
	{
		tempValue = 32;
	}
	byte tempPosX = (index % 17) * 6;
	byte tempPosY = index / 17;
	drawCharacter(tempPosX, tempPosY, tempValue);
}

void promptText(byte *buffer)
{
	clearDisplay();
	drawText(0, 0, buffer);
	byte index = getTextLength(buffer);
	drawTextCursor(index);
	while (true)
	{
		byte lastIndex = index;
		escapeKeyHasBeenPressed = false;
		byte tempKey = promptKey();
		if (escapeKeyHasBeenPressed)
		{
			return;
		}
		byte tempLength = getTextLength(buffer);
		if (tempKey == 17)
		{
			index -= 1;
		}
		if (tempKey == 18)
		{
			index += 17;
		}
		if (tempKey == 19)
		{
			index -= 17;
		}
		if (tempKey == 20)
		{
			index += 1;
		}
		if (tempKey > 31 && tempKey < 127)
		{
			byte tempBuffer[200];
			copyText(tempBuffer, buffer + index);
			copyText(buffer + index + 1, tempBuffer);
			*(buffer + index) = tempKey;
			index += 1;
			byte tempPosX = (index % 17) * 6;
			byte tempPosY = index / 17;
			drawText(tempPosX, tempPosY, buffer + index);
			tempLength += 1;
		}
		if (tempKey == 8 && index > 0)
		{
			byte tempBuffer[200];
			copyText(tempBuffer, buffer + index);
			copyText(buffer + index - 1, tempBuffer);
			index -= 1;
			byte tempPosX = ((index - 1) % 17) * 6;
			byte tempPosY = (index - 1) / 17;
			drawText(tempPosX, tempPosY, buffer + index - 1);
			tempPosX = ((tempLength - 1) % 17) * 6;
			tempPosY = (tempLength - 1) / 17;
			drawCharacter(tempPosX, tempPosY, ' ');
		}
		if (tempKey == 10)
		{
			return;
		}
		if (index > 240)
		{
			index = 0;
		}
		if (index > tempLength)
		{
			index = tempLength;
		}
		if (index != lastIndex)
		{
			eraseTextCursor(lastIndex, buffer);
			drawTextCursor(index);
		}
	}
}

byte promptByteNumber()
{
	byte tempBuffer[10];
	tempBuffer[0] = 0;
	promptText(tempBuffer);
	return (byte)(atoi((char *)tempBuffer));
}

long promptLongNumber()
{
	byte tempBuffer[10];
	tempBuffer[0] = 0;
	promptText(tempBuffer);
	return atol((char *)tempBuffer);
}

float promptFloatNumber()
{
	byte tempBuffer[10];
	tempBuffer[0] = 0;
	promptText(tempBuffer);
	return atof((char *)tempBuffer);
}

byte promptFile()
{
	byte tempBuffer[1000];
	byte tempHandleList[100];
	byte index = 0;
	byte tempHandle = 0;
	while (tempHandle < 170)
	{
		long tempSize = getFileSize(tempHandle);
		if (tempSize == 0xFFFFFFFF)
		{
			break;
		}
		if (tempSize > 0)
		{
			getFileName(tempBuffer + (long)index * 17, tempHandle);
			tempHandleList[index] = tempHandle;
			index += 1;
		}
		tempHandle += 1;
	}
	byte tempResult = promptSelection(index, tempBuffer);
	return tempHandleList[tempResult];
}

void delayMilliseconds(long amount)
{
	long endTime = time + amount;
	while (time < endTime || (time > 1000000 && endTime < -1000000))
	{
		checkEscapeKey();
		if (escapeKeyHasBeenPressed)
		{
			return;
		}
	}
}

void setPinMode(byte pin, byte mode)
{
	if (pin < 8)
	{
		if (mode == 0)
		{
			DDRA &= ~(1 << pin);
		}
		if (mode == 1)
		{
			DDRA |= (1 << pin);
		}
	}
	if (pin > 7 && pin < 16)
	{
		byte tempNumber = 15 - pin;
		if (mode == 0)
		{
			DDRC &= ~(1 << tempNumber);
		}
		if (mode == 1)
		{
			DDRC |= (1 << tempNumber);
		}
	}
	if (pin == 16)
	{
		if (mode == 0)
		{
			DDRD &= ~(1 << 7);
		}
		if (mode == 1)
		{
			DDRD |= (1 << 7);
		}
	}
}

byte digitalReadPin(byte pin)
{
	if (pin < 8)
	{
		return ((PINA & (1 << pin)) > 0);
	}
	if (pin > 7 && pin < 16)
	{
		byte tempNumber = 15 - pin;
		return ((PINC & (1 << tempNumber)) > 0);
	}
	if (pin == 16)
	{
		return ((PIND & (1 << 7)) > 0);
	}
	return 0;
}

void digitalWritePin(byte pin, byte value)
{
	if (pin < 8)
	{
		if (value == 0)
		{
			PORTA &= ~(1 << pin);
		} else {
			PORTA |= (1 << pin);
		}
	}
	if (pin > 7 && pin < 16)
	{
		byte tempNumber = 15 - pin;
		if (value == 0)
		{
			PORTC &= ~(1 << tempNumber);
		} else {
			PORTC |= (1 << tempNumber);
		}
	}
	if (pin == 16)
	{
		if (value == 0)
		{
			PORTD &= ~(1 << 7);
		} else {
			PORTD |= (1 << 7);
		}
	}
}

long analogReadPin(byte pin)
{
    ADMUX = (ADMUX & 0xF0) | (pin & 0x0F);
    ADCSRA |= (1 << ADSC);
    while(ADCSRA & (1 << ADSC))
	{
		
	}
	return ADC;
}

long pixelBufferStartAddress = 11000;

void drawPoint(byte posX, byte posY, byte color)
{
	if (posX > 101 || posY > 63)
	{
		return;
	}
	byte tempPosY = posY / 8;
	long index = pixelBufferStartAddress + (long)posX + (long)tempPosY * (long)102;
	byte tempData = *(programMemory + index);
	byte tempOffsetY = posY % 8;
	if (color == 0)
	{
		tempData &= ~(1 << tempOffsetY);
	} else {
		tempData |= (1 << tempOffsetY);
	}
	*(programMemory + index) = tempData;
	drawPixels(posX, tempPosY, tempData);
}

void executeCommand()
{
	readLineFromEeprom(command, commandAddress);
	byte tempCharacter = *command;
	if (tempCharacter == 13)
	{
		commandAddress += 1;
		return;
	}
	if (tempCharacter == 0)
	{
		programIsFinished = true;
		return;
	}
	byte index = 0;
	while (index < 3)
	{
		byte tempCharacter = *(command + index);
		if (tempCharacter < 65 || (tempCharacter > 90 && tempCharacter < 97) || tempCharacter > 122)
		{
			reportError(ERROR_MESSAGE_1);
			return;
		}
		index += 1;
	}
	tempCharacter = *(command + 3);
	if (tempCharacter != 32 && tempCharacter != 10 && tempCharacter != 0)
	{
		reportError(ERROR_MESSAGE_1);
		return;
	}
	byte commandNameCharacter1 = toUpperCase(*command);
	byte commandNameCharacter2 = toUpperCase(*(command + 1));
	byte commandNameCharacter3 = toUpperCase(*(command + 2));
	
	if (branchStackLevel > branchStackIgnoreLevel - 1)
	{
		byte commandIndex = 3;
		tempCharacter = *(command + commandIndex);
		while (tempCharacter != 10 && tempCharacter != 0)
		{
			commandIndex += 1;
			tempCharacter = *(command + commandIndex);
		}
		commandAddress += commandIndex + 1;
		if (tempCharacter == 0)
		{
			programIsFinished = true;
		}
		if (commandNameCharacter1 == 'F')
		{
			if (commandNameCharacter2 == 'E')
			{
				if (commandNameCharacter3 == 'N')
				{
					branchStackLevel -= 1;
					long tempAddress = *(long *)(branchStack + branchStackLevel * 4);
					if (tempAddress > -1)
					{
						commandAddress = tempAddress;
					}
				}
			}
			if (commandNameCharacter2 == 'I' || commandNameCharacter2 == 'N' || commandNameCharacter2 == 'W')
			{
				if (commandNameCharacter3 == 'B' || commandNameCharacter3 == 'L')
				{
					*(long *)(branchStack + branchStackLevel * 4) = -1;
					branchStackLevel += 1;
				}
			}
			if (commandNameCharacter2 == 'S')
			{
				if (commandNameCharacter3 == 'B')
				{
					*(long *)(branchStack + branchStackLevel * 4) = -1;
					branchStackLevel += 1;
				}
			}
		}
		
	} else {
		long lastCommandAddress = commandAddress;
		byte argumentValueIndex = 0;
		byte argumentLocationIndex = 0;
		byte commandIndex = 3;
		tempCharacter = *(command + commandIndex);
		while (tempCharacter == 32)
		{
			commandIndex += 1;
			byte *tempPointer = command + commandIndex;
			tempCharacter = *tempPointer;
			if (tempCharacter == '"')
			{
				byte *tempPointer2 = argumentValues + argumentValueIndex;
				argumentLocations[argumentLocationIndex] = tempPointer2;
				argumentTypes[argumentLocationIndex] = 1;
				byte tempIndex = 0;
				tempCharacter = *(tempPointer + tempIndex + 1);
				while (tempCharacter != '"')
				{
					*(tempPointer2 + tempIndex) = tempCharacter;
					argumentValueIndex += 1;
					tempIndex += 1;
					tempCharacter = *(tempPointer + tempIndex + 1);
				}
				*(tempPointer2 + tempIndex) = 0;
				argumentValueIndex += 1;
				commandIndex += tempIndex + 2;
			} else if (tempCharacter == '(')
			{
				byte *tempPointer2 = argumentValues + argumentValueIndex;
				argumentLocations[argumentLocationIndex] = tempPointer2;
				argumentTypes[argumentLocationIndex] = 2;
				byte tempIndex = 1;
				while (true)
				{
					*(long *)tempPointer2 = atol((char *)(tempPointer + tempIndex));
					argumentValueIndex += 4;
					tempPointer2 += 4;
					tempIndex += 1;
					byte tempCharacter = *(tempPointer + tempIndex);
					while (tempCharacter != ' ' && tempCharacter != ')')
					{
						tempIndex += 1;
						tempCharacter = *(tempPointer + tempIndex);
					}
					if (tempCharacter == ')')
					{
						break;
					}
				}
				argumentValueIndex += 1;
				commandIndex += tempIndex + 1;
			} else {
				if (tempCharacter == '$')
				{
					byte tempCharacter2 = *(tempPointer + 1);
					if (tempCharacter2 == '$')
					{
						long tempNumber = atol((char *)(tempPointer + 2));
						if (tempNumber > sizeof(programMemory) || tempNumber < 0)
						{
							reportError(ERROR_MESSAGE_4);
							return;
						}
						byte *tempLocation = programMemory + tempNumber;
						tempNumber = *(long *)tempLocation;
						if (tempNumber > sizeof(programMemory) || tempNumber < 0)
						{
							reportError(ERROR_MESSAGE_4);
							return;
						}
						argumentLocations[argumentLocationIndex] = programMemory + tempNumber;
					} else {
						long tempNumber = atol((char *)(tempPointer + 1));
						if (tempNumber > sizeof(programMemory) || tempNumber < 0)
						{
							reportError(ERROR_MESSAGE_4);
							return;
						}
						argumentLocations[argumentLocationIndex] = programMemory + tempNumber;
					}
					argumentTypes[argumentLocationIndex] = 0;
				} else {
					byte *tempPointer2 = argumentValues + argumentValueIndex;
					long tempNumber = atol((char *)tempPointer);
					*((long *)tempPointer2) = tempNumber;
					argumentLocations[argumentLocationIndex] = tempPointer2;
					argumentTypes[argumentLocationIndex] = 2;
					argumentValueIndex += 4;
				}
				commandIndex += findWhiteSpace(tempPointer);
			}
			argumentLocationIndex += 1;
			tempCharacter = *(command + commandIndex);
		}
		commandAddress += commandIndex + 1;
		if (tempCharacter == 0)
		{
			programIsFinished = true;
		}
		
		byte hasRecognizedCommand = false;
		switch (commandNameCharacter1)
		{
			case 'A':
				if (commandNameCharacter2 == 'A')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue1 = getArgumentByte(1);
						byte tempValue2 = getArgumentByte(2);
						setDestinationByte(tempValue1 + tempValue2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue1 = getArgumentFloat(1);
						float tempValue2 = getArgumentFloat(2);
						setDestinationFloat(tempValue1 + tempValue2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue1 = getArgumentLong(1);
						long tempValue2 = getArgumentLong(2);
						setDestinationLong(tempValue1 + tempValue2);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'D')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue1 = getArgumentByte(1);
						byte tempValue2 = getArgumentByte(2);
						setDestinationByte(tempValue1 / tempValue2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue1 = getArgumentFloat(1);
						float tempValue2 = getArgumentFloat(2);
						setDestinationFloat(tempValue1 / tempValue2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue1 = getArgumentLong(1);
						long tempValue2 = getArgumentLong(2);
						setDestinationLong(tempValue1 / tempValue2);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'M')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue1 = getArgumentByte(1);
						byte tempValue2 = getArgumentByte(2);
						setDestinationByte(tempValue1 * tempValue2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue1 = getArgumentFloat(1);
						float tempValue2 = getArgumentFloat(2);
						setDestinationFloat(tempValue1 * tempValue2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue1 = getArgumentLong(1);
						long tempValue2 = getArgumentLong(2);
						setDestinationLong(tempValue1 * tempValue2);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'R')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue1 = getArgumentByte(1);
						byte tempValue2 = getArgumentByte(2);
						setDestinationByte(tempValue1 % tempValue2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue1 = getArgumentLong(1);
						long tempValue2 = getArgumentLong(2);
						setDestinationLong(tempValue1 % tempValue2);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'S')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue1 = getArgumentByte(1);
						byte tempValue2 = getArgumentByte(2);
						setDestinationByte(tempValue1 - tempValue2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue1 = getArgumentFloat(1);
						float tempValue2 = getArgumentFloat(2);
						setDestinationFloat(tempValue1 - tempValue2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue1 = getArgumentLong(1);
						long tempValue2 = getArgumentLong(2);
						setDestinationLong(tempValue1 - tempValue2);
						hasRecognizedCommand = true;
					}
				}
				break;
			case 'B':
				if (commandNameCharacter2 == 'A')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempData1 = getArgumentByte(1);
						byte tempData2 = getArgumentByte(2);
						setDestinationByte(tempData1 & tempData2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempData1 = getArgumentLong(1);
						long tempData2 = getArgumentLong(2);
						setDestinationLong(tempData1 & tempData2);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'I')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempData = getArgumentByte(1);
						setDestinationByte(!tempData);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempData = getArgumentLong(1);
						setDestinationLong(!tempData);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'L')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempData = getArgumentByte(1);
						byte tempAmount = getArgumentByte(2);
						setDestinationByte(tempData << tempAmount);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempData = getArgumentLong(1);
						byte tempAmount = getArgumentByte(2);
						setDestinationLong(tempData << tempAmount);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'N')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempData = getArgumentByte(1);
						setDestinationByte(~tempData);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempData = getArgumentLong(1);
						setDestinationLong(~tempData);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'O')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempData1 = getArgumentByte(1);
						byte tempData2 = getArgumentByte(2);
						setDestinationByte(tempData1 | tempData2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempData1 = getArgumentLong(1);
						long tempData2 = getArgumentLong(2);
						setDestinationLong(tempData1 | tempData2);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'R')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempData = getArgumentByte(1);
						byte tempAmount = getArgumentByte(2);
						setDestinationByte(tempData >> tempAmount);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempData = getArgumentLong(1);
						byte tempAmount = getArgumentByte(2);
						setDestinationLong(tempData >> tempAmount);
						hasRecognizedCommand = true;
					}
				}
				break;
			case 'C':
				if (commandNameCharacter2 == 'E')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempData1 = getArgumentByte(1);
						byte tempData2 = getArgumentByte(2);
						setDestinationByte(tempData1 == tempData2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempData1 = getArgumentLong(1);
						long tempData2 = getArgumentLong(2);
						setDestinationByte(tempData1 == tempData2);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'G')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue1 = getArgumentByte(1);
						byte tempValue2 = getArgumentByte(2);
						setDestinationByte(tempValue1 > tempValue2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue1 = getArgumentFloat(1);
						float tempValue2 = getArgumentFloat(2);
						setDestinationByte(tempValue1 > tempValue2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue1 = getArgumentLong(1);
						long tempValue2 = getArgumentLong(2);
						setDestinationByte(tempValue1 > tempValue2);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'L')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue1 = getArgumentByte(1);
						byte tempValue2 = getArgumentByte(2);
						setDestinationByte(tempValue1 < tempValue2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue1 = getArgumentFloat(1);
						float tempValue2 = getArgumentFloat(2);
						setDestinationByte(tempValue1 < tempValue2);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue1 = getArgumentLong(1);
						long tempValue2 = getArgumentLong(2);
						setDestinationByte(tempValue1 < tempValue2);
						hasRecognizedCommand = true;
					}
				}
				break;
			case 'D':
				if (commandNameCharacter2 == 'B')
				{
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 0)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						drawImage(0, 0, 816, programMemory + pixelBufferStartAddress, 102);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'C')
				{
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 0)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						clearDisplay();
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'I')
				{
					if (commandNameCharacter3 == 'M')
					{
						if (argumentLocationIndex != 5)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempPosX = getArgumentByte(0);
						byte tempPosY = getArgumentByte(1);
						long tempAmount = getArgumentLong(2);
						byte tempWidth = getArgumentByte(4);
						setDisplayPos(tempPosX, tempPosY);
						byte tempOffsetX = 0;
						long index = 0;
						while (index < tempAmount)
						{
							byte tempData = getArgumentListByte(3, index);
							sendPixelsToDisplay(tempData);
							tempOffsetX += 1;
							if (tempOffsetX > tempWidth - 1)
							{
								tempOffsetX = 0;
								tempPosY += 1;
								setDisplayPos(tempPosX, tempPosY);
							}
							index += 1;
						}
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'L')
				{
					if (commandNameCharacter3 == 'N')
					{
						if (argumentLocationIndex != 5)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempPosX1 = getArgumentByte(0);
						byte tempPosY1 = getArgumentByte(1);
						byte tempPosX2 = getArgumentByte(2);
						byte tempPosY2 = getArgumentByte(3);
						byte tempColor = getArgumentByte(4);
						byte tempMagnitude1 = abs((long)tempPosX2 - (long)tempPosX1);
						byte tempMagnitude2 = abs((long)tempPosY2 - (long)tempPosY1);
						if (tempMagnitude1 == 0 && tempMagnitude2 == 0)
						{
							drawPoint(tempPosX1, tempPosY1, tempColor);
						} else if (tempMagnitude1 > tempMagnitude2)
						{
							if (tempPosX1 > tempPosX2)
							{
								byte tempPosX3 = tempPosX1;
								byte tempPosY3 = tempPosY1;
								tempPosX1 = tempPosX2;
								tempPosY1 = tempPosY2;
								tempPosX2 = tempPosX3;
								tempPosY2 = tempPosY3;
							}
							float tempSlope = ((float)tempPosY2 - (float)tempPosY1) / ((float)tempPosX2 - (float)tempPosX1);
							while (tempPosX1 < tempPosX2 + 1)
							{
								tempPosY1 = (byte)(((float)tempPosX1 - (float)tempPosX2) * tempSlope + (float)tempPosY2);
								drawPoint(tempPosX1, tempPosY1, tempColor);
								tempPosX1 += 1;
							}
						} else {
							if (tempPosY1 > tempPosY2)
							{
								byte tempPosX3 = tempPosX1;
								byte tempPosY3 = tempPosY1;
								tempPosX1 = tempPosX2;
								tempPosY1 = tempPosY2;
								tempPosX2 = tempPosX3;
								tempPosY2 = tempPosY3;
							}
							float tempSlope = ((float)tempPosX2 - (float)tempPosX1) / ((float)tempPosY2 - (float)tempPosY1);
							while (tempPosY1 < tempPosY2 + 1)
							{
								tempPosX1 = (byte)(((float)tempPosY1 - (float)tempPosY2) * tempSlope + (float)tempPosX2);
								drawPoint(tempPosX1, tempPosY1, tempColor);
								tempPosY1 += 1;
							}
						}
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'N')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempPosX = getArgumentByte(0);
						byte tempPosY = getArgumentByte(1);
						byte tempValue = getArgumentByte(2);
						drawByteNumber(tempPosX, tempPosY, tempValue);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempPosX = getArgumentByte(0);
						byte tempPosY = getArgumentByte(1);
						float tempValue = getArgumentFloat(2);
						drawFloatNumber(tempPosX, tempPosY, tempValue);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempPosX = getArgumentByte(0);
						byte tempPosY = getArgumentByte(1);
						long tempValue = getArgumentLong(2);
						drawLongNumber(tempPosX, tempPosY, tempValue);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'P')
				{
					if (commandNameCharacter3 == 'T')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempPosX = getArgumentByte(0);
						byte tempPosY = getArgumentByte(1);
						byte tempColor = getArgumentByte(2);
						drawPoint(tempPosX, tempPosY, tempColor);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'X')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempPosX = getArgumentByte(0);
						byte tempPosY = getArgumentByte(1);
						byte tempData = getArgumentByte(2);
						drawPixels(tempPosX, tempPosY, tempData);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'R')
				{
					if (commandNameCharacter3 == 'C')
					{
						if (argumentLocationIndex != 5)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempPosX = getArgumentByte(0);
						byte tempPosY = getArgumentByte(1);
						byte tempWidth = getArgumentByte(2);
						byte tempHeight = getArgumentByte(3);
						byte tempColor = getArgumentByte(4);
						byte tempOffsetX = 0;
						byte tempOffsetY = 0;
						while (tempOffsetY < tempHeight)
						{
							drawPoint(tempPosX + tempOffsetX, tempPosY + tempOffsetY, tempColor);
							tempOffsetX += 1;
							if (tempOffsetX > tempWidth - 1)
							{
								tempOffsetX = 0;
								tempOffsetY += 1;
							}
						}
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'S')
				{
					if (commandNameCharacter3 == 'T')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempPosX = getArgumentByte(0);
						byte tempPosY = getArgumentByte(1);
						byte *tempPointer = argumentLocations[2];
						drawText(tempPosX, tempPosY, tempPointer);
						hasRecognizedCommand = true;
					}
				}
				break;
			case 'F':
				if (commandNameCharacter2 == 'B')
				{
					if (commandNameCharacter3 == 'R')
					{
						if (argumentLocationIndex != 0)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						branchStackIgnoreLevel -= 1;
						while (*(long *)(branchStack + (branchStackIgnoreLevel - 1) * 4) < 0 && branchStackIgnoreLevel > 1)
						{
							branchStackIgnoreLevel -= 1;
						}
						*(long *)(branchStack + (branchStackIgnoreLevel - 1) * 4) = -1;
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'C')
				{
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						branchStackIgnoreLevel += 1;
						*(long *)(branchStack + branchStackLevel * 4) = commandAddress;
						branchStackLevel += 1;
						commandAddress = getArgumentLong(0);
						programIsFinished = false;
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'E')
				{
					if (commandNameCharacter3 == 'N')
					{
						if (argumentLocationIndex != 0)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						if (branchStackLevel == 0)
						{
							reportError(ERROR_MESSAGE_5);
							return;
						}
						branchStackIgnoreLevel -= 1;
						branchStackLevel -= 1;
						long tempAddress = *(long *)(branchStack + branchStackLevel * 4);
						if (tempAddress > -1)
						{
							commandAddress = tempAddress;
							programIsFinished = false;
						}
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'I')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue = getArgumentByte(0);
						if (tempValue != 0)
						{
							branchStackIgnoreLevel += 1;
						}
						*(long *)(branchStack + branchStackLevel * 4) = -1;
						branchStackLevel += 1;
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue = getArgumentLong(0);
						if (tempValue != 0)
						{
							branchStackIgnoreLevel += 1;
						}
						*(long *)(branchStack + branchStackLevel * 4) = -1;
						branchStackLevel += 1;
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'N')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue = getArgumentByte(0);
						if (tempValue == 0)
						{
							branchStackIgnoreLevel += 1;
						}
						*(long *)(branchStack + branchStackLevel * 4) = -1;
						branchStackLevel += 1;
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue = getArgumentLong(0);
						if (tempValue == 0)
						{
							branchStackIgnoreLevel += 1;
						}
						*(long *)(branchStack + branchStackLevel * 4) = -1;
						branchStackLevel += 1;
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'S')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						setDestinationLong(commandAddress);
						*(long *)(branchStack + branchStackLevel * 4) = -1;
						branchStackLevel += 1;
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'W')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue = getArgumentByte(0);
						if (tempValue != 0)
						{
							branchStackIgnoreLevel += 1;
							*(long *)(branchStack + branchStackLevel * 4) = lastCommandAddress;
						} else {
							*(long *)(branchStack + branchStackLevel * 4) = -1;
						}
						branchStackLevel += 1;
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue = getArgumentLong(0);
						if (tempValue != 0)
						{
							branchStackIgnoreLevel += 1;
							*(long *)(branchStack + branchStackLevel * 4) = lastCommandAddress;
						} else {
							*(long *)(branchStack + branchStackLevel * 4) = -1;
						}
						branchStackLevel += 1;
						hasRecognizedCommand = true;
					}
				}
				break;
			case 'K':
				if (commandNameCharacter2 == 'I')
				{
					if (commandNameCharacter3 == 'P')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempKey = getArgumentByte(1);
						byte tempResult = keyIsPressed(tempKey);
						setDestinationByte(tempResult);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'R')
				{
					if (commandNameCharacter3 == 'D')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempKey = readKeys();
						setDestinationByte(tempKey);
						hasRecognizedCommand = true;
					}
				}
				break;
			case 'L':
				if (commandNameCharacter2 == 'E')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 4)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempLength = getArgumentLong(1);
						byte tempResult = true;
						long index = 0;
						while (index < tempLength)
						{
							byte tempValue1 = getArgumentListByte(2, index);
							byte tempValue2 = getArgumentListByte(3, index);
							if (tempValue1 != tempValue2)
							{
								tempResult = false;
								break;
							}
							index += 1;
						}
						setDestinationByte(tempResult);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 4)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempLength = getArgumentLong(1);
						byte tempResult = true;
						long index = 0;
						while (index < tempLength)
						{
							long tempValue1 = getArgumentListLong(2, index);
							long tempValue2 = getArgumentListLong(3, index);
							if (tempValue1 != tempValue2)
							{
								tempResult = false;
								break;
							}
							index += 1;
						}
						setDestinationByte(tempResult);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'F')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 4)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue = getArgumentByte(1);
						long tempLength = getArgumentLong(2);
						byte valueHasBeenFound = false;
						long index = 0;
						while (index < tempLength)
						{
							byte tempValue2 = getArgumentListByte(3, index);
							if (tempValue == tempValue2)
							{
								valueHasBeenFound = true;
								break;
							}
							index += 1;
						}
						if (!valueHasBeenFound)
						{
							index = -1;
						}
						setDestinationLong(index);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 4)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue = getArgumentLong(1);
						long tempLength = getArgumentLong(2);
						byte valueHasBeenFound = false;
						long index = 0;
						while (index < tempLength)
						{
							long tempValue2 = getArgumentListLong(3, index);
							if (tempValue == tempValue2)
							{
								valueHasBeenFound = true;
								break;
							}
							index += 1;
						}
						if (!valueHasBeenFound)
						{
							index = -1;
						}
						setDestinationLong(index);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'S')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempLength = getArgumentLong(1);
						byte tempValue = getArgumentByte(2);
						long index = 0;
						while (index < tempLength)
						{
							setDestinationListByte(index, tempValue);
							index += 1;
						}
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempLength = getArgumentLong(1);
						float tempValue = getArgumentFloat(2);
						long index = 0;
						while (index < tempLength)
						{
							setDestinationListFloat(index, tempValue);
							index += 1;
						}
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempLength = getArgumentLong(1);
						long tempValue = getArgumentLong(2);
						long index = 0;
						while (index < tempLength)
						{
							setDestinationListLong(index, tempValue);
							index += 1;
						}
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'W')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempLength = getArgumentLong(1);
						long index = 0;
						while (index < tempLength)
						{
							byte tempValue = getArgumentListByte(2, index);
							setDestinationListByte(index, tempValue);
							index += 1;
						}
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempLength = getArgumentLong(1);
						long index = 0;
						while (index < tempLength)
						{
							long tempValue = getArgumentListLong(2, index);
							setDestinationListLong(index, tempValue);
							index += 1;
						}
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempLength = getArgumentLong(1);
						long index = 0;
						while (index < tempLength)
						{
							float tempValue = getArgumentListFloat(2, index);
							setDestinationListFloat(index, tempValue);
							index += 1;
						}
						hasRecognizedCommand = true;
					}
				}
				break;
			case 'M':
				if (commandNameCharacter2 == 'A')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue = getArgumentFloat(1);
						setDestinationFloat(fabs(tempValue));
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'C')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue = getArgumentFloat(1);
						setDestinationFloat(acos(tempValue));
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'S')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue = getArgumentFloat(1);
						setDestinationFloat(asin(tempValue));
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'T')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue = getArgumentFloat(1);
						setDestinationFloat(atan(tempValue));
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == '2')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempPosX = getArgumentFloat(1);
						float tempPosY = getArgumentFloat(2);
						setDestinationFloat(atan2(tempPosY, tempPosX));
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'C')
				{
					if (commandNameCharacter3 == 'S')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue = getArgumentFloat(1);
						setDestinationFloat(cos(tempValue));
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'D')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue = getArgumentByte(0);
						setDestinationByte(tempValue - 1);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue = getArgumentLong(0);
						setDestinationLong(tempValue - 1);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'I')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue = getArgumentByte(0);
						setDestinationByte(tempValue + 1);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue = getArgumentLong(0);
						setDestinationLong(tempValue + 1);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'L')
				{
					if (commandNameCharacter3 == 'N')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue = getArgumentFloat(1);
						setDestinationFloat(log(tempValue));
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'G')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue = getArgumentFloat(1);
						setDestinationFloat(log10(tempValue));
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'P')
				{
					if (commandNameCharacter3 == 'W')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempBase = getArgumentFloat(1);
						float tempExponent = getArgumentFloat(2);
						setDestinationFloat(pow(tempBase, tempExponent));
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'R')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue = getArgumentByte(1);
						setDestinationByte(rand() % tempValue);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue = getArgumentLong(1);
						long tempNumber = ((long)(rand()) << 16) | rand();
						setDestinationLong(tempNumber % tempValue);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'S')
				{
					if (commandNameCharacter3 == 'N')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue = getArgumentFloat(1);
						setDestinationFloat(sin(tempValue));
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'R')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue = getArgumentFloat(1);
						setDestinationFloat(sqrt(tempValue));
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'T')
				{
					if (commandNameCharacter3 == 'N')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempValue = getArgumentFloat(1);
						setDestinationFloat(tan(tempValue));
						hasRecognizedCommand = true;
					}
				}
				break;
			case 'N':
				if (commandNameCharacter2 == 'B')
				{
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempData = getArgumentByte(1);
						setDestinationLong((long)tempData);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempData = getArgumentByte(1);
						setDestinationFloat((float)tempData);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'S')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempData = getArgumentByte(1);
						byte *tempPointer = argumentLocations[0];
						itoa(tempData, (char *)tempPointer, 10);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'F')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempData = getArgumentFloat(1);
						setDestinationByte((byte)tempData);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempData = getArgumentFloat(1);
						setDestinationLong((long)tempData);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'S')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempData = getArgumentFloat(1);
						byte *tempPointer = argumentLocations[0];
						convertFloatToText(tempPointer, tempData);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'L')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempData = getArgumentLong(1);
						setDestinationByte((byte)tempData);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempData = getArgumentLong(1);
						setDestinationFloat((float)tempData);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'S')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempData = getArgumentLong(1);
						byte *tempPointer = argumentLocations[0];
						ltoa(tempData, (char *)tempPointer, 10);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'W')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempData = getArgumentByte(1);
						setDestinationByte(tempData);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						float tempData = getArgumentFloat(1);
						setDestinationFloat(tempData);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempData = getArgumentLong(1);
						setDestinationLong(tempData);
						hasRecognizedCommand = true;
					}
				}
				break;
			case 'P':
				if (commandNameCharacter2 == 'F')
				{
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						setDestinationByte(promptFile());
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'K')
				{
					if (commandNameCharacter3 == 'Y')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						setDestinationByte(promptKey());
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'N')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						setDestinationByte(promptByteNumber());
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						setDestinationFloat(promptFloatNumber());
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						setDestinationLong(promptLongNumber());
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'S')
				{
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempAmount = getArgumentByte(1);
						byte *tempSource = argumentLocations[2];
						setDestinationByte(promptSelection(tempAmount, tempSource));
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'T')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempDestination = argumentLocations[0];
						promptText(tempDestination);
						hasRecognizedCommand = true;
					}
				}
				break;
			case 'R':
				if (commandNameCharacter2 == 'C')
				{
					if (commandNameCharacter3 == 'R')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempSource = argumentLocations[1];
						if (getTextLength(tempSource) > 15)
						{
							reportError(ERROR_MESSAGE_6);
							return;
						}
						long tempSize = getArgumentLong(2);
						if (tempSize > (long)30000 || tempSize < 1)
						{
							reportError(ERROR_MESSAGE_7);
							return;
						}
						byte tempHandle = createFile(tempSource, tempSize);
						setDestinationByte(tempHandle);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'D')
				{
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempHandle = getArgumentByte(0);
						long tempSize = getFileSize(tempHandle);
						if (tempSize == 0 || tempSize == 0xFFFFFFFF)
						{
							reportError(ERROR_MESSAGE_9);
							return;
						}
						deleteFile(tempHandle);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'G')
				{
					if (commandNameCharacter3 == 'N')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempHandle = getArgumentByte(1);
						byte *tempDestination = argumentLocations[0];
						getFileName(tempDestination, tempHandle);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'S')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempHandle = getArgumentByte(1);
						long tempSize = getFileSize(tempHandle);
						setDestinationLong(tempSize);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'N')
				{
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempAmount = 0;
						byte tempHandle = 0;
						while (tempHandle < 170)
						{
							long tempSize = getFileSize(tempHandle);
							if (tempSize == 0xFFFFFFFF)
							{
								break;
							}
							if (tempSize > 0)
							{
								tempAmount += 1;
							}
							tempHandle += 1;
						}
						setDestinationByte(tempAmount);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'O')
				{
					if (commandNameCharacter3 == 'I')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempEndIndex = getArgumentByte(1);
						byte index = 0;
						byte tempHandle = 0;
						while (tempHandle < 170)
						{
							long tempSize = getFileSize(tempHandle);
							if (tempSize == 0xFFFFFFFF)
							{
								setDestinationByte(255);
								break;
							}
							if (tempSize > 0)
							{
								if (index == tempEndIndex)
								{
									setDestinationByte(tempHandle);
									break;
								}
								index += 1;
							}
							tempHandle += 1;
						}
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'N')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempSource = argumentLocations[1];
						byte tempHandle = 0;
						while (tempHandle < 170)
						{
							long tempSize = getFileSize(tempHandle);
							if (tempSize == 0xFFFFFFFF)
							{
								setDestinationByte(255);
								break;
							}
							if (tempSize > 0)
							{
								byte tempBuffer[16];
								getFileName(tempBuffer, tempHandle);
								if (equalText(tempBuffer, tempSource))
								{
									setDestinationByte(tempHandle);
									break;
								}
							}
							tempHandle += 1;
						}
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'R')
				{
					if (commandNameCharacter3 == 'D')
					{
						if (argumentLocationIndex != 4)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempAmount = getArgumentLong(1);
						byte tempHandle = getArgumentByte(2);
						long index = getArgumentLong(3);
						long tempAddress = getFileAddress(tempHandle) + index;
						byte *tempDestination = argumentLocations[0];
						readSequenceFromEeprom(tempAmount, tempDestination, tempAddress);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'N')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempHandle = getArgumentByte(0);
						long tempSize = getFileSize(tempHandle);
						if (tempSize == 0 || tempSize == 0xFFFFFFFF)
						{
							reportError(ERROR_MESSAGE_9);
							return;
						}
						nextFileHandleToRun = tempHandle;
						programIsFinished = true;
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'S')
				{
					if (commandNameCharacter3 == 'N')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempHandle = getArgumentByte(0);
						byte *tempSource = argumentLocations[1];
						if (getTextLength(tempSource) > 15)
						{
							reportError(ERROR_MESSAGE_6);
							return;
						}
						setFileName(tempHandle, tempSource);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'S')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempHandle = getArgumentByte(0);
						long tempLastSize = getFileSize(tempHandle);
						if (tempLastSize == 0 || tempLastSize == 0xFFFFFFFF)
						{
							reportError(ERROR_MESSAGE_9);
							return;
						}
						long tempSize = getArgumentLong(1);
						if (tempSize > (long)30000 || tempSize < 1)
						{
							reportError(ERROR_MESSAGE_7);
							return;
						}
						resizeFile(tempHandle, tempSize);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'W')
				{
					if (commandNameCharacter3 == 'R')
					{
						if (argumentLocationIndex != 4)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempHandle = getArgumentByte(0);
						long tempSize = getFileSize(tempHandle);
						if (tempSize == 0 || tempSize == 0xFFFFFFFF)
						{
							reportError(ERROR_MESSAGE_9);
							return;
						}
						long index = getArgumentLong(1);
						long tempAmount = getArgumentLong(2);
						if (index < 0 || index + tempAmount > tempSize)
						{
							reportError(ERROR_MESSAGE_8);
							return;
						}
						long tempAddress = getFileAddress(tempHandle) + index;
						byte *tempSource = argumentLocations[3];
						writeSequenceToEeprom(tempAddress, tempAmount, tempSource);
						hasRecognizedCommand = true;
					}
				}
				break;
			case 'S':
				if (commandNameCharacter2 == 'C')
				{
					if (commandNameCharacter3 == 'N')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempSource = argumentLocations[1];
						byte *tempDestination = argumentLocations[0];
						long tempLength = getTextLength(tempDestination);
						copyText(tempDestination + tempLength, tempSource);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'D')
				{
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempPointer = argumentLocations[1];
						byte index = 0;
						byte tempValue = *tempPointer;
						while (tempValue != 0)
						{
							if (tempValue == 32)
							{
								index += 1;
							}
							tempPointer += 1;
							tempValue = *tempPointer;
						}
						setDestinationByte(index + 1);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'S')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempSource = argumentLocations[1];
						byte tempAmount = getArgumentByte(2);
						byte *tempDestination = argumentLocations[0];
						byte index = 0;
						byte tempValue = *tempSource;
						while (tempValue != 0 && index < tempAmount)
						{
							if (tempValue == 32)
							{
								index += 1;
							}
							tempSource += 1;
							tempValue = *tempSource;
						}
						byte tempLength = findWhiteSpace(tempSource);
						copyData(tempLength, tempDestination, tempSource);
						*(tempDestination + tempLength) = 0;
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'E')
				{
					if (commandNameCharacter3 == 'Q')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempPointer1 = argumentLocations[1];
						byte *tempPointer2 = argumentLocations[2];
						byte tempResult = equalText(tempPointer1, tempPointer2);
						setDestinationByte(tempResult);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'F')
				{
					if (commandNameCharacter3 == 'N')
					{
						if (argumentLocationIndex != 3)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempPointer1 = argumentLocations[1];
						long tempLength1 = getTextLength(tempPointer1);
						byte *tempPointer2 = argumentLocations[2];
						long tempLength2 = getTextLength(tempPointer2);
						byte hasFoundPattern = false;
						long index = 0;
						while (index < (tempLength2 - tempLength1) + 1)
						{
							byte tempResult = equalData(tempLength1, tempPointer1, tempPointer2);
							if (tempResult)
							{
								hasFoundPattern = true;
								break;
							}
							tempPointer2 += 1;
							index += 1;
						}
						if (!hasFoundPattern)
						{
							index = -1;
						}
						setDestinationLong(index);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'L')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempPointer = argumentLocations[1];
						byte tempResult = getTextLength(tempPointer);
						setDestinationByte(tempResult);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempPointer = argumentLocations[1];
						long tempResult = getTextLength(tempPointer);
						setDestinationLong(tempResult);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'N')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempPointer = argumentLocations[1];
						byte tempResult = atoi((char *)tempPointer);
						setDestinationByte(tempResult);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'F')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempPointer = argumentLocations[1];
						float tempResult = atof((char *)tempPointer);
						setDestinationFloat(tempResult);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempPointer = argumentLocations[1];
						long tempResult = atol((char *)tempPointer);
						setDestinationLong(tempResult);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'S')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 4)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempSource = argumentLocations[1];
						byte index1 = getArgumentByte(2);
						byte index2 = getArgumentByte(3);
						byte *tempDestination = argumentLocations[0];
						byte tempLength = index2 - index1;
						copyData(tempLength, tempDestination, tempSource + index1);
						*(tempDestination + tempLength) = 0;
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 4)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempSource = argumentLocations[1];
						long index1 = getArgumentLong(2);
						long index2 = getArgumentLong(3);
						byte *tempDestination = argumentLocations[0];
						long tempLength = index2 - index1;
						copyData(tempLength, tempDestination, tempSource + index1);
						*(tempDestination + tempLength) = 0;
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'W')
				{
					if (commandNameCharacter3 == 'R')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte *tempSource = argumentLocations[1];
						byte *tempDestination = argumentLocations[0];
						copyText(tempDestination, tempSource);
						hasRecognizedCommand = true;
					}
				}
				break;
			case 'T':
				if (commandNameCharacter2 == 'R')
				{
					if (commandNameCharacter3 == 'D')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						setDestinationLong(time);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'S')
				{
					if (commandNameCharacter3 == 'B')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempValue = getArgumentByte(0);
						delayMilliseconds(tempValue);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'L')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue = getArgumentLong(0);
						delayMilliseconds(tempValue);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'W')
				{
					if (commandNameCharacter3 == 'R')
					{
						if (argumentLocationIndex != 1)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						long tempValue = getArgumentLong(0);
						while (time != tempValue)
						{
							time = tempValue;
						}
						hasRecognizedCommand = true;
					}
				}
				break;
			case 'X':
				if (commandNameCharacter2 == 'A')
				{
					if (commandNameCharacter3 == 'R')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempPin = getArgumentByte(1);
						long tempValue = analogReadPin(tempPin);
						setDestinationLong(tempValue);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'D')
				{
					if (commandNameCharacter3 == 'R')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempPin = getArgumentByte(1);
						byte tempValue = digitalReadPin(tempPin);
						setDestinationByte(tempValue);
						hasRecognizedCommand = true;
					}
					if (commandNameCharacter3 == 'W')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempPin = getArgumentByte(0);
						byte tempValue = getArgumentByte(1);
						digitalWritePin(tempPin, tempValue);
						hasRecognizedCommand = true;
					}
				}
				if (commandNameCharacter2 == 'S')
				{
					if (commandNameCharacter3 == 'M')
					{
						if (argumentLocationIndex != 2)
						{
							reportError(ERROR_MESSAGE_3);
							return;
						}
						byte tempPin = getArgumentByte(0);
						byte tempMode = getArgumentByte(1);
						setPinMode(tempPin, tempMode);
						hasRecognizedCommand = true;
					}
				}
				break;
			default:
				break;
		}
		if (!hasRecognizedCommand)
		{
			reportError(ERROR_MESSAGE_2);
			return;
		}
	}
	checkEscapeKeyDelay += 1;
	if (checkEscapeKeyDelay > 25)
	{
		checkEscapeKey();
		checkEscapeKeyDelay = 0;
	}
	if (escapeKeyHasBeenPressed)
	{
		programIsFinished = true;
		while (readKeys() != 0)
		{
			
		}
	}
}

void executeFile(byte handle)
{
	nextFileHandleToRun = handle;
	while (nextFileHandleToRun != 255)
	{
		commandAddress = getFileAddress(nextFileHandleToRun);
		nextFileHandleToRun = 255;
		branchStackLevel = 0;
		branchStackIgnoreLevel = 1;
		programIsFinished = false;
		programHasError = false;
		escapeKeyHasBeenPressed = false;
		while (!programIsFinished)
		{
			executeCommand();
		}
	}
	if (!programHasError)
	{
		drawTextFromProgMem(78, 7, MESSAGE_2);
	}
	promptKey();
}

long loadFileLineAddresses(byte handle)
{
	long tempAddress = getFileAddress(handle);
	long addressListIndex = 0;
	*(long *)(programMemory + (addressListIndex * 4)) = tempAddress;
	addressListIndex += 1;
	while (true)
	{
		byte tempBuffer[200];
		readLineFromEeprom(tempBuffer, tempAddress);
		byte hasFoundEndOfFile = false;
		byte index = 0;
		while (index < 200)
		{
			byte tempValue = *(tempBuffer + index);
			if (tempValue == 10)
			{
				break;
			}
			if (tempValue == 0)
			{
				hasFoundEndOfFile = true;
				break;
			}
			tempAddress += 1;
			index += 1;
		}
		tempAddress += 1;
		*(long *)(programMemory + (addressListIndex * 4)) = tempAddress;
		addressListIndex += 1;
		if (hasFoundEndOfFile)
		{
			break;
		}
	}
	return addressListIndex;
}

void drawEditCursor(long index, long pageIndex)
{
	drawCharacter(0, index - pageIndex, '>');
}

void eraseEditCursor(long index, long pageIndex)
{
	drawCharacter(0, index - pageIndex, ' ');
}

void drawEditLines(long index, long pageIndex, long addressListSize)
{
	clearDisplay();
	byte tempOffset = 0;
	while (tempOffset < 8 && pageIndex + tempOffset < addressListSize - 1)
	{
		byte tempBuffer[200];
		long tempAddress = *(long *)(programMemory + (pageIndex + tempOffset) * 4);
		readLineFromEeprom(tempBuffer, tempAddress);
		byte tempIndex = getLineLength(tempBuffer);
		if (tempIndex > 16)
		{
			tempIndex = 16;
		}
		tempBuffer[tempIndex] = 0;
		drawText(6, tempOffset, tempBuffer);
		tempOffset += 1;
	}
	drawEditCursor(index, pageIndex);
}

void moveEditLinesLeft(long index, long offset, long addressListSize)
{
	while (index < addressListSize - 1)
	{
		byte tempBuffer[200];
		long tempAddress = *(long *)(programMemory + (index * 4));
		readLineFromEeprom(tempBuffer, tempAddress);
		tempAddress += offset;
		writeLineToEeprom(tempAddress, tempBuffer);
		*(long *)(programMemory + (index * 4)) = tempAddress;
		index += 1;
	}
}

void moveEditLinesRight(long index, long offset, long addressListSize)
{
	long tempIndex = addressListSize - 1;
	while (tempIndex > index - 1)
	{
		byte tempBuffer[200];
		long tempAddress = *(long *)(programMemory + (tempIndex * 4));
		readLineFromEeprom(tempBuffer, tempAddress);
		tempAddress += offset;
		writeLineToEeprom(tempAddress, tempBuffer);
		*(long *)(programMemory + (tempIndex * 4)) = tempAddress;
		tempIndex -= 1;
	}
}

void moveEditLines(long index, long offset, long addressListSize)
{
	if (offset == 0)
	{
		return;
	}
	if (offset > 0)
	{
		moveEditLinesRight(index, offset, addressListSize);
	} else {
		moveEditLinesLeft(index, offset, addressListSize);
	}
}

void editFile(byte handle)
{
	byte clipboard[200];
	clipboard[0] = 0;
	long addressListSize = loadFileLineAddresses(handle);
	long index = 0;
	long pageIndex = 0;
	drawEditLines(index, pageIndex, addressListSize);
	while (true)
	{
		long lastIndex = index;
		escapeKeyHasBeenPressed = false;
		byte tempKey = promptKey();
		if (escapeKeyHasBeenPressed)
		{
			return;
		}
		if (tempKey == 19)
		{
			index -= 1;
		}
		if (tempKey == 18)
		{
			index += 1;
		}
		if (tempKey == 'b')
		{
			index -= 8;
		}
		if (tempKey == 'n')
		{
			index += 8;
		}
		if (index == addressListSize - 1)
		{
			if (tempKey == 10 || tempKey == 'i' || tempKey == 'v')
			{
				byte tempBuffer[200];
				long tempAddress = *(long *)(programMemory + (index * 4));
				if (tempKey == 'v')
				{
					copyText(tempBuffer, clipboard);
				} else {
					tempBuffer[0] = 0;
					promptText(tempBuffer);
				}
				if (!escapeKeyHasBeenPressed)
				{
					byte tempValue = 10;
					writeSequenceToEeprom(tempAddress - 1, 1, &tempValue);
					writeLineToEeprom(tempAddress, tempBuffer);
					byte tempLength = getTextLength(tempBuffer);
					*(long *)(programMemory + ((index + 1) * 4)) = tempAddress + tempLength + 1;
					addressListSize += 1;
					index += 1;
					pageIndex = (index / 8) * 8;
					lastIndex = index;
				}
				drawEditLines(index, pageIndex, addressListSize);
			}
		} else {
			if (tempKey == 10)
			{
				byte tempBuffer[200];
				long tempAddress = *(long *)(programMemory + (index * 4));
				readLineFromEeprom(tempBuffer, tempAddress);
				byte tempIndex = getLineLength(tempBuffer);
				tempBuffer[tempIndex] = 0;
				promptText(tempBuffer);
				if (!escapeKeyHasBeenPressed)
				{
					long tempAddress2 = *(long *)(programMemory + ((index + 1) * 4));
					long tempLength1 = (tempAddress2 - tempAddress) - 1;
					long tempLength2 = getTextLength(tempBuffer);
					long tempOffset = tempLength2 - tempLength1;
					moveEditLines(index + 1, tempOffset, addressListSize);
					if (index < addressListSize - 2)
					{
						byte tempIndex = getTextLength(tempBuffer);
						tempBuffer[tempIndex] = 10;
					}
					writeLineToEeprom(tempAddress, tempBuffer);
				}
				drawEditLines(index, pageIndex, addressListSize);
			}
			if (tempKey == 'i' || tempKey == 'v')
			{
				byte tempBuffer[200];
				long tempAddress = *(long *)(programMemory + (index * 4));
				if (tempKey == 'v')
				{
					copyText(tempBuffer, clipboard);
				} else {
					tempBuffer[0] = 0;
					promptText(tempBuffer);
				}
				if (!escapeKeyHasBeenPressed)
				{
					long tempOffset = getTextLength(tempBuffer) + 1;
					moveEditLines(index, tempOffset, addressListSize);
					byte tempIndex2 = getTextLength(tempBuffer);
					tempBuffer[tempIndex2] = 10;
					long tempIndex = addressListSize - 1;
					while (tempIndex > index - 1)
					{
						long tempAddress2 = *(long *)(programMemory + (tempIndex * 4));
						*(long *)(programMemory + ((tempIndex + 1) * 4)) = tempAddress2;
						tempIndex -= 1;
					}
					addressListSize += 1;
					*(long *)(programMemory + (index * 4)) = tempAddress;
					writeLineToEeprom(tempAddress, tempBuffer);
					index += 1;
					pageIndex = (index / 8) * 8;
					lastIndex = index;
				}
				drawEditLines(index, pageIndex, addressListSize);
			}
			if (tempKey == 'c' || tempKey == 'x')
			{
				long tempAddress = *(long *)(programMemory + (index * 4));
				readLineFromEeprom(clipboard, tempAddress);
				byte tempIndex = getLineLength(clipboard);
				clipboard[tempIndex] = 0;
			}
			if (tempKey == 8 || tempKey == 'x')
			{
				long tempAddress1 = *(long *)(programMemory + (index * 4));
				if (index < addressListSize - 2)
				{
					long tempAddress2 = *(long *)(programMemory + ((index + 1) * 4));
					long tempOffset = tempAddress1 - tempAddress2;
					moveEditLines(index + 1, tempOffset, addressListSize);
					long tempIndex = index + 1;
					while (tempIndex < addressListSize - 1)
					{
						long tempAddress = *(long *)(programMemory + ((tempIndex + 1) * 4));
						*(long *)(programMemory + (tempIndex * 4)) = tempAddress;
						tempIndex += 1;
					}
				} else {
					byte tempValue = 0;
					if (index == 0)
					{
						writeSequenceToEeprom(tempAddress1, 1, &tempValue);
					} else {
						writeSequenceToEeprom(tempAddress1 - 1, 1, &tempValue);
					}
				}
				addressListSize -= 1;
				drawEditLines(index, pageIndex, addressListSize);
			}
		}
		if (lastIndex != index)
		{
			if (index < 0)
			{
				index = addressListSize - 1;
			}
			if (index > addressListSize - 1)
			{
				index = 0;
			}
			if (index < pageIndex || index > pageIndex + 7)
			{
				pageIndex = (index / 8) * 8;
				drawEditLines(index, pageIndex, addressListSize);
			} else {
				eraseEditCursor(lastIndex, pageIndex);
				drawEditCursor(index, pageIndex);
			}
		}
	}
}

void promptFileAndAction()
{
	while (true)
	{
		byte tempHandle = promptFile();
		if (escapeKeyHasBeenPressed)
		{
			break;
		}
		while (true)
		{
			byte tempResult = promptSelectionFromProgMem(6, sizeof(MENU_2), MENU_2);
			if (escapeKeyHasBeenPressed)
			{
				break;
			}
			if (tempResult == 0)
			{
				executeFile(tempHandle);
			}
			if (tempResult == 1)
			{
				editFile(tempHandle);
			}
			if (tempResult == 2)
			{
				clearDisplay();
				drawTextFromProgMem(0, 0, MESSAGE_6);
				byte tempBuffer[17];
				getFileName(tempBuffer, tempHandle);
				drawText(0, 1, tempBuffer);
				drawTextFromProgMem(0, 2, MESSAGE_7);
				drawByteNumber(0, 3, tempHandle);
				drawTextFromProgMem(0, 4, MESSAGE_8);
				long tempAddress = getFileAddress(tempHandle);
				drawLongNumber(0, 5, tempAddress);
				drawTextFromProgMem(0, 6, MESSAGE_9);
				long tempSize = getFileSize(tempHandle);
				drawLongNumber(0, 7, tempSize);
				promptKey();
			}
			if (tempResult == 3)
			{
				clearDisplay();
				drawTextFromProgMem(0, 0, MESSAGE_4);
				promptKey();
				byte tempBuffer[200];
				tempBuffer[0] = 0;
				promptText(tempBuffer);
				if (!escapeKeyHasBeenPressed)
				{
					if (getTextLength(tempBuffer) > 15)
					{
						clearDisplay();
						drawTextFromProgMem(0, 0, ERROR_MESSAGE_6);
						promptKey();
					} else {
						setFileName(tempHandle, tempBuffer);
						clearDisplay();
						drawTextFromProgMem(0, 0, MESSAGE_14);
						promptKey();
					}
				}
			}
			if (tempResult == 4)
			{
				clearDisplay();
				drawTextFromProgMem(0, 0, MESSAGE_5);
				promptKey();
				long tempSize = promptLongNumber();
				if (!escapeKeyHasBeenPressed)
				{
					if (tempSize > (long)30000 || tempSize < 1)
					{
						clearDisplay();
						drawTextFromProgMem(0, 0, ERROR_MESSAGE_7);
						promptKey();
					} else {
						resizeFile(tempHandle, tempSize);
						clearDisplay();
						drawTextFromProgMem(0, 0, MESSAGE_15);
						promptKey();
					}
				}
			}
			if (tempResult == 5)
			{
				clearDisplay();
				drawTextFromProgMem(0, 0, MESSAGE_10);
				byte tempBuffer[17];
				getFileName(tempBuffer, tempHandle);
				drawText(0, 4, tempBuffer);
				promptKey();
				byte tempResult = promptSelectionFromProgMem(2, sizeof(MENU_3), MENU_3);
				if (tempResult == 1 && !escapeKeyHasBeenPressed)
				{
					deleteFile(tempHandle);
					clearDisplay();
					drawTextFromProgMem(0, 0, MESSAGE_13);
					promptKey();
					break;
				}
			}
		}
	}
}

byte promptFileCreation()
{
	clearDisplay();
	drawTextFromProgMem(0, 0, MESSAGE_4);
	promptKey();
	byte tempBuffer[200];
	tempBuffer[0] = 0;
	promptText(tempBuffer);
	if (escapeKeyHasBeenPressed)
	{
		return 255;
	}
	if (getTextLength(tempBuffer) > 15)
	{
		clearDisplay();
		drawTextFromProgMem(0, 0, ERROR_MESSAGE_6);
		promptKey();
		return 255;
	}
	clearDisplay();
	drawTextFromProgMem(0, 0, MESSAGE_5);
	promptKey();
	long tempSize = promptLongNumber();
	if (escapeKeyHasBeenPressed)
	{
		return 255;
	}
	if (tempSize > (long)30000 || tempSize < 1)
	{
		clearDisplay();
		drawTextFromProgMem(0, 0, ERROR_MESSAGE_7);
		promptKey();
		return 255;
	}
	byte tempHandle = createFile(tempBuffer, tempSize);
	clearDisplay();
	drawTextFromProgMem(0, 0, MESSAGE_12);
	promptKey();
	return tempHandle;
}

void displayHelpFromProgMem(byte amount, const byte * const helpMessageList[])
{
	escapeKeyHasBeenPressed = false;
	byte index = 0;
	while (true)
	{
		const byte *tempPointer = (const byte *)pgm_read_word(&(helpMessageList[index]));
		clearDisplay();
		drawTextFromProgMem(0, 0, tempPointer);
		while (true)
		{
			byte tempKey = promptKey();
			if (escapeKeyHasBeenPressed)
			{
				return;
			}
			if (tempKey == 19 || tempKey == 'b')
			{
				if (index == 0)
				{
					index = amount - 1;
				} else {
					index -= 1;
				}
				break;
			}
			if (tempKey == 18 || tempKey == 'n')
			{
				if (index > amount - 2)
				{
					index = 0;
				} else {
					index += 1;
				}
				break;
			}
		}
	}
}

void promptHelp()
{
	while (true)
	{
		byte tempResult = promptSelectionFromProgMem(16, sizeof(MENU_4), MENU_4);
		if (escapeKeyHasBeenPressed)
		{
			break;
		}
		if (tempResult == 0)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_16) / sizeof(HELP_MESSAGE_LIST_16[0]), HELP_MESSAGE_LIST_16);
		}
		if (tempResult == 1)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_1) / sizeof(HELP_MESSAGE_LIST_1[0]), HELP_MESSAGE_LIST_1);
		}
		if (tempResult == 2)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_2) / sizeof(HELP_MESSAGE_LIST_2[0]), HELP_MESSAGE_LIST_2);
		}
		if (tempResult == 3)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_3) / sizeof(HELP_MESSAGE_LIST_3[0]), HELP_MESSAGE_LIST_3);
		}
		if (tempResult == 4)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_4) / sizeof(HELP_MESSAGE_LIST_4[0]), HELP_MESSAGE_LIST_4);
		}
		if (tempResult == 5)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_5) / sizeof(HELP_MESSAGE_LIST_5[0]), HELP_MESSAGE_LIST_5);
		}
		if (tempResult == 6)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_6) / sizeof(HELP_MESSAGE_LIST_6[0]), HELP_MESSAGE_LIST_6);
		}
		if (tempResult == 7)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_7) / sizeof(HELP_MESSAGE_LIST_7[0]), HELP_MESSAGE_LIST_7);
		}
		if (tempResult == 8)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_8) / sizeof(HELP_MESSAGE_LIST_8[0]), HELP_MESSAGE_LIST_8);
		}
		if (tempResult == 9)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_9) / sizeof(HELP_MESSAGE_LIST_9[0]), HELP_MESSAGE_LIST_9);
		}
		if (tempResult == 10)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_10) / sizeof(HELP_MESSAGE_LIST_10[0]), HELP_MESSAGE_LIST_10);
		}
		if (tempResult == 11)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_11) / sizeof(HELP_MESSAGE_LIST_11[0]), HELP_MESSAGE_LIST_11);
		}
		if (tempResult == 12)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_12) / sizeof(HELP_MESSAGE_LIST_12[0]), HELP_MESSAGE_LIST_12);
		}
		if (tempResult == 13)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_13) / sizeof(HELP_MESSAGE_LIST_13[0]), HELP_MESSAGE_LIST_13);
		}
		if (tempResult == 14)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_14) / sizeof(HELP_MESSAGE_LIST_14[0]), HELP_MESSAGE_LIST_14);
		}
		if (tempResult == 15)
		{
			displayHelpFromProgMem(sizeof(HELP_MESSAGE_LIST_15) / sizeof(HELP_MESSAGE_LIST_15[0]), HELP_MESSAGE_LIST_15);
		}
	}
}

void disengageEeprom()
{
	EEPROM_DATA_PIN_INPUT;
	EEPROM_CLOCK_PIN_INPUT;
}

void engageEeprom()
{
	EEPROM_DATA_PIN_OUTPUT;
	EEPROM_CLOCK_PIN_OUTPUT;
	EEPROM_DATA_PIN_HIGH;
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
	sendStopConditionToEeprom();
}

int main(void)
{
	DISPLAY_RESET_PIN_OUTPUT;
	DISPLAY_DATA_PIN_OUTPUT;
	DISPLAY_CLOCK_PIN_OUTPUT;
	DISPLAY_MODE_PIN_OUTPUT;
	
	KEYBOARD_ROW_0_PIN_INPUT;
	KEYBOARD_ROW_1_PIN_INPUT;
	KEYBOARD_ROW_2_PIN_INPUT;
	KEYBOARD_ROW_3_PIN_INPUT;
	KEYBOARD_ROW_4_PIN_INPUT;
	KEYBOARD_LOAD_PIN_OUTPUT;
	KEYBOARD_CLOCK_PIN_OUTPUT;
	KEYBOARD_DATA_PIN_INPUT;
	
	engageEeprom();
	
	KEYBOARD_ROW_0_PIN_HIGH;
	KEYBOARD_ROW_1_PIN_HIGH;
	KEYBOARD_ROW_2_PIN_HIGH;
	KEYBOARD_ROW_3_PIN_HIGH;
	KEYBOARD_ROW_4_PIN_HIGH;
	KEYBOARD_LOAD_PIN_HIGH;
	KEYBOARD_CLOCK_PIN_LOW;
	
	TIMSK0 |= (1 << OCIE0A);
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << CS02);
	TCCR0B |= (1 << CS00);
	OCR0A = 125;
	sei();
	
	ADCSRA |= (1 << ADPS2);
	ADCSRA |= (1 << ADPS1);
	ADCSRA |= (1 << ADPS0);
	ADMUX |= (1 << REFS0);
	ADCSRA |= (1 << ADEN);
	
	resetDisplay();
	clearDisplay();
	drawTextFromProgMem(6, 1, MESSAGE_1);
	srand(promptKeySeed());

	/*
	clearDisplay();
	while (true)
	{
		drawLongNumber(0, 0, time);
	}
	 */
	
	// Erase file allocation table.
	//setFileSize(0, 0xFFFFFFFF);
	
	//createFile((byte *)"Test", 1000);
	//writeSequenceToEepromFromProgMem(4096, sizeof(MESSAGE_14), MESSAGE_14);
	
	/*
	 // Test keys.
	 byte index = 0;
	 while (index < 5)
	 {
	 drawPixels(index * 5, 0, 0x55);
	 index += 1;
	 }
	 while (true)
	 {
	 byte index = 0;
	 while (index < 5)
	 {
	 long tempData = receiveRowFromKeyboard(index);
	 drawImage(index * 5 + 1, 0, 2, (byte *)&tempData, 2);
	 index += 1;
	 }
	 }
	 */
	
	while (true)
	{
		byte tempResult = promptSelectionFromProgMem(5, sizeof(MENU_1), MENU_1);
		if (tempResult == 0)
		{
			promptFileAndAction();
		}
		if (tempResult == 1)
		{
			promptFileCreation();
		}
		if (tempResult == 2)
		{
			disengageEeprom();
			clearDisplay();
			drawTextFromProgMem(0, 0, MESSAGE_16);
			promptKey();
			engageEeprom();
		}
		if (tempResult == 3)
		{
			promptHelp();
		}
		if (tempResult == 4)
		{
			clearDisplay();
			drawTextFromProgMem(0, 0, MESSAGE_11);
			promptKey();
		}
	}
	
	return 0;
}

ISR(TIMER0_COMPA_vect)
{
	time += 16;
}
