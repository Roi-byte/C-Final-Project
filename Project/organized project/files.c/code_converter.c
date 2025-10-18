


#include "code_converter.h"
#include <stdio.h>
#include <stdlib.h>
#include "def.h"
#include "errors.h"
#include "labels_Fixer.h"
#include "macro.h"
#include "constract.h"



void add_data_code(int* data, int* DC, int number) {
    /* Extract 24-bit 2's complement of `number` using MASK_24BIT (0xFFFFFF). */
   int word = (int)(number & MASK_24BIT);

    /* Adding the code to the data array */
    data[*DC] = word;
    (*DC)++;  /* Incrementing data count */
}


void add_instruction_code(int* code, int* Usage, int* IC, int word, int* errors_found) {
    /* Checking if memory limit (2^21) was reached */
    if (*Usage == CAPACITY) {
        print_system_error(Error_73);
        *errors_found = 1;
        (*Usage)++;  /* Incrementing usage count so the next iteration will not print another error message */
        return;  /* Scanning line finished */
    }
    if (*Usage > CAPACITY) {  /* Checking if memory limit was exceeded */
        return;  /* Scanning line finished */
    }
    /* Adding the code to the code array */
    code[*IC] = word;
    (*IC)++;  /* Incrementing data count */
    *Usage += 1;  /* Incrementing usage count */
}



void handle_one_operand(int* code, int* Usage, int* IC, Line* line, int method, char* operand, int code_id, int* errors_found) {
    int word, reg; 
    word = reg = 0; /*Set word and register to 0*/
    op_code* opcodes = get_opcodes();
    if (method == 3) { /*Update register of relevant*/
        if (which_regis(operand) != -1)
            reg = which_regis(operand);
    }
    /*Setting opcode and absolute flag in correct position*/
    word |= (opcodes[code_id].code_num << MOVE_OPCODE_POS) | ABSOLUTE_FLAG;
    word |= (opcodes[code_id].funct_num << MOVE_FUNC); /*Setting funct in correct position*/
    word |= (reg << MOVE_REG_DST); /*Setting destination register in correct position (if register is not relevant 0 will be put)*/
    word |= (method <<  MOVE_OPCODE_DST);  /* Setting detination sorting type in the correct position */
    add_instruction_code(code, Usage, IC, word, errors_found);  /* Adding word into the code array */

    /* Handling the second word */
    process_code(code, Usage, IC, line, method, operand, code_id, errors_found);
}



void process_code(int* code, int* Usage, int* IC, Line* line, int method, char* operand, int code_id, int* errors_found) {
    int word = 0, temp = 0;
    op_code* opcodes = get_opcodes();
   

    /* Handling the word */
    switch (method) {
    case IMMEDIATE:
        operand++;  /* Skipping the 'HASH' sign */
        word |= ABSOLUTE_FLAG;  /* Setting absolute flag in correct position*/
        temp |= atoi(operand);
        word |= temp << MOVE_FUNC;  
        add_instruction_code(code, Usage, IC, word, errors_found);  /* Adding word into the code array (second word) */
        return;  /* Scanning line finished */
    case DIRECT: 
        if (add_new_label(operand, *IC, OPERAND, TBD) == NULL) {  /* Indicates memory allocation failed */
            fclose(line->file);
            free_line(line);
            free_all_labels();
            free_macros();
            i_want_to_break_free_memory();
            exit(1);  /* Exiting program */
        }
        word |= 3;  /* Setting bits 0 and 1 to signal to the "second Phase" that it need to update this label address */
        word |= (line->line_num) << 3;  /* Setting bits 3-24 to represent the line number for a potential error message in the "second pass"*/ 
        add_instruction_code(code, Usage, IC, word, errors_found);  /* Adding word into the code array (second word) */
        return;  /* Scanning line finished */
    case RELATIVE:  
        if (strcmp(opcodes[code_id].opcode, "jmp") != 0 && /*check if this is according to instruction if not print error and return*/
            strcmp(opcodes[code_id].opcode, "bne") != 0 &&
            strcmp(opcodes[code_id].opcode, "jsr") != 0) {
            print_system_error(Error_57);
            *errors_found = 1;
            return; /* Scanning line finished */
        }
        operand++;  /* Skipping the 'AMPERSAND' sign */
        Label* temp_label = find_label_by_name(operand); 
        if (temp_label == NULL) { /* Checking if label is not in the system already */
            if (add_new_label(operand, *IC, OPERAND, TBD) == NULL) {  /* Indicates memory allocation failed */
                fclose(line->file);
                free_line(line);
                free_all_labels();
                free_macros();
                i_want_to_break_free_memory();
                exit(1);  /* Exiting program */
            }

            word |= 7;  /* Setting bits 0,1,2 to signal to the "second Phase" to update this label address for sorting type "RELATIVE" */
            word |= *IC << 3;  /* Setting bits 3-24 to represent the address for so we can take care of it in the second Phase"*/ 
            add_instruction_code(code, Usage, IC, word, errors_found); /* Adding word into the code array (second word) */
            return; /* Scanning line finished */
        } /* If we enter here label is in the system already */
        if (strcmp(temp_label->name, operand) != 0 || temp_label->type == 1 || temp_label->type == 0) { /*checking label not extern or entry (not allowed in instreaction) and word = label*/
            print_system_error(Error_47);
            *errors_found = 1;
            return; /* Scanning line finished */
        }
        temp = (temp_label->address) - ((*IC)-1); /* calculation distance per instruction */
        word |= ABSOLUTE_FLAG; /* Setting absolute flag in correct position*/
        word |= temp << 3; /*moving calculation distance to correct position */
        add_instruction_code(code, Usage, IC, word, errors_found);  /* Adding word into the code array (second word) */
        return;  /* Scanning line finished */
    case DIRECT_REGISTER: /* if register should not add any line */
        return; /* Scanning line finished */
    }
}



void handle_two_operands(int* code, int* Usage, int* IC, Line * line, char* operand, char* second_operand, int code_id, int* errors_found) {
    int word = 0, first_reg = 0, second_reg = 0;
    op_code* opcodes = get_opcodes();
    
    int method = which_method(operand, line, errors_found);
    if (method == 3) { /*Update register of relevant*/
        if (which_regis(operand) != -1)
            first_reg = which_regis(operand);
    }
    int method_2 = which_method(second_operand, line, errors_found);
    if (method_2 == 3) { /*Update register of relevant*/
        if (which_regis(second_operand) != -1)
            second_reg = which_regis(second_operand);
    }
    word |= (opcodes[code_id].code_num << MOVE_OPCODE_POS) | ABSOLUTE_FLAG; /*Setting opcode and absolute flag in correct position*/
    word |= (opcodes[code_id].funct_num << MOVE_FUNC); /*Setting funct in correct position*/
    word |= (first_reg << MOVE_REG_SRC);  /*Setting origin register in correct position (if register is not relevant 0 will be put)*/
    word |= (second_reg << MOVE_REG_DST); /*Setting destination register in correct position (if register is not relevant 0 will be put)*/
    word |= (method << MOVE_OPERAND_SRC);  /* Setting origin sorting type in the correct position */
    word |= (method_2 << MOVE_OPCODE_DST);  /* Setting detination sorting type in the correct position */
    add_instruction_code(code, Usage, IC, word, errors_found);  /* Adding word into the code array */


   
    /* Handling the second word */
    process_code(code, Usage, IC, line, method, operand, code_id, errors_found);
    /* Handling the third word */
    process_code(code, Usage, IC, line, method_2, second_operand, code_id, errors_found);  
}
