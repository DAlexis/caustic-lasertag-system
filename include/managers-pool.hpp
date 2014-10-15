/*
 * managers-pool.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_MANAGERS_POOL_HPP_
#define LAZERTAG_RIFLE_INCLUDE_MANAGERS_POOL_HPP_

#include "dynamic-memory.hpp"

template <class ManagerType,
    class ManagersPoolType,
    int ManagersCount,
    typename DeviceControlStructType,
    typename... ManagerConstructorArgumentTypes>
class ManagersPoolBase
{
public:
    static ManagersPoolType& getInstance()
    {
        if (m_self == nullptr) {
            createInstance(m_self);
        }
        return *m_self;
    }

    inline void create(DeviceControlStructType device, ManagerConstructorArgumentTypes... args)
    {
        int index = deviceToIndex(device);
        if (m_pManagers[index] == nullptr)
        {
            createInstance<ManagerType, DeviceControlStructType, ManagerConstructorArgumentTypes...>(m_pManagers[index], device, args...);
        }
    }

    inline ManagerType& get(DeviceControlStructType device)
    {
        return *m_pManagers[deviceToIndex(device)];
    }

    ManagersPoolBase()
    {
        for (int i=0; i<ManagersCount; i++)
            m_pManagers[i] = nullptr;
    }
    virtual ~ManagersPoolBase() {}
protected:
    virtual int deviceToIndex(DeviceControlStructType device) = 0;
    static ManagersPoolType* m_self;

private:
    ManagerType* m_pManagers[ManagersCount];
};

#endif /* LAZERTAG_RIFLE_INCLUDE_MANAGERS_POOL_HPP_ */
