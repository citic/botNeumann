#ifndef bn_cpp_generator_h
#define bn_cpp_generator_h

// Global variables and functions to easy file generation of test cases in C++ 11

#include <cstdlib>
#include <fstream>

#define bn_fail(code, msg) { std::cerr << argv[0] << ": " << msg << std::endl; return bn_last_error = code; }

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
			bn_fail(1, "received " << argc << " arguments, expected 7");

		testcase_number = atoi( argv[1] );
		if ( testcase_number <= 0 )
			bn_fail(2, "test case number must be positive: " << testcase_number);

		testcase_count = atoi( argv[2] );
		if ( testcase_number > testcase_count )
			bn_fail(3, "test case number (" << testcase_number << ") greater than count (" << testcase_count << ")");

		input_filename = argv[3];
		input.open(input_filename);
		if ( ! input )
			bn_fail(4, "could not create input file: " << input_filename);

		output_ex_filename = argv[4];
		output_ex.open(output_ex_filename);
		if ( ! output_ex )
			bn_fail(5, "could not create expected output file: " << output_ex_filename);

		error_ex_filename = argv[5];
		error_ex.open(error_ex_filename);
		if ( ! error_ex )
			bn_fail(6, "could not create expected error file: " << error_ex_filename);

		args_filename = argv[6];
		args.open(args_filename);
		if ( ! args )
			bn_fail(7, "could not create arguments file: " << args_filename);

		return 0;
	}

	int close_files()
	{
		input.close();
		output_ex.close();
		error_ex.close();
		args.close();
		return 0;
	}

	inline bool operator!() const
	{
		return last_error != 0;
	}
};

#endif
