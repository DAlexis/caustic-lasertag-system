/*
 * devices-connectors.hpp
 *
 *  Created on: 08 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_DEVICE_INCLUDE_DEVICES_CONNECTORS_HPP_
#define LAZERTAG_DEVICE_INCLUDE_DEVICES_CONNECTORS_HPP_

using IncomingConnectorCallback = void (*)(void* object, unsigned int size, void *data);

class AnyConnectorBase
{
public:
    AnyConnectorBase();
    virtual ~AnyConnectorBase();

    void setIncomingCallback(IncomingConnectorCallback callback, void* object);
    virtual void interrogate() = 0;

private:
    IncomingConnectorCallback m_incomingCallback;
    void *m_incomingCallbackObject;
};

/*
class IRifleToMainSensorConnector : public IAnyConnector
{
public:

};

class IMainSensorToRifleConnector : public IAnyConnector
{
public:

};

class IAdditionalSensorToMainSensorConnector : public IAnyConnector
{
public:

};
*/




#endif /* LAZERTAG_DEVICE_INCLUDE_DEVICES_CONNECTORS_HPP_ */
