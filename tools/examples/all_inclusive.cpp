/* Brainstorming:

redirect stdin, stdout, stderr
call main with arguments
have all types of global variables
find global variables with ctags
create variable-objects for all global variables
have a C++ struct with private/protected/public members
inherit a C++ class with constructor, virtual destructor
one global variable is an instance of sub-class
set breakpoints for all functions in all sources
set breakpoints for dynamic memory management: malloc/calloc/realloc/free
has three threads
have variable-objects for stdin/stdout/stderr
use new, new[], delete, delete[]
throw an exception
has a template
overloads two operators (free function and member function)

When a function is called list arguments, then locals
Always exec-next and exec-finish, do not exec-step (into, out)
After each exec-next:
If stopped for range (still in the same function):
	List frame variables (stack-list-variables 2)
	Update all variable-objects
If stopped in a function with symbols
	Create function calls and do as same with main()
If stopped in dynamic memory function
	Store size: -stack-list-arguments 2
	Call exec-finish
	Store return address
	If memory function called by user code
		Animate memory creation
		Call exec-next to go back to user code

If execution-thread created
	Animate it
	Ask for thread-info
If execution-thread finished
	Destroy it

If any variable-object changed for io
	Animate stdin/out/err consuming/generating chars
	Animate robot reading or writing
	Check if output matches expected ouput and turn on the output tester
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* program_name = NULL;

int main(int argc, char* argv[])
{
	long long* argc_copy = (long long*) malloc( sizeof(long long) );
	if ( argc_copy == NULL )
		return 1;

	*argc_copy = argc;

	if ( ( program_name = calloc(strlen(argv[0]) + 1, sizeof(char)) ) == NULL )
		return 2;

	strcpy(program_name, argv[0]);

	printf("%lli: %s\n", *argc_copy, program_name);

	free(argc_copy);

	// Memory leak:
	// free(program_name);

	return 0;
}
