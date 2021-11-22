#include <cstdio>
#include <cstdarg>
#include "trace/Trace"

void Trace::initialize()
{
}

void Trace::outln( const char * const format , ... )
{
	va_list arguments;

	va_start( arguments , format );

	// Print to the stderr output.
	vfprintf( stderr , format , arguments );
	fprintf( stderr , "\n" );

	va_end( arguments );
}


void Trace::outToString( char * destination , const char * const format , ... )
{
	va_list arguments;

	va_start( arguments , format );

	// Print to buffer.
	vsprintf( destination , format , arguments );

	va_end( arguments );
}


void Trace::outToString( char * destination , size_t size , const char * const format , ... )
{
	va_list arguments;

	va_start( arguments , format );

	// Print to buffer.
	vsnprintf( destination , size , format , arguments );

	va_end( arguments );
}
