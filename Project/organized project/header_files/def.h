#pragma once
#ifndef DEF_H
#define DEF_H


#define EXE_LEN 3 /* The length of the ".as" file extension. */
#define MAX_LINE_LENGTH 81 /* Maximum line length (80 characters + newline character). */
#define REG_COUNT 8 /* Number of registers. */
#define CAPACITY 2097152 /* Maximum memory capacity (2^21 = 2097152). */
#define MACR_LEN 4 /* Length of the macro keyword "mcro". */
#define STARTING_ADDRESS 100 /* The initial address for instructions and data. */

#define MASK_24BIT 0xFFFFFF /* Mask for extracting 24-bit values. */
#define MASK_22BIT 0x3FFFFF /* Mask for extracting 22-bit values. */
#define ABSOLUTE_FLAG 4 /* Flag indicating absolute addressing. */
#define MAX_22BIT_NUMBER 2097151 /* Maximum positive value for a signed 22-bit number. */
#define MIN_22BIT_NUMBER (-2097152) /* Minimum negative value for a signed 22-bit number. */
#define MAX_24BIT_NUMBER 16777215 /* Maximum positive value for a signed 24-bit number. */
#define MIN_24BIT_NUMBER -8388608 /* Minimum negative value for a signed 24-bit number. */

#define MOVE_OPCODE_POS 18 /* Bit position for the opcode in an instruction word. */
#define MOVE_OPERAND_SRC 16 /* Bit position for the source operand. */
#define MOVE_REG_SRC 13 /* Bit position for the source register. */
#define MOVE_OPCODE_DST 11 /* Bit position for the destination operand. */
#define MOVE_REG_DST 8 /* Bit position for the destination register. */
#define MOVE_FUNC 3 /* Bit position for the function bits. */
#define MOVE_ADDRESS 3 /* Addressing mode bit position. */

#define MAX_LABEL 31 /* Maximum length for a label name. */
#define MAX_MACRO 31 /* Maximum length for a macro name. */
#define DECIMAL_BASE 10 /* Decimal base used for number conversions. */
#define INSTRUCTIONS_COUNT 4 /* Number of instruction types supported. */
#define BUFFER 20 /* Buffer size for temporary storage. */
#define MAX_DATA_NUMBERS 38 /* Maximum number of data values in a single .data directive. */
#define ENDMACR_LEN 7 /* Length of "ENDMACR" keyword. */
#define UP_FLAG 3 /* Flag indicating an reference. */
#define EXTERNAL_FLAG 1 /* Flag indicating an external label. */
#define RELOCAT_FLAG 2 /* Flag for relocatable symbols. */
#define RELATIVE_FLAG 7 /* Flag for relative addressing. */

#define IC_INIT_VALUE 100 /* Initial value of the instruction counter (IC). */

/* Special character definitions used in parsing. */
#define AMPERSAND '&' 
#define DOT '.' 
#define HASH '#' 
#define PLUS '+' 
#define MINUS '-' 
#define COLON ':' 
#define COMMA ',' 
#define COMMENT ';' /* Indicates the start of a comment in assembly files. */
#define DOUBLE_QUOTE '\"' 
#define NULL_TERMINATOR '\0' /* Null terminator used for strings. */



#endif
