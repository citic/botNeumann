#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

char nonstatic_global_uninit;
int nonstatic_global_init = 210;
static long static_global_uninit;
static long long static_global_init = LLONG_MAX;

int main(int argc, char* argv[])
{
	static const char* static_local_uninit;
	static unsigned long long static_local_init = 3llu;

	long double nonstatic_local_uninit;
	long long* nonstatic_local_init = malloc( sizeof(long long) );

	printf("%25s: '%c'\n", "nonstatic_global_uninit", nonstatic_global_uninit);
	printf("%25s: %d\n", "nonstatic_global_init", nonstatic_global_init);
	printf("%25s: %ld\n", "static_global_uninit", static_global_uninit);
	printf("%25s: %lld\n", "static_global_init", static_global_init);
	printf("%25s: %p\n", "static_local_uninit", static_local_uninit);
	printf("%25s: %llu\n", "static_local_init", static_local_init);
	printf("%25s: %Lf\n", "nonstatic_local_uninit", nonstatic_local_uninit);
	printf("%25s: %lld\n", "nonstatic_local_init", *nonstatic_local_init);
	printf("%25s: %d\n", "nonstatic_local_argc", argc);
	printf("%25s: %p\n", "nonstatic_local_argv", argv);

	free(nonstatic_local_init);
	return 0;
}
