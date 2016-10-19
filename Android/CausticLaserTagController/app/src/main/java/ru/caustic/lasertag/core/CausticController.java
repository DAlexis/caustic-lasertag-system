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
    public SettingsEditorContext getSettingsEditorContext() { return settingsEditorContext; }
    public DevicesManager getDevicesManager() { return devicesManager; }
    public BroadcastCalls getBroadcastCalls() { return broadcastCalls; }
    public GameStatistics getGameStatistics() { return gameStatistics; }

    public class BroadcastCalls {
        public static final int RED    = 1;
        public static final int BLUE   = 2;
        public static final int YELLOW = 4;
        public static final int GREEN  = 8;
        public static final int ALL_TEAMS  = RED | BLUE | YELLOW | GREEN;

        /**
         * Respawn players
         * @param team May be RED, BLUE, ..., ALL_TEAMS
         */
        public void respawn(int team) {
            broadcastCallForTeam(
                    team,
                    RCSProtocol.Operations.HeadSensor.functionsSerializers,
                    RCSProtocol.Operations.HeadSensor.Functions.playerRespawn.getId(),
                    ""
            );
        }
        public void kill(int team) {
            broadcastCallForTeam(
                    team,
                    RCSProtocol.Operations.HeadSensor.functionsSerializers,
                    RCSProtocol.Operations.HeadSensor.Functions.playerKill.getId(),
                    ""
            );
        }

        // Private
        private void broadcastCallForTeam(int team, RCSProtocol.FunctionsContainer functionsContainer, int operationId, String argument) {
            if (team == ALL_TEAMS) {
                devicesManager.remoteCall(
                        BridgeConnector.Broadcasts.headSensors,
                        functionsContainer,
                        operationId,
                        argument
                );
                return;
            }
            if ((team & RED) != 0) {
                devicesManager.remoteCall(
                        BridgeConnector.Broadcasts.headSensorsRed,
                        functionsContainer,
                        operationId,
                        argument
                );
            }
            if ((team & BLUE) != 0) {
                devicesManager.remoteCall(
                        BridgeConnector.Broadcasts.headSensorsBlue,
                        functionsContainer,
                        operationId,
                        argument
                );
            }
            if ((team & YELLOW) != 0) {
                devicesManager.remoteCall(
                        BridgeConnector.Broadcasts.headSensorsYellow,
                        functionsContainer,
                        operationId,
                        argument
                );
            }
            if ((team & GREEN) != 0) {
                devicesManager.remoteCall(
                        BridgeConnector.Broadcasts.headSensorsGreen,
                        functionsContainer,
                        operationId,
                        argument
                );
            }
        }
    }

    private static CausticController ourInstance = new CausticController();
    private BridgeConnector bridgeConnector = new BridgeConnector();
    private DevicesManager devicesManager = new DevicesManager(bridgeConnector);
    private SettingsEditorContext settingsEditorContext = new SettingsEditorContext(devicesManager);


    private GameStatistics gameStatistics = new GameStatistics(devicesManager);
    private BroadcastCalls broadcastCalls = new BroadcastCalls();
}
