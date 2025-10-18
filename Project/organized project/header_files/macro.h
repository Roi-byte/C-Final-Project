#ifndef MACROS_H
#define MACROS_H
/*forward declaration of the Line structure*/
typedef struct Line Line;

/*
   Definition of a Macro structure.
   "name": A pointer to a string that holds the name of the macro.
   "content": A pointer to a string that holds the content of the macro.
   "line": The line number in the source code where the macro was declared.
   "next": A pointer to the next Macro in the linked list, allowing for traversal.
*/
typedef struct Macro {
    char* name;         /* Name of the macro */
    char* content;      /* Content associated with the macro */
    int line;           /* Line number where the macro is declared */
    struct Macro* next; /* Pointer to the next macro in the linked list */
} Macro;

/*
   Function to add a new macro to the list.
   Parameters:
     "name": The name of the new macro.
     "line": The line number where the macro is declared.
   Returns:
     0 if the macro was added successfully.
     1 if there was a memory allocation failure.
*/
int add_macro(char* name, int line);


/*
   Function to check if a given name corresponds to an existing macro.
   Parameters:
    "macro_name": The name of the macro to search for.
   
   Returns:
    A pointer to the Macro structure if a match is found.
    NULL if no matching macro is found.
*/
Macro* is_macro_name(char* macro_name);


/*
   Function to add content to the most recently added macro.
   Parameters:
     "new_content": The content to be added to the macro.
   Returns:
     0 if the content was added successfully.
     1 if there was a memory allocation failure.
*/
int add_macro_content(char* new_content);


/*
   Function to remove the last macro in the list.
   This function frees the memory associated with the last macro and updates the list.
*/
void remove_last_macro();


/*
   Function to free the memory allocated for all macros in the list.
   It iterates through the list and frees the memory for each macro's name, content, and the macro structure itself.
*/
void free_macros();


/*
   Function to validate the declaration of a macro.
   Parameters:
     "file_name": The name of the file where the macro is declared.
     "decl": The declaration string to be checked.
     "line_count": The line number in the file where the declaration is located.
   Returns:
     The name of the macro if the declaration is valid.
     NULL if the declaration is invalid.
*/
char* valid_macro_decl(char* file_name, char* decl, int line_count);


/*
   Function to validate the name of a macro.
   Parameters:
     "file_name": The name of the file where the macro is declared.
     "macro_name": The name of the macro to validate.
     "line_count": The line number where the macro is declared.
   Returns:
     0 if the macro name is valid.
     1 if the macro name is invalid (e.g., too long, contains whitespace, or is a reserved word).
*/
int valid_macro_name(char* file_name, char* macro_name, int line_count);


/*
   Function to get the last macro in the linked list.
   Returns:
     A pointer to the last macro in the list.
*/
Macro* last_macro();

#endif
