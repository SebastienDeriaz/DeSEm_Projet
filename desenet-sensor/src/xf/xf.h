#ifndef XF_H
#define XF_H

#include <stdint.h>
#include <stdbool.h>

//
// What is seen only by the C++ compiler
//
#ifdef __cplusplus


#ifdef TC_MESHSIM /***************************************************************************************************/

#include <QtWidgets/QApplication>

/**
 * Macro used when working with Qt (TC_MESHSIM). MUST be called in the first line in function main().
 */
#define XF_PREPARE QApplication application( argc , argv )

#elif defined(TC_QTCONSOLE)

#include <QCoreApplication>

#define XF_PREPARE QCoreApplication application( argc , argv );

#else /***************************************************************************************************************/

#define XF_PREPARE

#endif /**************************************************************************************************************/


#include "xfthread.h"

/**
 * @mainpage PTR Execution Framework Documentation
 * Link to @ref xf "XF module".
 */

/**
 * @defgroup xf XF
 * @{
 *
 * @section xf_cd Class Diagram
 * The XF (eXecution Framework) enables the implementation of complex finite state machines. The class diagram looks
 * like this:
 * @image html XF.png
 * For more details have a look into the documentation...
 *
 * - XF Version: 0.4.2 (2021-11-18)
 *  + sth: Updated doxygen file
 *  + sth: Fixed code optimised away by compiler
 * - XF Version: 0.4.1 (2021-10-25)
 *  + sth: Added support for toolchain TC_STM32CUBEIDE
 *  + sth: Removed compiler warnings
 *  + sth: Replaced tabs with spaces
 * - XF Version: 0.4.0 (2015-03-02)
 *  + sth: XFTimoutManager now returns timeouts with same value correctly
 * - XF Version: 0.3.0 (2011-04-13)
 *  + sth: Added more critical sections in XFTimeoutManager
 *  + sth: Fixed  bug in XFTimeoutManager::removeTimeouts
 * - XF Version: 0.2.0 (2011-03-07)
 *  + sth: Fixed bug in XFTimeoutManager::addTimeout method
 * - XF Version: 0.1.0 (2010-11-26)
 *  + sth: First release
 */

#ifndef NULL
#	define NULL 0
#endif

/**
 * Static class grouping the basic methods for the XF together.
 */
class XF
{
public:
    static void init(int timeInterval = 10);


#if defined(TC_STM32) || defined(TC_STM32CUBEIDE) /*******************************************************************/

    static void start();

#elif defined( TC_MESHSIM ) /*****************************************************************************************/

    static inline void start() __attribute__(( always_inline )) { QApplication::exec(); }

#elif defined( TC_QTCONSOLE ) /***************************************************************************************/

    static inline void start() __attribute__(( always_inline )) { QCoreApplication::exec(); }

#endif /**************************************************************************************************************/

    static bool isRunning();            ///< Returns true in case the XF is initialized and running.

    static XFThread * getMainThread();	///< Returns pointer the main thread.

protected:
    static bool _bInitialized;			///< Changes from false to true after calling method init(int). Used to handle multiple calls to init(int).
    static bool _isRunning;             ///< Tells if the XF is initialized and running.
    static XFThread _mainThread;		///< Main thread executing the main loop

};

#endif // __cplusplus

//
// What is seen by the C and C++ compiler
//
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void XF_init(int timeInterval);             ///< C wrapper to XF::init() method.
void XF_start();                            ///< C wrapper to XF::start() method.
bool XF_isRunning();                        ///< C function wrapping to XF::isRunning() method.

void XF_tick();                             ///< C wrapper to XFTimeoutManager::tick() method.
int32_t XF_tickIntervalInMilliseconds();    ///< C wrapper to XFTimeoutManager::tickInterval() method.

#ifdef __cplusplus
}
#endif // __cplusplus

/** @} */
#endif // XF_H
