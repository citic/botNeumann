#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
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
	explicit AbstractArgument(ull number, const DataType& value)
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

class InputArgument : public AbstractArgument<char*>
{
	DISABLE_COPY_CLASS(InputArgument);

  protected:
	std::string value2;

  public:
	InputArgument(ull number, char* value)
		: AbstractArgument(number, value)
		, value2(value)
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

	virtual std::ostream& print(std::ostream& out) const override
	{
		return out << getNumber() << ": '" << value2 << "'\n";
	}

	virtual int print(FILE* file) const override
	{
		return fprintf(file, "%llu: %s\n", getNumber(), value1);
	}

	inline bool operator<(const AbstractArgument& other) const
	{
		return this->value2 < other.value2;
	}

	inline bool operator==(const AbstractArgument& other) const
	{
		return this->value2 == other.value2;
	}
};

static std::vector<InputArgument*> arguments;
InputArgument* sorted_arguments;
InputArgument global_program_name(0, "all_inclusive");
static size_t sorted_argument_count = 0;

inline bool compare(const InputArgument* arg1, const InputArgument* arg2)
{
	return *arg1 < *arg2;
}

void read_arguments()
{
	for ( int index = 1; index < sorted_argument_count; ++index )
	{
		std::string value;
		std::cin >> value;
		arguments.push_back( new InputArgument(index, value );
	}
}

void sort_arguments()
{
	std::sort(sorted_arguments, sorted_arguments + sorted_argument_count, compare);
};

template <typename Output>
void print_arguments(Output& out)
{
	for ( int index = 1; index < sorted_argument_count; ++index )
		arguments.push_back( new InputArgument(index, argv[index]) );
}

void clean_arguments()
{
	for ( int index = 0; index < arguments.size(); ++index )
		delete arguments;
}

int main(int argc, char* argv[])
{
	global_program_name.setValue2( argv[0] );
	sorted_arguments = (InputArgument*) malloc( argc * sizeof(InputArgument*) );

	try
	{
		assert(argc >= 2);
		InputArgument input_count(1, argv[1]);

		sorted_argument_count = input_count.valueToUll();

		read_arguments();

		std::thread printer1( print_arguments, stdout );

		sorted_arguments = realloc( arguments, sorted_argument_count * sizeof(InputArgument*) );
		std::thread sorter( print_arguments, sort_arguments );

		// ToDo: also use new, new[], delete, delete[]

		std::thread printer2( print_arguments, std::cout );

		free(arguments);
		return 0;
	}
	catch(std::exception& exc)
	{
		std::cerr << exc.what() << std::endl;
		return 1;
	}
}
