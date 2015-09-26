package ru.caustic.lasertag.causticlasertagcontroller;

import android.util.Log;
import android.widget.Toast;

import java.util.Map;
import java.util.TreeMap;

/**
 * Created by alexey on 20.09.15.
 */
public class CausticDevicesManager {

    private static final String TAG = "CC.CausticDevManager";
    private static final int MSG_DEVICES_LIST_UPDATED = 1;

    private static CausticDevicesManager ourInstance = new CausticDevicesManager();

    private CausticDevicesManager() { }

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

        // @todo Send broadcast
        int reqSize = 23;
        byte[] request = new byte[reqSize];
        MemoryUtils.zerify(request);

        int result = RCSProtocol.ParametersContainer.serializeParameterRequest(RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.deviceName, request, 0, reqSize);
        if (result == 0) {
            Log.e(TAG, "Fatal: Cannot serialize request for device name!");
            return;
        }

        BridgeConnector.getInstance().sendMessage(BridgeConnector.Broadcasts.any, request, result);
    }

    public void remoteCall(BridgeConnector.DeviceAddress target, int operationId, String argument)
    {
        int reqSize = 23;
        byte[] request = new byte[reqSize];
        MemoryUtils.zerify(request);

        /// @todo Add code here
        //int result = RCSProtocol.RemoteFunctionsContainer
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
