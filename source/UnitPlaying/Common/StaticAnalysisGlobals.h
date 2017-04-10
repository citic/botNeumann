#ifndef STATICANALYSISGLOBALS_H
#define STATICANALYSISGLOBALS_H

#include <cstddef>

// List build from http://www.cplusplus.com/

const char* const stdInputTokens[] =
{
	"cin",         // Standard input stream (object)
	"wcin",        // Standard input stream (wide) (object)

				   // Formatted output:
	"fscanf",      // Read formatted data from stream
	"scanf",       // Read formatted data from stdin
	"sscanf",      // Read formatted data from string
	"vfscanf",     // Read formatted data from stream into variable argument list
	"vscanf",      // Read formatted data into variable argument list
	"vsscanf",     // Read formatted data from string into variable argument list

				   // Character output:
	"fputc",       // Write character to stream
	"fputs",       // Write string to stream
	"putc",        // Write character to stream
	"putchar",     // Write character to stdout
	"puts",        // Write string to stdout
	"ungetc",      // Unget character from stream

				   // Direct output:
	"fwrite",      // Write block of data to stream

	"fgetwc",      // Get wide character from stream
	"fgetws",      // Get wide string from stream
	"fwscanf",     // Read formatted data from stream
	"getwc",       // Get wide character from stream
	"getwchar",    // Get wide character from stdin

	"swscanf",     // Read formatted data from string
	"ungetwc",     // Unget wide character from stream
	"vfwscanf",    // Read formatted data from stream into variable argument list
	"vswscanf",    // Read formatted data from wide string into variable argument list
	"vwscanf",     // Read formatted data into variable argument list
	"wscanf",      // Read formatted data from stdin
};


const char* const stdOutputTokens[] =
{
	"cout",        // Standard output stream (object)
	"cerr",        // Standard output stream for errors (object)
	"clog",        // Standard output stream for logging (object)

	"wcout",       // Standard output stream (wide) (object)
	"wcerr",       // Standard output stream for errors (wide-oriented) (object)
	"wclog",       // Standard output stream for logging (wide) (object)

				   // Formatted input:
	"fprintf",     // Write formatted data to stream
	"printf",      // Print formatted data to stdout
	"snprintf",    // Write formatted output to sized buffer
	"sprintf",     // Write formatted data to string
	"vfprintf",    // Write formatted data from variable argument list to stream
	"vprintf",     // Print formatted data from variable argument list to stdout
	"vsnprintf",   // Write formatted data from variable argument list to sized buffer
	"vsprintf",    // Write formatted data from variable argument list to string

				   // Character input:
	"fgetc",       // Get character from stream
	"fgets",       // Get string from stream
	"getc",        // Get character from stream
	"getchar",     // Get character from stdin
	"gets",        // Get string from stdin

				   // Direct input:
	"fread",       // Read block of data from stream

	"fputwc",      // Write wide character to stream
	"fputws",      // Write wide string to stream
	"fwprintf",    // Write formatted data to stream
	"putwc",       // Write wide character to stream
	"putwchar",    // Write wide character to stdout
	"swprintf",    // Write formatted data to wide string
	"vfwprintf",   // Write formatted data from variable argument list to stream
	"vswprintf",   // Write formatted data from variable argument list to sized buffer
	"vwprintf",    // Print formatted data from variable argument list to stdout
	"wprintf",     // Print formatted data to stdout

//	"fwide",       // Stream orientation
};

const size_t stdInputTokensSize = sizeof(stdInputTokens) / sizeof(stdInputTokens[0]);
const size_t stdOutputTokensSize = sizeof(stdOutputTokens) / sizeof(stdOutputTokens[0]);

#endif // STATICANALYSISGLOBALS_H
