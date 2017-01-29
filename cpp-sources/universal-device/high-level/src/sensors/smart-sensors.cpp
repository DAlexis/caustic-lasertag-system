/*
 * smart-sensors.cpp
 *
 *  Created on: 22 янв. 2017 г.
 *      Author: dalexies
 */

#include "sensors/smart-sensors.hpp"
#include "ssp-master.h"
#include "ssp-master-driver.h"
#include "hal/system-clock.hpp"
#include "core/logging.hpp"

static SmartSensorsManager* activeManager = nullptr;

void SmartSensorPhysical::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

void SmartSensorPhysical::init()
{
}

void SmartSensorPhysical::interrogate()
{
    if (m_hasMessage)
    {
        m_callback(m_currentMassage.data, m_currentMassage.bits_count);
        m_hasMessage = false;
    }
}

void SmartSensorPhysical::addMessage(const SSP_IR_Buffer& message)
{
    m_currentMassage = message;
    m_hasMessage = true;
}

void SmartSensorsManager::init(IUARTManager* uart)
{
    debug << "Smart sensors manager initialization";
    m_uart = uart;
    m_uart->init(115200);
    ssp_master_init();
    activeManager = this;
    m_discoveringBeginned = true;
    ssp_start_address_discovering();

    m_smartSensorTask.setStackSize(256);
    m_smartSensorTask.setTask([this] { task(); });
}

void SmartSensorsManager::run()
{
    m_smartSensorTask.run();
}

void SmartSensorsManager::setPresentationLayerFactory(IIRPresentationReceiver::FactoryMethod factory)
{
    m_presFactory = factory;
}

void SmartSensorsManager::task()
{
    ssp_master_task_tick();
    if (m_discoveringBeginned)
    {
        if (ssp_is_address_discovering_now() == 0)
        {
            // Discovering done
            m_discoveringBeginned = false;
            debug << "Address discovering done";
            createPhysPresLayersForSensors();
        }
    }
    SSP_IR_Message* msg = ssp_get_next_ir_buffer();
    if (msg)
    {
        m_physPresLayers[msg->sensor_address].physical.addMessage(msg->buffer);
    }
}

void SmartSensorsManager::createPhysPresLayersForSensors()
{
    if (!m_presFactory)
    {
        error << "Smart sensors manager: Presentation layer factory not set";
        return;
    }

    for (int i=0; i<ssp_registered_addrs.size; i++)
    {
        debug << "Discovered " << ssp_registered_addrs.address[i] << "; creating layers...";
        auto it = m_physPresLayers.emplace(std::make_pair(ssp_registered_addrs.address[i], PhysPresLayers())).first;
        it->second.presentation = m_presFactory(&it->second.physical);
    }
}

///////////////////////////////////////////////////
// Driver

void ssp_drivers_init(void) { }


uint32_t ssp_get_time_ms()
{
    return systemClock->getTime() / 1000;
}

void ssp_send_data(uint8_t* data, uint16_t size)
{
    if (activeManager)
    {
        activeManager->uart()->transmit(data, size);
        while (activeManager->uart()->txBusy()) { }
    }
}

void ssp_write_debug(uint8_t* data, uint16_t size)
{
    std::string s(data, data+size);
    debug << s.c_str();
}

/***********************************************
 * This functions implemented by SSP:
 */

/* Should be called when data byte is ready.
 * It is safe to call it from in interrupt
 */
void ssp_receive_byte(uint8_t byte);

