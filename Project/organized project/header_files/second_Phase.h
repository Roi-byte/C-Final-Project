#pragma once
#ifndef SECOND_PHASE_H
#define SECOND_PHASE_H
/*
This is the last main function in this project. If no errors are found,
it will create 3 files: .ob, .ent, and .ext. This function also calls
"code_rela_labels" and "code_operand_labels" to resolve any unresolved issues.
*/
int second_Phase(char* file_am_name, int* code, int* data, int* IC, int* DC);

/*update the unresulve lines that use metohds RELATIVE in the instrucation code*/
int code_rela_labels(char* file_am_name, int* code, int* IC);

/*update the unresulve lines that use metohd DIRECT in the instrucation code*/
int code_operand_labels(char* file_am_name, int* code, int* IC);


#endif