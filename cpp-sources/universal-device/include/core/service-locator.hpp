/*
 * service-locator.hpp
 *
 *  Created on: 28 окт. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_INCLUDE_CORE_SERVICE_LOCATOR_HPP_
#define UNIVERSAL_DEVICE_INCLUDE_CORE_SERVICE_LOCATOR_HPP_

#include "rcsp/RCSP-aggregator.hpp"

class ServiceLocator
{
public:
	RCSPAggregator& rcspAggregator();

private:
	RCSPAggregator* m_rcspAggregator = nullptr;

};






#endif /* UNIVERSAL_DEVICE_INCLUDE_CORE_SERVICE_LOCATOR_HPP_ */
