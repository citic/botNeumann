#include "GdbTest.h"

int main(int argc, char *argv[])
{
	// The executable to be debugged must be provided by parameter
	if ( argc < 2 )
		return printf("Usage: GdbTest executable-path\n");

    GdbTest app(argc, argv);
    return app.run();
}
