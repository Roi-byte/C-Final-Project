#ifndef LABELS_FIXER_H
#define LABELS_FIXER_H


#include <stdbool.h>
#include <stdio.h>
#include "def.h"


/*forward declaration of the Line structure*/
typedef struct Line Line;

/* Enum for location types */
typedef enum { TBD, DATA, CODE } Location;
/* TBD(To Be Determined), DATA(data segment), CODE(code segment) */

/* Enum for label types */
typedef enum { ENTRY, EXTERN, OPERAND, DEFAULT } Type;
/*ENTRY(for entry point labels), EXTERN(for external labels), OPERAND(for operand labels), DEFAULT(for standard labels)*/ 

/* Label Structure */
typedef struct Label {
    char* name;              /* Name of the label (e.g., "label1") */
    int address;             /* Address associated with the label (e.g., memory location) */
    Type type;               /* Type of the label (ENTRY, EXTERN, OPERAND, DEFAULT) */
    Location location;       /* Location of the label (TBD, DATA, CODE) */
    struct Label* next;      /* Pointer to the next label in the linked list */
} Label;

/* Function prototypes */

/*
 add_new_label:
 Adds a new label to the linked list. If the list is empty, it initializes the head and tail.
 Returns a pointer to the newly created label.
 */
Label* add_new_label(char* name, int address, Type type, Location location);

/*
 find_label_by_name:
 Searches for a label by its name in the linked list.
 Returns a pointer to the label if found, or NULL if not.
 */
Label* find_label_by_name(char* label_name);

/*
 check_if_label_defined:
 Checks if a label with the specified name is defined in the list.
 Returns the label if it is defined or NULL if not.
 */
Label* check_if_label_defined(char* label_name);

/*
 check_for_entry_labels:
 Checks if there are any undefined entry labels in the list.
 Prints errors for undefined entry labels.
 Returns the number of errors found.
 */
int check_for_entry_labels(char* file_am_name);

/*
 update_labels_with_data:
 Updates the addresses of labels that are in the data segment (DATA)
 by adding the current instruction counter (IC) to their addresses.
 */
void update_labels_with_data(int* IC);

/*
 get_operand_label_node:
 Retrieves the first label of type OPERAND from the list.
 Returns a pointer to the label or NULL if not found.
 */
Label* get_operand_label_node(void);

/*
 check_if_entry_exists:
 Checks if any label of type ENTRY exists in the list.
 Returns true if such a label exists, false otherwise.
 */
bool check_if_entry_exists(void);

/*
 check_if_extern_exists:
 Checks if any label of type EXTERN exists in the list.
 Returns true if such a label exists, false otherwise.
 */
bool check_if_extern_exists(void);

/*
 get_label_list_head:
 Returns the head (first label) of the linked list.
 */
Label* get_label_list_head(void);

/*
 find_last_label:
 Returns the last label in the linked list.
 */
Label* find_last_label(void);

/*
 remove_last_label_node:
 Removes the last label from the linked list.
 If the list has only one label, it will free the head as well.
 */
void remove_last_label_node(void);

/*
 remove_specific_label:
 Removes a specific label from the list.
 The label to be removed is passed as a parameter.
 */
void remove_specific_label(Label* label);

/*
 free_all_labels:
 Frees all labels in the linked list and clears the list.
 */
void free_all_labels(void);

/*
 valid_label_name:
 Validates a label's name based on various conditions such as length, allowed characters,
 reserved words, and whether the label is already defined.
 Returns 0 if the label name is valid, 1 if it is invalid, or -1 in specific cases.
 */
int valid_label_name(char* label_name, Type type, Line* line, int* errors_found);

/*
 find_label_OP_by_address:
 Searches for an operand label (type OPERAND) with a specific address.
 Returns the label if found, or NULL if not.
 */
Label* find_label_OP_by_address(int address_og);

#endif /* LABELS_FIXER */

#pragma once

