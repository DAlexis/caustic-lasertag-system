/*
 * rcsp-aggregator-tests.cpp
 *
 *  Created on: 17 окт. 2017 г.
 *      Author: dalexies
 */

#include "rcsp/aggregator.hpp"
#include "rcsp/operation-codes.hpp"
#include "rcsp/stream.hpp"
#include "mock/network-layer-mock.hpp"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;

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
	ASSERT_NO_THROW(a.dispatchStreamNew(buf.data(), buf.size()));
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

    ASSERT_NO_THROW(a.dispatchStreamNew(buf.data(), buf.size()));
    ASSERT_EQ(healthStart, 25);
    ASSERT_EQ(frendlyFireCoeff, initialFF);
    ASSERT_EQ(devAddr, origAddr);
}

/**
 * Test of `call` operation: sending and receiving it
 */
TEST(RCSPAggregator, CallTwoSidesNoArg)
{
	constexpr uint16_t code = 123;
	RCSPAggregator a;

	bool isSet = false;
	auto f = [&isSet] () { isSet = true; };
	DefaultFunctionAccessor<> testAccessor {code, "test function", f, a};

    Buffer buf;
    ASSERT_NO_THROW(a.serializeCall(code, buf));

    ASSERT_FALSE(isSet);
    ASSERT_NO_THROW(a.dispatchStreamNew(buf.data(), buf.size()));
    ASSERT_TRUE(isSet);
}

/**
 * Test of `call` operation: sending and receiving it
 */
TEST(RCSPAggregator, CallTwoSidesOneArg)
{
	constexpr uint16_t code = 123;
	RCSPAggregator a;
	struct ArgType
	{
		int a=0;
		int b=0;
	};

	bool isSet = false;
	bool argumentCorrect = false;
	auto f = [&isSet, &argumentCorrect] (ArgType argument) {

		isSet = true;
		argumentCorrect = (argument.a == 12 && argument.b == 34);
	};
	DefaultFunctionAccessor<ArgType> testAccessor {code, "test function", f, a};

	ArgType argument = {12, 34};
    Buffer buf;
    ASSERT_NO_THROW(a.serializeCall(code, buf, argument));

    ASSERT_FALSE(isSet);
    ASSERT_FALSE(argumentCorrect);
    ASSERT_NO_THROW(a.dispatchStreamNew(buf.data(), buf.size()));
    ASSERT_TRUE(isSet) << "Callback was not called";
    ASSERT_TRUE(argumentCorrect) << "Callback argument not correct";
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

/**
 * Test for buffer verifying/splitting
 */
class RCSPAggregatorBufferUtils : public ::testing::Test
{
public:
	void setPars()
	{
		devAddr = {1, 2, 3};
		healthStart = 25;
		frendlyFireCoeff = 0.25;
		postRespawnDelay = 1;
		preRespawnDelay = 1;
		autoRespawn = true;
		shockDelayInactive = 0;
		shockDelayImmortal = 0;
	}

	void serializeShort()
	{
		a.serializePush(ConfigCodes::HeadSensor::Configuration::healthStart, buf);
	}

	void serialize()
	{
		serializeShort();
		a.serializePush(ConfigCodes::HeadSensor::Configuration::frendlyFireCoeff, buf);
		a.serializePush(ConfigCodes::HeadSensor::Configuration::postRespawnDelay, buf);
		a.serializePush(ConfigCodes::HeadSensor::Configuration::preRespawnDelay, buf);
		a.serializePush(ConfigCodes::HeadSensor::Configuration::autoRespawn, buf);
		a.serializeCall(123, buf);
		a.serializePush(ConfigCodes::HeadSensor::Configuration::shockDelayInactive, buf);
		a.serializePush(ConfigCodes::HeadSensor::Configuration::shockDelayImmortal, buf);
		a.serializePush(ConfigCodes::AnyDevice::Configuration::devAddr, buf);
	}
	RCSPAggregator a;

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, healthStart, a);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, frendlyFireCoeff, a);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, postRespawnDelay, a);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, preRespawnDelay, a);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, autoRespawn, a);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, shockDelayInactive, a);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, shockDelayImmortal, a);
	PAR_CL(NOT_RESTORABLE, ConfigCodes::AnyDevice::Configuration, devAddr, a);

	Buffer buf;
};

/**
 * Test for buffer validation function with valid buffer
 */
TEST_F(RCSPAggregatorBufferUtils, VerifyValid)
{
	bool res = false;
	ASSERT_NO_THROW(res = RCSPAggregator::verifyBuffer(buf));
	ASSERT_TRUE(res);
	setPars();
	serialize();
	//cout << "buf = " << hexStr(buf.data(), buf.size()) << endl;

	ASSERT_NO_THROW(res = RCSPAggregator::verifyBuffer(buf));
	ASSERT_TRUE(res);
	buf.push_back(0);
	ASSERT_NO_THROW(res = RCSPAggregator::verifyBuffer(buf));
	ASSERT_TRUE(res);
	buf.push_back(0);
	ASSERT_NO_THROW(res = RCSPAggregator::verifyBuffer(buf));
	ASSERT_TRUE(res);
	buf.push_back(0);
	ASSERT_NO_THROW(res = RCSPAggregator::verifyBuffer(buf));
	ASSERT_TRUE(res);
	buf.push_back(0);
	ASSERT_NO_THROW(res = RCSPAggregator::verifyBuffer(buf));
	ASSERT_TRUE(res);
	buf.push_back(0);

	buf.clear();
	ASSERT_TRUE(RCSPAggregator::verifyBuffer(buf));
	buf.push_back(0);
	ASSERT_TRUE(RCSPAggregator::verifyBuffer(buf));
	buf.push_back(0);
	ASSERT_TRUE(RCSPAggregator::verifyBuffer(buf));
	buf.push_back(0);
	ASSERT_TRUE(RCSPAggregator::verifyBuffer(buf));
	buf.push_back(0);
	ASSERT_TRUE(RCSPAggregator::verifyBuffer(buf));
}

/**
 * Test for buffer validation function with invalid buffer
 */
TEST_F(RCSPAggregatorBufferUtils, VerifyInvalid)
{
	buf.push_back(3);
	ASSERT_FALSE(RCSPAggregator::verifyBuffer(buf));
	buf.push_back(0);
	ASSERT_FALSE(RCSPAggregator::verifyBuffer(buf));
	buf.push_back(0);
	ASSERT_FALSE(RCSPAggregator::verifyBuffer(buf));

	buf.clear();
	buf.push_back(2);
	ASSERT_FALSE(RCSPAggregator::verifyBuffer(buf));
	buf.push_back(1);
	ASSERT_FALSE(RCSPAggregator::verifyBuffer(buf));
}

/**
 * Test for buffer splitting function with valid buffer
 */
TEST_F(RCSPAggregatorBufferUtils, SplittingValid)
{
	setPars();
	serialize();

	int blockSize = 10;
	bool failed = false;
	auto testBlock = [this, blockSize, &failed](const uint8_t* begin, uint16_t size)
	{
		failed = failed || (begin < buf.data()) || (begin > buf.data() + buf.size()) || (size > blockSize);
	};
	bool res = false;
	ASSERT_NO_THROW(res = RCSPAggregator::splitBuffer(buf, blockSize, testBlock));
	ASSERT_FALSE(failed) << "Some condition failed in test callback";
	ASSERT_TRUE(res);
	buf.push_back(0);
	buf.push_back(0);
	buf.push_back(0);
	ASSERT_TRUE(RCSPAggregator::splitBuffer(buf, blockSize, testBlock));
}

/**
 * Test for buffer splitting function with valid buffer
 */
TEST_F(RCSPAggregatorBufferUtils, SplittingValidShort)
{
	setPars();
	serializeShort();

	int blockSize = 30;
	bool failed = false;
	int count = 0;
	auto testBlock = [this, blockSize, &failed, &count](const uint8_t* begin, uint16_t size)
	{
		failed = failed || (begin < buf.data()) || (begin > buf.data() + buf.size()) || (size > blockSize);
		count++;
	};
	bool res = false;
	ASSERT_NO_THROW(res = RCSPAggregator::splitBuffer(buf, blockSize, testBlock));
	ASSERT_FALSE(failed) << "Some condition failed in test callback";
	ASSERT_TRUE(res);
	ASSERT_EQ(count, 1) << "Too many callback calls";
	buf.push_back(0);
	buf.push_back(0);
	buf.push_back(0);
	ASSERT_TRUE(RCSPAggregator::splitBuffer(buf, blockSize, testBlock));
}

/**
 * Test for buffer splitting function with valid buffer
 */
TEST_F(RCSPAggregatorBufferUtils, SplittingInvalid)
{
	setPars();
	serialize();

	int blockSize = 10;
	bool failed = false;
	buf[buf.size() - 13] = buf[buf.size() - 13] + 1; // We are modifying size for shockDelayImmortal
	auto testBlock = [this, blockSize, &failed](const uint8_t* begin, uint16_t size)
	{
		failed = failed || (begin < buf.data()) || (begin > buf.data() + buf.size()) || (size > blockSize);
	};
	bool res = false;

	ASSERT_NO_THROW(res = RCSPAggregator::splitBuffer(buf, blockSize, testBlock));
	ASSERT_FALSE(res);
	ASSERT_FALSE(failed) << "Callback was called for invalid block";
}

TEST(RCSPStream, Operating)
{
	RCSPAggregator a;
	NetworkLayerMock nl;
	OrdinaryNetworkClient onc;
	onc.connectNetworkLayer(&nl);
	onc.setMyAddress(DeviceAddress(1, 1, 1));

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, healthStart, a);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, frendlyFireCoeff, a);
	PAR_CL(NOT_RESTORABLE, ConfigCodes::AnyDevice::Configuration, devAddr, a);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, postRespawnDelay, a);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, preRespawnDelay, a);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, autoRespawn, a);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, shockDelayInactive, a);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, shockDelayImmortal, a);
	devAddr = {1, 2, 3};
	auto origAddr = devAddr;
	healthStart = 25;
	frendlyFireCoeff = 0.25;
	postRespawnDelay = 1000;
	preRespawnDelay = 2000;
	autoRespawn = false;
	shockDelayInactive = 3000;
	shockDelayImmortal = 4000;

	RCSPStream stream(&a);
	ASSERT_NO_THROW(stream.addPush(ConfigCodes::HeadSensor::Configuration::healthStart));
	ASSERT_NO_THROW(stream.addPush(ConfigCodes::HeadSensor::Configuration::frendlyFireCoeff));
	ASSERT_NO_THROW(stream.addPush(ConfigCodes::AnyDevice::Configuration::devAddr));
	ASSERT_NO_THROW(stream.addPush(ConfigCodes::HeadSensor::Configuration::postRespawnDelay));
	ASSERT_NO_THROW(stream.addPush(ConfigCodes::HeadSensor::Configuration::preRespawnDelay));
	ASSERT_NO_THROW(stream.addPush(ConfigCodes::HeadSensor::Configuration::autoRespawn));
	ASSERT_NO_THROW(stream.addPush(ConfigCodes::HeadSensor::Configuration::shockDelayInactive));
	ASSERT_NO_THROW(stream.addPush(ConfigCodes::HeadSensor::Configuration::shockDelayImmortal));

	//cout << "stream = " << hexStr(stream.buffer().data(), stream.buffer().size()) << endl;
	ASSERT_NO_THROW(stream.send(&onc, DeviceAddress(1, 1, 2)));


	// Changing all variables to another values
	devAddr = {3, 5, 9};
	healthStart = 4;
	frendlyFireCoeff = 0.89;
	postRespawnDelay = 12;
	preRespawnDelay = 32;
	autoRespawn = true;
	shockDelayInactive = 43;
	shockDelayImmortal = 87;

	ASSERT_NO_THROW(a.dispatchStreamNew(nl.buffer()));
	//cout << "buf = " << hexStr(nl.buffer().data(), nl.buffer().size()) << endl;

	ASSERT_EQ(healthStart, 25);
	ASSERT_EQ(frendlyFireCoeff, 0.25);
	ASSERT_EQ(devAddr, origAddr);
	ASSERT_EQ(postRespawnDelay, 1000);
	ASSERT_EQ(preRespawnDelay, 2000);
	ASSERT_EQ(autoRespawn, false);
	ASSERT_EQ(shockDelayInactive, 3000);
	ASSERT_EQ(shockDelayImmortal, 4000);
}
