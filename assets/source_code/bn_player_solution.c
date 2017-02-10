#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

const char* const bn_player_dir = ".";
const char* bn_build_path(const char* file_type, int test_case)
{
	static char buffer[1024];
	sprintf(buffer, "%s/bn_%02d_%s.txt", bn_player_dir, test_case, file_type);
	return buffer;
}

FILE* bn_reopen_stdin(int test_case)  { return freopen(bn_build_path("input", test_case), "r", stdin ); }
FILE* bn_reopen_stdout(int test_case) { return freopen(bn_build_path("output_ps", test_case), "w", stdout ); }
FILE* bn_reopen_stderr(int test_case) { return freopen(bn_build_path("error_ps", test_case), "w", stderr ); }

long bn_tell_stdin()  { return ftell(stdin);  }
long bn_tell_stdout() { return ftell(stdout); }
long bn_tell_stderr() { return ftell(stderr); }

#if defined(__cplusplus)
}
#endif
