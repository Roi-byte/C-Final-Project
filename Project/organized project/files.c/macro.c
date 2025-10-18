

#include <ctype.h>
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "errors.h"
#include "constract.h"
#include "labels_Fixer.h"


static Macro* head = NULL;  /* Pointer to the first macro in the linked list */
static Macro* tail = NULL;  /* Pointer to the last macro in the linked list */

int add_macro(char* name, int line) {
    Macro* last_macro;

    Macro* new_macro = (Macro*)malloc(sizeof(Macro));
    if (new_macro == NULL) {
        print_system_error(Error_1);
        return 1;  /* Memory allocation failed */
    }
    /* Allocate memory for the macro's name and copy the provided name */
    new_macro->name = (char*)malloc(strlen(name) + 1);  /* +1 to include the null-terminator */
    if (new_macro->name == NULL) {
        print_system_error(Error_1);
        free(new_macro);  /* Free previously allocated memory for the macro */
        return 1;  /* Memory allocation failed */
    }
    strcpy(new_macro->name, name);

    /* Initialize the content pointer to NULL */
    new_macro->content = NULL;

    /* Set the line number for the macro and initialize the next pointer to NULL */
    new_macro->line = line;
    new_macro->next = NULL;

    /* If the list is empty, make this the first node */
    if (head == NULL) {
        head = new_macro;
        tail = new_macro;  /* First macro in the list is also the tail */
    }
    else {
        tail->next = new_macro;  /* Link the new macro to the last node */
        tail = new_macro;  /* Update the tail to the new macro */
    }
    return 0;  /* Macro added successfully */
}

Macro* is_macro_name(char* macro_name) {
    Macro* current = head;

    while (current != NULL) {
        if (strcmp(current->name, macro_name) == 0) {
            return current;  /* Macro name found, return pointer to its node */
        }
        current = current->next;
    }
    return NULL;  /* Macro name not found */
}

int add_macro_content(char* new_content) {
    Macro* current;
    int current_length, new_content_length, total_length;
    char* new_memory;

    current = tail;  /* Get the last macro in the list */

    /* If the macro already has content, calculate its length, otherwise set it to 0 */
    if (current->content != NULL) {
        current_length = strlen(current->content);
    }
    else {
        current_length = 0;
    }

    /* Calculate the total length needed for the new content */
    new_content_length = strlen(new_content);
    total_length = current_length + new_content_length + 1;  /* +1 for the null-terminator */

    /* Reallocate memory for the new content */
    new_memory = realloc(current->content, total_length);
    if (new_memory == NULL) {
        print_system_error(Error_1);
        return 1;  /* Memory reallocation failed */
    }

    current->content = new_memory;

    /* If the current content is empty, initialize it with a null-terminator */
    if (current_length == 0) {
        current->content[0] = NULL_TERMINATOR;
    }

    /* Append the new content to the existing content */
    strcat(current->content, new_content);

    return 0;  /* Content added successfully */
}


void remove_last_macro() {
    Macro* current;

    if (head->next == NULL) {  /* If there's only one macro in the list */
        free(head->name);
        if (head->content)  /* If content exists, free it */
            free(head->content);
        free(head);
        head = NULL;
        return;
    }
    current = head;

    /* Traverse the list to find the second-to-last macro */
    while (current->next->next != NULL) {
        current = current->next;
    }

    free(current->next->name);
    if (current->next->content)  /* Free content if it exists */
        free(current->next->content);
    free(current->next);

    /* Set the second-to-last macro as the new last macro */
    current->next = NULL;
}

void free_macros() {
    Macro* current = head;
    Macro* next;

    /* Traverse through the list and free each macro */
    while (current != NULL) {
        next = current->next;  /* Save the next macro in the list */

        free(current->name);  /* Free the memory for the macro's name */
        free(current->content);  /* Free the memory for the macro's content */
        free(current);  /* Free the macro itself */

        current = next;  /* Move to the next macro in the list */
    }
    head = NULL;  /* Set head to NULL, indicating the list is empty */
}





char* valid_macro_decl(char* file_name, char* decl, int line_count) {
    char* macro_name;

    /* Check if the declaration starts with "macr" */
    if (strncmp(decl, "mcro", MACR_LEN) == 0 && isspace(decl[MACR_LEN])) {
        decl += MACR_LEN;  /* Move past "macr" to get the macro name */
        macro_name = remove_spaces(decl);

        if (valid_macro_name(file_name, macro_name, line_count) != 0)  /* Validate the macro name */
            return NULL;  /* Invalid macro name */
    }
    else {
        print_syntax_error(Error_9, file_name, line_count);
        return NULL;  /* Syntax error: invalid macro declaration */
    }
    return macro_name;  /* Return the valid macro name */
}

int valid_macro_name(char* file_name, char* macro_name, int line_count) {
    /* Check if the macro name length exceeds the maximum allowed length */
    if (strlen(macro_name) > MAX_MACRO) {
        print_syntax_error(Error_16, file_name, line_count);
        return 1;  /* Invalid macro name: exceeds max length */
    }
    if (include_whitespace(macro_name)) {
        print_syntax_error(Error_13, file_name, line_count);
        return 1;  /* Invalid macro name: contains whitespace */
    }
    /* Compare the macro name with reserved words to ensure it's not a system keyword */
    if (is_reserved_str(file_name, macro_name, line_count, DEFAULT) != 0)
        return 1;  /* Invalid macro name: reserved word */

    return 0;  /* Valid macro name */
}

Macro* last_macro() {
    return tail;  /* Return the last macro in the list */
}

