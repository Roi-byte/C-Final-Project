#ifndef CONSTRACT_H
#define CONSTRACT_H
#include "labels_Fixer.h"
#include <stdio.h>

typedef struct Label Label;  /* Forward declaration of Label */



/* Enum for defining the valid methods that can be used for operand addressing. */
typedef enum valid_methods {
    NO,                 /* No valid method */
    METHOD_1,           /* Single method type */
    METHODS_1_2,        /* methods 1, 2 allowed */
    METHODS_1_3,        /* methods 1, 3 allowed */
    METHODS_0_1_3       /* Combination of methods 0, 1, and 3 allowed */
} valid_methods;

/* Structure representing an operation code (opcode) and its associated properties. */
typedef struct op_code {
    char* opcode;            /* The opcode name */
    int code_num;            /* Numeric representation of the opcode in base 10 */
    int funct_num;           /* Function number related to the opcode */
    int group_operand;       /* for each opcode each group it is in */
    valid_methods source_method; /* Valid addressing methods for the source operand */
    valid_methods dest_method;   /* Valid addressing methods for the destination operand */
} op_code;

/* Enum to represent the different addressing methods that can be used. */
typedef enum Addressing_Method {
    IMMEDIATE,           
    DIRECT,              
    RELATIVE,            
    DIRECT_REGISTER      
} Addressing_Method;

/* Void memory node struct definition (void* can represent any type of variable, making the structure flexible for different types of memory allocation) */
typedef struct free_memory_Node {
    void* ptr;                    /* Pointer to the allocated memory */
    struct free_memory_Node* next; /* Pointer to the next node in the list */
} free_memory_Node;

/* Structure to represent a line */
typedef struct Line {
    FILE* file;            /* File pointer */
    char* file_am_name;    /* Name of the file */
    char* content;         /* The content in the line */
    int line_num;          /* Line number */
    Label* label;          /* Label associated with the line, if any */
} Line;


/* Function Prototypes */

/* Searches for a file open it in read mode and returns its pointer */
FILE* search_file(char* filename);

/* Changes the file extension of a file name */
char* change_extension(char* file_name, char* new_extension);

/* Removes all extra spaces from a string (include leading and closing spaces) */
char* remove_spaces(char str[]);

/* Checks if a character is a space or a tab */
int is_space_or_tab(char c);

/* Checks if the string includes whitespace */
int include_whitespace(const char* str);

/* Checks if the string is a reserved word */
int is_reserved_str(char* file_name, char* str, int line_count, Type type);

/* Checks if the line has an operation */
int is_operation(int* code, int* Usage, int* IC, Line* line, char* ptr, char* current_word, int* errors_found);

/* Identifies if the string is an opcode */
int which_opcode(char* str);

/* Validates an operation in a line */
void check_valid_operation(int* code, int* Usage, int* IC, Line* line, char* ptr, int code_id, int* errors_found);

/* Identifies the method used in the operand */
int which_method(char* operand, Line* line, int* errors_found);

/* Identifies the register from a given string */
int which_regis(char* str);

/* Frees all allocated memory and clears the linked list */
void i_want_to_break_free_memory();

/* Deallocates memory previously allocated and removes it from the linked list */
void deallocate_memory(void* ptr);

/* Checks if the line contains instruction */
int is_instruction(int* data, int* Usage, int* DC, Line* line, char* ptr, char* current_word, int* errors_found);

/* Identifies the instruction corresponding to a string */
int which_instr(char* str);

/* Handles the case when data is found in the line */
void data_found(int* data, int* Usage, int* DC, Line* line, char* ptr, int* errors_found);

/* Handles the case when a string is found in the line */
void string_found(int* data, int* Usage, int* DC, Line* line, char* ptr, int* errors_found);

/* Handles entry in a given line */
void entry_found(Line* line, char* ptr, int* errors_found);

/* Handles extern in a given line */
void extern_found(Line* line, char* ptr, int* errors_found);

/* Analyzes the numbers in the ".data" instruction line */
void analyze_numbers(int* data, int* Usage, int* DC, Line* line, char* ptr, int* errors_found);


/* Retrieves the array of operation codes (opcodes) */
op_code* get_opcodes();

/* Retrieves the numeric values from a given line as an array */
int* get_numbers(Line* line, char* ptr, int* num_count, int* errors_found);

/* Frees the memory allocated in the line struct */
void free_line(Line* line);

/* Deletes a file with the given name */
void delete_file(char* filename);

/* Checks if the given word is a standalone word */
int check_standalone_word(char* str, char* word);

/* Extracts the first word from a string. */
char* get_first_word(char* str);

/* Allocates memory and put it in a linked list */
void* allocate_memory(long size);

/* Checks if the operand addressing method is legal */
int is_legal(Line* line, int method, int code_id, int group_operand, int* errors_found);

/* Creates and returns a new Line object. */
Line* create_line(FILE* file, char* file_am_name, char* content, int line_num);

/* Creates an object file containing the machine code and data. */
void create_ob_file(char* file_ob_name, int* code, int* data, int* IC, int* DC);

/* Creates an entry file with name and address of labels of type ENTRY */
void create_ent_file(char* file_ent_name);

/* Creates an external file with name and address of labels of type EXTERN. */
void create_ext_file(char* file_ext_name);

/* Validates and returns a file name */
char* valid_name(char* filename);

/* Function to add an extension to a file name */
char* add_extension(char* filename, char* extension);

#endif
