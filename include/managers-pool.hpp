/*
 * managers-pool.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_MANAGERS_POOL_HPP_
#define LAZERTAG_RIFLE_INCLUDE_MANAGERS_POOL_HPP_

#include "dynamic-memory.hpp"

template <class ManagerType, int ManagersCount, typename DeviceControlStructType, typename... ManagerConstructorArgumentTypes>
class ManagersPoolBase
{
public:
    static ManagersPoolBase& getInstance()
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
            createInstance(m_pManagers[index], args...);
    }

    inline ManagerType& get(DeviceControlStructType device)
    {
        return *m_pManagers[deviceToIndex(device)];
    }

protected:
    virtual int deviceToIndex(DeviceControlStructType device) = 0;

private:
    ManagersPoolBase()
    {
        for (int i=0; i<ManagersCount; i++)
            m_pManagers[i] = nullptr;
    }

    ManagerType* m_pManagers[ManagersCount];

    static ManagersPoolBase* m_self;
};



#endif /* LAZERTAG_RIFLE_INCLUDE_MANAGERS_POOL_HPP_ */
