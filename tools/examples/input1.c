#include <stdio.h>

int main(int argc, char* argv[])
{
	printf("%d command-line arguments:\n", argc);
	for ( int index = 0; index < argc; ++index )
		printf("[%d] [%s]\n", index, argv[index]);

	printf("\nStandard input:\n");
	char ch = 0;
	while ( (ch = getchar() ) != EOF )
		putchar(ch);

	return 0;
}
