package ru.caustic.lasertag.core;

import android.os.Handler;
import android.os.SystemClock;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * Created by alexey on 20.09.15.
 */
public class CausticDevicesManager {
    // Public classes
    public interface SynchronizationEndHandler {
        void onSynchronizationEnd(boolean isSuccess);
    }
    public static class RCSPStream {
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

        public boolean addFunctionCall2(RCSProtocol.FunctionsContainer functionsContainer, int id, String argument) {
            int size = functionsContainer.serializeCall(
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
            if (cursor != 0)
                BridgeConnector.getInstance().sendMessage(addr, request, cursor);
        }
    }
    public static class RCSPMultiStream {
        List<RCSPStream> streams = new ArrayList<>();

        public RCSPMultiStream() {
            streams.add(new RCSPStream());
        }

        public void addObjectRequest(int id) {
            if (!streams.get(streams.size()-1).addObjectRequest(id)) {
                streams.add(new RCSPStream());
                streams.get(streams.size()-1).addObjectRequest(id);
            }
        }

        public void addSetObject(CausticDevice dev, int id) {
            if (!streams.get(streams.size()-1).addSetObject(dev, id)) {
                streams.add(new RCSPStream());
                streams.get(streams.size()-1).addSetObject(dev, id);
            }
        }


        public void send(BridgeConnector.DeviceAddress addr) {
            for (RCSPStream stream : streams) {
                stream.send(addr);
            }
        }
    }
    public static class CausticDevice {
        private boolean parametersAreAdded = false;
        public BridgeConnector.DeviceAddress address = new BridgeConnector.DeviceAddress();
        public RCSProtocol.ParametersContainer parameters = new RCSProtocol.ParametersContainer();

        public CausticDevice() {
            // Registering parameters for any devices
            RCSProtocol.Operations.AnyDevice.parametersDescriptions.addParameters(parameters);
        }

        public String getName() {
            return parameters.get(RCSProtocol.Operations.AnyDevice.Configuration.deviceName.getId()).getValue();
        }
        public boolean hasName() {
            return ( (RCSProtocol.DevNameParameter.Serializer) parameters.get(RCSProtocol.Operations.AnyDevice.Configuration.deviceName.getId()) ).initialized();
        }
        public boolean isTypeKnown() {
            return (
                    Integer.parseInt(parameters.get(RCSProtocol.Operations.AnyDevice.Configuration.deviceType.getId()).getValue())
                            != RCSProtocol.Operations.AnyDevice.Configuration.DEV_TYPE_UNDEFINED
            );
        }
        public String getType() {
            return RCSProtocol.Operations.AnyDevice.getDevTypeString(
                    Integer.parseInt(
                            parameters.get(RCSProtocol.Operations.AnyDevice.Configuration.deviceType.getId()).getValue()
                    )
            );
        }
        public boolean areDeviceRelatedParametersAdded() {
            return parametersAreAdded;
        }
        /// Initially add all missing parameters to parameters list according to device type if set
        public void addDeviceRelatedParameters() {
            if (parametersAreAdded)
                return;

            RCSProtocol.AnyParameterSerializer typeParam = parameters.get(RCSProtocol.Operations.AnyDevice.Configuration.deviceType.getId());
            if (typeParam == null)
                return;
            int type = Integer.parseInt(typeParam.getValue());
            switch (type) {
                case RCSProtocol.Operations.AnyDevice.Configuration.DEV_TYPE_UNDEFINED:
                    return;
                case RCSProtocol.Operations.AnyDevice.Configuration.DEV_TYPE_RIFLE:
                    RCSProtocol.Operations.Rifle.parametersDescriptions.addParameters(parameters);
                    break;
                case RCSProtocol.Operations.AnyDevice.Configuration.DEV_TYPE_HEAD_SENSOR:
                    RCSProtocol.Operations.HeadSensor.parametersDescriptions.addParameters(parameters);
                    break;
                default:
                    return;
            }

            parametersAreAdded = true;
        }
        public void pushToDevice() {
            RCSPMultiStream stream = new RCSPMultiStream();
            for (Map.Entry<Integer, RCSProtocol.AnyParameterSerializer> entry : parameters.entrySet()) {
                if (!entry.getValue().isSync())
                    stream.addSetObject(this, entry.getKey());
            }
            stream.send(address);
        }
        public void popFromDevice() {
            RCSPMultiStream stream = new RCSPMultiStream();
            for (Map.Entry<Integer, RCSProtocol.AnyParameterSerializer> entry : parameters.entrySet()) {
                if (!entry.getValue().isSync())
                    stream.addObjectRequest(entry.getKey());
            }
            stream.send(address);
        }

        public boolean areParametersSync()
        {
            for (Map.Entry<Integer, RCSProtocol.AnyParameterSerializer> entry : parameters.entrySet()) {
                if (!entry.getValue().isSync())
                    return false;
            }
            return true;
        }
    }
    /**
     * This object gets all device parameters on
     */
    public class AsyncDataPopper extends Thread {
        /// @todo Make this class reusable without re-creation
        private final SynchronizationEndHandler handler;
        public final Set<BridgeConnector.DeviceAddress> devicesToPop;

        public AsyncDataPopper(SynchronizationEndHandler endHandler, final Set<BridgeConnector.DeviceAddress> devices) {
            handler = endHandler;
            devicesToPop = devices;
        }

        @Override
        public void run() {
            if (devicesToPop == null)
                return;
            boolean syncSuccess = true;
            // Giving 'pop' command for every device
            for (BridgeConnector.DeviceAddress addr : devicesToPop) {
                CausticDevice dev = devices.get(addr);
                if (dev != null) {
                    dev.popFromDevice();
                }
                if (!waitForDeviceSync(dev)) {
                    syncSuccess = false;
                }

            }
            if (handler != null)
                handler.onSynchronizationEnd(syncSuccess);
        }

        private final int timeout = 10000;
        private boolean needStop;
        public void stopWaiting() {
            needStop = true;
        }

        private boolean waitForDeviceSync(CausticDevice device) {
            // @todo Add timeout and handler support here
            needStop = false;
            for (Map.Entry<Integer, RCSProtocol.AnyParameterSerializer> entry : device.parameters.entrySet()) {
                while (!entry.getValue().isSync()) {
                    SystemClock.sleep(30);
                    if (needStop)
                        return false;
                }
            }
            return true;
        }
    }

    // Public methods
    public static CausticDevicesManager getInstance() {
        return ourInstance;
    }
    public void remoteCall(BridgeConnector.DeviceAddress target, RCSProtocol.FunctionsContainer functionsContainer, int operationId, String argument) {
        RCSPStream stream = new RCSPStream();
        stream.addFunctionCall2(functionsContainer, operationId, argument);
        stream.send(target);
    }
    public boolean updateDevicesList(Handler devicesListUpdated) {
        this.devicesListUpdatedHandler = devicesListUpdated;
        currentTask = new TaskUpdateDevicesList();
        return true;
    }
    public void asyncPopParametersFromDevices(SynchronizationEndHandler endHandler, final Set<BridgeConnector.DeviceAddress> devices) {
        // @todo Remove this line and create AsyncDataPopper only once. This line prevents crash on second run dataPopper.start() if devs list item checked, unchecked and checked again
        dataPopper = new AsyncDataPopper(endHandler, devices);
        dataPopper.start();
    }
    public int getPlayerGameId(BridgeConnector.DeviceAddress deviceAddress)
    {
        CausticDevice dev = devices.get(deviceAddress);
        if (dev == null)
            return 0;
        int parameterId = RCSProtocol.Operations.HeadSensor.Configuration.teamMT2Id.getId();
        int playerGameId = Integer.parseInt(dev.parameters.get(parameterId).getValue());
        return playerGameId;
    }

    // Public fields
    public static final int DEVICES_LIST_UPDATED = 1;
    public Map<BridgeConnector.DeviceAddress, CausticDevice> devices = new HashMap<BridgeConnector.DeviceAddress, CausticDevice>();

    // Private
    // @todo Connect one instance of the Receiver permanently to BridgeConnector
    private class Receiver implements BridgeConnector.IncomingPackagesListener {
        @Override
        public void getData(BridgeConnector.DeviceAddress address, byte[] data, int offset, int size)
        {
            //boolean newDeviceAdded = false;
            CausticDevice dev = devices.get(address);
            if (dev == null) {
                dev = new CausticDevice();
                dev.address = new BridgeConnector.DeviceAddress(address);
                devices.put(address, dev);
                //newDeviceAdded = true;
            }

            // This line replaced by cycle below for parsing direct command for android device
            // dev.parameters.deserializeStream(data, offset, size);
            int notParsed = size;
            for (int i = 0; i<size; )
            {
                int result = dev.parameters.deserializeOneParamter(data, offset + i, notParsed);
                if (result == 0) {
                    // It is not parameter of device so maybe it is command for android?
                    result = deserializeAndroidMsg(address, data, offset + i, notParsed);
                }
                if (result == 0) {
                    // It is absolutely unknown command so we should skip it
                    result = RCSProtocol.nextCommandSize(data, offset + i);
                    if (result > notParsed)
                        break;
                }

                i += result;
                notParsed -= result;
            }

            if (dev.hasName()
                    && dev.isTypeKnown()
                    && !dev.areDeviceRelatedParametersAdded()) {
                // Device is ready to operate with it: we know address, type and name
                dev.addDeviceRelatedParameters();
                //addPreferenceHeaderForDevice(dev);
                //dev.popFromDevice();
                devicesListUpdatedHandler.obtainMessage(DEVICES_LIST_UPDATED, 0, 0, devices).sendToTarget();
            }
        }
    }

    // @todo Remove or change this class. For now it is simply one method
    private class TaskUpdateDevicesList implements DeviceManagerTask {

        @Override
        public String taskText() {
            return "Updating devices list";
        }

        public TaskUpdateDevicesList() {
            RCSPStream stream = new RCSPStream();
            stream.addObjectRequest(RCSProtocol.Operations.AnyDevice.Configuration.deviceName.getId());
            stream.addObjectRequest(RCSProtocol.Operations.AnyDevice.Configuration.deviceType.getId());
            devices.clear();
            stream.send(BridgeConnector.Broadcasts.any);
        }
    }
    private interface DeviceManagerTask {
        String taskText();
    }

    private CausticDevicesManager() {
        BridgeConnector.getInstance().setReceiver(new Receiver());
    }

    private int deserializeAndroidMsg(BridgeConnector.DeviceAddress address, byte[] memory, int position, int size)
    {
        return 0;
    }

    private static final String TAG = "CC.CausticDevManager";
    private DeviceManagerTask currentTask = null;
    private static CausticDevicesManager ourInstance = new CausticDevicesManager();
    private Handler devicesListUpdatedHandler = null;
    private AsyncDataPopper dataPopper = null;
}
