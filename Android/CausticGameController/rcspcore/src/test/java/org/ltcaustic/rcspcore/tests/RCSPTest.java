package org.ltcaustic.rcspcore.tests;

import org.junit.Assert;
import junit.framework.TestCase;

import org.junit.Test;

import org.ltcaustic.rcspcore.RCSP;

/**
 * Created by dalexies on 28.11.17.
 */

public class RCSPTest {
    private void testAnyParameterSerDeser(RCSP.AnyParameterSerializer par, String originalValue, String otherValue, int bufferLength, int offset) {
        byte buffer[] = new byte[bufferLength];
        par.setValue(originalValue);
        par.serialize(buffer, offset);
        par.setValue(otherValue);
        par.deserialize(buffer, offset);
        Assert.assertEquals(originalValue, par.getValue());
    }

    @Test
    public void testOperationCodesManipulation()
    {
        int code = 2005;
        Assert.assertEquals(
                RCSP.removeOperationTypeBits(RCSP.makeOperationCodeType(code, RCSP.OperationCodeType.SET_OBJECT)),
                code
        );
        Assert.assertEquals(
                RCSP.dispatchOperationCodeType(RCSP.makeOperationCodeType(code, RCSP.OperationCodeType.SET_OBJECT)),
                RCSP.OperationCodeType.SET_OBJECT
        );
        Assert.assertEquals(
                RCSP.dispatchOperationCodeType(RCSP.makeOperationCodeType(code, RCSP.OperationCodeType.OBJECT_REQUEST)),
                RCSP.OperationCodeType.OBJECT_REQUEST
        );
        Assert.assertEquals(
                RCSP.dispatchOperationCodeType(RCSP.makeOperationCodeType(code, RCSP.OperationCodeType.CALL_REQUEST)),
                RCSP.OperationCodeType.CALL_REQUEST
        );
        Assert.assertEquals(
                RCSP.dispatchOperationCodeType(RCSP.makeOperationCodeType(code, RCSP.OperationCodeType.RESERVED)),
                RCSP.OperationCodeType.RESERVED
        );
    }

    @Test
    public void testUint16SerializationDeserialization()
    {
        RCSP.AnyParameterSerializer par = new RCSP.UintParameter.Serializer(null);
        testAnyParameterSerDeser(par, "1", "0", 10, 0);
        testAnyParameterSerDeser(par, "129", "0", 10, 1);
        testAnyParameterSerDeser(par, "160", "0", 10, 2);
        testAnyParameterSerDeser(par, "32768", "0", 10, 0);
        testAnyParameterSerDeser(par, "65535", "0", 10, 6);
        testAnyParameterSerDeser(par, "0", "1", 10, 0);
    }

    @Test
    public void testDeviceNameSerializationDeserialization() {
        RCSP.AnyParameterSerializer par = new RCSP.DevNameParameter.Serializer(null);
        //RCSP.Parameter par = new RCSP.Parameter("Test", RCSP.Parameter.TYPE_DEVICE_NAME, 123);
        testAnyParameterSerDeser(par, "Test name 1", "", 40, 20);
        testAnyParameterSerDeser(par, "The device ASCII na", "", 20, 0);

        // String cropping test
        byte buffer[] = new byte[20];
        par.setValue("123456789012345678901234567890");
        par.serialize(buffer, 0);
        par.setValue("");
        par.deserialize(buffer, 0);
        Assert.assertEquals("1234567890123456789", par.getValue());
    }

    @Test
    public void testMT2idSerializationDeserialization() {
        RCSP.AnyParameterSerializer par = new RCSP.MT2IdParameter.Serializer(null);
        testAnyParameterSerDeser(par, "1", "0", 2, 0);
        testAnyParameterSerDeser(par, "80", "0", 2, 1);
        testAnyParameterSerDeser(par, "127", "0", 10, 5);
        testAnyParameterSerDeser(par, "0", "1", 2, 0);

    }

    @Test
    public void testIntSerializationDeserialization() {
        RCSP.AnyParameterSerializer par = new RCSP.IntParameter.Serializer(null);
        testAnyParameterSerDeser(par, "1", "0", 2, 0);
        testAnyParameterSerDeser(par, "23767", "0", 10, 5);
        testAnyParameterSerDeser(par, "-1", "0", 4, 1);
        testAnyParameterSerDeser(par, "0", "1", 2, 0);
    }

    @Test
    public void testFloatSerializationDeserialization() {

        RCSP.AnyParameterSerializer par = new RCSP.FloatParameter.Serializer(null);
        testAnyParameterSerDeser(par, Float.toString(Float.parseFloat("1.0")), "0.0", 4, 0);
        testAnyParameterSerDeser(par, Float.toString(Float.parseFloat("3.1415926")), "0.0", 4, 0);
        testAnyParameterSerDeser(par, Float.toString(Float.parseFloat("-243.6124123")), "0.0", 4, 0);
    }

    @Test
    public void testDevAddrSerializationDeserialization() {
        RCSP.AnyParameterSerializer par = new RCSP.DevAddrParameter.Serializer(null);
        testAnyParameterSerDeser(par, "123.43.8", "0.0.0", 4, 0);
        testAnyParameterSerDeser(par, "255.240.1", "0.0.0", 4, 0);
        testAnyParameterSerDeser(par, "0.0.0", "1.2.3", 4, 0);

    }

    @Test
    public void testBoolSerializationDeserialization() {
        RCSP.AnyParameterSerializer par = new RCSP.BooleanParameter.Serializer(null);
        testAnyParameterSerDeser(par, "true", "false", 1, 0);
        testAnyParameterSerDeser(par, "false", "true", 4, 0);
    }

    @Test
    public void timeIntervalSerializationDeserialization() {
        RCSP.AnyParameterSerializer par = new RCSP.TimeIntervalParameter.Serializer(null);
        testAnyParameterSerDeser(par, "2345", "0", 4, 0);
        testAnyParameterSerDeser(par, "1", "0", 4, 0);
        testAnyParameterSerDeser(par, "0", "24", 4, 0);
        testAnyParameterSerDeser(par, "4294967295", "1", 4, 0);
    }

    @Test
    public void testMT2IdSerializationDeserialization() {
        RCSP.AnyParameterSerializer par = new RCSP.MT2IdParameter.Serializer(null);
        testAnyParameterSerDeser(par, "24", "0", 1, 0);
        testAnyParameterSerDeser(par, "127", "4", 4, 0);
    }

    @Test
    public void testUByteSerializationDeserialization() {
        RCSP.AnyParameterSerializer par = new RCSP.UByteParameter.Serializer(null);
        testAnyParameterSerDeser(par, "24", "0", 1, 0);
        testAnyParameterSerDeser(par, "255", "4", 4, 0);
    }

    @Test
    public void testStreamReadWriteOneParameter() {
        RCSP.ParametersDescriptionsContainer description = new RCSP.ParametersDescriptionsContainer();
        RCSP.ParametersContainer container = new RCSP.ParametersContainer();
        int paramId = 321;
        RCSP.ParameterDescription testParam
                = new RCSP.UintParameter(description, paramId, "Test parameter description", 1, 200);
        description.addParameters(container);

        byte arr[] = new byte[20];
        String etalonValue = "48";

        container.get(paramId).setValue(etalonValue);
        int size = container.serializeSetObject(paramId, arr, 0, 20);
        Assert.assertTrue(size != 0);
        container.get(paramId).setValue("0");

        RCSP.RCSPOperation operation = RCSP.RCSPOperation.parse(arr, 0, 20);
        Assert.assertNotNull(operation);

        container.deserializeOneParamter(operation);
        Assert.assertEquals(etalonValue, container.get(paramId).getValue());
    }

    @Test
    public void testAllTypesSerDeserStream() {
        RCSP.ParametersDescriptionsContainer description = new RCSP.ParametersDescriptionsContainer();
        RCSP.ParametersContainer container = new RCSP.ParametersContainer();
        RCSP.ParameterDescription testParam1
                = new RCSP.UintParameter(description, 1, "Test uintparameter description", 1, 200);
        RCSP.ParameterDescription testParam2
                = new RCSP.IntParameter(description, 2, "Test int parameter description", -150, 200);
        RCSP.ParameterDescription testParam3
                = new RCSP.FloatParameter(description, 3, "Test float parameter description", true, 0.0f, 1.1f);
        RCSP.ParameterDescription testParam4
                = new RCSP.DevNameParameter(description, 4, "Test device name parameter description", false);
        RCSP.ParameterDescription testParam5
                = new RCSP.BooleanParameter(description, 5, "Test bool parameter description", true);
        RCSP.ParameterDescription testParam6
                = new RCSP.MT2IdParameter(description, 6, "Test MT2 id parameter description", true);
        RCSP.ParameterDescription testParam7
                = new RCSP.DevAddrParameter(description, 7, "Test dev address parameter description", true);
        RCSP.ParameterDescription testParam8
                = new RCSP.TimeIntervalParameter(description, 8, "Test time interval parameter description", 0, 1_000_000);
        RCSP.ParameterDescription testParam9
                = new RCSP.UByteParameter(description, 9, "Test unsigned byte parameter description", 0, 255);

        description.addParameters(container);

        int bufferSize = 80;
        byte arr[] = new byte[bufferSize];
        int ui = 2345;
        float f = Float.parseFloat(Float.toString(Float.parseFloat("-3.1415926")));
        String name = "Test name";
        int i = -2022;
        int id = 17;
        String addr = "101.255.0";
        long timeInt = 500_000;
        int ub = 211;

        container.get(1).setValue(Integer.toString(ui));
        container.get(2).setValue(Integer.toString(i));
        container.get(3).setValue(Float.toString(f));
        container.get(4).setValue(name);
        container.get(5).setValue("true");
        container.get(6).setValue(Integer.toString(id));
        container.get(7).setValue(addr);
        container.get(8).setValue(Long.toString(timeInt));
        container.get(9).setValue(Integer.toString(ub));

        int cursor = 0;
        cursor += container.serializeSetObject(1, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(2, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(3, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(4, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(5, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(6, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(7, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(8, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(9, arr, cursor, bufferSize);
        container.get(1).setValue("0");
        container.get(2).setValue("0");
        container.get(3).setValue("No");
        container.get(4).setValue("0");
        container.get(5).setValue("false");
        container.get(6).setValue("45");
        container.get(7).setValue("12.34.56");
        container.get(8).setValue("10000");
        container.get(9).setValue("10");

        // Testing now
        container.deserializeStream(arr, 0, cursor);
        Assert.assertEquals(Integer.toString(ui), container.get(1).getValue());
        Assert.assertEquals(Integer.toString(i), container.get(2).getValue());
        Assert.assertEquals(Float.toString(f), container.get(3).getValue());
        Assert.assertEquals(name, container.get(4).getValue());
        Assert.assertEquals("true", container.get(5).getValue());
        Assert.assertEquals(Integer.toString(id), container.get(6).getValue());
        Assert.assertEquals(addr, container.get(7).getValue());
        Assert.assertEquals(timeInt, Long.parseLong(container.get(8).getValue()));
        Assert.assertEquals(ub, Integer.parseInt(container.get(9).getValue()));
    }
}
