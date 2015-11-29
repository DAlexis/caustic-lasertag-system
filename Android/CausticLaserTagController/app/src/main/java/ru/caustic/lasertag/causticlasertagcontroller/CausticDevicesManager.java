package ru.caustic.lasertag.causticlasertagcontroller;

import android.os.Handler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by alexey on 20.09.15.
 */
public class CausticDevicesManager {

    private static final String TAG = "CC.CausticDevManager";
    public static final int DEVICES_LIST_UPDATED = 1;

    private DeviceManagerTask currentTask = null;

    private static CausticDevicesManager ourInstance = new CausticDevicesManager();

    private CausticDevicesManager() {

    }

    public static CausticDevicesManager getInstance() {
        return ourInstance;
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

    /////////////////////////
    // New version
    public Map<BridgeConnector.DeviceAddress, CausticDevice> devices2 = new HashMap<BridgeConnector.DeviceAddress, CausticDevice>();
    private Handler devicesListUpdated = null;

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
    }

    public boolean updateDevicesList(Handler devicesListUpdated) {
        this.devicesListUpdated = devicesListUpdated;
        currentTask = new TaskUpdateDevicesList();
        return true;
    }

    class TaskUpdateDevicesList implements DeviceManagerTask {

        @Override
        public String taskText() {
            return "Updating devices list";
        }

        public TaskUpdateDevicesList() {
            BridgeConnector.getInstance().setReceiver(new Receiver());
            RCSPStream stream = new RCSPStream();
            stream.addObjectRequest(RCSProtocol.Operations.AnyDevice.Configuration.deviceName.getId());
            stream.addObjectRequest(RCSProtocol.Operations.AnyDevice.Configuration.deviceType.getId());
            devices2.clear();
            stream.send(BridgeConnector.Broadcasts.any);
        }
    }

    public void remoteCall(BridgeConnector.DeviceAddress target, RCSProtocol.FunctionsContainer functionsContainer, int operationId, String argument)
    {
        RCSPStream stream = new RCSPStream();
        stream.addFunctionCall2(functionsContainer, operationId, argument);
        stream.send(target);
    }

    class Receiver implements BridgeConnector.IncomingPackagesListener {
        @Override
        public void getData(BridgeConnector.DeviceAddress address, byte[] data, int offset, int size)
        {
            //boolean newDeviceAdded = false;
            CausticDevice dev = devices2.get(address);
            if (dev == null) {
                dev = new CausticDevice();
                dev.address = new BridgeConnector.DeviceAddress(address);
                devices2.put(address, dev);
                //newDeviceAdded = true;
            }

            dev.parameters.deserializeStream(data, offset, size);

            if (dev.hasName()
                    && dev.isTypeKnown()
                    && !dev.areDeviceRelatedParametersAdded()) {
                // Device is ready to operate with it: we know address, type and name
                dev.addDeviceRelatedParameters();
                //addPreferenceHeaderForDevice(dev);
                dev.popFromDevice();
                devicesListUpdated.obtainMessage(DEVICES_LIST_UPDATED, 0, 0, devices2).sendToTarget();
            }
        }
    }
}
