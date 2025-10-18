
#include "errors.h"
#include <stdio.h>


 /* Using an array for most of the errors and making sure we display the errors in a friendly way */
Error errors[] = {
        {Error_0, " [CODE_0] | ERROR | No files were provided >:("},
        {Error_1, " [CODE_1] | ERROR | Memory allocation failed :("},
        {Error_2, " [CODE_2] | ERROR | File names must be entered without the \".as\" extension :("},
        {Error_3, "Illegal comment detected, whitespace characters are not allowed before the semicolon (;) :("},
        {Error_4, " [CODE_4] | ERROR | Could not delete the redundant file :("},
        {Error_5, " [CODE_5] | ERROR | Failed to open the existing file for reading :("},
        {Error_6, " [CODE_6] | ERROR | Failed to create a new file for writing :("},
        {Error_7, "Line exceeds the maximum length of 80 characters :("},
        {Error_8, "Invalid macro declaration, macro name is missing :("},
        {Error_9, "Invalid macro declaration, \"mcro\" command contains extraneous text :("},
        {Error_10, "Invalid macro declaration, opcode names cannot be used as macro names :("},
        {Error_11, "Invalid macro declaration, register names cannot be used as macro names :("},
        {Error_12, "Invalid macro declaration, instruction names cannot be used as macro names :("},
        {Error_13, "Invalid macro declaration, a macro must be assigned exactly one name :("},
        {Error_14, "Invalid macro declaration, this macro name is already in use :("},
        {Error_15, "Invalid macro declaration, \"mcroend\" command contains extraneous text :("},
        {Error_16, "Invalid macro declaration, macro name exceeds the maximum length of 31 characters :("},
        {Error_17, "Invalid macro declaration, macro content cannot be empty :("},
        {Error_18, "Invalid macro declaration, \"mcro\" is a reserved word and cannot be used :("},
        {Error_19, "Invalid macro declaration, \"mcroend\" is a reserved word and cannot be used :("},
        {Error_20, "Invalid label declaration, label name exceeds the maximum length of 31 characters :("},
        {Error_21, "Invalid label declaration, label names can only contain alphabetic and numeric characters :("},
        {Error_22, "Invalid label declaration, no value is associated with this label :("},
        {Error_23, "Invalid label declaration, a label name cannot be the same as a macro name :("},
        {Error_24, "Invalid label declaration, opcode names cannot be used as label names :("},
        {Error_25, "Invalid label declaration, register names cannot be used as label names :("},
        {Error_26, "Invalid label declaration, instruction names cannot be used as label names :("},
        {Error_27, "Invalid label declaration, this label name is already in use :("},
        {Error_28, "Invalid label declaration, label name is missing :("},
        {Error_29, "Invalid label declaration, label names must start with an alphabetical character :("},
        {Error_30, "Invalid label declaration, \"mc\" is a reserved word and cannot be used :("},
        {Error_31, "Invalid label declaration, \"mcroend\" is a reserved word and cannot be used :("},
        {Error_32, "Invalid label declaration, local label name cannot be the same as an external label name :("},
        {Error_33, "Macro call was missed during preprocessing, macro calls are allowed only once per line with no extra text :("},
        {Error_34, "Invalid \".data\" instruction, an illegal comma follows the instruction name :("},
        {Error_35, "Invalid \".data\" instruction, only integers are allowed :("},
        {Error_36, "Invalid \".data\" instruction, a comma is missing :("},
        {Error_37, "Invalid \".data\" instruction, multiple consecutive commas detected :("},
        {Error_38, "Invalid \".data\" instruction, an integer is expected after the last comma :("},
        {Error_39, "Invalid \".data\" instruction, number is out of range :("},
        {Error_40, "Invalid \".data\" instruction, no parameters provided :("},
        {Error_41, "Invalid \".string\" instruction, expects a single string enclosed in double quotes (\" \") :("},
        {Error_42, "Invalid \".string\" instruction, no parameter provided :("},
        {Error_43, "Invalid \".entry\" or \".extern\" instruction, a label declaration is required :("},
        {Error_44, "Invalid \".entry\" or \".extern\" instruction, only one label declaration is allowed :("},
        {Error_45, "Invalid \".entry\" or \".extern\" instruction, label name must refer to a valid label :("},
        {Error_46, "Invalid \".entry\" or \".extern\" instruction, label name cannot be the same as a macro name :("},
        {Error_47, "Invalid \".entry\" instruction, label name cannot be the same as an external label name :("},
        {Error_48, "Invalid \".extern\" instruction, label name cannot be the same as a local label name :("},
        {Error_49, "Invalid operation, extraneous text found, no operands are required :("},
        {Error_50, "Invalid operation, an operand is missing :("},
        {Error_51, "Invalid operation, multiple operands are missing :("},
        {Error_52, "Invalid operation, extraneous text found, only one operand is required :("},
        {Error_53, "Invalid operation, illegal comma found after the operation name :("},
        {Error_54, "Invalid operation, extraneous text found, only two operands are required :("},
        {Error_55, "Invalid operation, multiple consecutive commas detected :("},
        {Error_56, "Invalid operation, a comma is missing :("},
        {Error_57, "Invalid operation, destination operand uses an illegal addressing method :("},
        {Error_58, "Invalid operation, source operand uses an illegal addressing method :("},
        {Error_59, "Invalid operand, recognized as a label but not valid, please check the syntax :("},
        {Error_60, "Invalid operation, operand uses 'IMMEDIATE' addressing but has no value :("},
        {Error_61, "Invalid operand for 'IMMEDIATE' addressing, only integers are allowed :("},
        {Error_62, "Invalid operand for 'IMMEDIATE' addressing, value is out of range :("},
        {Error_63, "Invalid operation, operand uses 'RELATIVE' addressing but has no value :("},
        {Error_64, "Invalid operand for 'RELATIVE' addressing, not a valid register :("},
        {Error_65, "Unrecognized command, label declarations must have a space after the colon (:) :("},
        {Error_66, "Unrecognized command, label declarations must have the colon (:) directly attached to the name :("},
        {Error_67, "Unrecognized command, instructions must start with a dot (.) :("},
        {Error_68, "Unrecognized command, please check the syntax :("},
        {Error_69, "Unrecognized operand, please check the syntax :("},
        {Error_70, "Invalid operand, reserved words and macro names cannot be used as operands :("},
        {Error_71, "Invalid command, label name is not a valid command :("},
        {Error_72, "Label was declared as \".entry\" but was not defined :("},
        {Error_73, " [CODE_8] | ERROR | Memory capacity exceeded! Assembler machine-coding has been suspended :(, but line scanning will continue"}
       
};

void print_system_error(int error_code) {
    /* Printing the error message */
    printf("%s\n", errors[error_code].error_msg);
}

void print_syntax_error(int error_code, char* file_name, int line_num) {
    /* Printing the file name, line number and the error message */
    printf(" ERROR | File \"%s\" at line %d | %s\n", file_name, line_num, errors[error_code].error_msg);
}

void print_specific_error(int error_code, char* file_name, int line_num, char* word) {
    /* Printing the file name, line number, specific error quoted and the error message */
    printf(" ERROR | File \"%s\" at line %d | \"%s\" | %s\n", file_name, line_num, word, errors[error_code].error_msg);
}
