package org.ltcaustic.rcspcore;


import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import java.lang.Thread;

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

    /**
     * Thread does this:
     * for every known device:
     * {
     *     pop from device (send parameters requests);
     *     wait while all parameters are synchronized or timeouted;
     * }
     */
    public class AsyncDataPuller extends Thread {
        /// @todo Make this class reusable without re-creation
        /// @todo Add previous operation stopping
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
            running = true;
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
                if (needStop)
                    break;
            }
            if (!syncSuccess) {
                for (RCSP.DeviceAddress addr : failedToSync) {
                    devices.remove(addr);
                }
            }
            if (handler != null)
                handler.onSynchronizationEnd(syncSuccess, failedToSync);
            running = false;
        }

        public boolean isRunning() {
            return running;
        }
        public void stopWaiting() {
            needStop = true;
        }
        public void stopAndJoin() {
            stopWaiting();
            try {
                join();
            } catch (InterruptedException ex) {
            }
        }

        private boolean waitForDeviceSync(CausticDevice device) {
            // @todo Add timeout and handler support here

            needStop = false;
            long startTime = System.currentTimeMillis();
            for (Map.Entry<Integer, RCSP.AnyParameterSerializer> entry : device.parameters.entrySet()) {
                while (!entry.getValue().isSync()) {
                    try {
                        Thread.sleep(30);
                    } catch (InterruptedException ex) { }
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
                try {
                    Thread.sleep(30);
                } catch (InterruptedException ex) { }
                if (needStop || System.currentTimeMillis() - startTime > syncTimeout)
                    return false;
            }
            return true;
        }

        private boolean needStop;
        private boolean running = false;
    }

    // Public methods
    public void remoteCall(RCSP.DeviceAddress target, RCSP.FunctionsContainer functionsContainer, int operationId, String argument) {
        RCSPStream stream = new RCSPStream(communicator);
        stream.addFunctionCall(functionsContainer, operationId, argument);
        stream.send(target);
    }

    public void subscribeDevicesListUpdated(DeviceListUpdatedListener deviceListUpdatedListener) {
        this.deviceListUpdatedListeners.add(deviceListUpdatedListener);
    }
    /**
     * Update devices list. Handler will be called after every new discovered device
     */
    public void updateDevicesList() {
        RCSPStream stream = new RCSPStream(communicator);
        stream.addObjectRequest(RCSP.Operations.AnyDevice.Configuration.deviceName.getId());
        stream.addObjectRequest(RCSP.Operations.AnyDevice.Configuration.deviceType.getId());
        removeOutdatedDevices();
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
        for (RCSP.DeviceAddress key : toDelete)
            devices.remove(key);
    }

    public void devicesWatchingTaskTick() {

    }

    public void asyncPullAllParameters(SynchronizationEndListener endHandler, final Set<RCSP.DeviceAddress> devices) {
        stopAsyncPopping();
        dataPopper = new AsyncDataPuller(endHandler, devices);
        dataPopper.start();
    }
    public void asyncPullOneParameter(SynchronizationEndListener endHandler, final Set<RCSP.DeviceAddress> devices, int parameterId) {
        stopAsyncPopping();
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
        /*
        String deviceName = BluetoothManager.getInstance().getDeviceName();
        if (deviceName.equals("Integrated CBB")) {
            associatedHeadSensorAddress = new RCSP.DeviceAddress(10, 0, 3);
            return true;
        }
        if (deviceName.equals("nrfBridge")) {
            associatedHeadSensorAddress = new RCSP.DeviceAddress(5, 0, 1);
            return true;
        }*/
        return false;
    }
    public int devicesCount() {
        return devices.size();
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
                dev = new CausticDevice(communicator);
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

    public DevicesManager(BridgeDriver bridgeDriver, LTSCommunicator communicator) {
        this.bridgeDriver = bridgeDriver;
        this.communicator = communicator;
        communicator.addOperationDispatcher(new DevParDispatcher());
        devicesListUpdatedWaiter = new Utils.SimpleWaiter(1000, 100, new Runnable() {
            @Override
            public void run() {
                devicesListUpdatedWaiter.enableCallback(false);
                for (DeviceListUpdatedListener listener : deviceListUpdatedListeners)
                    listener.onDevicesListUpdated();
            }
        });
    }

    private void stopAsyncPopping() {
        if (dataPopper != null && dataPopper.isRunning())
            dataPopper.stopAndJoin();
    }
    private BridgeDriver bridgeDriver;
    private static final String TAG = "CC.CausticDevManager";
    private AsyncDataPuller dataPopper = null;
    private List<DeviceListUpdatedListener> deviceListUpdatedListeners = new ArrayList<>();
    private Utils.SimpleWaiter devicesListUpdatedWaiter;
    private LTSCommunicator communicator;
}
