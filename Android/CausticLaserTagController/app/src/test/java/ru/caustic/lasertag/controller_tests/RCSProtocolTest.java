package ru.caustic.lasertag.controller_tests;

import junit.framework.Assert;
import junit.framework.TestCase;

import org.junit.Test;

import ru.caustic.lasertag.causticlasertagcontroller.RCSProtocol;

/**
 * Created by alexey on 19.09.15.
 */
public class RCSProtocolTest extends TestCase {

    private void testParameterSerDeser(RCSProtocol.Parameter par, String originalValue, String otherValue, int bufferLength, int positionInBuffer) {
        byte buffer[] = new byte[bufferLength];
        par.setValue(originalValue);
        par.serialize(buffer, positionInBuffer);
        par.setValue(otherValue);
        par.parse(buffer, positionInBuffer);
        Assert.assertEquals(originalValue, par.getValue());
    }


    @Test
    public void testUint16SerializationDeserialization()
    {
        RCSProtocol.Parameter par = new RCSProtocol.Parameter("Test", RCSProtocol.Parameter.TYPE_UINT_PARAMETER, 123);
        testParameterSerDeser(par, "1", "0", 10, 0);
        testParameterSerDeser(par, "129", "0", 10, 1);
        testParameterSerDeser(par, "160", "0", 10, 2);
        testParameterSerDeser(par, "32768", "0", 10, 0);
        testParameterSerDeser(par, "65535", "0", 10, 6);
        testParameterSerDeser(par, "0", "1", 10, 0);
    }

    @Test
    public void testDeviceNameSerializationDeserialization() {
        RCSProtocol.Parameter par = new RCSProtocol.Parameter("Test", RCSProtocol.Parameter.TYPE_DEVICE_NAME, 123);
        testParameterSerDeser(par, "Test name 1", "", 40, 20);
        testParameterSerDeser(par, "The device ASCII na", "", 20, 0);

        // String cropping test
        byte buffer[] = new byte[20];
        par.setValue("123456789012345678901234567890");
        par.serialize(buffer, 0);
        par.setValue("");
        par.parse(buffer, 0);
        Assert.assertEquals("1234567890123456789", par.getValue());
    }

    @Test
    public void testMT2idSerializationDeserialization() {
        RCSProtocol.Parameter par = new RCSProtocol.Parameter("Test", RCSProtocol.Parameter.TYPE_MT2_ID, 123);
        testParameterSerDeser(par, "1", "0", 2, 0);
        testParameterSerDeser(par, "80", "0", 2, 1);
        testParameterSerDeser(par, "127", "0", 10, 5);
        testParameterSerDeser(par, "0", "1", 2, 0);

    }

    @Test
    public void testIntSerializationDeserialization() {
        RCSProtocol.Parameter par = new RCSProtocol.Parameter("Test", RCSProtocol.Parameter.TYPE_INT_PARAMETER, 123);
        testParameterSerDeser(par, "1", "0", 2, 0);
        testParameterSerDeser(par, "23767", "0", 10, 5);
        testParameterSerDeser(par, "-1", "0", 4, 1);
        testParameterSerDeser(par, "0", "1", 2, 0);
    }

    @Test
    public void testStreamReadWrite() {
        RCSProtocol.ParametersContainer pars = new RCSProtocol.ParametersContainer();
        pars.add("TestPar1", RCSProtocol.Parameter.TYPE_UINT_PARAMETER, 25);
        byte arr[] = new byte[20];
        String etalonValue = "48";
        pars.get(25).setValue(etalonValue);
        int size = pars.serialize(25, arr, 0, 20);
        Assert.assertTrue(size != 0);
        pars.get(25).setValue("0");
        pars.readOneParamter(arr, 0, 20);
        Assert.assertEquals(etalonValue, pars.get(25).getValue());

    }
}