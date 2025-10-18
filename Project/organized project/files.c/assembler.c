

#include <stdio.h>
#include "errors.h"
#include "initialize.h"
#include "firstPhase.h"
#include "def.h"
#include "constract.h"

 

int main(int argc, char* argv[]) {
    int i;
    FILE* file;
    char* file_name;
    
   if (argc < 2) { /* Checking if no files were entered */
        print_system_error(Error_0);
        return 1;  /* Indicates faliure */
    }
   /* Scanning files */
    for (i = 1; i < argc; i++) {
        file_name = valid_name(argv[i]);  
        if (file_name == NULL)
            continue;  /* Skipping to the next file */

        file = search_file(file_name);
        if (file == NULL)
            continue; /* Skipping to the next file */

        printf("\nProcessing file: \"%s\"\n", file_name); 

        
        /* Starting initialize procses */
        if (initialize(file_name) != 0) {
            printf("Process Failed :(\n");
            continue;   
        }
        /* Starting first pass */
        if (firstPhase(file_name) != 0) {  
        printf("Process Failed :(\n");
         continue;  /* Skipping to the next file */
        }
        printf("Process ended :)\n"); 
        
        i_want_to_break_free_memory(); 
       
    }
    return 0;  /* Indicates success */
}
