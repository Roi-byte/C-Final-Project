#pragma once
#ifndef CODE_COVERTER_H
#define CODE_COVERTER_H
#include "constract.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "errors.h"
#include "labels_Fixer.h"
#include "code_converter.h"
#include "macro.h"
#include "firstPhase.h"
#include "initialize.h"


/*
 Adds an instruction word to the code array and updates the instruction counter (IC).
 Also checks for errors and updates the Usage number.
 */
void add_instruction_code(int* code, int* Usage, int* IC, int word, int* errors_found);

/*
 Adds a data word to the data array and updates the data counter (DC).
 */
void add_data_code(int* data, int* DC, int number);

/*
 Processes a single-operand instruction: validates the operand, encodes it, and updates the code array.
 Updates the instruction counter (IC) and checks for errors (it update code array and IC counter with add_instruction_code)
 */
void handle_one_operand(int* code, int* Usage, int* IC, Line* line, int method, char* operand, int code_id, int* errors_found);

/*
 Processes an instruction based on the method encodes the instruction and operands,
 updates the code array (it update code array and IC counter with add_instruction_code), and checks for errors.
 */
void process_code(int* code, int* Usage, int* IC, Line* line, int method, char* operand, int operands_num, int* errors_found);

/*
 Processes a two-operand instruction: validates both operands, encodes them, and updates the code array.
 Updates the instruction counter (IC) (it update code array and IC counter with add_instruction_code) and checks for errors.
 */
void handle_two_operands(int* code, int* Usage, int* IC, Line* line, char* operand, char* second_operand, int code_id, int* errors_found);

#endif
