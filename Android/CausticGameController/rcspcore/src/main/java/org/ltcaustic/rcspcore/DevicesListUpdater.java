package org.ltcaustic.rcspcore;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by dalexies on 19.12.17.
 */

public class DevicesListUpdater {

    private static long period = 3000;

    DevicesManager devicesManager = null;

    DevicesListUpdater(DevicesManager mgr) {
        devicesManager = mgr;
        runner = new Utils.PeriodicRunner(period, new Runnable() {
            @Override
            public void run() {
                devicesManager.updateDevicesList();
            }
        });
    }

    public Utils.PeriodicRunner runner;
}
