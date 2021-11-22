#include <QMutex>
#include "critical.h"


volatile unsigned char bInISR = 0;

static QMutex mutex(QMutex::Recursive);

void enterCritical()
{
    mutex.lock();
}

void exitCritical()
{
    mutex.unlock();
}
