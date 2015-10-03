package ru.caustic.lasertag.causticlasertagcontroller;

import android.content.Context;
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

        public boolean hasName() {
            return ( (RCSProtocol.DevNameGroupRCSP) parameters.get(RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.deviceName) ).initialized();
        }

        public boolean isTypeKnown() {
            return (
                    Integer.parseInt(parameters.get(RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.deviceType).getValue())
                    != RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.DEV_TYPE_UNDEFINED
            );
        }

        public String getType() {
            return RCSProtocol.RCSPOperationCodes.AnyDevice.getDevTypeString(
                    Integer.parseInt(
                            parameters.get(RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.deviceType).getValue()
                    )
            );
        }

        public boolean areDeviceRelatedParametersAdded() {
            return parametersAreAdded;
        }

        public void addDeviceRelatedParameters() {
            if (parametersAreAdded)
                return;

            RCSProtocol.RCSPParameterGroup typeParam = parameters.get(RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.deviceType);
            if (typeParam == null)
                return;
            int type = Integer.parseInt(typeParam.getValue());
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

        public void popFromSharedPreferences(Context context) {
            parameters.popFromSharedPreferences(context);
        }

        public void pushToSharedPreferences(Context context) {
            parameters.pushToSharedPreferences(context);
        }

        public void pushToDevice() {
            RCSPMultiStream stream = new RCSPMultiStream();
            for (Map.Entry<Integer, RCSProtocol.RCSPParameterGroup> entry : parameters.entrySet()) {
                if (!entry.getValue().isSync())
                    stream.addSetObject(this, entry.getKey());
            }
            stream.send(address);
        }

        public void popFromDevice() {
            RCSPMultiStream stream = new RCSPMultiStream();
            for (Map.Entry<Integer, RCSProtocol.RCSPParameterGroup> entry : parameters.entrySet()) {
                if (!entry.getValue().isSync())
                    stream.addObjectRequest(entry.getKey());
            }
            stream.send(address);
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

        public boolean addSetObject(CausticDevice2 dev, int id) {
            RCSProtocol.ParametersContainer2 pars = dev.parameters;

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

        public boolean addFunctionCall2(RCSProtocol.FunctionsContainer2 functionsContainer, int id, String argument) {
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
        public void addSetObject(CausticDevice2 dev, int id) {
            if (!streams.get(streams.size()-1).addSetObject(dev, id)) {
                streams.add(new RCSPStream());
                streams.get(streams.size()-1).addSetObject(dev, id);
            }
        }

        public void addFunctionCall(CausticDeviceClass devClass, int id, String argument) {
            if (!streams.get(streams.size()-1).addFunctionCall(devClass, id, argument)) {
                streams.add(new RCSPStream());
                streams.get(streams.size()-1).addFunctionCall(devClass, id, argument);
            }
        }

        public void send(BridgeConnector.DeviceAddress addr) {
            for (RCSPStream stream : streams) {
                stream.send(addr);
            }
        }
    }

    void addPreferenceHeaderForDevice(CausticDevice dev) {
        CausticDeviceSettings.getDeviceHeaders().add(
                CausticDeviceSettings.buildHeader(
                        dev.getName(),
                        dev.getType(),
                        dev.address.toString()
                )
        );
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
            stream.addObjectRequest(RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.deviceType);
            devices.clear();
            CausticDeviceSettings.getDeviceHeaders().clear();
            stream.send(BridgeConnector.Broadcasts.any);
        }
    }

    class Receiver implements BridgeConnector.IncomingPackagesListener {
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

            dev.parameters.deserializeStream(data, offset, size);

            if (dev.hasName() && dev.isTypeKnown() && !dev.areDeviceRelatedParametersAdded()) {
                // Device is ready to operate with it: we know address, type and name
                dev.addDeviceRelatedParameters();
                addPreferenceHeaderForDevice(dev);
                dev.popFromDevice();
                devicesListUpdated.obtainMessage(DEVICES_LIST_UPDATED, 0, 0, devices).sendToTarget();
            }
        }
    }

    /////////////////////////
    // New version
    public Map<BridgeConnector.DeviceAddress, CausticDevice2> devices2 = new HashMap<BridgeConnector.DeviceAddress, CausticDevice2>();
    private Handler devicesListUpdated2 = null;

    public class CausticDevice2 {
        private boolean parametersAreAdded = false;
        public BridgeConnector.DeviceAddress address = new BridgeConnector.DeviceAddress();
        public RCSProtocol.ParametersContainer2 parameters = new RCSProtocol.ParametersContainer2();

        public CausticDevice2() {
            // Registering parameters for any devices
            RCSProtocol.Operations.AnyDevice.parametersDescriptions.addParameters(parameters);
        }

        public String getName() {
            return parameters.get(RCSProtocol.Operations.AnyDevice.Configuration.deviceName.getId()).getValue();
        }

        public boolean hasName() {
            return ( (RCSProtocol.DevNameParameterSerializer) parameters.get(RCSProtocol.Operations.AnyDevice.Configuration.deviceName.getId()) ).initialized();
        }

        public boolean isTypeKnown() {
            return (
                    Integer.parseInt(parameters.get(RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.deviceType).getValue())
                            != RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.DEV_TYPE_UNDEFINED
            );
        }

        public String getType() {
            return RCSProtocol.RCSPOperationCodes.AnyDevice.getDevTypeString(
                    Integer.parseInt(
                            parameters.get(RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.deviceType).getValue()
                    )
            );
        }

        public boolean areDeviceRelatedParametersAdded() {
            return parametersAreAdded;
        }

        public void addDeviceRelatedParameters() {
            if (parametersAreAdded)
                return;

            RCSProtocol.AnyParameterSerializer typeParam = parameters.get(RCSProtocol.RCSPOperationCodes.AnyDevice.Configuration.deviceType);
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

    public boolean updateDevicesList2(Handler devicesListUpdated) {
        this.devicesListUpdated2 = devicesListUpdated;
        currentTask = new TaskUpdateDevicesList2();
        return true;
    }

    class TaskUpdateDevicesList2 implements DeviceManagerTask {

        @Override
        public String taskText() {
            return "Updating devices list";
        }

        public TaskUpdateDevicesList2() {
            BridgeConnector.getInstance().setReceiver(new Receiver2());
            RCSPStream stream = new RCSPStream();
            stream.addObjectRequest(RCSProtocol.Operations.AnyDevice.Configuration.deviceName.getId());
            stream.addObjectRequest(RCSProtocol.Operations.AnyDevice.Configuration.deviceType.getId());
            devices2.clear();
            stream.send(BridgeConnector.Broadcasts.any);
        }
    }

    public void remoteCall2(BridgeConnector.DeviceAddress target, RCSProtocol.FunctionsContainer2 functionsContainer, int operationId, String argument)
    {
        RCSPStream stream = new RCSPStream();
        stream.addFunctionCall2(functionsContainer, operationId, argument);
        stream.send(target);
    }

    class Receiver2 implements BridgeConnector.IncomingPackagesListener {
        @Override
        public void getData(BridgeConnector.DeviceAddress address, byte[] data, int offset, int size)
        {
            //boolean newDeviceAdded = false;
            CausticDevice2 dev = devices2.get(address);
            if (dev == null) {
                dev = new CausticDevice2();
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
                devicesListUpdated2.obtainMessage(DEVICES_LIST_UPDATED, 0, 0, devices2).sendToTarget();
            }
        }
    }
}
