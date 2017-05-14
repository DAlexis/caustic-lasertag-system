package ru.caustic.rcspcore;

import android.os.SystemClock;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * This class knows all connected devices and can pop their full states (all their parameters).
 * Also it dispatch RCSP messages. External OperationDispatcher s may be connected
 */
public class DevicesManager {
    public final int syncTimeout = 4000;
    // Public classes
    public interface SynchronizationEndListener {
        void onSynchronizationEnd(boolean isSuccess, final Set<RCSP.DeviceAddress> notSynchronized);
    }
    // @todo use it instead of handler
    public interface DeviceListUpdatedListener {
        void onDevicesListUpdated();
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
            int size = RCSP.ParametersContainer.serializeParameterRequest(
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
            RCSP.ParametersContainer pars = dev.parameters;

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

        public boolean addFunctionCall2(RCSP.FunctionsContainer functionsContainer, int id, String argument) {
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

        public void send(RCSP.DeviceAddress addr) {
            if (cursor != 0)
                bridgeDriver.sendMessage(addr, request, cursor);
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


        public void send(RCSP.DeviceAddress addr) {
            for (RCSPStream stream : streams) {
                stream.send(addr);
            }
        }
    }
    public class CausticDevice {
        public RCSP.DeviceAddress address = new RCSP.DeviceAddress();
        public RCSP.ParametersContainer parameters = new RCSP.ParametersContainer();

        public CausticDevice() {
            // Registering parameters for any devices
            RCSP.Operations.AnyDevice.parametersDescriptions.addParameters(parameters);
            touch();
        }

        /**
         * Update timestam of last access
         */
        public void touch() {
            lastAccessTime = System.currentTimeMillis();
        }
        public boolean isOutdated() {
            return System.currentTimeMillis() - lastAccessTime > deviceLifetime;
        }
        public String getName() {
            return parameters.get(RCSP.Operations.AnyDevice.Configuration.deviceName.getId()).getValue();
        }
        public boolean hasName() {
            return ( (RCSP.DevNameParameter.Serializer) parameters.get(RCSP.Operations.AnyDevice.Configuration.deviceName.getId()) ).initialized();
        }
        public boolean isTypeKnown() {
            return (
                    Integer.parseInt(parameters.get(RCSP.Operations.AnyDevice.Configuration.deviceType.getId()).getValue())
                            != RCSP.Operations.AnyDevice.Configuration.DEV_TYPE_UNDEFINED
            );
        }
        public String getType() {
            return RCSP.Operations.AnyDevice.getDevTypeString(
                    Integer.parseInt(
                            parameters.get(RCSP.Operations.AnyDevice.Configuration.deviceType.getId()).getValue()
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

            RCSP.AnyParameterSerializer typeParam = parameters.get(RCSP.Operations.AnyDevice.Configuration.deviceType.getId());
            if (typeParam == null)
                return;
            int type = Integer.parseInt(typeParam.getValue());
            switch (type) {
                case RCSP.Operations.AnyDevice.Configuration.DEV_TYPE_UNDEFINED:
                    return;
                case RCSP.Operations.AnyDevice.Configuration.DEV_TYPE_RIFLE:
                    RCSP.Operations.Rifle.parametersDescriptions.addParameters(parameters);
                    break;
                case RCSP.Operations.AnyDevice.Configuration.DEV_TYPE_HEAD_SENSOR:
                    RCSP.Operations.HeadSensor.parametersDescriptions.addParameters(parameters);
                    break;
                default:
                    return;
            }

            parametersAreAdded = true;
        }
        public void invalidateParameters() {
            if (!parametersAreAdded)
                return;
            for (RCSP.AnyParameterSerializer param : parameters.allParameters.values())
            {
                param.invalidate();
            }

        }
        public void invalidateMapParameters() {
            if (!parametersAreAdded)
                return;
            for (RCSP.AnyParameterSerializer param : parameters.allParameters.values())
            {
                int id = param.getDescription().getId();
                if (id == RCSP.Operations.AnyDevice.Configuration.deviceName.getId()) {
                    param.invalidate();
                }
                if (id == RCSP.Operations.HeadSensor.Configuration.teamMT2Id.getId()) {
                    param.invalidate();
                }
                if (id == RCSP.Operations.HeadSensor.Configuration.markerColor.getId()) {
                    param.invalidate();
                }
                if (id == RCSP.Operations.HeadSensor.Configuration.playerLat.getId()) {
                    param.invalidate();
                }
                if (id == RCSP.Operations.HeadSensor.Configuration.playerLon.getId()) {
                    param.invalidate();
                }
                if (id == RCSP.Operations.HeadSensor.Configuration.currentHealth.getId()) {
                    param.invalidate();
                }
                if (id == RCSP.Operations.HeadSensor.Configuration.deathCount.getId()) {
                    param.invalidate();
                }
            }

        }
        public void pushToDevice() {
            RCSPMultiStream stream = new RCSPMultiStream();
            for (Map.Entry<Integer, RCSP.AnyParameterSerializer> entry : parameters.entrySet()) {
                if (!entry.getValue().isSync())
                    stream.addSetObject(this, entry.getKey());
            }
            stream.send(address);
        }
        public void pushToDevice(int parameterToPush) {
            RCSPMultiStream stream = new RCSPMultiStream();
            stream.addSetObject(this, parameterToPush);
            stream.send(address);
        }
        public void popFromDevice() {
            RCSPMultiStream stream = new RCSPMultiStream();
            for (Map.Entry<Integer, RCSP.AnyParameterSerializer> entry : parameters.entrySet()) {
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
        public boolean areParametersSync() {
            for (Map.Entry<Integer, RCSP.AnyParameterSerializer> entry : parameters.entrySet()) {
                if (!entry.getValue().isSync())
                    return false;
            }
            return true;
        }

        private final long deviceLifetime = 10000;
        private boolean parametersAreAdded = false;
        private long lastAccessTime = 0;
    }
    /**
     * This object gets all device parameters on
     */
    public class AsyncDataPuller extends Thread {
        /// @todo Make this class reusable without re-creation
        private final SynchronizationEndListener handler;
        public final Set<RCSP.DeviceAddress> devicesToPop;
        public final int parameterToPop;

        public AsyncDataPuller(SynchronizationEndListener endHandler, final Set<RCSP.DeviceAddress> devices) {
            handler = endHandler;
            devicesToPop = devices;
            this.parameterToPop = 0;
        }
        public AsyncDataPuller(SynchronizationEndListener endHandler, final Set<RCSP.DeviceAddress> devices, int parameterToPop) {
            handler = endHandler;
            devicesToPop = devices;
            this.parameterToPop = parameterToPop;
        }

        @Override
        public void run() {
            if (devicesToPop == null)
                return;
            boolean syncSuccess = true;
            Set<RCSP.DeviceAddress> failedToSync = new HashSet<>();
            // Giving 'pop' command for every device
            for (RCSP.DeviceAddress addr : devicesToPop) {
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
                for (RCSP.DeviceAddress addr : failedToSync) {
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
            for (Map.Entry<Integer, RCSP.AnyParameterSerializer> entry : device.parameters.entrySet()) {
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
    public void remoteCall(RCSP.DeviceAddress target, RCSP.FunctionsContainer functionsContainer, int operationId, String argument) {
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
        stream.addObjectRequest(RCSP.Operations.AnyDevice.Configuration.deviceName.getId());
        stream.addObjectRequest(RCSP.Operations.AnyDevice.Configuration.deviceType.getId());
        //removeOutdatedDevices();
        stream.send(BridgeDriver.Broadcasts.anyGameDevice);
        devicesListUpdatedWaiter.touch();
        devicesListUpdatedWaiter.enableCallback(true);
    }
    public void removeOutdatedDevices() {
        List<RCSP.DeviceAddress> toDelete = new ArrayList<>();
        for (Map.Entry<RCSP.DeviceAddress, CausticDevice> dev : devices.entrySet())
        {
            if (dev.getValue().isOutdated())
                toDelete.add(dev.getKey());
        }
        devices.remove(toDelete);
    }

    public void devicesWatchingTaskTick() {

    }

    public void asyncPullAllParameters(SynchronizationEndListener endHandler, final Set<RCSP.DeviceAddress> devices) {
        // @todo Remove this line and create AsyncDataPuller only once. This line prevents crash on second run dataPopper.start() if devs list item checked, unchecked and checked again
        // @todo What about timeout, if device not respond?
        dataPopper = new AsyncDataPuller(endHandler, devices);
        dataPopper.start();
    }
    public void asyncPullOneParameter(SynchronizationEndListener endHandler, final Set<RCSP.DeviceAddress> devices, int parameterId) {
        // @todo Remove this line and create AsyncDataPuller only once. This line prevents crash on second run dataPopper.start() if devs list item checked, unchecked and checked again
        dataPopper = new AsyncDataPuller(endHandler, devices, parameterId);
        dataPopper.start();
    }
    /*
    public void asyncPullPlayerIdsForAllSupportingDevices(SynchronizationEndListener endHandler) {
        asyncPullOneParameter(endHandler, devices.keySet(), RCSP.Operations.HeadSensor.Configuration.playerGameId.getId());
    }*/

    public void invalidateDevsParams(final Collection<CausticDevice> devices) {
        for (CausticDevice dev : devices)
        {
            dev.invalidateParameters();
        }
    }
    public void invalidateDevsMapParams(final Collection<CausticDevice> devices) {
        for (CausticDevice dev : devices)
        {
            dev.invalidateMapParameters();
        }
    }
    public int getPlayerGameId(RCSP.DeviceAddress deviceAddress) {
        CausticDevice dev = devices.get(deviceAddress);
        if (dev == null)
            return 0;
        int parameterId = RCSP.Operations.HeadSensor.Configuration.playerGameId.getId();
        int playerGameId = Integer.parseInt(dev.parameters.get(parameterId).getValue());
        return playerGameId;
    }
    public boolean associateWithHeadSensor() {
        ///@todo Add proper head sensor association
        String deviceName = BluetoothManager.getInstance().getDeviceName();
        if (deviceName.equals("Integrated CBB")) {
            associatedHeadSensorAddress = new RCSP.DeviceAddress(10, 0, 3);
            return true;
        }
        if (deviceName.equals("nrfBridge")) {
            associatedHeadSensorAddress = new RCSP.DeviceAddress(5, 0, 1);
            return true;
        }
        return false;
    }

    // Public fields
    public static final int DEVICES_LIST_UPDATED = 1;
    public Map<RCSP.DeviceAddress, CausticDevice> devices = new HashMap<RCSP.DeviceAddress, CausticDevice>();
    public RCSP.DeviceAddress associatedHeadSensorAddress;

    // Private

    private class DevParDispatcher implements RCSP.OperationDispatcher {
        @Override
        public boolean dispatchOperation(RCSP.DeviceAddress address, RCSP.RCSPOperation operation) {
            if (operation.type != RCSP.OperationCodeType.SET_OBJECT)
                return false;

            CausticDevice dev = devices.get(address);
            if (dev == null) {
                dev = new CausticDevice();
                dev.address = new RCSP.DeviceAddress(address);
                devices.put(address, dev);
                //newDeviceAdded = true;
            }

            dev.parameters.deserializeOneParamter(operation);
            dev.touch();

            if (dev.hasName()
                    && dev.isTypeKnown()
                    && !dev.areDeviceRelatedParametersAdded())
            {
                // Device is ready to operate with it: we know address, type and name
                dev.addDeviceRelatedParameters();
                devicesListUpdatedWaiter.touch();
            }

            return true;
        }
    }

    public DevicesManager(BridgeDriver bridgeDriver, RCSP.OperationDispatcherUser dispatchersUser) {
        this.bridgeDriver = bridgeDriver;
        dispatchersUser.addOperationDispatcher(new DevParDispatcher());
        devicesListUpdatedWaiter = new Utils.SimpleWaiter(1000, 100, new Runnable() {
            @Override
            public void run() {
                devicesListUpdatedWaiter.enableCallback(false);
                if (deviceListUpdatedListener != null)
                    deviceListUpdatedListener.onDevicesListUpdated();
            }
        });
    }

    private BridgeDriver bridgeDriver;
    private static final String TAG = "CC.CausticDevManager";
    private AsyncDataPuller dataPopper = null;
    private DeviceListUpdatedListener deviceListUpdatedListener = null;
    private Utils.SimpleWaiter devicesListUpdatedWaiter;
}
