#pragma once
#ifndef INITIALIZE_H
#define INITIALIZE_H


/*

This function is responsible for initializing the macros in a given file
It takes the `file_name` as input, changes its extension to `.am`, and then calls `handle_macros` to process the macros in the file
If any issues are found during the processing, the function frees any allocated memory and terminates with a failure status (returning 1). If successful, it prints a completion message and returns 0
*/
int initialize(char* file_name);


/*

This function handles the parsing and processing of macros in the input file
It reads the file line by line, checks for macro declarations and macro calls, and processes them accordingly
It also handles errors related to invalid macro syntax or undefined macros. If any errors are found, the function returns a non-zero value to indicate failure
If the processing is successful, it generates a new file (`file_am_name`) with the macro content included and returns 0. It ensures that macro definitions are properly handled and avoids issues such as missing or mismatched macro ends
*/
int handle_macros(char* file_name, char* file_am_name);

#endif
