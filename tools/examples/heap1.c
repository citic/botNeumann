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
