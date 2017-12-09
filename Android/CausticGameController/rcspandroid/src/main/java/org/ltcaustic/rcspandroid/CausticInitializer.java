package org.ltcaustic.rcspandroid;

import org.ltcaustic.rcspcore.CausticController;

/**
 * Access point for CausticController for android applications.
 *
 * Initializes CausticController
 */

public class CausticInitializer {

    public static CausticInitializer getInstance() { return instance; }
    static CausticInitializer instance = new CausticInitializer();

    CausticController controller = null;
    BluetoothManager bluetoothManager = null;

    public CausticController controller() {
        if (controller == null)
            createController();
        return controller;
    }

    public BluetoothManager bluetooth() {
        if (bluetoothManager == null)
            bluetoothManager = new BluetoothManager();
        return bluetoothManager;
    }

    private void createController() {
        controller = new CausticController(bluetooth());
    }
}
