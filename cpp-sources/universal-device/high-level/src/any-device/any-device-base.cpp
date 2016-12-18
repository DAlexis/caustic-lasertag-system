
#include "any-device/any-device-base.hpp"
#include "rcsp/aggregator.hpp"

AnyDeviceBase::AnyDeviceBase() :
    m_aggregator(&RCSPAggregator::getActiveAggregator()),
    m_stateSaver(m_aggregator)
{

}
