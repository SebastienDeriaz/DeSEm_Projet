- ``platform_config.h`` (``ide-stm32cubeide\platform\nucleo-stm32l476rg\platform-config.h``)
  - Changement du ``DESENET_SLOT_NUMBER``
- ``factory.h`` (``src\app``)
  - Ajout du joystick (plus include pour ``joystickapplication.h``)
- ``factory.cpp`` (``src\app``)
  - Include de ``joystick.h``
  - Initialisation du joystick (+ ``setObserver``)
  - Start Joystick / joystickApplication
  - méthodes ``joystickApplication()`` et ``joystick()``
- ``abstractapplication.cpp`` (``src\common\mdw\desenet\sensor``)
  - Ajout de ``subscribeToSvGroup()`` dans ``svPublishRequest``
  - Ajout de ``eventReceived()`` dans ``evPublishRequest``
- ``abstractapplication.h`` (``src\common\mdw\desenet\sensor``)
  - Ajout ``const`` a l'event id
- ``net.cpp`` (``src\common\mdw\desenet\sensor``)
  - Ajout de ``slotNumber`` à l'initialisation de ``networkentity``
- ``joystick.cpp`` (``src\common\platform\nucleo-stm32l476rg\board``)
  - Ajout du namespace ``board``
- ``joystick.h`` (``src\common\platform\nucleo-stm32l476rg\board``)
  - Ajout du namespace ``board``
- ``joystick.cpp`` (``src\common\platform\qt-meshsim\board``)
  - Ajout du namespace ``board``
- ``joystick.h`` (``src\common\platform\qt-meshsim\board``)
  - Ajout du namespace ``board``
- ``networkentity.h`` (``src\common\mdw\desenet\sensor``)
  - Ajout de l'include pour ``mpdu.h``
  - Héritage de ``ITimeSlotManager`` + Ajout de la méthode virtuelle ``onTimeSlotSignal``
  - Ajout de paramètre ``slotNumber`` à initalize
  - Ajout des méthodes ``subscribeToSvGroup`` et ``unsubscribe``
  - Ajout de la méthode ``eventReceived``
  - Liste des applications et struct ``AppBind``
- ``networkentity.cpp`` (``src\common\mdw\desenet\sensor``)
  - Ajout de ``slotNumber`` dans l'initialisation
  - Initialisation des relations de ``pTimeSlotManager``
  - Complétion de la méthode ``onReceive``
  - Ajout de la méthode virtuelle ``onTimeSlotSignal``
  - Ajout des méthodes ``subscribeToSvGroup`` et ``unsubscribe``
  - Ajout de la méthode ``eventReceived``