/*
 * smart-sensors.hpp
 *
 *  Created on: 22 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR_SMART_SENSORS_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR_SMART_SENSORS_HPP_

#include "sensors/ir-physical-receiver.hpp"
#include "ir-presentation-receiver.hpp"
#include "output/illumination.hpp"
#include "hal/uart.hpp"
#include "core/os-wrappers.hpp"
#include "ssp-master.h"
#include <map>

static_assert(sizeof(SSP_Address) == sizeof(UintParameter), "SSP_Address and UintParameter shoukd be the same type");

/**
 * This class represents physical layer for one smart sensor
 */
class SmartSensorReceiver : public IIRReceiverPhysical
{
public:
	SmartSensorReceiver(UintParameter id);
	void getData(uint8_t*& data, uint16_t& size) override;
	UintParameter getId() override;
	bool isDataReady() override;
	void setEnabled(bool enabled) override;
	void interrogate() override;

    void addMessage(const SSP_IR_Buffer& message);

private:
    UintParameter m_id;
    SSP_IR_Buffer m_currentMassage;
    bool m_hasMessage = false;
    bool m_enabled = true;
};

class SmartSensorRGBVibro : public IRGBVibroPointPhysical
{
public:
	SmartSensorRGBVibro(UintParameter id);
	void applyIlluminationScheme(const IllumitationScheme* scheme) override;
	UintParameter getId() override;
	void interrogate() override;

	bool sendScheme();

private:
	const IllumitationScheme* m_currentScheme = nullptr;
	uint8_t m_currentTaskIndex = 0;
	UintParameter m_id;
	bool m_resetWasSent = false;
};

class SmartSensorsManager
{
public:
	using DiscoveringDoneCallback = std::function<void(void)>;

	SmartSensorsManager(LedVibroManager& lv, IRReceiversManager& ir);
    void init(IUARTManager* uart);
    void run(DiscoveringDoneCallback onDone = nullptr);

    IUARTManager* uart() { return m_uart; }

private:

    void task();
    void discoveringTask();
    void irReqTask();
    bool sendRgbVibroTask();

    void startDiscovering();
    void onDiscoveringFinished();

    LedVibroManager* m_lv;
	IRReceiversManager* m_ir;
    IUARTManager* m_uart = nullptr;
    bool m_discoveringBeginned = false;

    std::map<UintParameter, SmartSensorReceiver> m_sensorReceivers;
    std::map<UintParameter, SmartSensorReceiver>::iterator m_nextToAsk;

    std::list<SmartSensorRGBVibro> m_sensorRgbVibros;
    std::list<SmartSensorRGBVibro>::iterator m_nextRgbVibro;

    TaskCycled m_smartSensorTask;
    DiscoveringDoneCallback m_discDoneCb = nullptr;
};







#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR_SMART_SENSORS_HPP_ */
