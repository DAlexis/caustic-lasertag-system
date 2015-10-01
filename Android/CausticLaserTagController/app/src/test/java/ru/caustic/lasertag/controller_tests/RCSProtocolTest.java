package ru.caustic.lasertag.controller_tests;

import junit.framework.Assert;
import junit.framework.TestCase;

import org.junit.Test;

import ru.caustic.lasertag.causticlasertagcontroller.RCSProtocol;

/**
 * Created by alexey on 19.09.15.
 */
public class RCSProtocolTest extends TestCase {

    private void testAnyParameterSerDeser(RCSProtocol.RCSPParameterGroup par, String originalValue, String otherValue, int bufferLength, int offset) {
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
                RCSProtocol.removeOperationTypeBits(RCSProtocol.makeOperationCodeType(code, RCSProtocol.OperationCodeType.SET_OBJECT)),
                code
        );
        Assert.assertEquals(
                RCSProtocol.dispatchOperationCodeType(RCSProtocol.makeOperationCodeType(code, RCSProtocol.OperationCodeType.SET_OBJECT)),
                RCSProtocol.OperationCodeType.SET_OBJECT
        );
        Assert.assertEquals(
                RCSProtocol.dispatchOperationCodeType(RCSProtocol.makeOperationCodeType(code, RCSProtocol.OperationCodeType.OBJECT_REQUEST)),
                RCSProtocol.OperationCodeType.OBJECT_REQUEST
        );
        Assert.assertEquals(
                RCSProtocol.dispatchOperationCodeType(RCSProtocol.makeOperationCodeType(code, RCSProtocol.OperationCodeType.CALL_REQUEST)),
                RCSProtocol.OperationCodeType.CALL_REQUEST
        );
        Assert.assertEquals(
                RCSProtocol.dispatchOperationCodeType(RCSProtocol.makeOperationCodeType(code, RCSProtocol.OperationCodeType.RESERVED)),
                RCSProtocol.OperationCodeType.RESERVED
        );
    }

    @Test
    public void testUint16SerializationDeserialization()
    {
        RCSProtocol.RCSPParameterGroup par = new RCSProtocol.UintGroupRCSP(123, "Test", "TESTKEY");
        testAnyParameterSerDeser(par, "1", "0", 10, 0);
        testAnyParameterSerDeser(par, "129", "0", 10, 1);
        testAnyParameterSerDeser(par, "160", "0", 10, 2);
        testAnyParameterSerDeser(par, "32768", "0", 10, 0);
        testAnyParameterSerDeser(par, "65535", "0", 10, 6);
        testAnyParameterSerDeser(par, "0", "1", 10, 0);
    }

    @Test
    public void testDeviceNameSerializationDeserialization() {
        RCSProtocol.RCSPParameterGroup par = new RCSProtocol.DevNameGroupRCSP(123, "Test", "TESTKEY");
        //RCSProtocol.Parameter par = new RCSProtocol.Parameter("Test", RCSProtocol.Parameter.TYPE_DEVICE_NAME, 123);
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
        RCSProtocol.RCSPParameterGroup par = new RCSProtocol.MT2IdGroupRCSP(123, "Test", "TESTKEY");
        testAnyParameterSerDeser(par, "1", "0", 2, 0);
        testAnyParameterSerDeser(par, "80", "0", 2, 1);
        testAnyParameterSerDeser(par, "127", "0", 10, 5);
        testAnyParameterSerDeser(par, "0", "1", 2, 0);

    }

    @Test
    public void testIntSerializationDeserialization() {
        RCSProtocol.RCSPParameterGroup par = new RCSProtocol.IntGroupRCSP(123, "Test", "TESTKEY");
        testAnyParameterSerDeser(par, "1", "0", 2, 0);
        testAnyParameterSerDeser(par, "23767", "0", 10, 5);
        testAnyParameterSerDeser(par, "-1", "0", 4, 1);
        testAnyParameterSerDeser(par, "0", "1", 2, 0);
    }

    @Test
    public void testFloatSerializationDeserialization() {

        RCSProtocol.RCSPParameterGroup par = new RCSProtocol.FloatGroupRCSP(123, "Test", "TESTKEY");
        testAnyParameterSerDeser(par, Float.toString(Float.parseFloat("1.0")), "0.0", 4, 0);
        testAnyParameterSerDeser(par, Float.toString(Float.parseFloat("3.1415926")), "0.0", 4, 0);
        testAnyParameterSerDeser(par, Float.toString(Float.parseFloat("-243.6124123")), "0.0", 4, 0);
    }

    @Test
    public void testDevAddrSerializationDeserialization() {
        RCSProtocol.RCSPParameterGroup par = new RCSProtocol.DevAddrGroupRCSP(123, "Test", "TESTKEY");
        testAnyParameterSerDeser(par, "123.43.8", "0.0.0", 4, 0);
        testAnyParameterSerDeser(par, "255.240.1", "0.0.0", 4, 0);
        testAnyParameterSerDeser(par, "0.0.0", "1.2.3", 4, 0);

    }

    @Test
    public void testStreamReadWriteOneParameter() {
        RCSProtocol.ParametersContainer pars = new RCSProtocol.ParametersContainer();
        //pars.add("TestPar1", RCSProtocol.Parameter.TYPE_UINT_PARAMETER, 25);
        pars.add(new RCSProtocol.UintGroupRCSP(25, "Test Parameter 1", "TESTKEY"));
        byte arr[] = new byte[20];
        String etalonValue = "48";
        pars.get(25).setValue(etalonValue);
        int size = pars.serializeSetObject(25, arr, 0, 20);
        Assert.assertTrue(size != 0);
        pars.get(25).setValue("0");
        pars.deserializeOneParamter(arr, 0, 20);
        Assert.assertEquals(etalonValue, pars.get(25).getValue());
    }

    @Test
    public void testStreamSerDeserStream() {
        RCSProtocol.ParametersContainer pars = new RCSProtocol.ParametersContainer();
        pars.add(new RCSProtocol.UintGroupRCSP(1, "Test Parameter 1", "TESTKEY"));
        pars.add(new RCSProtocol.FloatGroupRCSP(2, "Test Parameter 2", "TESTKEY"));
        pars.add(new RCSProtocol.DevNameGroupRCSP(3, "Test Parameter 3", "TESTKEY"));
        pars.add(new RCSProtocol.IntGroupRCSP(4, "Test Parameter 3", "TESTKEY"));
        int bufferSize = 40;
        byte arr[] = new byte[bufferSize];
        int ui = 2345;
        float f = Float.parseFloat(Float.toString(Float.parseFloat("-3.1415926")));
        String name = "Test name";
        int i = -2022;

        pars.get(1).setValue(Integer.toString(ui));
        pars.get(2).setValue(Float.toString(f));
        pars.get(3).setValue(name);
        pars.get(4).setValue(Integer.toString(i));
        int cursor = 0;
        cursor += pars.serializeSetObject(1, arr, cursor, bufferSize);
        cursor += pars.serializeSetObject(2, arr, cursor, bufferSize);
        cursor += pars.serializeSetObject(3, arr, cursor, bufferSize);
        cursor += pars.serializeSetObject(4, arr, cursor, bufferSize);
        pars.get(1).setValue("0");
        pars.get(2).setValue("0");
        pars.get(3).setValue("No");
        pars.get(4).setValue("0");

        pars.deserializeStream(arr, 0, cursor);
        Assert.assertEquals(Integer.toString(ui), pars.get(1).getValue());
        Assert.assertEquals(Float.toString(f), pars.get(2).getValue());
        Assert.assertEquals(name, pars.get(3).getValue());
        Assert.assertEquals(Integer.toString(i), pars.get(4).getValue());
    }
}