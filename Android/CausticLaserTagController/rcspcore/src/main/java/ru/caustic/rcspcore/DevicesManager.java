package ru.caustic.rcspcore;

import android.os.SystemClock;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

/**
 * This class knows all connected devices and can pop their full states (all their parameters).
 * Also it dispatch RCSP messages. External RCSPOperationDispatcher s may be connected
 */
public class DevicesManager {
    public final int syncTimeout = 4000;
    // Public classes
    public interface SynchronizationEndListener {
        void onSynchronizationEnd(boolean isSuccess, final Set<BridgeConnector.DeviceAddress> notSynchronized);
    }
    // @todo use it instead of handler
    public interface DeviceListUpdatedListener {
        void onDevicesListUpdated();
    }
    /**
     * Interface for any kinds of RCSP operations dispatcher outside from DevicesManager
     */
    public interface RCSPOperationDispatcher {
        /**
         * Dispatch RCSP operation from buffer
         * @param address Address of message sender
         * @param operation Operation to dispatch
         * @return size of block used by dispatcher or 0 if did not dispatch anything
         */
        void dispatchOperation(BridgeConnector.DeviceAddress address, RCSProtocol.RCSPOperation operation);
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
                bridgeConnector.sendMessage(addr, request, cursor);
        }
    }
    public class RCSPMultiStream {
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
    public class CausticDevice {
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

        public void popFromDevice(int parameterToPop) {
            // @todo Optimize: do not request parameters than not exists
            RCSPMultiStream stream = new RCSPMultiStream();
            stream.addObjectRequest(parameterToPop);
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
        private final SynchronizationEndListener handler;
        public final Set<BridgeConnector.DeviceAddress> devicesToPop;
        public final int parameterToPop;

        public AsyncDataPopper(SynchronizationEndListener endHandler, final Set<BridgeConnector.DeviceAddress> devices) {
            handler = endHandler;
            devicesToPop = devices;
            this.parameterToPop = 0;
        }
        public AsyncDataPopper(SynchronizationEndListener endHandler, final Set<BridgeConnector.DeviceAddress> devices, int parameterToPop) {
            handler = endHandler;
            devicesToPop = devices;
            this.parameterToPop = parameterToPop;
        }

        @Override
        public void run() {
            if (devicesToPop == null)
                return;
            boolean syncSuccess = true;
            Set<BridgeConnector.DeviceAddress> failedToSync = new HashSet<>();
            // Giving 'pop' command for every device
            for (BridgeConnector.DeviceAddress addr : devicesToPop) {
                CausticDevice dev = devices.get(addr);
                if (dev != null) {
                    if (parameterToPop == 0) {
                        dev.popFromDevice();
                        if (!waitForDeviceSync(dev)) {
                            syncSuccess = false;
                            failedToSync.add(addr);
                        }
                    } else {
                        dev.popFromDevice(parameterToPop);
                        if (!waitForDeviceSync(dev, parameterToPop)) {
                            syncSuccess = false;
                            failedToSync.add(addr);
                        }
                    }
                }
            }
            if (!syncSuccess) {
                for (BridgeConnector.DeviceAddress addr : failedToSync) {
                    devices.remove(addr);
                }
            }
            if (handler != null)
                handler.onSynchronizationEnd(syncSuccess, failedToSync);
        }

        private final int timeout = 10000;
        private boolean needStop;
        public void stopWaiting() {
            needStop = true;
        }

        private boolean waitForDeviceSync(CausticDevice device) {
            // @todo Add timeout and handler support here
            needStop = false;
            long startTime = System.currentTimeMillis();
            for (Map.Entry<Integer, RCSProtocol.AnyParameterSerializer> entry : device.parameters.entrySet()) {
                while (!entry.getValue().isSync()) {
                    SystemClock.sleep(30);
                    if (needStop || System.currentTimeMillis() - startTime > syncTimeout)
                        return false;
                }
            }
            return true;
        }

        private boolean waitForDeviceSync(CausticDevice device, int parameterId) {
            // @todo Add timeout and handler support here
            needStop = false;
            long startTime = System.currentTimeMillis();
            while (!device.parameters.get(parameterId).isSync()) {
                SystemClock.sleep(30);
                if (needStop || System.currentTimeMillis() - startTime > syncTimeout)
                    return false;
            }
            return true;
        }
    }

    // Public methods
    public void remoteCall(BridgeConnector.DeviceAddress target, RCSProtocol.FunctionsContainer functionsContainer, int operationId, String argument) {
        RCSPStream stream = new RCSPStream();
        stream.addFunctionCall2(functionsContainer, operationId, argument);
        stream.send(target);
    }

    public void setDeviceListUpdatedListener(DeviceListUpdatedListener deviceListUpdatedListener) {
        this.deviceListUpdatedListener = deviceListUpdatedListener;
    }
    /**
     * Update devices list. Handler will be called after every new discovered device
     */
    public void updateDevicesList() {
        RCSPStream stream = new RCSPStream();
        stream.addObjectRequest(RCSProtocol.Operations.AnyDevice.Configuration.deviceName.getId());
        stream.addObjectRequest(RCSProtocol.Operations.AnyDevice.Configuration.deviceType.getId());
        devices.clear();
        stream.send(BridgeConnector.Broadcasts.anyGameDevice);
    }
    public void asyncPopParametersFromDevices(SynchronizationEndListener endHandler, final Set<BridgeConnector.DeviceAddress> devices) {
        // @todo Remove this line and create AsyncDataPopper only once. This line prevents crash on second run dataPopper.start() if devs list item checked, unchecked and checked again
        // @todo What about timeout, if device not respond?
        dataPopper = new AsyncDataPopper(endHandler, devices);
        dataPopper.start();
    }
    public void asyncPopOneParameter(SynchronizationEndListener endHandler, final Set<BridgeConnector.DeviceAddress> devices, int parameterId) {
        // @todo Remove this line and create AsyncDataPopper only once. This line prevents crash on second run dataPopper.start() if devs list item checked, unchecked and checked again
        dataPopper = new AsyncDataPopper(endHandler, devices, parameterId);
        dataPopper.start();
    }
    public void asyncPopPlayerIdsForAllSupportingDevices(SynchronizationEndListener endHandler) {
        asyncPopOneParameter(endHandler, devices.keySet(), RCSProtocol.Operations.HeadSensor.Configuration.playerGameId.getId());
    }

    public int getPlayerGameId(BridgeConnector.DeviceAddress deviceAddress) {
        CausticDevice dev = devices.get(deviceAddress);
        if (dev == null)
            return 0;
        int parameterId = RCSProtocol.Operations.HeadSensor.Configuration.playerGameId.getId();
        int playerGameId = Integer.parseInt(dev.parameters.get(parameterId).getValue());
        return playerGameId;
    }
    public void registerRCSPOperationDispatcher(int operationCode, RCSPOperationDispatcher dispatcher) {
        externalDispatchers.put(operationCode, dispatcher);
    }

    // Public fields
    public static final int DEVICES_LIST_UPDATED = 1;
    public Map<BridgeConnector.DeviceAddress, CausticDevice> devices = new HashMap<BridgeConnector.DeviceAddress, CausticDevice>();

    // Private
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

            List<RCSProtocol.RCSPOperation> operations = RCSProtocol.RCSPOperation.parseStream(data, offset, size);
            for (RCSProtocol.RCSPOperation operation : operations)
            {
                if (operation.isNOP())
                    continue;
                if (dispatchByExternal(address, operation) == 0) {
                    // It is not parameter of device so maybe it is command for android?
                    dev.parameters.deserializeOneParamter(operation);
                }
            }

            if (dev.hasName()
                    && dev.isTypeKnown()
                    && !dev.areDeviceRelatedParametersAdded()) {
                // Device is ready to operate with it: we know address, type and name
                dev.addDeviceRelatedParameters();
                //addPreferenceHeaderForDevice(dev);
                //dev.popFromDevice();
                if (deviceListUpdatedListener != null)
                    deviceListUpdatedListener.onDevicesListUpdated();
            }
        }
    }

    public DevicesManager(BridgeConnector bridgeConnector) {
        this.bridgeConnector = bridgeConnector;
        bridgeConnector.setReceiver(new Receiver());
    }

    private int dispatchByExternal(BridgeConnector.DeviceAddress address, RCSProtocol.RCSPOperation operation) {
        if (operation.type != RCSProtocol.OperationCodeType.CALL_REQUEST)
            return 0;

        RCSPOperationDispatcher externalDispatcher = externalDispatchers.get(operation.id);
        if (externalDispatcher == null)
            return 0;

        externalDispatcher.dispatchOperation(address, operation);
        return operation.size();
    }

    private BridgeConnector bridgeConnector;
    private static final String TAG = "CC.CausticDevManager";
    private AsyncDataPopper dataPopper = null;
    private Map<Integer, RCSPOperationDispatcher> externalDispatchers = new TreeMap<>();
    private DeviceListUpdatedListener deviceListUpdatedListener = null;
}
