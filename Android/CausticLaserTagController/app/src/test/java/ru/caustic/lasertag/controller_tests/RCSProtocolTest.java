package ru.caustic.lasertag.controller_tests;

import junit.framework.Assert;
import junit.framework.TestCase;

import org.junit.Test;

import ru.caustic.lasertag.causticlasertagcontroller.RCSProtocol;

/**
 * Created by alexey on 19.09.15.
 */
public class RCSProtocolTest extends TestCase {
    @Test
    public void testEquals() {
        Assert.assertEquals(4, 2 + 2);
        Assert.assertTrue(4 == 2 + 2);
    }

    @Test
    public void testUint16Serialization()
    {
        RCSProtocol rcsp = new RCSProtocol();
    }
}