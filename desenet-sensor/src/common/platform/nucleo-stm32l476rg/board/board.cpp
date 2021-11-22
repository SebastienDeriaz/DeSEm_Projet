#include "platform-config.h"
#include "trace/trace.h"
#include "board.h"

void board::initialize()
{
#if (USE_TRACE != 0)
	Trace::initialize();
#endif // USE_TRACE
}

