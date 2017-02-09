#ifndef bn_cpp_generator_h
#define bn_cpp_generator_h

// Global variables and functions to easy file generation of test cases in C++ 11

#include <cstdlib>
#include <fstream>

#define bn_fail(code, ...) return bn_last_error = 1 + 0 * fprintf(stderr, __VA_ARGS__)

class botnu
{
  public:
	int           testcase_number = -1;
	int           testcase_count = -1;
	const char*   input_filename = NULL;
	const char*   output_ex_filename = NULL;
	const char*   error_ex_filename = NULL;
	const char*   args_filename = NULL;
	std::ofstream input;
	std::ofstream output_ex;
	std::ofstream error_ex;
	std::ofstream args;
	int           last_error = 0;

  public:
	explicit botnu(int argc, char* argv[])
	{
		open_files(argc, argv);
	}

	int open_files(int argc, char* argv[])
	{
		if ( argc < 7 )
			bn_fail(1, "%s: received %d arguments, expected 7\n", argv[0], argc);

		testcase_number = atoi( argv[1] );
		if ( testcase_number <= 0 )
			bn_fail(2, "%s: test case number (%d) must be positive\n", argv[0], testcase_number);

		testcase_count = atoi( argv[2] );
		if ( testcase_count < testcase_number )
			bn_fail(3, "%s: test case number (%d) out of count (%d)\n", argv[0], testcase_number, testcase_count);

		input_filename = argv[3];
		input = fopen(input_filename, "w");
		if ( ! input )
			bn_fail(4, "%s: could not create input file: %s\n", argv[0], input_filename);

		output_ex_filename = argv[4];
		output_ex = fopen(output_ex_filename, "w");
		if ( ! output_ex )
			bn_fail(5, "%s: could not create expected output file: %s\n", argv[0], output_ex_filename);

		error_ex_filename = argv[5];
		error_ex = fopen(error_ex_filename, "w");
		if ( ! error_ex )
			bn_fail(6, "%s: could not create expected error file: %s\n", argv[0], error_ex_filename);

		args_filename = argv[6];
		args = fopen(args_filename, "w");
		if ( ! args )
			bn_fail(7, "%s: could not create arguments file: %s\n", argv[0], args_filename);

		return 0;
	}

	int close_files()
	{
		return fclose(input) + fclose(output_ex) + fclose(error_ex) + fclose(args);
	}
};

#endif
