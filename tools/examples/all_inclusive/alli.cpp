// All Inclusive: a convoluted program to use many features of C/C++
// The program considers lines in standard input as command line arguments,
// and prints all arguments sorted alphabetically in lowercase.

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#define DISABLE_COPY_CLASS(Class) \
	Class(const Class&) = delete; \
	Class(Class&&) = delete; \
	Class& operator=(const Class&) = delete; \
	Class& operator=(Class&&) = delete

typedef unsigned long long ull;

template <typename DataType>
struct AbstractArgument
{
	DISABLE_COPY_CLASS(AbstractArgument);

  private:
	ull number = ULLONG_MAX;

  protected:
	DataType value1;

  public:
	explicit AbstractArgument(ull number, DataType value)
		: number(number)
		, value1(value)
	{
	}

	virtual ~AbstractArgument() { }
	inline ull getNumber() const { return number; }
	virtual std::ostream& print(std::ostream& out) const = 0;
	virtual int print(FILE* file) const = 0;
};

template <typename DataType>
inline std::ostream& operator<<(std::ostream& out, const AbstractArgument<DataType>& arg)
{
	return arg.print(out);
}

class InputArgument : public AbstractArgument<const char*>
{
	DISABLE_COPY_CLASS(InputArgument);

  protected:
	std::string value2;

  public:
	InputArgument(ull number, const char* value)
		: AbstractArgument(number, value)
		, value2( std::string(value) )
	{
		std::transform(value2.begin(), value2.end(), value2.begin(), ::tolower);
	}

	inline void setValue2(const std::string& value2)
	{
		this->value2 = value2;
	}

	inline ull valueToUll() const
	{
		return std::stoull(value2);
	}

	virtual bool operator<(const InputArgument& other) const
	{
		return this->value2 < other.value2;
	}

	virtual std::ostream& print(std::ostream& out) const override
	{
		return out << getNumber() << "\t" << value2 << '\n';
	}

	virtual int print(FILE* file) const override
	{
		return fprintf(file, "%llu\t%s\n", getNumber(), value1);
	}
};

static std::vector<InputArgument*> all_arguments;
char** buffer = nullptr;
ull input_count;
InputArgument** sorted_arguments;
static size_t sorted_count = 0;
InputArgument global_program_name(0, "all_inclusive");

void read_arguments(ull input_count)
{
	buffer = (char**) calloc( input_count, sizeof(char*) );
	static const size_t buffer_size = 128;
	for ( ull index = 0; index < input_count; ++index )
	{
		buffer[index] = static_cast<char*>( malloc(buffer_size * sizeof(char) ) );
		fgets(buffer[index], buffer_size, stdin);
		size_t len = strlen(buffer[index]);
		buffer[index][len - 1] = '\0';
		buffer[index] = reinterpret_cast<char*>( realloc(buffer[index], len + 1) );
		all_arguments.push_back( new InputArgument( all_arguments.size(), buffer[index] ) );
	}

	::input_count = input_count;
}

void print_arguments_1()
{
	for ( size_t index = 0; index < all_arguments.size(); ++index )
		all_arguments[index]->print( stdout );
}

void print_arguments_2()
{
	std::cout << std::endl;
	for ( ull index = 0; index < sorted_count; ++index )
		std::cout << *sorted_arguments[index];
}

inline bool compare(const InputArgument* arg1, const InputArgument* arg2)
{
	return *arg1 < *arg2;
}

void sort_arguments()
{
	sorted_arguments = new InputArgument*[ all_arguments.size() ];
	for ( sorted_count = 0; sorted_count < all_arguments.size(); ++ sorted_count )
		sorted_arguments[sorted_count] = all_arguments[sorted_count];

	std::sort(sorted_arguments, sorted_arguments + sorted_count, compare);
}

void delete_arguments()
{
	for ( ull index = 0; index < all_arguments.size(); ++index )
		delete all_arguments[index];

	for ( ull index = 0; index < input_count; ++index )
		free( buffer[index] );

	free( buffer );

	delete [] sorted_arguments;
}

int main(int argc, char* argv[])
{
	global_program_name.setValue2( argv[0] );
	sorted_arguments = (InputArgument**) malloc( argc * sizeof(InputArgument*) );

	try
	{
		for ( int index = 0; index < argc; ++index )
			all_arguments.push_back( new InputArgument(index, argv[index]) );

		assert(argc >= 2);
		InputArgument input_count(1, argv[1]);
		read_arguments( input_count.valueToUll() );

		std::thread printer1( print_arguments_1 );
		std::thread sorter( sort_arguments );

		printer1.join();
		sorter.join();
		std::thread printer2( print_arguments_2 );

		printer2.join();
		delete_arguments();

		return 0;
	}
	catch(std::exception& exc)
	{
		std::cerr << "Exception: " << exc.what() << std::endl;
		return 1;
	}
}
