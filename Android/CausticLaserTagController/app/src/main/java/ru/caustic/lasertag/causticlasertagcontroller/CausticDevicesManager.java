package ru.caustic.lasertag.causticlasertagcontroller;

import java.util.Map;
import java.util.TreeMap;

/**
 * Created by alexey on 20.09.15.
 */
public class CausticDevicesManager {

    private static CausticDevicesManager ourInstance = new CausticDevicesManager();

    private CausticDevicesManager() {

    }

    public static CausticDevicesManager getInstance() {
        return ourInstance;
    }



    public Map<BridgeConnector.DeviceAddress, CausticDevice> devices = new TreeMap<BridgeConnector.DeviceAddress, CausticDevice>();

    public static class CausticDevice {
        public BridgeConnector.DeviceAddress address = new BridgeConnector.DeviceAddress();
        public RCSProtocol.ParametersContainer parameters = new RCSProtocol.ParametersContainer();
        public RCSProtocol.RemoteFunctionsContainer functions = new RCSProtocol.RemoteFunctionsContainer();
    }

    public void updateDevicesList() {
        BridgeConnector.getInstance().setReceiver(new Receiver());
        
    }

    class Receiver implements BridgeConnector.IncomingPackagesListener {
        @Override
        public void getData(BridgeConnector.DeviceAddress address, byte[] data, int offset, int size)
        {
            CausticDevice dev = devices.get(address);
            if (dev == null) {
                dev = new CausticDevice();
                devices.put(address, dev);
            }
            dev.address = new BridgeConnector.DeviceAddress(address);
            dev.parameters.deserializeStream(data, offset, size);
        }
    }
}
