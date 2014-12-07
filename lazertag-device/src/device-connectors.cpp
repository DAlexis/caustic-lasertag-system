/*
 * device-connectors.cpp
 *
 *  Created on: 08 дек. 2014 г.
 *      Author: alexey
 */

#include "devices-connectors.hpp"

AnyConnectorBase::AnyConnectorBase() :
    m_incomingCallback(nullptr),
    m_incomingCallbackObject(nullptr)
{
}

AnyConnectorBase::~AnyConnectorBase()
{
}

void AnyConnectorBase::setIncomingCallback(IncomingConnectorCallback callback, void* object)
{
    m_incomingCallback = callback;
    m_incomingCallbackObject = object;
}
