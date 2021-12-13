#include "platform-config.h"
#include "main.h"
#include "board/transceiver/nrftransceiver.h"
#include "board/joystick.h"
#include "board/accel/accelerometer.h"
#include "desenet/timeslotmanager.h"

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin)
    {
    case NRF_IRQ_Pin:   // nRF24L01 IRQ
#if (EXCLUDE_BOARD_NRFTRANSCEIVER == 0)
        // Call transceivers interrupt reception method
        board::NrfTransceiver::instance().onIrq();
#endif
        break;
    case NAVS_UP_Pin:
        //no break;
    case NAVS_DOWN_Pin:
        //no break;
    case NAVS_LEFT_Pin:
        //no break;
    case NAVS_RIGHT_Pin:
        //no break;
    case NAVS_CENTER_Pin:
        board::Joystick::instance().onIrq(GPIO_Pin);
        break;
    case ACCEL_IRQ1_Pin:
        //no break;
    case ACCEL_IRQ2_Pin:
        board::Accelerometer::onIrq(GPIO_Pin);
        break;
    default:
        break;
    }
}

extern "C" void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef * htim)
{
    if (htim->Instance == TIM2)
    {
        TimeSlotManager::onIrq();
    }
}
