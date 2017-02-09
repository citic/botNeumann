#ifndef bn_c_generator_h
#define bn_c_generator_h

// Global variables and functions to easy file generation of test cases in C 11

#include <stdio.h>
#include <stdlib.h>

int           bn_testcase_number = -1;
int           bn_testcase_count = -1;
const char*   bn_input_filename = NULL;
const char*   bn_output_ex_filename = NULL;
const char*   bn_error_ex_filename = NULL;
const char*   bn_args_filename = NULL;
FILE*         bn_input = NULL;
FILE*         bn_output_ex = NULL;
FILE*         bn_error_ex = NULL;
FILE*         bn_args = NULL;
int           bn_last_error = 0;

#define bn_fail(code, ...) return bn_last_error = 1 + 0 * fprintf(stderr, __VA_ARGS__)

int bn_open_files(int argc, char* argv[])
{
	if ( argc < 7 )
		bn_fail(1, "%s: received %d arguments, expected 7\n", argv[0], argc);

	bn_testcase_number = atoi( argv[1] );
	if ( bn_testcase_number <= 0 )
		bn_fail(2, "%s: test case number (%d) must be positive\n", argv[0], bn_testcase_number);

	bn_testcase_count = atoi( argv[2] );
	if ( bn_testcase_count < bn_testcase_number )
		bn_fail(3, "%s: test case number (%d) out of count (%d)\n", argv[0], bn_testcase_number, bn_testcase_count);

	bn_input_filename = argv[3];
	bn_input = fopen(bn_input_filename, "w");
	if ( ! bn_input )
		bn_fail(4, "%s: could not create input file: %s\n", argv[0], bn_input_filename);

	bn_output_ex_filename = argv[4];
	bn_output_ex = fopen(bn_output_ex_filename, "w");
	if ( ! bn_output_ex )
		bn_fail(5, "%s: could not create expected output file: %s\n", argv[0], bn_output_ex_filename);

	bn_error_ex_filename = argv[5];
	bn_error_ex = fopen(bn_error_ex_filename, "w");
	if ( ! bn_error_ex )
		bn_fail(6, "%s: could not create expected error file: %s\n", argv[0], bn_error_ex_filename);

	bn_args_filename = argv[6];
	bn_args = fopen(bn_args_filename, "w");
	if ( ! bn_args )
		bn_fail(7, "%s: could not create arguments file: %s\n", argv[0], bn_args_filename);

	return 0;
}

int bn_close_files()
{
	return fclose(bn_input) + fclose(bn_output_ex) + fclose(bn_error_ex) + fclose(bn_args);
}

#endif
