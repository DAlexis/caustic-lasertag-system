package ru.caustic.lasertag.causticlasertagcontroller;

import android.os.Handler;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by alexey on 20.09.15.
 */
public class CausticDevicesManager {

    private static final String TAG = "CC.CausticDevManager";
    public static final int DEVICES_LIST_UPDATED = 1;

    private DeviceManagerTask currentTask = null;
    private Handler devicesListUpdated = null;

    public Map<BridgeConnector.DeviceAddress, CausticDevice> devices = new HashMap<BridgeConnector.DeviceAddress, CausticDevice>();
    public static CausticDeviceClass headSensor = new CausticDeviceClass();
    public static CausticDeviceClass rife = new CausticDeviceClass();

    private static CausticDevicesManager ourInstance = new CausticDevicesManager();

    private CausticDevicesManager() {
        RCSProtocol.RCSPOperationCodes.AnyDevice.registerFunctions(headSensor.functions);
        RCSProtocol.RCSPOperationCodes.HeadSensor.registerFunctions(headSensor.functions);

        RCSProtocol.RCSPOperationCodes.AnyDevice.registerFunctions(rife.functions);
        RCSProtocol.RCSPOperationCodes.Rifle.registerFunctions(rife.functions);
    }

    public static CausticDevicesManager getInstance() {
        return ourInstance;
    }

    public class CausticDevice {
        private boolean parametersAreAdded = false;
        public BridgeConnector.DeviceAddress address = new BridgeConnector.DeviceAddress();
        public RCSProtocol.ParametersContainer parameters = new RCSProtocol.ParametersContainer();

        public CausticDevice() {
            RCSProtocol.RCSPOperationCodes.AnyDevice.registerParameters(parameters);
        }

        public String getName() {
            return parameters.get(RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.deviceName).getValue();
        }

        public void determineType() {
            RCSPStream stream = new RCSPStream();
            stream.addObjectRequest(RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.deviceType);
            stream.send(address);
        }

        public void addDeviceRelatedParameters() {
            if (parametersAreAdded)
                return;

            int type = Integer.parseInt(parameters.get(RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.deviceType).getValue());
            switch (type) {
                case RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.DEV_TYPE_UNDEFINED:
                    return;
                case RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.DEV_TYPE_RIFLE:
                    RCSProtocol.RCSPOperationCodes.Rifle.registerParameters(parameters);
                    break;
                case RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.DEV_TYPE_HEAD_SENSOR:
                    RCSProtocol.RCSPOperationCodes.HeadSensor.registerParameters(parameters);
                    break;
                default:
                    return;
            }

            parametersAreAdded = true;
        }
    }

    public static class CausticDeviceClass {
        public RCSProtocol.RemoteFunctionsContainer functions = new RCSProtocol.RemoteFunctionsContainer();
    }

    public boolean updateDevicesList(Handler devicesListUpdated) {
        this.devicesListUpdated = devicesListUpdated;
        currentTask = new TaskUpdateDevicesList();
        return true;
    }

    public void remoteCall(BridgeConnector.DeviceAddress target, CausticDeviceClass device, int operationId, String argument)
    {
        RCSPStream stream = new RCSPStream();
        stream.addFunctionCall(device, operationId, argument);
        stream.send(target);
    }

    private interface DeviceManagerTask {
        String taskText();
    }

    public class RCSPStream {
        int requestSize = 23;
        byte[] request = new byte[requestSize];
        int cursor = 0;

        public RCSPStream()
        {
            MemoryUtils.zerify(request);
        }

        public boolean addObjectRequest(int id) {
            int size = RCSProtocol.ParametersContainer.serializeParameterRequest(
                    id,
                    request,
                    cursor,
                    requestSize
            );

            if (size != 0) {
                cursor += size;
                return true;
            } else {
                return false;
            }
        }

        public boolean addSetObject(CausticDevice dev, int id) {
            RCSProtocol.ParametersContainer pars = dev.parameters;

            int size = pars.serializeSetObject(
                    id,
                    request,
                    cursor,
                    requestSize
            );

            if (size != 0) {
                cursor += size;
                return true;
            } else {
                return false;
            }
        }

        public boolean addFunctionCall(CausticDeviceClass devClass, int id, String argument) {
            int size = devClass.functions.serializeCall(
                    id,
                    argument,
                    request,
                    cursor,
                    requestSize
            );

            if (size != 0) {
                cursor += size;
                return true;
            }
            return false;
        }

        public void send(BridgeConnector.DeviceAddress addr) {
            BridgeConnector.getInstance().sendMessage(addr, request, cursor);
        }
    }

    class TaskUpdateDevicesList implements DeviceManagerTask {

        @Override
        public String taskText() {
            return "Updating devices list";
        }

        public TaskUpdateDevicesList() {
            BridgeConnector.getInstance().setReceiver(new Receiver());
            RCSPStream stream = new RCSPStream();
            stream.addObjectRequest(RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.deviceName);
            devices.clear();
            stream.send(BridgeConnector.Broadcasts.any);
        }
    }

    class Receiver implements BridgeConnector.IncomingPackagesListener {
        @Override
        public void getData(BridgeConnector.DeviceAddress address, byte[] data, int offset, int size)
        {
            boolean newDeviceAdded = false;
            CausticDevice dev = devices.get(address);
            if (dev == null) {
                dev = new CausticDevice();
                dev.address = new BridgeConnector.DeviceAddress(address);
                devices.put(address, dev);
                newDeviceAdded = true;
            }
            dev.parameters.deserializeStream(data, offset, size);
            if (newDeviceAdded && devicesListUpdated != null) {
                devicesListUpdated.obtainMessage(DEVICES_LIST_UPDATED, 0, 0, devices).sendToTarget();
            }
        }
    }

}
