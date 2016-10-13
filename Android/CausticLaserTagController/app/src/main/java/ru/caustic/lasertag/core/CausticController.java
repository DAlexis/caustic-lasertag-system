package ru.caustic.lasertag.core;

/**
 * Created by dalexies on 13.10.16.
 */

public class CausticController {
    // Public methods
    // @todo This class should be the one singleton and all other should be inside it
    public static CausticController getInstance() { return ourInstance; }

    public static void systemInit(BridgeConnector.IBluetoothManager bluetoothManager) {
        RCSProtocol.Operations.init();
        BridgeConnector.getInstance().init(bluetoothManager);
    }

    // Private
    private static CausticController ourInstance = new CausticController();
}
