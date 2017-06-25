package com.caustic.rcspcore;

/**
 * The main entry point to rcspcore. Most of operations with Caustic LTS may be done over that class
 */

public class CausticController {
    // Public methods
    // @todo This class should be the one singleton and all other should be inside it
    public static CausticController getInstance() { return ourInstance; }
    public void systemInit(BridgeDriver.IBluetoothManager bluetoothManager) {
        RCSP.Operations.init();
        bridgeDriver.init(bluetoothManager);
        bridgeDriver.setReceiver(communicator);
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
                    RCSP.Operations.HeadSensor.functionsSerializers,
                    RCSP.Operations.HeadSensor.Functions.playerRespawn.getId(),
                    ""
            );
        }
        public void kill(int team) {
            broadcastCallForTeam(
                    team,
                    RCSP.Operations.HeadSensor.functionsSerializers,
                    RCSP.Operations.HeadSensor.Functions.playerKill.getId(),
                    ""
            );
        }

        public void resetPlayers(int team) {
            broadcastCallForTeam(
                    team,
                    RCSP.Operations.HeadSensor.functionsSerializers,
                    RCSP.Operations.HeadSensor.Functions.playerReset.getId(),
                    ""
            );
        }

        public void resetStats(int team) {
            broadcastCallForTeam(
                    team,
                    RCSP.Operations.HeadSensor.functionsSerializers,
                    RCSP.Operations.HeadSensor.Functions.resetStats.getId(),
                    ""
            );
        }

        public void newRound() {
            resetPlayers(ALL_TEAMS);
            respawn(ALL_TEAMS);
            resetStats(ALL_TEAMS);

        }


        // Private
        private void broadcastCallForTeam(int team, RCSP.FunctionsContainer functionsContainer, int operationId, String argument) {
            if (team == ALL_TEAMS) {
                devicesManager.remoteCall(
                        BridgeDriver.Broadcasts.headSensors,
                        functionsContainer,
                        operationId,
                        argument
                );
                return;
            }
            if ((team & RED) != 0) {
                devicesManager.remoteCall(
                        BridgeDriver.Broadcasts.headSensorsRed,
                        functionsContainer,
                        operationId,
                        argument
                );
            }
            if ((team & BLUE) != 0) {
                devicesManager.remoteCall(
                        BridgeDriver.Broadcasts.headSensorsBlue,
                        functionsContainer,
                        operationId,
                        argument
                );
            }
            if ((team & YELLOW) != 0) {
                devicesManager.remoteCall(
                        BridgeDriver.Broadcasts.headSensorsYellow,
                        functionsContainer,
                        operationId,
                        argument
                );
            }
            if ((team & GREEN) != 0) {
                devicesManager.remoteCall(
                        BridgeDriver.Broadcasts.headSensorsGreen,
                        functionsContainer,
                        operationId,
                        argument
                );
            }
        }
    }

    private static CausticController ourInstance = new CausticController();
    private BridgeDriver bridgeDriver = new BridgeDriver();
    private LTSCommunicator communicator = new LTSCommunicator(bridgeDriver);
    private DevicesManager devicesManager = new DevicesManager(bridgeDriver, communicator);
    private SettingsEditorContext settingsEditorContext = new SettingsEditorContext(devicesManager);

    private GameStatistics gameStatistics = new GameStatistics(devicesManager, communicator);
    private BroadcastCalls broadcastCalls = new BroadcastCalls();
}
