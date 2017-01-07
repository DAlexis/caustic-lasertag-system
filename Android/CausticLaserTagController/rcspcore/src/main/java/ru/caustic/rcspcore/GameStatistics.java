package ru.caustic.rcspcore;

import java.util.TreeMap;

/**
 * Created by dalexies on 13.10.16.
 */

public class GameStatistics {

    public static final int KILLS_COUNT = 0;
    public static final int DAMAGE      = 1;
    public static final int HITS_COUNT  = 2;
    public static final int OP_CODE_GET_PVP_RESULTS = 3001;

    public static class PvPStats {
        public long damage = 0;
        public int kills = 0;
        public int hits = 0;
    }
    public interface StatsChangeListener {
        void onStatsChange(int victimId, int damagerId);
    }

    public GameStatistics(DevicesManager devicesManager) {
        this.devicesManager = devicesManager;
        this.devicesManager.registerRCSPOperationDispatcher(OP_CODE_GET_PVP_RESULTS, new PvPDamageResultsMessageDispatcher());
    }

    public void setStatsChangeListener(StatsChangeListener statsChangeListener) {
        this.statsChangeListener = statsChangeListener;
    }

    public void updateStats()
    {
        /*
        // Updating devices list @todo Think about regular devices list updating...
        devicesManager.updateDevicesList(null);
        // Giving some time to update
        Thread.sleep(1000);
        // Loading only one parameter: player id
        devicesManager.asyncPopPlayerIdsForAllSupportingDevices(new DevicesManager.SynchronizationEndListener() {
            @Override
            public void onSynchronizationEnd(boolean isSuccess) {
                devsSyncEnd = true;
            }
        });

        // Waiting for player ids to be loaded
        while (!devsSyncEnd) {
            Thread.sleep(10);
            if (shouldStop)
                return;
        }
        */
        // Now we really can wait statistics
        sendRequest();
    }

    public void clearStatistics() {
        pvpStatsMap.clear();
    }

    public String getStatisticsTableStrStub(int statParType) {
        String result = "";
        for (TreeMap.Entry<Integer, TreeMap<Integer, PvPStats>> playerStats : pvpStatsMap.entrySet())
        {
            result += playerStats.getKey().toString() + ": ";
            for (TreeMap.Entry<Integer, PvPStats> victim : playerStats.getValue().entrySet())
            {
                result += "to " + Integer.toString(victim.getKey()) + " <- ";
                switch (statParType) {
                    case KILLS_COUNT: result += Integer.toString(victim.getValue().kills); break;
                    case DAMAGE: result += Long.toString(victim.getValue().damage); break;
                    case HITS_COUNT: result += Integer.toString(victim.getValue().hits); break;
                }
                result += " | ";
            }
            result += "\n";
        }
        return result;
    }

    // Private
    private class PvPDamageResultsMessageDispatcher implements DevicesManager.RCSPOperationDispatcher {
        @Override
        public void dispatchOperation(BridgeConnector.DeviceAddress address, RCSProtocol.RCSPOperation operation)
        {
            // To be sure
            if (operation.id != OP_CODE_GET_PVP_RESULTS)
                return;

            PvPRawResults pvpRawResults = new PvPRawResults();
            pvpRawResults.deserialize(operation.argument, 0);

            addPvPRawResult(pvpRawResults);
            statsChangeListener.onStatsChange(pvpRawResults.victimId, pvpRawResults.enemyId);
        }
    }
    /**
     * This is stats incoming from devices
     */
    private class PvPRawResults {
        public void deserialize(byte[] memory, int offset)
        {
            /*
             * Original struct:
             *
             * struct PvPRawResults
             * {
             * PlayerGameId victimId = 0;
             * PlayerGameId enemyId = 0;
             * uint16_t killsCount = 0; //< Players bullet killed player
             * uint16_t hitsCount = 0; //< hits except kills
             * uint32_t totalDamage = 0;
             * };
             **/
            victimId = MemoryUtils.byteToUnsignedByte(memory[offset]);
            offset++;
            enemyId = MemoryUtils.byteToUnsignedByte(memory[offset]);
            offset++;
            killsCount = MemoryUtils.bytesArrayToUint16(memory, offset);
            offset += 2;
            hitsCount = MemoryUtils.bytesArrayToUint16(memory, offset);
            offset += 2;
            totalDamage = MemoryUtils.bytesArrayToUint32(memory, offset);
        }

        int size()
        {
            return 9;
        }

        public int victimId = 0;
        public int enemyId = 0;
        public int killsCount = 0; //< Players bullet killed player
        public int hitsCount = 0; //< hits except kills
        public long totalDamage = 0;
    }

    private void sendRequest() {
        devicesManager.remoteCall(
                BridgeConnector.Broadcasts.headSensors,
                RCSProtocol.Operations.HeadSensor.functionsSerializers,
                RCSProtocol.Operations.HeadSensor.Functions.readStats.getId(),
                ""
        );
    }

    private void addPvPRawResult(PvPRawResults pvpRawResults) {
        updateLastDataReceivedTime();

        int victimId = pvpRawResults.victimId;
        int enemyId = pvpRawResults.enemyId;

        TreeMap<Integer, PvPStats> enemyStats = pvpStatsMap.get(enemyId);
        if (enemyStats == null) {
            enemyStats = new TreeMap<>();
            pvpStatsMap.put(enemyId, enemyStats);
        }

        PvPStats pvpStats = enemyStats.get(victimId);
        if (pvpStats == null) {
            pvpStats = new PvPStats();
            enemyStats.put(victimId, pvpStats);
        }

        pvpStats.damage = pvpRawResults.totalDamage;
        pvpStats.hits   = pvpRawResults.hitsCount;
        pvpStats.kills  = pvpRawResults.killsCount;
    }

    private boolean isWaitingTimeOver() {
        return System.currentTimeMillis() - lastDataReceivedTime > statsReceiveTimeout;
    }
    private void updateLastDataReceivedTime()
    {
        lastDataReceivedTime = System.currentTimeMillis();
    }

    private static final long statsReceiveTimeout = 5000;
    private static final String TAG = "CC.GameStatistics";
    private DevicesManager devicesManager;
    private long lastDataReceivedTime = 0;
    StatsChangeListener statsChangeListener = null;
    /**
     * Map: pvpStatsMap[Player whose stats][his enemy]
     */
    private TreeMap<Integer, TreeMap<Integer, PvPStats>> pvpStatsMap = new TreeMap<>();
}
