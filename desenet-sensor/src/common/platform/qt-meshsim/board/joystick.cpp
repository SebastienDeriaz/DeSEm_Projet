#include "interfaces/ijoystickobserver.h"
#include "app/factory.h"
#include "meshsimboard.h"
#include "joystick.h"

Joystick::Joystick()
{
    _bInitialized = false;
    _pObserver = NULL;
}

Joystick::Joystick(const Joystick &)
{
    _bInitialized = false;
    _pObserver = NULL;
}

MeshSimBoard & Joystick::meshSimBoard() const
{
    return app::Factory::instance().meshSimBoard();
}

bool Joystick::initialize()
{
    if (!_bInitialized)
    {
        _bInitialized = true;

        meshSimBoard().initialize();

        return true;
    }
    return false;
}

bool Joystick::setObserver(IJoystickObserver * observer)
{
    return meshSimBoard().setObserver(observer);
}

void Joystick::start()
{

}

//static
Joystick & Joystick::instance()
{
    static Joystick joystick;

    return joystick;
}

IJoystick::Position Joystick::position() const
{
    return meshSimBoard().position();
}

