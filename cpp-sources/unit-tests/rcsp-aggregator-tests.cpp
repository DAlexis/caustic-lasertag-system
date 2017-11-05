/*
 * rcsp-aggregator-tests.cpp
 *
 *  Created on: 17 окт. 2017 г.
 *      Author: dalexies
 */

#include "rcsp/aggregator.hpp"
#include "rcsp/operation-codes.hpp"
#include "gtest/gtest.h"

/**
 * This file contains tests for RCSPAggregator -- unit responsible
 * for collecting parameters and functions with operation codes
 * and serializing/deserealizing it to buffers.
 */

/**
 * Test for simple instantiation and some trivial oprations
 * that shoul do nothing
 */
TEST(RCSPAggregator, Instantiation)
{
	ASSERT_NO_THROW(RCSPAggregator());

	RCSPAggregator a;

	Buffer buf;
	ASSERT_NO_THROW(a.dispatchStream(buf.data(), buf.size()));
}

/**
 * Test of pushValue operation serialization: stream consistency
 */
TEST(RCSPAggregator, PushVariableBinaryStream)
{
	RCSPAggregator a;
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, healthStart, a);
	healthStart = 0xffff;
	Buffer buf;
	ASSERT_NO_THROW(a.serializePush(ConfigCodes::HeadSensor::Configuration::healthStart, buf));
	auto pushCode = RCSPCodeManipulator::makePush(ConfigCodes::HeadSensor::Configuration::healthStart);
	ASSERT_EQ(buf,
		Buffer({
		0x02, // Argument size
		uint8_t(pushCode & 0xFF),
		uint8_t(pushCode >> 8),
		0xff,
		0xff
		})
	);
}

/**
 * Test of `push` operation: sending and receiving it
 */
TEST(RCSPAggregator, PushVariableTwoSides)
{
	RCSPAggregator a;

    PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, healthStart, a);
    PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, frendlyFireCoeff, a);
    PAR_CL(NOT_RESTORABLE, ConfigCodes::AnyDevice::Configuration, devAddr, a);
    devAddr = {1, 2, 3};
    auto origAddr = devAddr;
    healthStart = 25;
    float initialFF = 0.25;
    frendlyFireCoeff = initialFF;

    Buffer buf;
    ASSERT_NO_THROW(a.serializePush(ConfigCodes::HeadSensor::Configuration::healthStart, buf));
    ASSERT_NO_THROW(a.serializePush(ConfigCodes::HeadSensor::Configuration::frendlyFireCoeff, buf));
    ASSERT_NO_THROW(a.serializePush(ConfigCodes::AnyDevice::Configuration::devAddr, buf));

    healthStart = 40;
    frendlyFireCoeff = 0.11;
    devAddr = {5, 6, 7};

    ASSERT_NO_THROW(a.dispatchStream(buf.data(), buf.size()));
    ASSERT_EQ(healthStart, 25);
    ASSERT_EQ(frendlyFireCoeff, initialFF);
    ASSERT_EQ(devAddr, origAddr);
}

/**
 * Test of `call` operation: sending and receiving it
 */
TEST(RCSPAggregator, CallTwoSides)
{
	constexpr uint16_t code = 123;
	RCSPAggregator a;

	bool isSet = false;
	auto f = [&isSet] () { isSet = true; };
	DefaultFunctionAccessor<> testAccessor {code, "test function", f, a};

    Buffer buf;
    ASSERT_NO_THROW(a.serializeCall(code, buf));

    ASSERT_FALSE(isSet);
    ASSERT_NO_THROW(a.dispatchStream(buf.data(), buf.size()));
    ASSERT_TRUE(isSet);
}

/**
 * Test of `pull` operation: sending, receiving (forming answer stream) and dispatching answer stream
 */
TEST(RCSPAggregator, PullTwoSides)
{
	RCSPAggregator a;

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, healthStart, a);
	healthStart = 123;

    Buffer buf;
    Buffer ansBuf;
    ASSERT_NO_THROW(a.serializePull(ConfigCodes::HeadSensor::Configuration::healthStart, buf));
    // Now nothing changed in a and buf stores "pull" for healthStart
    healthStart = 321;

    ASSERT_NO_THROW(a.dispatchStreamNew(buf.data(), buf.size(), &ansBuf));
    // Now a generated "push" for healthStart with value 321
    ASSERT_EQ(healthStart, 321);

    healthStart = 987;
    ASSERT_NO_THROW(a.dispatchStreamNew(ansBuf.data(), ansBuf.size()));
    // Now a dispatched ansBuf that contained push with 321 value
    // And this should be fine:
    ASSERT_EQ(healthStart, 321);
}
