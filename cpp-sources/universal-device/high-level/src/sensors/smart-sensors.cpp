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

SmartSensorReceiver::SmartSensorReceiver(UintParameter id) :
		m_id(id)
{
}

void SmartSensorReceiver::getData(uint8_t*& data, uint16_t& size)
{
	data = m_currentMassage.data;
	size = m_currentMassage.bits_count;
	m_hasMessage = false;
}

UintParameter SmartSensorReceiver::getId()
{
	return m_id;
}

bool SmartSensorReceiver::isDataReady()
{
	return m_hasMessage;
}

void SmartSensorReceiver::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

void SmartSensorReceiver::interrogate()
{
	// nothing to do here
}

void SmartSensorReceiver::addMessage(const SSP_IR_Buffer& message)
{
    m_currentMassage = message;
    m_hasMessage = true;
}

/////////////////////////////////////
// SmartSensorRGBVibro

SmartSensorRGBVibro::SmartSensorRGBVibro(UintParameter id) :
		m_id(id)
{
}

void SmartSensorRGBVibro::applyIlluminationScheme(const IllumitationScheme* scheme)
{
	// @todo convert to SSP format and call ssp_push_animation_task
}

UintParameter SmartSensorRGBVibro::getId()
{
	return m_id;
}

void SmartSensorRGBVibro::interrogate()
{
	// Send while busy
}

/////////////////////////////////////
// SmartSensorsManager
SmartSensorsManager::SmartSensorsManager(LedVibroManager& lv, IRReceiversManager& ir) :
		m_lv(&lv), m_ir(&ir)
{
}

void SmartSensorsManager::init(IUARTManager* uart)
{
    debug << "Smart sensors manager initialization";
    m_uart = uart;
    m_uart->init(SSP_UART_SPEED, true);
    m_uart->setBlockSize(1);
    m_uart->setRXDoneCallback([](uint8_t *buffer, uint16_t size) { UNUSED_ARG(size); ssp_receive_byte(*buffer); });
    debug << "UART init done";
    ssp_master_init();
    debug << "SSP master init done";
    activeManager = this;

    m_smartSensorTask.setStackSize(512);
    m_smartSensorTask.setTask([this] { task(); });
}

void SmartSensorsManager::run()
{
	debug << "Discovering...";
	startDiscovering();
	debug << "Discovering started";
    m_smartSensorTask.run();
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
            onDiscoveringFinished();
        }
    }

    // Sending request for ir data
    if (ssp_is_busy() == 0 && !m_sensorReceivers.empty())
    {
    	debug << "Sending IR request";
    	ssp_push_ir_request(m_nextToAsk->first);
    	++m_nextToAsk;
    	if (m_nextToAsk == m_sensorReceivers.end())
    		m_nextToAsk = m_sensorReceivers.begin();
    }

    // Processing received data
    SSP_IR_Message* msg = ssp_get_next_ir_buffer();
    if (msg)
    {
    	auto it = m_sensorReceivers.find(msg->sensor_address);
    	if (it != m_sensorReceivers.end())
    	{
    		it->second.addMessage(msg->buffer);
    	}
    }
}

void SmartSensorsManager::startDiscovering()
{
	m_discoveringBeginned = true;
	ssp_start_address_discovering();
}

void SmartSensorsManager::onDiscoveringFinished()
{
	debug << "+- Address discovering done, lets see what do we have:";
    for (int i=0; i<ssp_registered_addrs.size; i++)
    {
    	UintParameter addr = ssp_registered_addrs.address[i];
    	debug << "|- " << addr;
    	auto it = m_sensorReceivers.find(addr);
    	if (it != m_sensorReceivers.end())
    	{
    		error << "Smart sensor address duplication!";
    		continue;
    	}

    	m_sensorReceivers.emplace(addr, addr);
    	m_ir->addPhysicalReceiver(&m_sensorReceivers.find(addr)->second);

    	m_sensorRgbVibros.emplace_back(ssp_registered_addrs.address[i]);
    	m_lv->addPoint(&m_sensorRgbVibros.back());
    }
    debug << "`- end of discovering";
    if (!m_sensorReceivers.empty())
    	m_nextToAsk = m_sensorReceivers.begin();
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
        while (activeManager->uart()->txBusy()) {  }
    }

}

void ssp_write_debug(uint8_t* data, uint16_t size)
{
    std::string s(data, data+size);
    debug << s.c_str();
}

