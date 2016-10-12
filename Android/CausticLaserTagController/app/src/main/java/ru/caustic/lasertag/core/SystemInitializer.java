package ru.caustic.lasertag.core;

/**
 * Created by dalexies on 15.09.16.
 */
public class SystemInitializer {
    public static void systemInit(BridgeConnector.IBluetoothManager bluetoothManager) {
        RCSProtocol.Operations.init();
        BridgeConnector.getInstance().init(bluetoothManager);
    }
}
