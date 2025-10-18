
#ifndef ERRORS_H
#define ERRORS_H
#pragma once

 /*
    Defines a struct to represent an error
    `code_num`: The unique error code
    `error_msg`: A message describing the error
 */
typedef struct Error {
    int code_num;
    char *error_msg;
} Error;


/*
   Enum for all possible error codes in the program
   Each error has a unique identifier (Error_0, Error_1, etc.)
   This makes it easier to reference errors in the code instead of using raw numbers
*/
typedef enum ERROR_CODES {
    Error_0 = 0, Error_1, Error_2,
    Error_3, Error_4, Error_5,
    Error_6, Error_7, Error_8,
    Error_9, Error_10, Error_11,
    Error_12, Error_13, Error_14,
    Error_15, Error_16, Error_17,
    Error_18, Error_19, Error_20,
    Error_21, Error_22, Error_23,
    Error_24, Error_25, Error_26,
    Error_27, Error_28, Error_29,
    Error_30, Error_31, Error_32,
    Error_33, Error_34, Error_35,
    Error_36, Error_37, Error_38,
    Error_39, Error_40, Error_41,
    Error_42, Error_43, Error_44,
    Error_45, Error_46, Error_47,
    Error_48, Error_49, Error_50,
    Error_51, Error_52, Error_53,
    Error_54, Error_55, Error_56,
    Error_57, Error_58, Error_59,
    Error_60, Error_61, Error_62,
    Error_63, Error_64, Error_65,
    Error_66, Error_67, Error_68,
    Error_69, Error_70, Error_71,
    Error_72, Error_73
} ERROR_CODES;

/*
   Prints a system-related error (e.g., memory issues, file handling errors).
   The error code determines which system error message to display.
*/
void print_system_error(int error_code);


/*
   Prints a syntax error, usually found while parsing a file.
   Includes the file name and line number to help locate the issue.
*/
void print_syntax_error(int error_code, char* file_name, int line_num);


/*
   Prints a more specific error, often related to a particular word in a line.
   Useful for catching incorrect instructions, labels, or macros.
*/
void print_specific_error(int error_code, char* file_name, int line_num, char* word);


#endif
