package ru.caustic.rcspcore;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

import java.util.HashMap;
import java.util.Map;

import ru.caustic.rcspcore.BridgeConnector;
import ru.caustic.rcspcore.CausticController;
import ru.caustic.rcspcore.DevicesManager;
import ru.caustic.rcspcore.RCSProtocol;

/**
 * Created by vashamporov on 1/12/17.
 */

public class DeviceUtils {

    public static Map<BridgeConnector.DeviceAddress, DevicesManager.CausticDevice> getHeadSensorsMap(Map<BridgeConnector.DeviceAddress, DevicesManager.CausticDevice> devices) {
        Map<BridgeConnector.DeviceAddress, DevicesManager.CausticDevice> returnMap = new HashMap<>();
        for (Map.Entry<BridgeConnector.DeviceAddress, DevicesManager.CausticDevice> entry : devices.entrySet()) {

            DevicesManager.CausticDevice dev = entry.getValue();
            int devTypeInt = Integer.parseInt(
                    dev.parameters.get(RCSProtocol.Operations.AnyDevice.Configuration.deviceType.getId()).getValue()
            );

            //Add only head sensors
            if (devTypeInt == RCSProtocol.Operations.AnyDevice.Configuration.DEV_TYPE_HEAD_SENSOR) {
                returnMap.put(entry.getKey(), entry.getValue());
            }
        }
        return returnMap;
    }

    public static boolean allDevsSynced (Map<BridgeConnector.DeviceAddress, DevicesManager.CausticDevice> devices)
    {
        boolean result = true;
        for (Map.Entry<BridgeConnector.DeviceAddress, DevicesManager.CausticDevice> entry : devices.entrySet()) {
            DevicesManager.CausticDevice dev = entry.getValue();
            result = result && dev.areDeviceRelatedParametersAdded() && dev.areParametersSync();
        }
        return result;
    }

    public static String getDeviceTeam(DevicesManager.CausticDevice device) {
        RCSProtocol.teamEnum teamEnum = (RCSProtocol.teamEnum) device.parameters.get(RCSProtocol.Operations.HeadSensor.Configuration.teamMT2Id.getId()).getDescription();
        return teamEnum.getCurrentValueString(Integer.parseInt(device.parameters.get(RCSProtocol.Operations.HeadSensor.Configuration.teamMT2Id.getId()).getValue()));
    }

    public static String getDeviceName(DevicesManager.CausticDevice device) {
        return device.parameters.get(RCSProtocol.Operations.AnyDevice.Configuration.deviceName.getId()).getValue();
    }

    public static int getMarkerColor(DevicesManager.CausticDevice device) {
        int result = Integer.parseInt(device.parameters.get(RCSProtocol.Operations.HeadSensor.Configuration.markerColor.getId()).getValue());
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
                headSensor.parameters.get(RCSProtocol.Operations.AnyDevice.Configuration.deviceName.getId())
                        .setValue(playerName);
                headSensor.pushToDevice(RCSProtocol.Operations.AnyDevice.Configuration.deviceName.getId());

                //Setting on-map marker color
                String markerColor = Integer.toString(sharedPref.getInt("marker_color_key", -1));
                headSensor.parameters.get(RCSProtocol.Operations.HeadSensor.Configuration.markerColor.getId())
                        .setValue(markerColor);
                headSensor.pushToDevice(RCSProtocol.Operations.HeadSensor.Configuration.markerColor.getId());
            }
        }
    }

}
