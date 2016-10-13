package ru.caustic.lasertag.core;

import ru.caustic.lasertag.ui.BluetoothManager;

/**
 * Created by dalexies on 13.10.16.
 */

public class CausticController {
    // Public methods
    // @todo This class should be the one singleton and all other should be inside it
    public static CausticController getInstance() { return ourInstance; }

    public void systemInit() {
        RCSProtocol.Operations.init();
        bridgeConnector.init(BluetoothManager.getInstance());
    }

    public SettingsEditorContext getSettingsEditorContext() {
        return settingsEditorContext;
    }

    public CausticDevicesManager getCausticDevicesManager() {
        return causticDevicesManager;
    }

    // Private
    private static CausticController ourInstance = new CausticController();
    private BridgeConnector bridgeConnector = new BridgeConnector();
    private CausticDevicesManager causticDevicesManager = new CausticDevicesManager(bridgeConnector);
    private SettingsEditorContext settingsEditorContext = new SettingsEditorContext(causticDevicesManager);
}
