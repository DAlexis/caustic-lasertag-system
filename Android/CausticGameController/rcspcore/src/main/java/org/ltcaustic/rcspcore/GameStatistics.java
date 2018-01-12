package org.ltcaustic.rcspcore;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

/**
 * Created by dalexies on 13.10.16.
 */

public class GameStatistics {

    public static final int ENEMY_KILLS_COUNT = 0;
    public static final int DAMAGE      = 1;
    public static final int HITS_COUNT  = 2;
    public static final int FRIENDLY_KILLS_COUNT = 3;
    public static final int OP_CODE_GET_PVP_RESULTS = 3001;


    public static class PvPStats {
        public long damage = 0;
        public int kills = 0;
        public int hits = 0;
    }
    public interface StatsChangeListener {
        void onStatsChange(int victimId, int damagerId);
    }

    public interface StatsUpdatedSubscriber {
        void onStatsUpdated();
    }

    public GameStatistics(DevicesManager devicesManager, RCSP.OperationDispatcherUser dispatchersUser) {
        this.devicesManager = devicesManager;
        dispatchersUser.addOperationDispatcher(OP_CODE_GET_PVP_RESULTS, new PvPDamageResultsMessageDispatcher());
        statsUpdatedWaiter = new Utils.SimpleWaiter(300, 100, new Runnable() {
            @Override
            public void run() {
                for (StatsUpdatedSubscriber s: updatedSubscribers)
                    s.onStatsUpdated();
            }
        });
        statsUpdatedWaiter.enableCallback(true);
    }

    public void runPeriodic(long period) {
        if (updateRunner != null)
            return;

        updateRunner = new Utils.PeriodicRunner(period, new Runnable() {
            @Override
            public void run() {
                updateStats();
            }
        });
    }

    /*
    public void setStatsChangeListener(StatsChangeListener statsChangeListener) {
        this.statsChangeListener = statsChangeListener;
    }*/

    public void subscribeStatsUpdated(StatsUpdatedSubscriber subscriber) {
        updatedSubscribers.add(subscriber);
    }

    public void clear() {
        pvpStatsMap.clear();
    }

    public void updateStats() {
        /*
        // Updating devices list @todo Think about regular devices list updating...
        devicesManager.updateDevicesList(null);
        // Giving some time to update
        Thread.sleep(1000);
        // Loading only one parameter: player id
        devicesManager.asyncPullPlayerIdsForAllSupportingDevices(new DevicesManager.SynchronizationEndListener() {
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
        for (Map.Entry<Integer, TreeMap<Integer, PvPStats>> playerStats : pvpStatsMap.entrySet())
        {
            result += playerStats.getKey().toString() + ": ";
            for (Map.Entry<Integer, PvPStats> victim : playerStats.getValue().entrySet())
            {
                result += "to " + Integer.toString(victim.getKey()) + " <- ";
                switch (statParType) {
                    case ENEMY_KILLS_COUNT: result += Integer.toString(victim.getValue().kills); break;
                    case DAMAGE: result += Long.toString(victim.getValue().damage); break;
                    case HITS_COUNT: result += Integer.toString(victim.getValue().hits); break;
                }
                result += " | ";
            }
            result += "\n";
        }
        return result;
    }
    public int getStatForPlayerID(int playerID, int statParType) {
        int result = 0;
        TreeMap<Integer, PvPStats> ownRegister = pvpStatsMap.get(playerID);
        if (ownRegister!=null)
        {
            for (Map.Entry<Integer, PvPStats> victim : ownRegister.entrySet())
            {
                    switch (statParType) {
                        case ENEMY_KILLS_COUNT:
                            if (!DeviceUtils.onSameTeam(playerID, victim.getKey())) {
                                result += victim.getValue().kills;
                            }
                            break;
                        case DAMAGE:
                            if (!DeviceUtils.onSameTeam(playerID, victim.getKey())) {
                                result += victim.getValue().damage;
                            }
                            break;
                        case HITS_COUNT:
                            if (!DeviceUtils.onSameTeam(playerID, victim.getKey())) {
                                result += victim.getValue().hits;
                            }
                            break;
                        case FRIENDLY_KILLS_COUNT:
                            if (DeviceUtils.onSameTeam(playerID, victim.getKey())) {
                                result += victim.getValue().kills;
                            }
                            break;
                    }
            }
        }
        return result;
    }
    public Set<Integer> getPlayerIds() {
        return pvpStatsMap.keySet();
    }

    // Private
    private class PvPDamageResultsMessageDispatcher implements RCSP.OperationDispatcher {
        @Override
        public boolean dispatchOperation(RCSP.DeviceAddress address, RCSP.RCSPOperation operation)
        {
            // To be sure
            if (operation.id != OP_CODE_GET_PVP_RESULTS)
                return false;

            PvPRawResults pvpRawResults = new PvPRawResults();
            pvpRawResults.deserialize(operation.argument, 0);

            addPvPRawResult(pvpRawResults);
            if (statsChangeListener != null) {
                statsChangeListener.onStatsChange(pvpRawResults.victimId, pvpRawResults.enemyId);
            }
            return true;
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
                BridgeDriver.Broadcasts.headSensors,
                RCSP.Operations.HeadSensor.functionsSerializers,
                RCSP.Operations.HeadSensor.Functions.readStats.getId(),
                ""
        );
    }
    private void addPvPRawResult(PvPRawResults pvpRawResults) {
        statsUpdatedWaiter.touch();

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

    private static final long statsReceiveTimeout = 5000;
    private static final String TAG = "CC.GameStatistics";
    private DevicesManager devicesManager;
    private long lastDataReceivedTime = 0;
    StatsChangeListener statsChangeListener = null;
    /**
     * Map: pvpStatsMap[Player whose stats][his enemy]
     */
    private Map<Integer, TreeMap<Integer, PvPStats>> pvpStatsMap = new TreeMap<>();
    private List<StatsUpdatedSubscriber> updatedSubscribers = new ArrayList<>();
    Utils.SimpleWaiter statsUpdatedWaiter;
    Utils.PeriodicRunner updateRunner;
}
