/*
 * rcsp-aggregator-tests.cpp
 *
 *  Created on: 17 окт. 2017 г.
 *      Author: dalexies
 */

#include "rcsp/aggregator.hpp"
#include "gtest/gtest.h"

TEST(RCSPAggregator, Instantiation)
{
	ASSERT_NO_THROW(RCSPAggregator());

	RCSPAggregator a;
    RCSPAggregator::setActiveAggregator(&a);
}
