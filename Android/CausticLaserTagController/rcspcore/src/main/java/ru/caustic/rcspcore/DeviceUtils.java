package ru.caustic.rcspcore;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by vashamporov on 1/12/17.
 */

public class DeviceUtils {

    public static Map<RCSP.DeviceAddress, DevicesManager.CausticDevice> getHeadSensorsMap(Map<RCSP.DeviceAddress, DevicesManager.CausticDevice> devices) {
        Map<RCSP.DeviceAddress, DevicesManager.CausticDevice> returnMap = new HashMap<>();
        for (Map.Entry<RCSP.DeviceAddress, DevicesManager.CausticDevice> entry : devices.entrySet()) {

            DevicesManager.CausticDevice dev = entry.getValue();
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

    public static boolean allDevsSynced (Map<RCSP.DeviceAddress, DevicesManager.CausticDevice> devices)
    {
        boolean result = true;
        for (Map.Entry<RCSP.DeviceAddress, DevicesManager.CausticDevice> entry : devices.entrySet()) {
            DevicesManager.CausticDevice dev = entry.getValue();
            result = result && dev.areDeviceRelatedParametersAdded() && dev.areParametersSync();
        }
        return result;
    }

    public static String getDeviceTeam(DevicesManager.CausticDevice device) {
        RCSP.teamEnum teamEnum = (RCSP.teamEnum) device.parameters.get(RCSP.Operations.HeadSensor.Configuration.teamMT2Id.getId()).getDescription();
        return teamEnum.getCurrentValueString(Integer.parseInt(device.parameters.get(RCSP.Operations.HeadSensor.Configuration.teamMT2Id.getId()).getValue()));
    }

    public static String getDeviceName(DevicesManager.CausticDevice device) {
        return device.parameters.get(RCSP.Operations.AnyDevice.Configuration.deviceName.getId()).getValue();
    }

    public static int getMarkerColor(DevicesManager.CausticDevice device) {
        int result = Integer.parseInt(device.parameters.get(RCSP.Operations.HeadSensor.Configuration.markerColor.getId()).getValue());
        return result;
    }

    public static int getCurrentHealth(DevicesManager.CausticDevice device) {
        int result = Integer.parseInt(device.parameters.get(RCSP.Operations.HeadSensor.Configuration.currentHealth.getId()).getValue());
        return result;
    }


    public static void pushLocalSettingsToAssociatedHeadSensor(Context context) {
        DevicesManager devMan = CausticController.getInstance().getDevicesManager();
        DevicesManager.CausticDevice headSensor = devMan.devices.get(devMan.associatedHeadSensorAddress);
        if (headSensor!=null) {
            if (headSensor.areDeviceRelatedParametersAdded()) {
                SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(context);

                //Setting displayed name
                //@todo Make separate fields for device name and player name
                String playerName = sharedPref.getString("display_name_key", "Default Player2");
                headSensor.parameters.get(RCSP.Operations.AnyDevice.Configuration.deviceName.getId())
                        .setValue(playerName);
                headSensor.pushToDevice(RCSP.Operations.AnyDevice.Configuration.deviceName.getId());

                //Setting on-map marker color
                String markerColor = Integer.toString(sharedPref.getInt("marker_color_key", -1));
                headSensor.parameters.get(RCSP.Operations.HeadSensor.Configuration.markerColor.getId())
                        .setValue(markerColor);
                headSensor.pushToDevice(RCSP.Operations.HeadSensor.Configuration.markerColor.getId());
            }
        }
    }



    public static boolean onSameTeam(int id1, int id2)
    {
        String team1="-1";
        String team2="-2";
        Map<RCSP.DeviceAddress, DevicesManager.CausticDevice> headSensors = getHeadSensorsMap(CausticController.getInstance().getDevicesManager().devices);
        for (Map.Entry<RCSP.DeviceAddress, DevicesManager.CausticDevice> entry : headSensors.entrySet())
        {
            DevicesManager.CausticDevice device = entry.getValue();
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
