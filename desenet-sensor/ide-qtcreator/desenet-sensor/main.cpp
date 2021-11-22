#include "platform-config.h"
#include "xf/xf.h"
#include "app/factory.h"

int main(int argc, char ** argv)
{
    XF_PREPARE;

    app::Factory factory(argc, argv);

    XF::init(10);	// Initialize XF

    // Create and initialize objects
    factory.buildApplication();

    XF::start();	// Start XF

    return 0;
}
