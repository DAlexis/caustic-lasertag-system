package org.ltcaustic.rcspcore;

import java.util.Map;

/**
 * Created by dalexies on 14.05.17.
 */

public class CausticDevice {
    public CausticDevice(LTSCommunicator communicator) {
        this.communicator = communicator;
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
        RCSPStream stream = new RCSPStream(communicator);
        for (Map.Entry<Integer, RCSP.AnyParameterSerializer> entry : parameters.entrySet()) {
            if (!entry.getValue().isSync())
                stream.addSetObject(this, entry.getKey());
        }
        stream.send(address);
    }
    public void pushToDevice(int parameterToPush) {
        RCSPStream stream = new RCSPStream(communicator);
        stream.addSetObject(this, parameterToPush);
        stream.send(address);
    }
    public void popFromDevice() {
        RCSPStream stream = new RCSPStream(communicator);
        for (Map.Entry<Integer, RCSP.AnyParameterSerializer> entry : parameters.entrySet()) {
            if (!entry.getValue().isSync())
                stream.addObjectRequest(entry.getKey());
        }
        stream.send(address);
    }
    public void popFromDevice(int parameterToPop) {
        // @todo Optimize: do not request parameters than not exists
        RCSPStream stream = new RCSPStream(communicator);
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

    public RCSP.DeviceAddress address = new RCSP.DeviceAddress();
    public RCSP.ParametersContainer parameters = new RCSP.ParametersContainer();

    private final long deviceLifetime = 10000;
    private boolean parametersAreAdded = false;
    private long lastAccessTime = 0;
    private LTSCommunicator communicator;
}
