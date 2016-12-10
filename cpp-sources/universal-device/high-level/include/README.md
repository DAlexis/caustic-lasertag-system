# Directories structure
For Russian description translate this text and make pull-request, no problem :)

We are in `cpp-sources/universal-device` directory. Here is directories structure desription:
This is "shared" directories used by any kind of lasertag device:
- `system-config/` -- C headers used to configure middleware and standard libraries. Contains system libs dependencies, not used anywhere except system part.
- `hal/` -- declaration of interfaces to manipulate MCU hardware (IO pins, SPI, sound, ...). This interfaces are thrivial and simply wrap standard libraries.
- `hw/` -- realization of interfaces from `hal/`. Files from this directory should not be included anywhere, all access to objects provided over interfaces in `hal` and some `extern` variables/
- `dev/` -- "devices", ready-to-use blocks that are providing more complicated functionality than interfaces from `hal/`. For example, classes like `WavPlayer`, `Random`, `RC552Wrapper`, `RGBLeds`. Some of them may contains third-party code or be a wrappers of it. Third-party code shoud be moved to `Middlewares/Third_Party/`
- `core/` -- OS wrappers, system initialization code (except HW-specific), some utils (TODO: move it to `utils/`?)
- `network/` -- network layer for radio transmission
- `rcsp/` -- Remote Control And Synchronization Protocol -- all things related to this this protocol usage
- `ir/` -- all about infrared receiver. TODO: Class `MilesTag2Transmitter` should be replaces by `IRPresentationTransmitterMT2`
- `utils/` -- some utils. Marcos, workarounds to fix some problems and other.
- `communication/` -- now contains one .hpp file with head sensor -- rifle communication helping classes
- `device/` -- base classes used by any kind of complete device
- `game/` -- generic game control, statistics and other. Now not ready, work in process

This directories related to concrete types of devices
- `bluetooth-bridge/` -- logic for bluetooth bridge
- `rifle/` -- logic for lasertag gun
- `head-sensor/` -- logic for player's head sensor
- `smart-point/` -- logic for smart point


