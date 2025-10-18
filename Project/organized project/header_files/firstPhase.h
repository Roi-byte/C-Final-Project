#pragma once
#ifndef firstPhase_h
#define firstPhase_h 
#include "labels_Fixer.h"
#include <stdio.h>


/*
 * This function runs the first phase of the assembler.
 * It reads the given file, identifies instructions, operations, and labels, 
 * and begins converting them into machine code while checking for errors.
 * If errors are found, it stops the process. Otherwise, it proceeds to the second phase.
 */
int firstPhase(char* file_name);

/*
* This function reads the given file line by line to process its content.
 * It identifies instructions, operations, and labels, converting them into code
 * while updating the instruction and data counters
 * It also calls the scan_word function to analyze each line in detail.
 */
int scan_text(char* file_am_name, int* code, int* data, int* IC, int* DC);


/*
 * This function analyzes each word in a line to determine if it's an instruction,
 * operation, or label. It then processes and converts them into code or data and put them in the array if they are 
 */
void scan_word(int* code, int* data, int* Usage, int* IC, int* DC, Line* line, int* errors_found);



#endif
