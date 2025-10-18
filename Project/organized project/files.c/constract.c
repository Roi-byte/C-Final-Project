



#include "constract.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "errors.h"
#include "labels_Fixer.h"
#include "code_converter.h"
#include "macro.h"
#include "firstPhase.h"
#include "initialize.h"


/*
The head of a linked list that stores memory locations for dynamically allocated memory.
*/
static free_memory_Node* head = NULL; 

/*
Array of opcode structures that define the available operations in the system, including
the opcode name, group, operand count, and addressing methods.
*/
op_code opcodes[] = {
        {"mov",  0, 0, 2, METHODS_0_1_3, METHODS_1_3},
        {"cmp",  1, 0, 2, METHODS_0_1_3, METHODS_0_1_3},
        {"add",  2, 1, 2, METHODS_0_1_3, METHODS_1_3},
        {"sub",  2, 2, 2, METHODS_0_1_3, METHODS_1_3},
        {"lea",  4, 0, 2, METHOD_1, METHODS_1_3},
        {"clr",  5, 1, 1, NO, METHODS_1_3},
        {"not",  5, 2, 1, NO, METHODS_1_3},
        {"inc",  5, 3, 1, NO, METHODS_1_3},
        {"dec",  5, 4, 1, NO, METHODS_1_3},
        {"jmp",  9, 1, 1, NO, METHODS_1_2},
        {"bne",  9, 2, 1, NO, METHODS_1_2},
        {"jsr",  9, 3, 1, NO, METHODS_1_2},
        {"red",  12, 0, 1, NO, METHODS_1_3},
        {"prn",  13, 0, 1, NO, METHODS_0_1_3},
        {"rts",  14, 0, 0, NO, NO},
        {"stop", 15, 0, 0, NO, NO}
};

/*
Array of register names for the system, ranging from "r0" to "r7".
*/
char* REGISTERS[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7" };

/*
Array defining special instructions such as ".data", ".string", ".entry", and ".extern".
*/
char* INSTRUCTIONS[] = { ".data", ".string", ".entry", ".extern" };


/* Function to search for a file and open it. Returns NULL if the file is not found.
 It will also print an error message and free memory if necessary. */
FILE* search_file(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("\n [CODE_3] | ERROR | File \"%s\" was not found :( \n", filename);
        i_want_to_break_free_memory();  /*Clean up memory if the file is not found */
        return NULL;
    }
    fclose(file);
    return file;
}

/*Function to return the opcodes array for access*/ 
op_code* get_opcodes() {
    return opcodes;
}

/*
Changes the extension of a given filename to the new extension specified.
It allocates memory for the new filename and returns it.
If memory allocation fails, the program exits.
*/
char* change_extension(char* file_name, char* new_extension) {
    /* Finding the last occurrence of the '.' character */
    char* dot = strrchr(file_name, DOT);

    /* Calculating the length for the filename with the new extension */
    int base_length = dot - file_name;
    int new_extension_length = strlen(new_extension);
    int new_filename_length = base_length + new_extension_length + 1;  /* +1 for the null terminator */

    /* Allocating memory for the new filename */
    char* new_filename = (char*)allocate_memory(new_filename_length);
    if (new_filename == NULL)  /* Indicates memory allocation failed */
        exit(1);  /* Exiting program */

    /* Copying the base part of the original filename */
    strncpy(new_filename, file_name, base_length);
    new_filename[base_length] = NULL_TERMINATOR;  /* Null-terminating the base part */

    /* Appending the new extension */
    strcat(new_filename, new_extension);
    return new_filename;
}

/*
Removes unnecessary whitespace from a string, including leading, trailing, and extra spaces
between words. The modified string is returned.
*/
char* remove_spaces(char str[]) {
    int i, j;
    char str_temp[MAX_LINE_LENGTH];
    i = j = 0;

    if (*str == NULL_TERMINATOR)  /* If the string is empty, return it as is */
        return str;

    /* Eliminating white-spaces at the beginning of the line */
    while (is_space_or_tab(*(str + i))) {
        i++;
    }
    while (*(str + i) != NULL_TERMINATOR) {
        /* Copying character */
        while (!is_space_or_tab(*(str + i)) && *(str + i) != NULL_TERMINATOR) {
            *(str_temp + j) = *(str + i);
            i++;
            j++;
        }
        /* Skipping any remaining white-space */
        while (is_space_or_tab(*(str + i))) {
            i++;
        }
        if (!(*(str + i) == '\n' || *(str + i) == NULL_TERMINATOR)) {
            *(str_temp + j) = ' ';  /* Replacing multiple spaces with a single one */
            j++;
        }
    }

    /* Remove newline if it exists at the end */
    if (j > 0 && str_temp[j - 1] == '\n') {
        j--;  /*Remove the newline character from the end*/ 
    }

    *(str_temp + j) = NULL_TERMINATOR; /*Null - terminate the string*/ 

    strcpy(str, str_temp);
    return str;
}

/*
Checks if a character is a space or a tab. Returns 1 if true, otherwise 0.
*/
int is_space_or_tab(char c) {
    return (c == ' ' || c == '\t');
}

/*
Checks if a string contains any whitespace characters.
Returns 1 if any whitespace is found, otherwise 0.
*/
int include_whitespace(const char* str) {
    while (*str) {
        if (isspace(*str++)) return 1;
    }
    return 0;
}

/*
Checks if a string is one of the reserved system words (opcode, register, instruction, macro).
If a match is found, the corresponding error is printed, and 1 is returned.
Otherwise, 0 is returned.
*/
int is_reserved_str(char* file_name, char* str, int line_count, Type type) {
    int len = strlen(file_name);

    if (which_opcode(str) != -1) {
        if (strcmp(&file_name[len - EXE_LEN], ".as") == 0 && type != OPERAND) {
            print_syntax_error(Error_10, file_name, line_count);
            return 1;
        }
        switch (type) {
        case OPERAND:
            print_syntax_error(Error_70, file_name, line_count);
            break;
        case DEFAULT:
            print_syntax_error(Error_24, file_name, line_count);
            break;
        default:
            print_syntax_error(Error_45, file_name, line_count);
            break;
        }
        return 1;
    }

    if (which_regis(str) != -1) {
        if (strcmp(&file_name[len - EXE_LEN], ".as") == 0 && type != OPERAND) {
            print_syntax_error(Error_11, file_name, line_count);
            return 1;
        }
        switch (type) {
        case OPERAND:
            print_syntax_error(Error_70, file_name, line_count);
            break;
        case DEFAULT:
            print_syntax_error(Error_25, file_name, line_count);
            break;
        default:
            print_syntax_error(Error_45, file_name, line_count);
            break;
        }
        return 1;
    }

    if (which_instr(str) != -1) {
        if (strcmp(&file_name[len - EXE_LEN], ".as") == 0 && type != OPERAND) {
            print_syntax_error(Error_12, file_name, line_count);
            return 1;
        }
        switch (type) {
        case OPERAND:
            print_syntax_error(Error_70, file_name, line_count);
            break;
        case DEFAULT:
            print_syntax_error(Error_26, file_name, line_count);
            break;
        default:
            print_syntax_error(Error_45, file_name, line_count);
            break;
        }
        return 1;
    }

    if (strcmp(str, "mcro") == 0) {
        if (strcmp(&file_name[len - EXE_LEN], ".as") == 0 && type != OPERAND) {
            print_syntax_error(Error_18, file_name, line_count);
            return 1;
        }
        switch (type) {
        case OPERAND:
            print_syntax_error(Error_70, file_name, line_count);
            break;
        case DEFAULT:
            print_syntax_error(Error_30, file_name, line_count);
            break;
        default:
            print_syntax_error(Error_45, file_name, line_count);
            break;
        }
        return 1;
    }
    if (strcmp(str, "mcroend") == 0) {
        if (strcmp(&file_name[len - EXE_LEN], ".as") == 0 && type != OPERAND) {
            print_syntax_error(Error_13, file_name, line_count);
            return 1;
        }
        switch (type) {
        case OPERAND:
            print_syntax_error(Error_70, file_name, line_count);
            break;
        case DEFAULT:
            print_syntax_error(Error_27, file_name, line_count);
            break;
        default:
            print_syntax_error(Error_45, file_name, line_count);
            break;
        }
        return 1;
    }

    return 0;
}



/*
This function frees the dynamically allocated memory from the free_memory_Node linked list.
It iterates through the list, frees each node's memory, and then sets the head of the list to NULL.
*/
void i_want_to_break_free_memory() {
    free_memory_Node* current = head;  /* Start from the head of the list */
    free_memory_Node* temp;  /* Temporary pointer for safe memory deallocation */

    while (current != NULL) {  /* Iterate through the list until all nodes are freed */
        temp = current;  /* Store the current node in temp to free it later */
        current = current->next;  /* Move to the next node in the list */
        free(temp->ptr);  /* Free the memory pointed to by the current node */
        free(temp);  /* Free the current node itself */
    }

    head = NULL;  /* Set the head of the list to NULL, indicating the list is now empty */
}

/*
This function checks if the current line represents an operation. It looks up the operation
by calling the 'which_opcode' function. If an operation is found, it validates it using the
'valid_operation' function and updates the label properties if necessary.
*/
int is_operation(int* code, int* Usage, int* IC, Line* line, char* ptr, char* current_word, int* errors_found) {
    int curr_word_len = strlen(current_word);  /* Recive the length of the current word */
    int code_id;

    /* Checking if the current word corresponds to a valid operation */
    code_id = which_opcode(current_word);  /* Look up the operation's index in the opcodes array */
    if (code_id != -1) {  /* Operation found */
        /* If the line has a label, update its properties */
        if (line->label != NULL) {
            line->label->address = *IC;  /* Set the label's address to the current IC value */
            line->label->location = CODE;  /* Indicate that the label is part of the code section */
        }

        ptr += curr_word_len;  /* Move the pointer forward by the length of the current word */

        /* Validate the operation (check if it's properly formatted and uses correct operands) */
        check_valid_operation(code, Usage, IC, line, ptr, code_id, errors_found);

        return 1;  /* Line has been processed as an operation */
    }

    return 0;  /* Line is not an operation, continue scanning */
}

/*
This function checks if the given string corresponds to a valid opcode in the system.
It compares the string against the available opcodes and returns the index of the matching opcode.
If no match is found, it returns -1.
*/
int which_opcode(char* str) {
    int i;

    /* If the string is NULL, it's not a valid opcode */
    if (str == NULL)
        return -1;

    /* Loop through the list of defined opcodes to find a match */
    for (i = 0; i < 16; i++) {
        if (strcmp(str, opcodes[i].opcode) == 0) {  /* Compare the string with the opcode name */
            return i;  /* Return the index of the matching opcode */
        }
    }

    /* No match found, return -1 */
    return -1;
}




void check_valid_operation(int* code, int* Usage, int* IC, Line* line, char* ptr, int code_id, int* errors_found) {
    char* opernad, * second_operand, * comma_pos;
    int operands_num = opcodes[code_id].group_operand, length, method, method_2;
    int word = 0;
    word |= (code_id << MOVE_OPCODE_POS) | ABSOLUTE_FLAG; 

    /* Analyzing opernads */
    switch (operands_num) {
    case 0:
        if (ptr[0] != NULL_TERMINATOR) {  /* Checking if there is a extraneous text */  
            print_syntax_error(Error_49, line->file_am_name, line->line_num);
            *errors_found = 1;
            return;  /* End scanning this line */
        }
        add_instruction_code(code, Usage, IC, word, errors_found);  /* Adding machine code */
        return;  /* End scanning this line */
    case 1: 
        if (ptr[0] == NULL_TERMINATOR) {  /* Checking if there is a missing operand */
            print_syntax_error(Error_50, line->file_am_name, line->line_num);
            *errors_found = 1;
            return;  /* End scanning this line */
        }
        while (*ptr && isspace(*ptr))  /* Skipping leading whitespace */
            ptr++;

        if (ptr[0] == COMMA) {  /* Checking if there is an illegal comma */
            print_syntax_error(Error_53, line->file_am_name, line->line_num);
            *errors_found = 1;
            return;  /* End scanning this line */
        }
        if (include_whitespace(ptr) || strchr(ptr, COMMA) != NULL) {  /* Checking for extraneous text */
            print_syntax_error(Error_52, line->file_am_name, line->line_num);
            *errors_found = 1;
            return;  /* End scanning this line */
        }
        /* Checking if the addressing method is legal */
        method = which_method(ptr, line, errors_found);
        if (method == -1) {
            return;  /* End scanning this line */
        }
        if (is_legal(line, method, code_id, operands_num, errors_found) != 0) {
            return;  /* End scanning this line */
        }
        handle_one_operand(code, Usage, IC, line, method, ptr, code_id, errors_found);
        return;  /* End scanning this line */
    case 2:  
        if (ptr[0] == NULL_TERMINATOR) {  /* Checking if there are missing operands */
            print_syntax_error(Error_51, line->file_am_name, line->line_num);
            *errors_found = 1;
            return;  /* End scanning this line */
        }
        while (*ptr && isspace(*ptr))  /* Skipping leading whitespace */
            ptr++;

        if (ptr[0] == COMMA) {  /* Checking if there is an illegal comma */
            print_syntax_error(Error_53, line->file_am_name, line->line_num);
            *errors_found = 1;
            return;  /* End scanning this line */
        }
        opernad = get_first_word(ptr);
        if (opernad == NULL) {  /* Indicates memory allocation failed */
            fclose(line->file);
            free_line(line);
            exit(1);  /* Exiting program */
        }
        length = strlen(opernad);

        comma_pos = strchr(opernad, COMMA);
        if (comma_pos != NULL) {  /* If a comma was found, spliting the string into two opernads */
            if (comma_pos - opernad == strlen(opernad) - 1 && ptr[length] == NULL_TERMINATOR) {
                print_syntax_error(Error_50, line->file_am_name, line->line_num);
                *errors_found = 1;
                deallocate_memory(opernad);
                return;  /* End scanning this line */
            }
            *comma_pos = NULL_TERMINATOR;
            length = comma_pos - opernad;
            second_operand = &ptr[length + 1];  /* Setting a pointer to the second operand */

            while (*second_operand && isspace(*second_operand))  /* Skipping leading whitespace */
                second_operand++;

            if (second_operand[0] == COMMA) {  /* Checking if there are consecutive commas */
                print_syntax_error(Error_55, line->file_am_name, line->line_num);
                *errors_found = 1;
                deallocate_memory(opernad);
                return;  /* End scanning this line */
            }
            if (include_whitespace(second_operand) || strchr(second_operand, COMMA) != NULL) {  /* Checking for extraneous text */
                print_syntax_error(Error_54, line->file_am_name, line->line_num);
                *errors_found = 1;
                deallocate_memory(opernad);
                return;  /* End scanning this line */
            }
        }
        else {  /* No comma was found in the first string */
            ptr = &ptr[length];
            while (*ptr && isspace(*ptr))  /* Skipping leading whitespace */
                ptr++;
            if (ptr[0] == NULL_TERMINATOR || (*ptr == COMMA && ptr[1] == NULL_TERMINATOR)) {  /* Checking if there is a missing operand */
                print_syntax_error(Error_50, line->file_am_name, line->line_num);
                *errors_found = 1;
                deallocate_memory(opernad);
                return;  /* End scanning this line */
            }
            if (ptr[0] != COMMA) {
                print_syntax_error(Error_56, line->file_am_name, line->line_num);
                *errors_found = 1;
                deallocate_memory(opernad);
                return;  /* End scanning this line */
            }
            ptr++;
            while (*ptr && isspace(*ptr))  /* Skipping leading whitespace */
                ptr++;
            second_operand = get_first_word(ptr);
            if (second_operand == NULL) {  /* Indicates memory allocation failed */
                fclose(line->file);
                free_line(line);
                exit(1);  /* Exiting program */
            }
            if (second_operand[0] == COMMA) {
                print_syntax_error(Error_55, line->file_am_name, line->line_num);
                *errors_found = 1;
                deallocate_memory(opernad);
                deallocate_memory(second_operand);
                return;  /* End scanning this line */
            }
            length = strlen(second_operand);
            if (strchr(second_operand, COMMA) != NULL || ptr[length] != NULL_TERMINATOR) {  /* Checking for extraneous text */
                print_syntax_error(Error_54, line->file_am_name, line->line_num);
                *errors_found = 1;
                deallocate_memory(opernad);
                deallocate_memory(second_operand);
                return;  /* End scanning this line */
            }
        }
        method = which_method(opernad, line, errors_found);
        method_2 = which_method(second_operand, line, errors_found);
        if (method == -1 || method_2 == -1) {
            deallocate_memory(opernad);
            deallocate_memory(second_operand);
            return;  /* End scanning this line */
        }
        if (is_legal(line, method, code_id, operands_num, errors_found) != 0 ||  /* Checking if the addressing method is legal */
            is_legal(line, method_2, code_id, operands_num - 1, errors_found) != 0) {  /* Check if the addressing method for the second operand is valid, operands_num-1 to signal that opernd is of type "destination" */
            deallocate_memory(opernad);
            deallocate_memory(second_operand);
            return;  /* End scanning this line */
        }
        handle_two_operands(code, Usage, IC, line, opernad, second_operand, code_id, errors_found);
        deallocate_memory(opernad);
        deallocate_memory(second_operand);
    }
}




/*Type of sorting*/
int which_method(char* operand, Line* line, int* errors_found) {
    char* endptr;
    long val;

    if (operand[0] == HASH) {
        operand++;
        /* Check if no value is provided after '#' */
        if (*operand == NULL_TERMINATOR) {
            print_syntax_error(Error_60, line->file_am_name, line->line_num);
            *errors_found = 1;
            return -1;  /* Indicates faliure */
        }
        val = strtol(operand, &endptr, DECIMAL_BASE); 
        /* Checking if the conversion was successful */
        if (*endptr != NULL_TERMINATOR || endptr == operand) { 
            print_specific_error(Error_61, line->file_am_name, line->line_num, operand);
            *errors_found = 1;
            return -1;  /* Indicates faliure */
        }
        /* Checking if the number is in range */
        if (val < MIN_22BIT_NUMBER || val > MAX_22BIT_NUMBER) { 
            print_specific_error(Error_62, line->file_am_name, line->line_num, operand);
            *errors_found = 1;
            return -1;  /* Indicates faliure */
        }
        return IMMEDIATE;  /* Returning the matching addressing method */  
    }
    if (operand[0] == AMPERSAND) {
        operand++;
        /* Checking if no value was entered */
        if (*operand == NULL_TERMINATOR) {
            print_syntax_error(Error_63, line->file_am_name, line->line_num); 
            *errors_found = 1;
            return -1;  /* Indicates faliure */
        }
        return RELATIVE;  /* Returning the matching addressing method */
    }
    if (which_regis(operand) != -1)  /* Indicates opernad is a register */
        return DIRECT_REGISTER;  /* Returning the matching addressing method */

   

    /* If the label is valid - returning the matching addressing method */
    return DIRECT;
}



/* Function to check if the string is a valid register name and if it is which register it is */

int which_regis(char* str) {
    int i;

    if (str == NULL)  /* If the string is empty, it's not a valid register */
        return -1;

    /* Compare the string with each system register name */
    for (i = 0; i < REG_COUNT; i++) {
        if (strcmp(str, REGISTERS[i]) == 0) {
            return i; /* Return the index of the matching register */
        }
    }
    return -1;  /* Return -1 if it's not a valid register */
}



int is_instruction(int* data, int* Usage, int* DC, Line* line, char* ptr, char* current_word, int* errors_found) {
    int curr_word_len = strlen(current_word);

    /* Identifying if the current word corresponds to a known instruction */
    switch (which_instr(current_word)) {
    case 0:
        ptr += curr_word_len;  /* Skipping the first word ".data" */
        data_found(data, Usage, DC, line, ptr, errors_found);
        return 1;  /* Line scanning is complete */
    case 1:
        ptr += curr_word_len;  /* Skipping the first word ".string" */
        string_found(data, Usage, DC, line, ptr, errors_found);
        return 1;  /* Line scanning is complete */
    case 2:
        ptr += curr_word_len;  /* Skipping the first word ".entry" */
        entry_found(line, ptr, errors_found);
        return 1;  /* Line scanning is complete */
    case 3:
        ptr += curr_word_len;  /* Skipping the first word ".extern" */
        extern_found(line, ptr, errors_found);
        return 1;  /* Line scanning is complete */
    default:
        return 0;  /* Indicates line is not an "instruction" line, continue scanning */
    }
}

int which_instr(char* str) {
    int i;

    if (str == NULL)  /* Indicates string is not an instruction name return -1 */
        return -1;

    /* Check if the string matches any known instruction names */
    for (i = 0; i < INSTRUCTIONS_COUNT; i++) {
        if (strcmp(str, INSTRUCTIONS[i]) == 0) {
            return i;  /* Returning the index of the matching instruction */
        }
    }
    return -1;  /* No match found, return -1 */
}


void data_found(int* data, int* Usage, int* DC, Line* line, char* ptr, int* errors_found) {
    /* Checking if there are no parameters */
    if (*ptr == NULL_TERMINATOR) {
        if (line->label != NULL)
            remove_last_label_node();
        print_syntax_error(Error_40, line->file_am_name, line->line_num);
        *errors_found = 1;
        return;
    }
    /* Analyzing input numbers */
    analyze_numbers(data, Usage, DC, line, ptr, errors_found);
}
   

    void string_found(int* data, int* Usage, int* DC, Line * line, char* ptr, int* errors_found) {
        char* trimmed_line;
        int trimmed_line_len, i;

        /* Checking for a parameter */
        if (*ptr == NULL_TERMINATOR) {
            if (line->label != NULL)
                remove_last_label_node();
            print_syntax_error(Error_42, line->file_am_name, line->line_num);
            *errors_found = 1;
            return;
        }
        trimmed_line = remove_spaces(ptr);
        trimmed_line_len = strlen(trimmed_line);

        /* Ensure the string starts and ends with double quotes */
        if (trimmed_line[0] != DOUBLE_QUOTE || trimmed_line[trimmed_line_len - 1] != DOUBLE_QUOTE) {
            if (line->label != NULL)
                remove_last_label_node();
            print_syntax_error(Error_41, line->file_am_name, line->line_num);
            *errors_found = 1;
            return;
        }
        /* Check for an empty string (only double quotes) */
        if (strlen(trimmed_line) == 2) {   /* Indicates string contains only double quotes */
            printf(" WARNING | File \"%s\" at line %d | Instruction \".string\" parameter"
                " is an empty string\n", line->file_am_name, line->line_num);
        }
        trimmed_line[trimmed_line_len - 1] = NULL_TERMINATOR;
        trimmed_line++;
        trimmed_line_len = strlen(trimmed_line);

        /* Updating label properties */
        if (line->label != NULL) {
            line->label->address = *DC;
            line->label->location = DATA;
        }
        /* Adding machine code to data array */
        for (i = 0; i < trimmed_line_len; i++) {
            if (*Usage + 1 == CAPACITY) {  /* Checking if memory limit was reached (including null-terminator) */
                print_system_error(Error_73);
                *errors_found = 1;
                (*Usage)++;  /* Incrementing usage count so the next iteration will not print another error message */
                return;  /* Stop scanning this line */
            }
            if (*Usage + 1 > CAPACITY) {  /* Checking if memory exceeded capacity */
                return;  /* Stop scanning this line */
            }
            /* Getting the ASCII value by converting 'char' type to 'int' and then adding code */
            add_data_code(data, DC, (int)trimmed_line[i]);
            *Usage += 1;  /* Incrementing usage count */
        }
        add_data_code(data, DC, 0);  /* Adding the null-terminator */
        *Usage += 1;  /* Incrementing usage count */
    }

void entry_found(Line *line, char *ptr, int *errors_found) {
    Label *label;
    char *trimmed_line;

    /* Checking if a label was already declared at the current line */
    if (line->label != NULL) {
        printf(" WARNING | File \"%s\" at line %d | Label defined at the start of an"
        " \".entry\" or \".extern\" instruction line will be ignored\n",line->file_am_name,line->line_num);
        remove_specific_label(line->label);
    }
    /* Checking if there is no label declaration */
    if (*ptr == NULL_TERMINATOR) {
        print_syntax_error(Error_43,line->file_am_name,line->line_num);
        *errors_found = 1;
        return;  /* Stop scanning this line */
    }
    /* Checking if there is more than one label declaration */
    trimmed_line = remove_spaces(ptr);
    if (include_whitespace(trimmed_line)) {
        print_syntax_error(Error_44,line->file_am_name,line->line_num);
        *errors_found = 1;
        return;  /* Stop scanning this line */
    }
    if (valid_label_name(trimmed_line,ENTRY,line,errors_found) != 0)
        return; /* Stop scanning this line */

    label = add_new_label(trimmed_line,0,ENTRY,TBD);
    if (label == NULL) {  /* Indicates memory allocation failed */
        fclose(line->file);
        free_line(line);
        free_all_labels();
        free_macros();
        i_want_to_break_free_memory();
        exit(1);  /* Exiting program */
    }
    line->label = label;  /* Setting the label pointer of struct line to the new entry label */
}

void extern_found(Line* line, char* ptr, int* errors_found) {
    Label* label;
    char* trimmed_line;

    /* Checking if a label was already declared at the current line */
    if (line->label != NULL) {
        printf(" WARNING | File \"%s\" at line %d | Label defined at the start of an"
            " \".entry\" or \".extern\" instruction line will be ignored\n", line->file_am_name, line->line_num);
        remove_specific_label(line->label);
    }
    /* Checking if there is no label declaration */
    if (*ptr == NULL_TERMINATOR) {
        print_syntax_error(Error_43, line->file_am_name, line->line_num);
        *errors_found = 1;
        return;  /* Stop scanning this line */
    }
    /* Checking if there is more than one label declaration */
    trimmed_line = remove_spaces(ptr);
    if (include_whitespace(trimmed_line)) {
        print_syntax_error(Error_44, line->file_am_name, line->line_num);
        *errors_found = 1;
        return;  /* Stop scanning this line */
    }
    if (valid_label_name(trimmed_line, EXTERN, line, errors_found) != 0)
        return;  /* Stop scanning this line */

    label = add_new_label(trimmed_line, 0, EXTERN, TBD);
    if (label == NULL) {  /* Indicates memory allocation failed */
        fclose(line->file);
        free_line(line);
        free_all_labels();
        free_macros();
        i_want_to_break_free_memory();
        exit(1);  /* Exiting program */
    }
    line->label = label;  /* Setting the label pointer of struct line to the new extern label */
}

void analyze_numbers(int* data, int* Usage, int* DC, Line* line, char* ptr, int* errors_found) {
    int* num_array;
    int num_count = 0, i = 0;

    num_array = get_numbers(line, ptr, &num_count, errors_found);
    if (num_array == NULL) {
        if (line->label != NULL)
            remove_last_label_node();
        return;
    }
    /* Updating label properties */
    if (line->label != NULL) {
        line->label->address = *DC;
        line->label->location = DATA;
    }
    /* Adding machine code to data array */
    for (; i < num_count; i++) {
        if (*Usage == CAPACITY) {  /* Checking if memory limit was reached */
            print_system_error(Error_73);
            *errors_found = 1;
            (*Usage)++;  /* Incrementing usage count so the next iteration will not print another error message */
            return;  /* Stop scanning this line */
        }
        if (*Usage > CAPACITY) {  /* Checking if memory exceeded capacity */
            return;  /* Stop scanning this line */
        }
        add_data_code(data, DC, num_array[i]);  /* Adding machine code */
        (*Usage)++;  /* Incrementing usage count */
    }
    deallocate_memory(num_array);
}

void deallocate_memory(void* ptr) {
    free_memory_Node* current = head;
    free_memory_Node* prev = NULL;

    while (current != NULL) {
        /*Check if the current node's pointer matches the one to deallocate */
        if (current->ptr == ptr) {
            /* If the node is at the beginning of the list, update the head*/
            if (prev == NULL) {
                head = current->next;
            }
            else {
                /*If the node is no at the beginning skip the current node by linking the previous node to the next one*/
                prev->next = current->next;
            }
            free(current->ptr);
            free(current);
            return;
        }
        prev = current;
        current = current->next; /*Move to the next node*/
    }
}



int* get_numbers(Line* line, char* ptr, int* num_count, int* errors_found) {
    int* result;
    char buffer[BUFFER];
    int numbers[MAX_DATA_NUMBERS];
    int temp_count = 0, i = 0, last_was_comma = 0, length = strlen(ptr), num, j;

    while (i < length && isspace(ptr[i]))  /* Skipping leading whitespace */
        i++;

    if (!isdigit(ptr[i]) && ptr[i] != MINUS && ptr[i] != PLUS) {  /* Checking for an invalid character */
        *errors_found = 1;
        if (ptr[i] == COMMA) {
            print_syntax_error(Error_34, line->file_am_name, line->line_num);
            return NULL;
        }
        print_syntax_error(Error_35, line->file_am_name, line->line_num);
        return NULL;
    }
    while (i < length) {
        while (i < length && isspace(ptr[i]))  /* Skipping whitespace characters */
            i++;
        if (i >= length)
            break;
        /* Checking for a number with an optional sign is encountered */
        if (isdigit(ptr[i]) || ((ptr[i] == MINUS || ptr[i] == PLUS) && isdigit(ptr[i + 1]))) {
            last_was_comma = 0;
            j = 0;

            if (ptr[i] == MINUS || ptr[i] == PLUS)  /* Handling optional sign */
                buffer[j++] = ptr[i++];
            while (i < length && isdigit(ptr[i]))  /* Recive the number */
                buffer[j++] = ptr[i++];

            /* Checking there are no invalid characters after the number */
            if (i < length && !isspace(ptr[i]) && ptr[i] != COMMA && ptr[i] != MINUS && ptr[i] != PLUS) {
                print_syntax_error(Error_35, line->file_am_name, line->line_num);
                *errors_found = 1;
                return NULL;
            }
            buffer[j] = NULL_TERMINATOR;
            num = atoi(buffer);

            /* Check if the number is within the valid range */
            if (num < MIN_24BIT_NUMBER || num > MAX_24BIT_NUMBER) {
                print_syntax_error(Error_39, line->file_am_name, line->line_num);
                *errors_found = 1;
                return NULL;
            }
            /* Adding the number to the array */
            numbers[temp_count] = num;
            temp_count++;

            while (i < length && isspace(ptr[i]))  /* Skipping whitespace characters after the number */
                i++;

            if (i < length && ptr[i] == COMMA) {  /* Checking for a comma after the number */
                last_was_comma = 1;
                i++;
            }
            else if (i < length && ptr[i] != COMMA) {
                print_syntax_error(Error_36, line->file_am_name, line->line_num);
                *errors_found = 1;
                return NULL;
            }
        }
        else {
            print_syntax_error(Error_35, line->file_am_name, line->line_num);
            *errors_found = 1;
            return NULL;
        }
        /* Checking for multiple consecutive commas */
        if (last_was_comma) {
            while (i < length && isspace(ptr[i]))
                i++;

            if (i < length && ptr[i] == COMMA) {
                print_syntax_error(Error_37, line->file_am_name, line->line_num);
                *errors_found = 1;
                return NULL;
            }
        }
    }
    /* If the last character was a comma, report an error */
    if (last_was_comma) {
        print_syntax_error(Error_38, line->file_am_name, line->line_num);
        *errors_found = 1;
        return NULL;
    }
    result = (int*)allocate_memory(temp_count * sizeof(int));
    if (result == NULL) {  /* Indicates memory allocation failed (all other allocations were freed inside function) */
        fclose(line->file);
        free_line(line);
        exit(1);  /* Exiting program */
    }
    memcpy(result, numbers, temp_count * sizeof(int));
    *num_count = temp_count;

    return result;
}


void free_line(Line* line) {
    /* Free the file name string if it was allocated */
    if (line->file_am_name != NULL)
        free(line->file_am_name);
    /* Free the content string if it was allocated */
    if (line->content != NULL)
        free(line->content);
    /* Free the line structure itself */
    free(line);
}

void delete_file(char* filename) {
    /* Remove the specified file and print error if deletion fails */
    if (remove(filename) != 0)
        print_system_error(Error_4);
}

int check_standalone_word(char* str, char* word) {
    int word_len = strlen(word);
    char* ptr = strstr(str, word);

    while (ptr != NULL) {  /* Checking if the character before the word is a space or the start of the string */
        if ((ptr == str || isspace(ptr[-1])) &&
            /* Checking if the character after the word is a space or the end of the string */
            (isspace(ptr[word_len]) || ptr[word_len] == '\0')) {
            return 1;  /* Indicates word was found */
        }
        /* Moving to the next occurrence of the word */
        ptr = strstr(ptr + word_len, word);
    }
    return 0;  /* Indicates word was not found */
}


char* get_first_word(char* str) {
    char* first_word;
    /* Finding the length of the first word */
    int length = 0;
    while (str[length] != NULL_TERMINATOR && !isspace(str[length])) {
        length++;
    }
    /* Allocating memory for the first word */
    first_word = (char*)allocate_memory((length + 1) * sizeof(char));
    if (first_word == NULL)
        return NULL;  /* Indicates memory allocation failed, return NULL */

    strncpy(first_word, str, length);   /* Copying the first word to the allocated memory */
    first_word[length] = NULL_TERMINATOR;  /* Null-terminating the string */

    return first_word;
}

void* allocate_memory(long size) {
    free_memory_Node* new_node;
    void* ptr = malloc(size);  /* Using void * for compatibility with different data types */
    if (ptr == NULL) { /*If memory allocation fails, print system error and return NULL*/
        print_system_error(Error_1);
        free_macros();
        free_all_labels();
        i_want_to_break_free_memory();
        return NULL;
    }
    new_node = (free_memory_Node*)malloc(sizeof(free_memory_Node));
    if (new_node == NULL) {/*If memory allocation fails, print system error and return NULL*/
        print_system_error(Error_1);
        free_macros();
        free_all_labels();
        i_want_to_break_free_memory();
        return NULL;
    }
    new_node->ptr = ptr;
    new_node->next = head;
    head = new_node;
    return ptr;
}

int is_legal(Line* line, int method, int code_id, int operands_num, int* errors_found) {
    /* Use opcodes array to sort and check if it is legal */
    /* Receiving the legal methods for the current operation */
    if (operands_num == 1) {  /* Indicates operand is of type "destination" */
        switch (opcodes[code_id].dest_method) {
        case METHODS_1_2:
            if (method == IMMEDIATE || method == DIRECT_REGISTER) {
                print_syntax_error(Error_57, line->file_am_name, line->line_num);
                *errors_found = 1;
                return 1;  /* Indicates method is illegal */
            }
            return 0;  /* Indicates method is legal */
        case METHODS_1_3:
            if (method == IMMEDIATE || method == RELATIVE) {
                print_syntax_error(Error_57, line->file_am_name, line->line_num);
                *errors_found = 1;
                return 1;  /* Indicates method is illegal */
            }
            return 0;
        case METHODS_0_1_3:
            if (method == RELATIVE) {
                print_syntax_error(Error_57, line->file_am_name, line->line_num);
                *errors_found = 1;
                return 1;  /* Indicates method is illegal */
            }
            break;  
        default:
            return 0;  /* Indicates method is legal */
        }
    }
    
    if (operands_num == 2) {
        switch (opcodes[code_id].source_method) {
        case METHODS_0_1_3:
            if (method == RELATIVE) {
                print_syntax_error(Error_58, line->file_am_name, line->line_num);
                *errors_found = 1;
                return 1;  /* Indicates method is illegal */
            }
            return 0;
        case METHOD_1:
            if (method != DIRECT) {
                print_syntax_error(Error_58, line->file_am_name, line->line_num);
                *errors_found = 1;
                return 1;  /* Indicates method is illegal */
            }
            break;  
        }
        return 0;  /* Indicates method is legal */
    }

    return 0;  
}

Line* create_line(FILE* file, char* file_am_name, char* content, int line_num) {
    Line* new_line = (Line*)malloc(sizeof(Line));
    if (new_line == NULL) {
        print_system_error(Error_1);
        return NULL;  /* Indicates failure, return NULL */
    }
    /* Allocating memory for the file name and the line's content */
    new_line->file_am_name = (char*)malloc(strlen(file_am_name) + 1);
    if (new_line->file_am_name == NULL) {
        print_system_error(Error_1);
        free(new_line);
        return NULL;  /* Indicates failure, return NULL*/
    }
    strcpy(new_line->file_am_name, file_am_name);

    new_line->content = (char*)malloc(strlen(content) + 1);
    if (new_line->content == NULL) {
        print_system_error(Error_1);
        free(new_line->file_am_name);
        free(new_line);
        return NULL;  /* Indicates failure, return NULL */
    }
    strcpy(new_line->content, content);
    /* Setting file pointer and line number */
    new_line->file = file;
    new_line->line_num = line_num;
    new_line->label = NULL;
    return new_line;  /* Indicates success */
}



void create_ob_file(char* file_ob_name, int* code, int* data, int* IC, int* DC) {
     /* Create an object file */
        FILE * file_ob = fopen(file_ob_name, "w");
    int i = 100, j = 0;

    if (file_ob == NULL) {  /* Failed to open file for writing */
        print_system_error(Error_6);
        free_all_labels();
        i_want_to_break_free_memory();
        exit(1);  /* Exiting program */
    }

    /* putting info into the header of the file */
    fprintf(file_ob, "  %d %d\n", *IC - STARTING_ADDRESS, *DC);

    /* Writing machine code into file and puting IC before DC */
    for (; i < *IC; i++) { /*Printing according to instruction*/
        fprintf(file_ob, "%07d %06x\n", i, code[i] & 0xFFFFFF); /*We print with this (code[i] & 0xFFFFFF) in order to make sure it is 6 digits*/
    }
    for (; j < *DC; j++) {
        fprintf(file_ob, "%07d %06x\n", j + i, data[j] & 0xFFFFFF); /*We print with this (code[i] & 0xFFFFFF) in order to make sure it is 6 digits*/
    }

    fclose(file_ob);
}


void create_ent_file(char* file_ent_name) {
    /* Create an entry file */
    FILE* file_ent = fopen(file_ent_name, "w");
    Label* current;

    if (file_ent == NULL) {  /* Failed to open file for writing */
        print_system_error(Error_6);
        free_all_labels();
        i_want_to_break_free_memory();
        exit(1);  /* Exiting program */
    }

    current = get_label_list_head();
    while (current != NULL) {
        if (current->type == ENTRY) { /* Write in entry file label name and address for entry labels */
            fprintf(file_ent, "%s %07d\n", current->name, current->address);
        }
        current = current->next;
    }
    fclose(file_ent); /* Close the entry file */
}

void create_ext_file(char* file_ext_name) {
    /* Create an entry external file */
    FILE* file_ext = fopen(file_ext_name, "w");
    Label* current;

    if (file_ext == NULL) {  /* Failed to open file for writing */
        print_system_error(Error_6);
        free_all_labels();
        i_want_to_break_free_memory();
        exit(1);  /* Exiting program */
    }

    current = get_label_list_head();
    while (current != NULL) {
        if (current->type == EXTERN && current->location == CODE) { /*Write in external file label name and address for entry labels */
            fprintf(file_ext, "%s %07d\n", current->name, current->address ); 
        }
        current = current->next;
    }
    fclose(file_ext); /* Close the external file */
}


char* valid_name(char* filename) {
    /* Appending the ".as" extension */
    char* new_file_name = add_extension(filename, ".as");
    if (new_file_name == NULL)
        return NULL;
    return new_file_name;
}

char* add_extension(char* filename, char* extension) {
    char* new_filename;
    long filename_len = strlen(filename);
    long extension_len = strlen(extension);

    /* Checking if file name already has the ".as" extention */
    if (extension_len < filename_len) {
        if (strcmp(filename + filename_len - extension_len, extension) == 0) {
            print_system_error(Error_2);
            return NULL;
        }
    }
    /* Allocating memory for the new filename */
    new_filename = (char*)allocate_memory(filename_len + extension_len + 1);  /* +1 for the null terminator */
    if (new_filename == NULL)  /* Indicates memory allocation failed */
        exit(1);  /* Exiting program */

    /* Duplicate the original filename and append the specified extension */
    strcpy(new_filename, filename);
    strcat(new_filename, extension);
    return new_filename;
}

