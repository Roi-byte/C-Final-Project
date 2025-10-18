
#include <stdio.h>
#include <stdlib.h>
#include "def.h"
#include "errors.h"
#include "constract.h"
#include "firstPhase.h"
#include "second_Phase.h"


int second_Phase(char* file_am_name, int* code, int* data, int* ICF, int* DCF) {
    char* file_ob_name, * file_ent_name, * file_ext_name;
    int errors_found = 0;
    /* Checking if all "entry" labels were defined */
    if (check_for_entry_labels(file_am_name) != 0) {
        errors_found = 1;  /* Indicates faliure */
    }
    /* Handling uncoded label addresses */
    update_labels_with_data(ICF);

    if (code_rela_labels(file_am_name, code, ICF) != 0) {
        free_all_labels();
        i_want_to_break_free_memory();
        return 1;  /* Indicates faliure */
    }

    if (code_operand_labels(file_am_name, code, ICF) != 0) {
        free_all_labels();
        i_want_to_break_free_memory();
        return 1;  /* Indicates faliure */
    }
    

    /* Getting the object file name */
    file_ob_name = change_extension(file_am_name, ".ob");

    /* Creating the object file */
    create_ob_file(file_ob_name, code, data, ICF, DCF);

    /* Creating "file.ent" if there are "entry" labels */
    if (check_if_entry_exists()) {
        file_ent_name = change_extension(file_am_name, ".ent");
        create_ent_file(file_ent_name);
        deallocate_memory(file_ent_name);
    }
    /* Creating "file.ext" if there are "extern" labels */
    if (check_if_extern_exists()) {
        file_ext_name = change_extension(file_am_name, ".ext");
        create_ext_file(file_ext_name);
        deallocate_memory(file_ext_name);
    }
    deallocate_memory(file_ob_name);
    free_all_labels();
    printf("* Second pass was successful :)\n");
    return errors_found;
}

/*for the labels define later in the file that in method relation this calculate the distance*/
int code_rela_labels(char* file_am_name, int* code, int* ICF) {
    int errors_found = 0, i = 100, j = 100, temp, flag =0, tem;
    Label* opernad_label_real, * label;
    int word = 0;

    /* Looping through code array */
    while (i <= *ICF) {
        while (j <= CAPACITY && j <= *ICF && (code[j] & RELATIVE_FLAG) != RELATIVE_FLAG) /* Searching for uncoded label addresses signaled by the first pass */
            j++;
       
        temp = code[j] >> 3;/*temp is address of the label*/
        
        opernad_label_real = find_label_OP_by_address(temp);
        if (opernad_label_real == NULL) {
            i = 100, j = 100;
            return errors_found;  /* Indicates no more labels of type "operand" left */
        }

        if ((label = check_if_label_defined(opernad_label_real->name)) != NULL) {  /* Checking if this label was defined */
            if (label->type == 1 || label->type == 0) { /*checking label not extern or entry (not allowed in instreaction)*/
                print_system_error(Error_47);
                free_all_labels();
                i_want_to_break_free_memory();
                exit(1);  /* Exiting program */
            }
            tem = label->address;
            word |= (tem - (temp-1)) << 3; /* Calculation distance per instruction and moving calculation distance to correct position  */
            word |= ABSOLUTE_FLAG; /*Setting absolute flag in correct position*/
            code[j] = word;  /* Updating machine code */
            remove_specific_label(opernad_label_real);
        }
        else {
            print_specific_error(Error_69, file_am_name, code[j] >> UP_FLAG, opernad_label_real->name);
            errors_found = 1;  /* Indicates faliure */
        }
        remove_specific_label(opernad_label_real); /*making sure we are moving on all of the labels*/
        word = 0;  /* Resetting word */
        i++;
        j++;
    }
    i = 100, j = 100;
    return errors_found;
}



int code_operand_labels(char* file_am_name, int* code, int* ICF) {
    int errors_found = 0, i = 100, j = 100;
    Label* opernad_label, * label;
    int word = 0;

    /* Looping through code array */
    while (i <= *ICF) {
        while (j <= CAPACITY && j <= *ICF && (code[j] & UP_FLAG) != UP_FLAG) /* Searching for uncoded label addresses signaled by the first pass */
            j++;

        opernad_label = get_operand_label_node();  /* Put the next label of type "operand" */
        if (opernad_label == NULL) {
            return errors_found;  /* Indicates no more labels of type "operand" left */
        }

        if ((label = check_if_label_defined(opernad_label->name)) != NULL) {  /* Checking if this label was defined */
            word |= (label->address) << MOVE_ADDRESS;
      
            if (label->type == EXTERN && label->location == TBD) {  /* Indicates operand label is of type "extern" */
                word |= EXTERNAL_FLAG;  /* Setting bit 0 for "External" */

                if (add_new_label(label->name, opernad_label->address, EXTERN, CODE) == NULL) {  /* Indicates memory allocation failed */
                    free_all_labels();
                    i_want_to_break_free_memory();
                    exit(1);  /* Exiting program */
                }
            }
            else if (label->type != EXTERN) {  /* Indicates operand label is of type "entry" */
                word |= RELOCAT_FLAG;  /* Setting bit 2 for "Relocatable" */
            }

            code[j] = word;  /* Updating assemblr code */
        }
        else {
            print_specific_error(Error_69, file_am_name, code[j] >> UP_FLAG, opernad_label->name);
            errors_found = 1;  /* Indicates faliure */
        }
        remove_specific_label(opernad_label); /*making sure we are moving on all of the labels*/
        word = 0;  /* Resetting word */
        i++;
        j++;
    }
    return errors_found;
}


