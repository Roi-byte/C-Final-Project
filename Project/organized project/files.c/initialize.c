

#include "initialize.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "errors.h"
#include "macro.h"
#include "constract.h"


int initialize(char* file_name) {
    /* Change the extension of the file name to ".am" */
    char* file_am_name = change_extension(file_name, ".am");

    /* Handle all macro declarations and calls in the file */
    if (handle_macros(file_name, file_am_name) != 0) {
        free_macros();  /* Free all allocated macros */
        i_want_to_break_free_memory();  /* Perform any additional memory cleanup */
        return 1;  /* Indicate failure */
    }
    deallocate_memory(file_am_name);  /* Free the allocated memory for file name */
    printf("* Initializing completed :)\n");
    return 0;  /* Indicate success */
}

int handle_macros(char* file_name, char* file_am_name) {
    char* macro_name, * trimmed_line;
    char line[MAX_LINE_LENGTH + 1], copy[MAX_LINE_LENGTH + 1];  /* +1 to accommodate '\0' */
    int errors_found = 0, macro_found = 0, line_count = 0, name_is_valid = 0, decl_line, line_length, ch;
    FILE* file, * file_am;
    Macro* macro_ptr;

    /* Open the input file for reading */
    file = fopen(file_name, "r");
    if (file == NULL) {  /* If the file cannot be opened */
        print_system_error(Error_5);
        i_want_to_break_free_memory();  /* Free any allocated memory before exiting */
        exit(1);  /* Exit the program */
    }
    /* Open the output file for writing */
    file_am = fopen(file_am_name, "w");
    if (file_am == NULL) {  /* If the output file cannot be opened */
        print_system_error(Error_6);
        fclose(file);  /* Close the input file */
        i_want_to_break_free_memory();  /* Free any allocated memory before exiting */
        exit(1);  /* Exit the program */
    }

    /* Process the file line by line */
    while (fgets(line, MAX_LINE_LENGTH + 1, file)) {
        line_count++;
        line_length = strlen(line);

        /* Check if the line length exceeds the maximum allowed */
        if (line_length == MAX_LINE_LENGTH && line[MAX_LINE_LENGTH - 1] != '\n') {
            if (check_standalone_word(line, "mcro") != 0) {
                if (macro_found == 0) {
                    macro_found = 1;  /* Start tracking a new macro */
                }
                else {
                    macro_found = 0; 
                }
            }
            print_syntax_error(Error_7, file_name, line_count);  /* Syntax error due to line length */
            errors_found = 1;
            while ((ch = fgetc(file)) != '\n' && ch != EOF);  /* Skip the rest of the line */
            continue;  /* Skip to the next line */
        }

        /* Skip the line if it's a comment */
        if (*line == COMMENT) {
            if (errors_found == 0)
                fputs(line, file_am);  /* Copy the comment to the output file */
            continue;  /* Skip to the next line */
        }

        /* Copy the line to preserve it for later use */
        strcpy(copy, line);
        trimmed_line = remove_spaces(line);  /* Remove leading and trailing spaces */

        /* Check if the line starts with an illegal comment */
        if (*trimmed_line == COMMENT) {
            print_syntax_error(Error_3, file_name, line_count);  /* Syntax error for illegal comment */
            errors_found = 1;
            continue;  /* Skip to the next line */
        }

        /* If the line matches a macro name, write its content to the output file */
        if ((macro_ptr = is_macro_name(trimmed_line)) != NULL) {
            if (errors_found == 0) {
                fputs(macro_ptr->content, file_am);  /* Write the macro content */
                fputs("\n", file_am);  /* Add a newline */
            }
            continue;  /* Skip to the next line */
        }

        /* Check if the line matches the "mcroend" keyword and process accordingly (there will always be a macroend for each macro, as per the instructions. We just need to handle the line on which it appears)*/
        if (macro_found == 1) {
            if (check_standalone_word(trimmed_line, "mcroend") == 0) {
                if (name_is_valid == 1 && add_macro_content(copy) != 0) {  /* Add macro content if valid */
                    fclose(file);
                    fclose(file_am);
                    delete_file(file_am_name);  /* Delete the output file in case of failure */
                    free_macros();  /* Free all allocated macros */
                    i_want_to_break_free_memory();  /* Free any additional memory */
                    exit(1);  /* Exit the program */
                }
                continue;  /* Skip to the next line */
            }

            /* Handle any errors related to the "mcroend" keyword */
            if (strlen(trimmed_line) > ENDMACR_LEN) {
                print_syntax_error(Error_15, file_name, line_count);  /* Syntax error for too long of a line */
                if (name_is_valid == 1)
                    remove_last_macro();  /* Remove the last added macro */
                errors_found = 1;
                macro_found = 0;  /* End macro tracking */
                name_is_valid = 0;  /* Reset validity of the macro */
                continue;  /* Skip to the next line */
            }

            /* Check if the macro content is empty and handle errors */
            if (name_is_valid == 1) {
                if (last_macro()->content == NULL || strlen(remove_spaces(last_macro()->content)) == 0) {
                    print_syntax_error(Error_17, file_name, line_count);  /* Error for empty content */
                    remove_last_macro();  /* Remove the last added macro */
                    errors_found = 1;
                }
            }
            name_is_valid = 0;
            macro_found = 0;  /* End macro tracking */
            continue;  /* Skip to the next line */
        }

        /* Check for a potential macro declaration */
        if (check_standalone_word(trimmed_line, "mcro") == 0) {
            if (errors_found == 0)
                fputs(copy, file_am);  /* Copy the macro declaration to the output file */
            continue;  /* Skip to the next line */
        }

        /* If we've reached this point, we've found a valid macro declaration */
        macro_found = 1;
        decl_line = line_count;

        /* Validate the macro declaration */
        if (strlen(trimmed_line) > MACR_LEN) {
            macro_name = valid_macro_decl(file_name, trimmed_line, line_count);  /* Validate macro name */
            if (macro_name) {
                if (is_macro_name(macro_name) != NULL) {  /* Check if the macro is already defined */
                    print_syntax_error(Error_14, file_name, line_count);  /* Error for duplicate macro name */
                    errors_found = 1;
                    name_is_valid = 0;
                    continue;  /* Skip to the next line */
                }
                /* Add the new macro to the linked list */
                if (add_macro(macro_name, decl_line) != 0) {  /* Error if memory allocation fails */
                    fclose(file);
                    fclose(file_am);
                    delete_file(file_am_name);  /* Delete the output file in case of failure */
                    free_macros();  /* Free all allocated macros */
                    i_want_to_break_free_memory();  /* Free any additional memory */
                    exit(1);  /* Exit the program */
                }
            }
            else {
                errors_found = 1;
                continue;  /* Skip to the next line */
            }
        }
        else {
            print_syntax_error(Error_8, file_name, line_count);  /* Error for invalid macro declaration length */
            errors_found = 1;
            macro_found = 1;  /* Continue tracking macro */
            name_is_valid = 0;  /* Reset validity */
            continue;  /* Skip to the next line */
        }
        name_is_valid = 1;
    }

    /* Close the files after processing */
    fclose(file);
    fclose(file_am);

    /* Delete the output file if errors were found */
    if (errors_found != 0)
        delete_file(file_am_name);

    return errors_found;  /* Return the errors that we found found */
}

