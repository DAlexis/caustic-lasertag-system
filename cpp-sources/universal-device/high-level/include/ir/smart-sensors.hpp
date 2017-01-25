/*
 * smart-sensors.hpp
 *
 *  Created on: 22 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR_SMART_SENSORS_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR_SMART_SENSORS_HPP_

#include "ir/ir-physical.hpp"
#include "ir/ir-presentation.hpp"
#include "hal/uart.hpp"
#include "core/os-wrappers.hpp"
#include "ssp-master.h"
#include <map>

/**
 * This class represents physical layer for one smart sensor
 */
class SmartSensorPhysical : public IRReceiverBase
{
public:
    void setEnabled(bool enabled) override;
    void init() override;
    void interrogate() override;

    void addMessage(const SSP_IR_Buffer& message);

private:
    SSP_IR_Buffer m_currentMassage;
    bool m_hasMessage = false;
    bool m_enabled = true;
};

class SmartSensorsManager
{
public:
    void init(IUARTManager* uart);
    void run();
    void setPresentationLayerFactory(IIRPresentationReceiver::FactoryMethod factory);

    IUARTManager* uart() { return m_uart; }

private:
    struct PhysPresLayers
    {
        ~PhysPresLayers() { if (presentation) delete presentation; }

        SmartSensorPhysical physical;
        IIRPresentationReceiver* presentation = nullptr;
    };
    void task();
    void createPhysPresLayersForSensors();

    IUARTManager* m_uart = nullptr;
    bool m_discoveringBeginned = false;
    IIRPresentationReceiver::FactoryMethod m_presFactory = nullptr;

    std::map<SSP_Address, PhysPresLayers> m_physPresLayers;
    TaskCycled m_smartSensorTask;
};







#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR_SMART_SENSORS_HPP_ */
