/*
 * state-saver-interface.hpp
 *
 *  Created on: 31 янв. 2016 г.
 *      Author: alexey
 */

#ifndef INCLUDE_RCSP_STATE_SAVER_INTERFACE_HPP_
#define INCLUDE_RCSP_STATE_SAVER_INTERFACE_HPP_

/**
 * Any object that can save its state to disk
 */
class IAnyStateSaver
{
public:
	virtual ~IAnyStateSaver() {}
	virtual void saveState() = 0;
};

#endif /* INCLUDE_RCSP_STATE_SAVER_INTERFACE_HPP_ */
