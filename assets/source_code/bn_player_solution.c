#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

FILE* bn_reopen_stdin()  { return freopen("./input.txt", "r", stdin ); }
FILE* bn_reopen_stdout() { return freopen("./output.txt", "w", stdout ); }
FILE* bn_reopen_stderr() { return freopen("./error.txt", "w", stderr ); }

long bn_tell_stdin()  { return ftell(stdin);  }
long bn_tell_stdout() { return ftell(stdout); }
long bn_tell_stderr() { return ftell(stderr); }

#if defined(__cplusplus)
}
#endif
