package com.caustic.rcspcore;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by vashamporov on 1/12/17.
 */
public class DeviceUtils {

    public static Map<RCSP.DeviceAddress, CausticDevice> getHeadSensorsMap(Map<RCSP.DeviceAddress, CausticDevice> devices) {
        Map<RCSP.DeviceAddress, CausticDevice> returnMap = new HashMap<>();
        for (Map.Entry<RCSP.DeviceAddress, CausticDevice> entry : devices.entrySet()) {

            CausticDevice dev = entry.getValue();
            int devTypeInt = Integer.parseInt(
                    dev.parameters.get(RCSP.Operations.AnyDevice.Configuration.deviceType.getId()).getValue()
            );

            //Add only head sensors
            if (devTypeInt == RCSP.Operations.AnyDevice.Configuration.DEV_TYPE_HEAD_SENSOR) {
                returnMap.put(entry.getKey(), entry.getValue());
            }
        }
        return returnMap;
    }

    public static boolean allDevsSynced (Map<RCSP.DeviceAddress, CausticDevice> devices) {
        boolean result = true;
        for (Map.Entry<RCSP.DeviceAddress, CausticDevice> entry : devices.entrySet()) {
            CausticDevice dev = entry.getValue();
            result = result && dev.areDeviceRelatedParametersAdded() && dev.areParametersSync();
        }
        return result;
    }
    public static String getDeviceTeam(CausticDevice device) {
        RCSP.teamEnum teamEnum = (RCSP.teamEnum) device.parameters.get(RCSP.Operations.HeadSensor.Configuration.teamMT2Id.getId()).getDescription();
        return teamEnum.getCurrentValueString(Integer.parseInt(device.parameters.get(RCSP.Operations.HeadSensor.Configuration.teamMT2Id.getId()).getValue()));
    }
    public static String getDeviceName(CausticDevice device) {
        return device.parameters.get(RCSP.Operations.AnyDevice.Configuration.deviceName.getId()).getValue();
    }
    public static int getMarkerColor(CausticDevice device) {
        int result = Integer.parseInt(device.parameters.get(RCSP.Operations.HeadSensor.Configuration.markerColor.getId()).getValue());
        return result;
    }
    public static int getCurrentHealth(CausticDevice device) {
        int result = Integer.parseInt(device.parameters.get(RCSP.Operations.HeadSensor.Configuration.currentHealth.getId()).getValue());
        return result;
    }
    public static boolean onSameTeam(int id1, int id2) {
        String team1="-1";
        String team2="-2";
        Map<RCSP.DeviceAddress, CausticDevice> headSensors = getHeadSensorsMap(CausticController.getInstance().getDevicesManager().devices);
        for (Map.Entry<RCSP.DeviceAddress, CausticDevice> entry : headSensors.entrySet())
        {
            CausticDevice device = entry.getValue();
            if (device.areDeviceRelatedParametersAdded())
            {
                int id = Integer.parseInt(device.parameters.get(RCSP.Operations.HeadSensor.Configuration.playerGameId.getId()).getValue());
                if (id == id1)
                {
                    team1 = device.parameters.get(RCSP.Operations.HeadSensor.Configuration.teamMT2Id.getId()).getValue();
                }
                if (id == id2)
                {
                    team2 = device.parameters.get(RCSP.Operations.HeadSensor.Configuration.teamMT2Id.getId()).getValue();
                }
            }

        }
        if (team1.equals(team2))
        {
            return true;
        }
        return false;
    }
}
