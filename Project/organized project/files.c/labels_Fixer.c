

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "labels_Fixer.h"
#include "def.h"
#include "errors.h"
#include "constract.h"
#include "macro.h"
#include "second_Phase.h"

/* Static variables representing the head and tail of the label list */
static Label* head = NULL;
static Label* tail = NULL;

/* Getter functions for label list */
Label* get_label_list_head(void) {
    return head;  /* Returns the head of the label list */
}

Label* find_last_label(void) {
    return tail;  /* Returns the last label in the list */
}

/* Functions for adding and manipulating labels */
Label* add_new_label(char* name, int address, Type type, Location location) {
    Label* new_label = (Label*)malloc(sizeof(Label));  /* Allocating memory for a new label */
    if (new_label == NULL) {
        print_system_error(Error_1);  /* Error handling for memory allocation failure */
        return NULL;  /* Indicates failure to create label */
    }

    /* Allocate memory for label name and copy the name */
    new_label->name = (char*)malloc(strlen(name) + 1);
    if (new_label->name == NULL) {
        print_system_error(Error_1);
        free(new_label);
        return NULL;  /* Indicates failure to allocate memory for name */
    }
    strcpy(new_label->name, name);

    /* Set label properties: address, type, and location */
    new_label->address = address;
    new_label->type = type;
    new_label->location = (location != TBD) ? location : TBD;
    new_label->next = NULL;

    /* If the label list is empty, set the head and tail */
    if (head == NULL) {
        head = tail = new_label;
    }
    else {
        tail->next = new_label;  /* Append the new label to the end of the list */
        tail = new_label;  /* Update the tail to point to the new label */
    }
    return new_label;  /* Return the newly created label */
}

Label* find_label_by_name(char* label_name) {
    Label* current = head;
    while (current != NULL) {
        if (strcmp(current->name, label_name) == 0 && current->type != OPERAND) {
            return current;  /* Return the label if found and it is not of type OPERAND */
        }
        current = current->next;  /* Move to the next label */
    }
    return NULL;  /* Label not found */
}

Label* check_if_label_defined(char* label_name) {
    Label* current = head;
    while (current != NULL) {
        if (strcmp(current->name, label_name) == 0 &&
            ((current->type != EXTERN && current->location != TBD) ||
                (current->type == EXTERN && current->location == TBD))) {
            return current;  /* Return the label if it is defined correctly */
        }
        current = current->next;  /* Move to the next label */
    }
    return NULL;  /* Label not found */
}

Label* get_operand_label_node(void) {
    Label* current = head;
    while (current != NULL) {
        if (current->type == OPERAND) {
            return current;  /* Return the label of type OPERAND */
        }
        current = current->next;  /* Move to the next label */
    }
    return NULL;  /* Operand label not found */
}

/* Boolean check functions for label types */
bool check_if_entry_exists(void) {
    Label* current = head;
    while (current != NULL) {
        if (current->type == ENTRY) {
            return true;  /* Return true if an ENTRY label exists */
        }
        current = current->next;
    }
    return false;  /* No ENTRY label found */
}

bool check_if_extern_exists(void) {
    Label* current = head;
    while (current != NULL) {
        if (current->type == EXTERN) {
            return true;  /* Return true if an EXTERN label exists */
        }
        current = current->next;
    }
    return false;  /* No EXTERN label found */
}

/* Functions for updating label data */
void update_labels_with_data(int* IC) {
    Label* current = head;
    while (current != NULL) {
        if (current->location == DATA) {
            current->address += *IC;  /* Update the address for labels of type DATA */
        }
        current = current->next;  /* Move to the next label */
    }
}

int check_for_entry_labels(char* file_am_name) {
    Label* current = head;
    int errors_found = 0;

    while (current != NULL) {
        if (current->type == ENTRY && current->location == TBD) {
            printf(" [CODE_7] | ERROR | File \"%s\" | Label \"%s\"", file_am_name, current->name);
            print_system_error(Error_72);  /* Error if ENTRY label location is undefined */
            errors_found = 1;
        }
        current = current->next;  /* Move to the next label */
    }
    return errors_found;  /* Return the error count */
}

/* Functions for removing labels */
void remove_last_label_node(void) {
    if (head == NULL) return;  /* If the list is empty, do nothing */

    if (head->next == NULL) {  /* Only one label in the list */
        free(head->name);
        free(head);
        head = tail = NULL;  /* Set both head and tail to NULL */
        return;
    }

    Label* current = head;
    while (current->next->next != NULL) {
        current = current->next;  /* Move to the second last label */
    }

    free(current->next->name);  /* Free the name and memory of the last label */
    free(current->next);
    current->next = NULL;  /* Set the next pointer to NULL */
    tail = current;  /* Update the tail to the second last label */
}

void remove_specific_label(Label* label) {
    Label* current = head;
    Label* prev = NULL;

    while (current != NULL) {
        if (current == label) {
            if (prev == NULL) {
                head = current->next;  /* If the label to remove is the head, update the head */
            }
            else {
                prev->next = current->next;  /* Bypass the label to remove it */
            }
            if (current == tail) {
                tail = prev;  /* If the label to remove is the tail, update the tail */
            }
            free(current->name);  /* Free the label's name */
            free(current);  /* Free the label's memory */
            return;  /* Label removed */
        }
        prev = current;
        current = current->next;  /* Move to the next label */
    }
}

/* Function to clean up all labels */
void free_all_labels(void) {
    Label* current = head;
    Label* next;

    while (current != NULL) {
        next = current->next;  /* Store the next label */
        free(current->name);  /* Free the label's name */
        free(current);  /* Free the label's memory */
        current = next;  /* Move to the next label */
    }
    head = NULL;  /* Reset the head and tail to NULL */
    tail = NULL;
}

/* Function to validate a label name */
int valid_label_name(char* label_name, Type type, Line* line, int* errors_found) {
    char first_char;
    int i, label_name_len;
    Label* label;
    label_name_len = strlen(label_name);
    first_char = label_name[0];

    /* Check if the label is part of an entry/extern instruction */
    if (type == DEFAULT) {
        label_name[label_name_len - 1] = NULL_TERMINATOR;  /* Remove the colon from the label name */
        label_name_len -= 1;
    }

    /* Check if the label name is empty */
    if (*label_name == NULL_TERMINATOR) {
        print_syntax_error(type == DEFAULT ? Error_28 : type == OPERAND ? Error_59 : Error_43,
            line->file_am_name, line->line_num);
        *errors_found = 1;
        return 1;  /* Invalid label name */
    }

    /* Check if the first character is alphabetic */
    if (!isalpha(first_char)) {
        if (which_instr(label_name) != -1 && type == DEFAULT) {  /* Check if the label is an instruction */
            print_syntax_error(Error_26, line->file_am_name, line->line_num);
            *errors_found = 1;
            return 1;  /* Invalid label name */
        }
        print_specific_error(type == DEFAULT ? Error_29 : type == OPERAND ? Error_59 : Error_45,
            line->file_am_name, line->line_num, label_name);
        *errors_found = 1;
        return 1;  /* Invalid label name */
    }

    /* Check if the label name length is valid */
    if (label_name_len > MAX_LABEL) {
        print_specific_error(type == DEFAULT ? Error_20 : type == OPERAND ? Error_59 : Error_45,
            line->file_am_name, line->line_num, label_name);
        *errors_found = 1;
        return 1;  /* Invalid label name */
    }

    /* Check if the label contains only alphanumeric characters */
    for (i = 0; label_name[i] != NULL_TERMINATOR; i++) {
        if (!isalnum(label_name[i])) {
            print_specific_error(type == DEFAULT ? Error_21 : type == OPERAND ? Error_59 : Error_45,
                line->file_am_name, line->line_num, label_name);
            *errors_found = 1;
            return 1;  /* Invalid label name */
        }
    }

    /* Check if the label is a macro name */
    if (is_macro_name(label_name) != NULL) {
        print_specific_error(type == DEFAULT ? Error_23 : type == OPERAND ? Error_70 : Error_46,
            line->file_am_name, line->line_num, label_name);
        *errors_found = 1;
        return 1;  /* Invalid label name */
    }

    /* Check if the label name is a reserved word */
    if (is_reserved_str(line->file_am_name, label_name, line->line_num, type) != 0) {
        *errors_found = 1;
        return 1;  /* Invalid label name */
    }

    if (type == OPERAND)
        return 0;  /* Validation finished for "operand" type labels */

    /* Check if the label has already been defined */
    label = find_label_by_name(label_name);
    if (label != NULL) {
        if (type == ENTRY) {
            if (label->type == EXTERN) {
                print_syntax_error(Error_47, line->file_am_name, line->line_num);
                *errors_found = 1;
                return 1;  /* Invalid label name */
            }
            if (label->type == ENTRY) {
                printf(" WARNING | File \"%s\" at line %d | Duplicate declarations of \".entry\" or \".extern\" will be ignored\n",
                    line->file_am_name, line->line_num);
                return 1;  /* Skip this label */
            }
            label->type = ENTRY;  /* Update label type to ENTRY */
            return 1;  /* Skip this label */
        }
        if (type == EXTERN) {
            if (label->type != EXTERN) {
                print_syntax_error(Error_48, line->file_am_name, line->line_num);
                *errors_found = 1;
                return 1;  /* Invalid label name */
            }
            printf(" WARNING | File \"%s\" at line %d | Duplicate declarations of \".entry\" or \".extern\" will be ignored\n",
                line->file_am_name, line->line_num);
            return 1;  /* Skip this label */
        }
        /* Handle other label type conflicts */
        if (label->type == EXTERN) {
            print_specific_error(Error_32, line->file_am_name, line->line_num, label_name);
            *errors_found = 1;
            return 1;  /* Invalid label name */
        }
        if (label->type == DEFAULT) {
            print_specific_error(Error_27, line->file_am_name, line->line_num, label_name);
            *errors_found = 1;
            return 1;  /* Invalid label name */
        }
        if (label->type == ENTRY && label->location != TBD) {
            print_specific_error(Error_27, line->file_am_name, line->line_num, label_name);
            *errors_found = 1;
            return 1;  /* Invalid label name */
        }
        remove_specific_label(label);
        return -1;  /* Special case to create a new label with type "ENTRY" */
    }
    return 0;  /* Label name is valid */
}

/* Function to find an operand label by its address */
Label* find_label_OP_by_address(int address_og) {
    Label* current = head;
    while (current != NULL) {
        if (current->address == address_og && current->type == OPERAND) {
            return current;  /* Return the operand label found at the given address */
        }
        current = current->next;
    }
    return NULL;  /* Operand label not found */
}
