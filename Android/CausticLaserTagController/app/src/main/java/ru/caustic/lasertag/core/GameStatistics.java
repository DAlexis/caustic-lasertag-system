package ru.caustic.lasertag.core;

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

    public GameStatistics(DevicesManager devicesManager) {
        this.devicesManager = devicesManager;
        this.devicesManager.registerRCSPOperationDispatcher(OP_CODE_GET_PVP_RESULTS, new PvPDamageResultsMessageDispatcher());
    }

    public void readStats()
    {
        sendRequest();
        // Run thread for waiting stats
    }

    // Private
    private void sendRequest() {
        devicesManager.remoteCall(
                BridgeConnector.Broadcasts.headSensors,
                RCSProtocol.Operations.HeadSensor.functionsSerializers,
                RCSProtocol.Operations.HeadSensor.Functions.readStats.getId(),
                ""
        );
    }

    private class PvPDamageResultsMessageDispatcher implements DevicesManager.RCSPOperationDispatcher {
        @Override
        public void dispatchOperation(BridgeConnector.DeviceAddress address, RCSProtocol.RCSPOperation operation)
        {

        }
    }

    /**
     * This class represents corresponding class on game device. It is minimal stats synchronization
     * element
     */
    private class PvPDamageResults {
        public void lookupPlayerId(BridgeConnector.DeviceAddress devAddr)
        {
            playerId = devicesManager.getPlayerGameId(devAddr);
        }

        public void deserialize(byte[] memory, int offset)
        {
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

    private void buildStatisticsTable()
    {
        for (Map.Entry<Integer, PvPDamageResults> entry : pvpResultsMap.entrySet())
        {
            int victimId = entry.getKey();
            int enemyId = entry.getValue().enemyId;
            PvPDamageResults pvpRes = entry.getValue();

            TreeMap<Integer, PvPStats> enemyStats = pvpStatsMap2.get(enemyId);
            if (enemyStats == null)
                enemyStats = pvpStatsMap2.put(enemyId, new TreeMap<Integer, PvPStats>());

            PvPStats pvpStats = enemyStats.get(victimId);
            if (pvpStats == null)
                pvpStats = enemyStats.put(victimId, new PvPStats());

            pvpStats.damage += pvpRes.totalDamage;
            pvpStats.hits   += pvpRes.hitsCount;
            pvpStats.kills  += pvpRes.killsCount;
        }
    }

    private String getStatisticsTableStrStub(int statParType)
    {
        String result = "";
        for (TreeMap.Entry<Integer, TreeMap<Integer, PvPStats>> playerStats : pvpStatsMap2.entrySet())
        {
            result = playerStats.getKey().toString() + ": ";
            for (TreeMap.Entry<Integer, PvPStats> victim : playerStats.getValue().entrySet())
            {
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

    private DevicesManager devicesManager;

    private TreeMap<Integer, TreeMap<Integer, PvPStats>> pvpStatsMap2 = new TreeMap<>();
    private Map<Integer, PvPDamageResults> pvpResultsMap = new TreeMap<>();
}
