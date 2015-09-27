package ru.caustic.lasertag.causticlasertagcontroller;

import android.util.Log;
import android.widget.Toast;

import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;

/**
 * Created by alexey on 20.09.15.
 */
public class CausticDevicesManager {

    private static final String TAG = "CC.CausticDevManager";
    private static final int MSG_DEVICES_LIST_UPDATED = 1;




    public Map<BridgeConnector.DeviceAddress, CausticDevice> devices = new HashMap<BridgeConnector.DeviceAddress, CausticDevice>();
    public static CausticDeviceClass headSensor = new CausticDeviceClass();

    private static CausticDevicesManager ourInstance = new CausticDevicesManager();

    private CausticDevicesManager() {
        RCSProtocol.RCSPOperationCodes.AnyDevice.registerFunctions(headSensor.functions);
        RCSProtocol.RCSPOperationCodes.HeadSensor.registerFunctions(headSensor.functions);
    }

    public static CausticDevicesManager getInstance() {
        return ourInstance;
    }

    public static class CausticDevice {
        public BridgeConnector.DeviceAddress address = new BridgeConnector.DeviceAddress();
        public RCSProtocol.ParametersContainer parameters = new RCSProtocol.ParametersContainer();
    }

    public static class CausticDeviceClass {
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

    public void remoteCall(BridgeConnector.DeviceAddress target, CausticDeviceClass device, int operationId, String argument)
    {
        int reqSize = 23;
        byte[] request = new byte[reqSize];
        MemoryUtils.zerify(request);

        int size = device.functions.serializeCall(operationId, argument, request, 0, reqSize);
        if (size == 0) {
            Log.e(TAG, "Fatal: Cannot serialize call request for " + operationId + "!");
            return;
        }
        BridgeConnector.getInstance().sendMessage(target, request, size);
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
