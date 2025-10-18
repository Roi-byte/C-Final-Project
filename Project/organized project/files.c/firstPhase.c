

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "errors.h"
#include "def.h"
#include "constract.h"
#include "labels_Fixer.h"
#include "macro.h"
#include "firstPhase.h"
#include "second_Phase.h"




int firstPhase(char* file_name) {

    /* Allocate memory for code and data arrays in size 2^21 */
    int* code = (int*)calloc(CAPACITY, sizeof(int)); /*Allocating memory using calloc for zero initialization */
    int* data = (int*)calloc(CAPACITY, sizeof(int));
    int IC = 100;   /* Instruction Counter starts from 100 */
    int DC = 0; /* Data Counter starts from 0 */
    char* file_new_name = change_extension(file_name, ".am");

    /* Process the file and scan its content */

    if (scan_text(file_new_name, code, data, &IC, &DC) != 0) {
        free_all_labels();
        free_macros();
        i_want_to_break_free_memory();
        return 1;  /* Indicates failure */
    }
    free_macros();  /* Macros are no longer needed */

    printf("* First phase completed successfully :)\n"); 

    /* Proceed to the second phase */
    if (second_Phase(file_new_name, code, data, &IC, &DC) != 0) {
        free_all_labels();
        i_want_to_break_free_memory();
        return 1;  /* Indicates failure */
    }
    deallocate_memory(file_new_name);
    return 0;  /* Indicates success */
}

int scan_text(char* file_am_name, int* code, int* data, int* IC, int* DC) {
    char temp[MAX_LINE_LENGTH + 1];  /* Buffer to store each line, +1 for null terminator */
    int Usage = 0, errors_found = 0, line_count = 0;
    char* trimmed_line;
    Line* line;

    FILE* file_am = fopen(file_am_name, "r");
    if (file_am == NULL) {  /* Check if the file opened successfully */
        print_system_error(Error_5);
        free_macros();
        i_want_to_break_free_memory();
        exit(1);  /* Terminate program due to file error */
    }
    /* Read the file line by line */
    while (fgets(temp, MAX_LINE_LENGTH + 1, file_am)) {
        line_count++;

        /* Check if the current line is a comment */
        if (temp[0] == COMMENT)
            continue;  /* Skip to the next line */

        /* Remove leading and trailing and any extra whitespaces */
        trimmed_line = remove_spaces(temp);

        /* Check if the current line is empty */
        if (strlen(trimmed_line) == 0)
            continue;  /* Skip to the next line */

        line = create_line(file_am, file_am_name, trimmed_line, line_count);
        if (line == NULL) {
            fclose(file_am);
            free_all_labels();
            free_macros();
            i_want_to_break_free_memory();
            exit(1);  /* Terminate program due to memory allocation failure */
        }
        scan_word(code, data, &Usage, IC, DC, line, &errors_found);
        free_line(line);
    }
    fclose(file_am);
    return errors_found;
}


 

void scan_word(int* code, int* data, int* Usage, int* IC, int* DC, Line * line, int* errors_found) {
    char* ptr = line->content;
    char* current_word, * temp;
    int curr_word_len, len, res;
    Label* label;

    /* Extracting the first word from the line */
    current_word = get_first_word(line->content);
    if (current_word == NULL) {  /* Memory allocation failure (previous allocations freed within the function) */
        fclose(line->file);
        free_line(line);
        exit(1);  /* Exiting program */
    }
    curr_word_len = strlen(current_word);

    /* Checking if the first word is a label definition (ends with ':') */
    if (current_word[curr_word_len - 1] == COLON) { 
        res = valid_label_name(current_word, DEFAULT, line, errors_found);
        if (res == 0) {
            label = add_new_label(current_word, 0, DEFAULT, TBD);  /* Adding default label to the list */
        }
        else if (res == -1) {  /* Special case */
            label = add_new_label(current_word, 0, ENTRY, TBD);  /* Adding entry label to the list */
        }
        else {
            deallocate_memory(current_word);
            return;  /* Stop processing this line */
        }
        if (label == NULL) {  /* Memory allocation failure */
            fclose(line->file);
            free_line(line);
            free_all_labels();
            free_macros();
            i_want_to_break_free_memory();
            exit(1);  /* Exiting program */
        }
        line->label = label;  /* Updating struct line label pointer */
        deallocate_memory(current_word);  /* Free no longer needed memory */
    }
    /* Scanning the next word */
    if (line->label != NULL) {
        if (include_whitespace(ptr)) {
            while (*ptr != NULL_TERMINATOR && !isspace(*ptr))  /* Skip over the label */
                ptr++;
            while (*ptr != NULL_TERMINATOR && isspace(*ptr))  /* Move to the next word */
                ptr++;
            /* Extract the next word */
            current_word = get_first_word(ptr);
            if (current_word == NULL) {  /* Memory allocation failure (previous allocations freed within the function) */
                fclose(line->file);
                free_line(line);
                exit(1);  /* Exiting program */
            }
        }
        else {
            ptr[strlen(ptr) - 1] = NULL_TERMINATOR;  /* Remove colon (:) for error message formatting */
            print_syntax_error(Error_22, line->file_am_name, line->line_num);
            *errors_found = 1;
            return;  /* Stop processing this line */
        }
    }
    /* Checking for a potential instruction, it is sorted by the struct and methods in constract */
    if (is_instruction(data, Usage, DC, line, ptr, current_word, errors_found) != 0) { 
        deallocate_memory(current_word);
        return;  /* Scanning line finished */
    }

    /* Checking for a potential operation, it is sorted by the struct and methods in constract */
    if (is_operation(code, Usage, IC, line, ptr, current_word, errors_found) != 0) {
        deallocate_memory(current_word);
        return;  /* Scanning line finished */
    }

    /* Handling special cases */
    if (is_macro_name(current_word) != NULL) {   /* Invalid macro call */
        print_specific_error(Error_33, line->file_am_name, line->line_num, current_word);
        *errors_found = 1;
        deallocate_memory(current_word);
        return;  /* Stop processing this line */
    }
    if (strchr(current_word + 1, COLON) != NULL) {  /* Invalid label declaration */
        print_specific_error(Error_65, line->file_am_name, line->line_num, current_word);
        *errors_found = 1;
        deallocate_memory(current_word);
        return;  /* Stop processing this line */
    }
    while (ptr && !isspace(*ptr))  /* Skip first word */
        ptr++;
    while (ptr && isspace(*ptr))  /* Skip whitespace */
        ptr++;
    if (ptr && *ptr == COLON) {  /* Invalid label declaration */
        print_specific_error(Error_66, line->file_am_name, line->line_num, current_word);
        *errors_found = 1;
        deallocate_memory(current_word);
        return;  /* Stop processing this line */
    }
    if (find_label_by_name(current_word) != NULL) {  /* Checking for a label at the start of the line */
        print_specific_error(Error_71, line->file_am_name, line->line_num, current_word);
        *errors_found = 1;
        deallocate_memory(current_word);
        return;  /* Stop processing this line */
    }
    len = strlen(current_word) + 2;  /* +1 for dot, +1 for null-terminator */
    temp = (char*)allocate_memory(len);
    if (temp == NULL) {
        fclose(line->file);
        free_line(line);
        exit(1);  /* Stop processing this line */
    }
    temp[0] = DOT;  /* Adding the dot at the beginning */
    strcpy(temp + 1, current_word);  /* Copying the word after the dot */
    if (which_instr(temp) != -1) {
        print_specific_error(Error_67, line->file_am_name, line->line_num, current_word);
        *errors_found = 1;
        deallocate_memory(temp);
        deallocate_memory(current_word);
        return;  /* Stop processing this line */
    }
    deallocate_memory(temp);
    print_specific_error(Error_68, line->file_am_name, line->line_num, current_word);
    *errors_found = 1;
    deallocate_memory(temp);
    deallocate_memory(current_word);
}


