#include <stdio.h>

int main()
{
	char ch = 0;
	while ( (ch = getchar() ) != EOF )
		putchar(ch);
	return 0;
}
