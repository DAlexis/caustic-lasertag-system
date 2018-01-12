package org.ltcaustic.rcspcore;

import java.util.Set;

/**
 * The main entry point to rcspcore. Most of operations with Caustic LTS may be done over that class
 */

public class CausticController {
    // Public methods
    public CausticController(BridgeDriver.IBluetoothManager bluetoothManager) {
        RCSP.Operations.init();
        bridgeDriver.init(bluetoothManager);
        bridgeDriver.setReceiver(communicator);
    }
    public SettingsEditorContext getSettingsEditorContext() { return settingsEditorContext; }
    public DevicesManager getDevicesManager() { return devicesManager; }

    public DevicesListUpdater getDevicesListUpdater() {
        if (devicesListUpdater == null)
            devicesListUpdater = new DevicesListUpdater(devicesManager);
        return devicesListUpdater;
    }
    public BroadcastCalls getBroadcastCalls() { return broadcastCalls; }
    public Calls getCalls() { return calls; }
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

    public class Calls {
        public void respawn(Set<RCSP.DeviceAddress> addrs) {
            playerMassCall(addrs, RCSP.Operations.HeadSensor.Functions.playerRespawn.getId());
        }

        public void kill(Set<RCSP.DeviceAddress> addrs) {
            playerMassCall(addrs, RCSP.Operations.HeadSensor.Functions.playerKill.getId());
        }

        public void reset(Set<RCSP.DeviceAddress> addrs) {
            playerMassCall(addrs, RCSP.Operations.HeadSensor.Functions.playerReset.getId());
        }

        public void fullAmmo(Set<RCSP.DeviceAddress> addrs) {
            //playerMassCall(addrs, RCSP.Operations.HeadSensor.Functions.playerRespawn.getId());
        }

        private void playerCall(RCSP.DeviceAddress addr, int id) {
            devicesManager.remoteCall(
                    addr,
                    RCSP.Operations.HeadSensor.functionsSerializers,
                    id,
                    ""
            );
        }

        private void playerMassCall(Set<RCSP.DeviceAddress> addrs, int id) {
            for (RCSP.DeviceAddress addr : addrs) {
                playerCall(addr, id);
            }
        }

        private void rifleCall(RCSP.DeviceAddress addr, int id) {
            devicesManager.remoteCall(
                    addr,
                    RCSP.Operations.HeadSensor.functionsSerializers,
                    id,
                    ""
            );
        }

        private void rifleMassCall(Set<RCSP.DeviceAddress> addrs, int id) {
            for (RCSP.DeviceAddress addr : addrs) {
                rifleCall(addr, id);
            }
        }
    }

    private BridgeDriver bridgeDriver = new BridgeDriver();
    private LTSCommunicator communicator = new LTSCommunicator(bridgeDriver);
    private DevicesManager devicesManager = new DevicesManager(bridgeDriver, communicator);
    private SettingsEditorContext settingsEditorContext = new SettingsEditorContext(devicesManager);
    private DevicesListUpdater devicesListUpdater = null;

    private GameStatistics gameStatistics = new GameStatistics(devicesManager, communicator);
    private BroadcastCalls broadcastCalls = new BroadcastCalls();
    private Calls calls = new Calls();
}
