package ru.caustic.lasertag.core;

import android.util.Log;

import java.util.Map;
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
        public int damage = 0;
        public int kills = 0;
        public int hits = 0;
    }
    public interface StatsSyncDoneListener {
        void onStatsSyncDone();
    }

    public GameStatistics(DevicesManager devicesManager) {
        this.devicesManager = devicesManager;
        this.devicesManager.registerRCSPOperationDispatcher(OP_CODE_GET_PVP_RESULTS, new PvPDamageResultsMessageDispatcher());
    }
    public void readStatsAsync(StatsSyncDoneListener statsSyncDoneListener) {
        if (statisticsWaiterThread != null && statisticsWaiterThread.isAlive())
        {
            Log.d(TAG, "Stop waiting signal to thread...");
            statisticsWaiterThread.stopWaiting();
            try {
                statisticsWaiterThread.join();
            } catch (Exception e) {
                Log.e(TAG, "Exception during statisticsWaiterThread.join();", e);
            }
        }
        statisticsWaiterThread = new StatisticsWaiterThread(statsSyncDoneListener);
        statisticsWaiterThread.start();
    }
    public String getStatisticsTableStrStub(int statParType) {
        buildStatisticsTable();
        String result = "";
        for (TreeMap.Entry<Integer, TreeMap<Integer, PvPStats>> playerStats : pvpStatsMap.entrySet())
        {
            result = playerStats.getKey().toString() + ": ";
            for (TreeMap.Entry<Integer, PvPStats> victim : playerStats.getValue().entrySet())
            {
                result += "to " + Integer.toString(victim.getKey()) + " <- ";
                switch (statParType) {
                    case KILLS_COUNT: result += Integer.toString(victim.getValue().kills); break;
                    case DAMAGE: result += Integer.toString(victim.getValue().damage); break;
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

            PvPDamageResults pvpDamageResults = new PvPDamageResults();
            pvpDamageResults.deserialize(operation.argument, 0);
            pvpDamageResults.lookupPlayerId(address);

            addPvPRawResult(pvpDamageResults);
        }
    }
    /**
     * This is stats incoming from devices
     */
    private class PvPDamageResults {
        public void lookupPlayerId(BridgeConnector.DeviceAddress devAddr)
        {
            playerId = devicesManager.getPlayerGameId(devAddr);
        }

        public void deserialize(byte[] memory, int offset)
        {
            /*
             * Original struct:
             *
             * struct PvPDamageResults
             * {
             * PlayerGameId enemyId = 0;
             * uint16_t killsCount = 0; //< Players bullet killed player
             * uint16_t hitsCount = 0; //< hits except kills
             * uint32_t totalDamage = 0;
             * };
             **/
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

        public int playerId = 0;
        public int enemyId = 0;
        public int killsCount = 0; //< Players bullet killed player
        public int hitsCount = 0; //< hits except kills
        public long totalDamage = 0;
    }

    private class StatisticsWaiterThread extends Thread {
        StatsSyncDoneListener statsSyncDoneListener;
        boolean shouldStop = false;
        boolean devsSyncEnd = false;

        public StatisticsWaiterThread(StatsSyncDoneListener statsSyncDoneListener) {
            super();
            this.statsSyncDoneListener = statsSyncDoneListener;
        }
        @Override
        public void run() {
            clearStatistics();
            try {
                // Updating devices list @todo Think about regular devices list updating...
                devicesManager.updateDevicesList(null);
                // Giving some time to update
                Thread.sleep(1000);
                // Loading only one parameter: player id
                devicesManager.asyncPopPlayerIdsForAllSupportingDevices(new DevicesManager.SynchronizationEndHandler() {
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

                // Now we really can wait statistics
                sendRequest();
                updateLastDataReceivedTime();
                while (!isWaitingTimeOver()) {
                    Thread.sleep(10);
                    if (shouldStop)
                        return;
                }
            } catch (Exception e) {
                Log.e(TAG, "Exception during statistics waiting", e);
            }
            statsSyncDoneListener.onStatsSyncDone();
        }

        public void stopWaiting() {
            shouldStop = true;
        }

    }
    private void sendRequest() {
        devicesManager.remoteCall(
                BridgeConnector.Broadcasts.headSensors,
                RCSProtocol.Operations.HeadSensor.functionsSerializers,
                RCSProtocol.Operations.HeadSensor.Functions.readStats.getId(),
                ""
        );
    }
    private void buildStatisticsTable() {
        pvpStatsMap.clear();
        for (Map.Entry<Integer, PvPDamageResults> entry : pvpResultsMap.entrySet())
        {
            int victimId = entry.getKey();
            int enemyId = entry.getValue().enemyId;
            PvPDamageResults pvpRes = entry.getValue();

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

            pvpStats.damage += pvpRes.totalDamage;
            pvpStats.hits   += pvpRes.hitsCount;
            pvpStats.kills  += pvpRes.killsCount;
        }
    }

    private void addPvPRawResult(PvPDamageResults pvpDamageResults) {
        pvpResultsMap.put(pvpDamageResults.playerId, pvpDamageResults);
        updateLastDataReceivedTime();
    }
    private void clearStatistics() {
        pvpStatsMap.clear();
        pvpResultsMap.clear();
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
    StatisticsWaiterThread statisticsWaiterThread;
//    StatsSyncDoneListener statsSyncDoneListener = null;
    /**
     * Map: pvpStatsMap[Player whose stats][his enemy]
     */
    private TreeMap<Integer, TreeMap<Integer, PvPStats>> pvpStatsMap = new TreeMap<>();
    /**
     * Map: victim id => PvPDamageResult (raw data from devices)
     */
    private Map<Integer, PvPDamageResults> pvpResultsMap = new TreeMap<>();

}
