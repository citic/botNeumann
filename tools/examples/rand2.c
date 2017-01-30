#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
	static unsigned long long seed = 3ll;
	seed = time(NULL);
	seed += clock();
	seed %= RAND_MAX;
	srand(seed);

	long long min = 0;
	long long max = RAND_MAX;

	if ( argc == 2 )
		max = strtoll(argv[1], NULL, 10);
	else if ( argc == 3 )
	{
		min = strtoll(argv[1], NULL, 10);
		max = strtoll(argv[2], NULL, 10);
	}

	long long* random = malloc( sizeof(long long) );
	*random = min + rand() % (max - min);

	printf("%lli\n", *random);

	free(random);

	return 0;
}
