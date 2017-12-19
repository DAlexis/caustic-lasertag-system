package org.ltcaustic.rcspcore;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by dalexies on 19.12.17.
 */

public class DevicesListUpdater {

    private static long period = 2000;

    DevicesManager devicesManager = null;

    DevicesListUpdater(DevicesManager mgr) {
        devicesManager = mgr;
        runner = new Utils.PeriodicRunner(5000, new Runnable() {
            @Override
            public void run() {
                devicesManager.updateDevicesList();
                for (DevicesManager.DeviceListUpdatedListener listener : deviceListUpdatedListeners)
                    listener.onDevicesListUpdated();
            }
        });
    }

    public void subscribe(DevicesManager.DeviceListUpdatedListener listener) {
        this.deviceListUpdatedListeners.add(listener);
    }

    public Utils.PeriodicRunner runner;
    private List<DevicesManager.DeviceListUpdatedListener> deviceListUpdatedListeners = new ArrayList<>();
}
