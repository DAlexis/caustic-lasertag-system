/*
 * interfaces.hpp
 *
 *  Created on: 8 янв. 2016 г.
 *      Author: alexey
 */

#ifndef INCLUDE_UTILS_INTERFACES_HPP_
#define INCLUDE_UTILS_INTERFACES_HPP_


class IInterrogatable
{
public:
	virtual ~IInterrogatable() {}
	virtual void interrogate() = 0;
};


#endif /* INCLUDE_UTILS_INTERFACES_HPP_ */
