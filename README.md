# Caustic Lasertag System
For Russian description scroll down the page.

Описание на русском --- внизу страницы.

## Overview
Caustic project is a full-featured outdoor and indoor wireless lasertag system. It consists of principial schemes, PCBs and firmware for devices used in lasertag game. For now, system consists of:
  - Player's gun
  - Player's sensor to detect shoots
  - Smart control point for 'capture the point' game and some other things
  - Bluetooth bridge that provide interconnection between your smartphone and all the devices in game near bridge
  - Android application (currently alpha state) to control the game and change settings

Caustic Lasertag System licensed under GPLv3. Third party code used in project may be licensed under other licences, see corresponding files and comments in the sources. Copyright 2016, see AUTHORS file.

## Project structure
Here is a directories description:
  - `Android/` --- Android application to control the game and change settings 
  - `cpp-sources/` --- MCU firmware. Here:
     - `bootloader-usb/` is a reference to https://github.com/DAlexis/stm32-usb-bootloader --- bootloader that supports cardreader functionality and flash controller from file `flash.bin`
     - `universal-device/` --- universal firmware for any Caustic LTS device.  
  - `Docs/` --- some datasheets and useful documents for project
  - `media/` --- sounds and its raws. Most of them were get from https://www.freesound.org/
  - `schematic/` --- electic schemes, PCBs and gerbers made with KiCad
  - `tools/` --- some scripts to simplify primitive jobs
  - `sdcards/` --- samples of devices' SD cards containment

## How to build firmware
Now only Linux OS supported to build Caustic LTS firmware, but I use `CMake` so add Windows support should not be a very complicated task.

You need to install `cmake`, `arm-none-eabi-gcc` compiler **with** `arm-none-eabi-newlib` and `arm-none-eabi-binutils` installed, and installation dirs should be in `$PATH`. Simply installation from your distro's repository usually enough. To flash the MCU with ST-link programmer you should install `stlink` utility. Then:

  - Clone this repository:
    
    `git clone https://github.com/DAlexis/caustic-lasertag-system.git`
  - Go to `cpp-sources` directory:
    
    `cd caustic-lasertag-system/cpp-sources`
  - Download git submodule containing bootloader:
    
    `git submodule init && git submodule update`
  - Run `build.sh` for debug build and `build.sh release` for release one

Then, to program the board:
  - Connect ST-link programmer to the board and to yout PC's USB
  - To flash main firmware run
  
    `flash-universal-device.sh`
  - To flash bootloader run
  
    `flash-bootloader.sh`
    To flash release version add `release` parameter to both scripts

    **NOTE:** flashing bootloader means removing previous firmware. To flash main program after flashing bootloader you should rename `universal-device.bin` to `flash.bin` and copy it to sd-card's root, then reboot device.

  - Then, copy proper sh-card containment to your device's sd-card. For example, for gun you should copy all files and dirs from `sdcards/rifle` to gun's sd-card.


# Лазертаг-система Caustic
Caustic --- это полноценная беспроводная система для внеаренного и аренного лазертага. Проект содержит принципиальные схемы, печатные платы, прошивки микроконтроллера и другой необходимый код. На данный момент, система включает:
  - Оружие игрока
  - Датчики попадания ("головная повязка") для игрока
  - Умная контрольная точка для игры в "захват точки", поддерживающая другие вспомогательные функции
  - Bluetooth-мост, позволяющий подключать смартфон одновременно к любому количеству устройств системы
  - Приложение для Android (на данный момент в альфа-версии) для управления игрой и настройки игровых устройств

Система Caustic лицензирована под GPLv3. Система включает код тертьих лиц, который может быть лицензирован под другими лицензиями. Copyright 2016, Алексей Булатов. 

## Структура проекта
Проект устроен следующим образом:
  - `Android/` --- Приложение для Android 
  - `cpp-sources/` --- Прошивка для контроллера
     - `bootloader-usb/` --- ссылка на https://github.com/DAlexis/stm32-usb-bootloader --- загрузчик, превращающий устройство в кардридер, если при запуске оно подключено к компьютеру по USB. Прошивает контроллер из файла `flash.bin`
     - `universal-device/` --- универсальная прошивка для любого устройства системы Caustic
  - `Docs/` --- даташиты, документация и некоторые полезные документы
  - `media/` --- звуки и их исходники. по большей части, взяты с https://www.freesound.org/
  - `schematic/` --- схемы, печатные платы, герберы, сделанные в KiCad
  - `tools/` --- некоторые скрипты, упрощающие некоторые простые действия
  - `sdcards/` --- содержимое SD-карт для устройств разных типов

## Как собрать прошивку и загрузчик
На данный момент поддерживается сборка только из-под Linux, однако проект использует `CMake`, так что добавить поддержку Windows не составит труда.

У вас дожен быть установлен `cmake`, компилятор `arm-none-eabi-gcc`, **включая** `arm-none-eabi-newlib` и `arm-none-eabi-binutils`. Компилятор должен быть доступен в `$PATH`. Обычно, достаточно просто установить соответствующие пакеты из репозитория вашего дистрибутива. Для прошивки нужен программатор ST-link и соответствующая утилита `stlink`. 

Загрузчик **не является** обязательным.

Итак, для установки:

  - Склонируйте этот репозиторий:
    
    `git clone https://github.com/DAlexis/caustic-lasertag-system.git`
  - Перейдите в папку `cpp-sources`:
    
    `cd caustic-lasertag-system/cpp-sources`
  - Инициализируйте и загрузите подмодуль, содержащий загрузчик:
    
    `git submodule init && git submodule update`
  - Запустите `build.sh` для сборки в режиме build, или `build.sh release` для сборки в release

Теперь, чтобы запрограммировать устройство:
  - Подключите программатор к плате устройства и к компьютеру. Включите питание устройства
  - Чтобы прошить основную программу, наберите
  
    `flash-universal-device.sh`
  - Чтобы прошить загрузчик:
  
    `flash-bootloader.sh`
    Чтобы прошить программу, собранную в release, добавьте параметр `release` к каждому из скриптов.

    **ВАЖНО:** При прошивке загрузчика стирается всё, что до этого было зашито в контроллер. Поэтому после прошивки загрузчика нужно сделать следующее: переименовать `universal-device.bin` в `flash.bin` и положить в корень sd-карты (это можно сделать, просто подключив устройство по USB к компьютеру). После перезагрузки устройства загрузчик автоматически зальет прошивку в контроллер. Вообще говоря, можно вообще не использовать загрузчик.

  - Скопируйте необходимое содержимое на sd-карту. Например, если устройство --- это оружие игрока, скопируйте всё содержимое `sdcards/rifle` на карту устройства. Именно так прошивка поймёт, "кем быть".
