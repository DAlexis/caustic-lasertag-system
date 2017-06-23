package ru.caustic.lasertagclientapp;


import android.content.Context;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Map;
import java.util.Set;

import ru.caustic.rcspcore.CausticController;
import ru.caustic.rcspcore.CausticDevice;
import ru.caustic.rcspcore.DeviceUtils;
import ru.caustic.rcspcore.DevicesManager;
import ru.caustic.rcspcore.GameStatistics;
import ru.caustic.rcspcore.RCSP;

import static ru.caustic.rcspcore.DeviceUtils.getCurrentHealth;
import static ru.caustic.rcspcore.DeviceUtils.getDeviceTeam;
import static ru.caustic.rcspcore.DeviceUtils.getHeadSensorsMap;
import static ru.caustic.rcspcore.DeviceUtils.getMarkerColor;


/**
 * A simple {@link Fragment} subclass.
 */
public class GameScoresFragment extends Fragment {

    private static final long SCORE_UPDATE_DELAY_MS = 3000;
    private static DevicesManager devMan = CausticController.getInstance().getDevicesManager();
    private static GameStatistics gameStats = CausticController.getInstance().getGameStatistics();
    private CausticDevice headSensor = devMan.devices.get(devMan.associatedHeadSensorAddress);
    private ArrayList<ScoreListEntryHolder> scoreEntries;
    private ListView scoreList;
    private ScoreListAdapter adapter;
    private static final String TAG = "GameScoresFragment";


    private static boolean devStateUpdaterRunning = false;

    private static DevicesManager.SynchronizationEndListener mListener;


    private Handler devStateUpdateHandler = new Handler();
    private static Runnable devStateUpdater = new Runnable(){
        @Override
        public void run() {
            devMan.invalidateDevsMapParams(DeviceUtils.getHeadSensorsMap(devMan.devices).values());
            devMan.asyncPullAllParameters(mListener, DeviceUtils.getHeadSensorsMap(devMan.devices).keySet());
            gameStats.updateStats();
        }
    };

    private DevicesManager.SynchronizationEndListener synchronizationEndListener = new DevicesManager.SynchronizationEndListener() {
        @Override
        public void onSynchronizationEnd(boolean isSuccess, Set<RCSP.DeviceAddress> notSynchronized) {

            Log.d(TAG, "Sync completed, result: " + isSuccess + ", not synchronized: " + notSynchronized.size());

            scoreEntries = populateScoresArray(DeviceUtils.getHeadSensorsMap(devMan.devices));
            FragmentActivity activity=getActivity();
            if (activity!=null)
            {
                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        refreshScoreListAdapter();
                        Log.d(TAG, "Refreshing scores");
                    }
                });
            }
            devStateUpdateHandler.postDelayed(devStateUpdater, SCORE_UPDATE_DELAY_MS);
        }

    };

    private GameStatistics.StatsChangeListener scoresUpdatedListener = new GameStatistics.StatsChangeListener() {

        @Override
        public void onStatsChange(int victimId, int damagerId)
        {
            final FragmentActivity activity=getActivity();
            if (activity!=null) {
                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        TextView kills = (TextView) activity.findViewById(R.id.kill_count);
                        TextView score = (TextView) activity.findViewById(R.id.score_count);
                        TextView deaths = (TextView) activity.findViewById(R.id.death_count);
                        int killCount = 0;
                        int friendlyKills = 0;
                        int scoreCount = 0;
                        int deathCount = 0;
                        int hitCount = 0;
                        if (headSensor!=null) {
                            int ownId = Integer.parseInt(headSensor.parameters.get(RCSP.Operations.HeadSensor.Configuration.playerGameId.getId()).getValue());
                            killCount = gameStats.getStatForPlayerID(ownId, GameStatistics.ENEMY_KILLS_COUNT);
                            hitCount = gameStats.getStatForPlayerID(ownId, GameStatistics.HITS_COUNT);
                            deathCount = Integer.parseInt(headSensor.parameters.get(RCSP.Operations.HeadSensor.Configuration.deathCount.getId()).getValue());
                            friendlyKills = gameStats.getStatForPlayerID(ownId, GameStatistics.FRIENDLY_KILLS_COUNT);
                            scoreCount = 2*killCount + hitCount - deathCount - 2*friendlyKills;
                            if (kills!=null) kills.setText(Integer.toString(killCount));
                            if (score!=null) score.setText(Integer.toString(scoreCount));
                            if (deaths!=null) deaths.setText(Integer.toString(deathCount));
                            Log.d(TAG, "Updating stats");
                        }
                    }
                });
            }
        }
    };

    public GameScoresFragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View layout = inflater.inflate(R.layout.fragment_score_list, container, false);

        adapter= new ScoreListAdapter();
        scoreList = (ListView) layout.findViewById(R.id.scores_listview);
        scoreList.setAdapter(adapter);

        scoreEntries = populateScoresArray(getHeadSensorsMap(devMan.devices));
        adapter.clear();

        gameStats.setStatsChangeListener(scoresUpdatedListener);

        for (ScoreListEntryHolder entry : scoreEntries) {
            adapter.addItem(entry);
        }
        Log.d(TAG, "Score list adapter refreshed");
        adapter.notifyDataSetChanged();
        Log.d(TAG, "Notifying adapter to redraw list");



        return layout;
    }

    public class ScoreListAdapter extends BaseAdapter {

        private ArrayList<ScoreListEntryHolder> mHolders = new ArrayList();
        private LayoutInflater mInflater;

        public ScoreListAdapter() {
            mInflater = (LayoutInflater) getActivity().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        public void addItem(ScoreListEntryHolder item) {
            mHolders.add(item);
        }

        public void clear() {
            mHolders.clear();
        }

        @Override
        public int getCount() {
            return mHolders.size();
        }

        @Override
        public Object getItem(int i) {
            return mHolders.get(i);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (mHolders.get(position).getView() == null) {
                mHolders.get(position).setView(mInflater.inflate(R.layout.layout_score_list_entry, null));
                mHolders.get(position).updateView();
            }
            return mHolders.get(position).getView();
        }

    }

    private class ScoreListEntryHolder implements Comparable<ScoreListEntryHolder> {
        public TextView rankTextView = null;

        public ImageView markerIcon = null;

        public TextView playerNameTextView = null;

        public TextView scoresTextView = null;
        public TextView killsTextView = null;
        public TextView deathsTextView = null;

        Bitmap icon;

        public int ranking = 0;
        public String playerName;
        public String team = "Default";

        public int kills = 0;
        public int deaths = 0;
        public int score = 0;

        public boolean isAlive = true;
        public boolean isHighlighted = false;

        View convertView = null;

        ScoreListEntryHolder(CausticDevice device) {
            playerName = device.getName();
            team = getDeviceTeam(device);
            if (getCurrentHealth(device) == 0) {
                isAlive = false;
            }
            int id = Integer.parseInt(device.parameters.get(RCSP.Operations.HeadSensor.Configuration.playerGameId.getId()).getValue());
            kills = gameStats.getStatForPlayerID(id, GameStatistics.ENEMY_KILLS_COUNT);
            deaths = Integer.parseInt(device.parameters.get(RCSP.Operations.HeadSensor.Configuration.deathCount.getId()).getValue());
            int hits = gameStats.getStatForPlayerID(id, GameStatistics.HITS_COUNT);
            int friendlyKills = gameStats.getStatForPlayerID(id, GameStatistics.FRIENDLY_KILLS_COUNT);
            score = 2*kills + hits - deaths - 2*friendlyKills;

            int ownId = Integer.parseInt(device.parameters.get(RCSP.Operations.HeadSensor.Configuration.playerGameId.getId()).getValue());
            if (id == ownId)
            {
                isHighlighted = true;
            }

            icon = MapFragment.createMarkerIcon(getActivity(), getMarkerColor(device), getDeviceTeam(device), isAlive);

        }

        public View getView() {
            return convertView;
        }

        public void setView(View convertView) {
            this.convertView = convertView;
        }

        public void updateView() {

            rankTextView = (TextView) convertView.findViewById(R.id.score_list_entry_rank);
            markerIcon = (ImageView) convertView.findViewById(R.id.score_list_entry_marker);

            playerNameTextView = (TextView) convertView.findViewById(R.id.score_list_entry_player_name);

            scoresTextView = (TextView) convertView.findViewById(R.id.score_list_entry_score);
            killsTextView = (TextView) convertView.findViewById(R.id.score_list_entry_kills);
            deathsTextView = (TextView) convertView.findViewById(R.id.score_list_entry_deaths);

            int backColor;
            switch (team)
            {
                case "Red":
                    backColor= ContextCompat.getColor(getContext(), R.color.red);
                    break;
                case "Blue":
                    backColor=ContextCompat.getColor(getContext(), R.color.blue);
                    break;
                case "Green":
                    backColor=ContextCompat.getColor(getContext(), R.color.green);
                    break;
                case "Yellow":
                    backColor=ContextCompat.getColor(getContext(), R.color.yellow);
                    break;
                default:
                    backColor=ContextCompat.getColor(getContext(), R.color.grey);
            }

            backColor=backColor-0x99000000;

            rankTextView.setText(Integer.toString(ranking));

            markerIcon.setImageBitmap(icon);

            playerNameTextView.setText(playerName);
            playerNameTextView.setBackgroundColor(backColor);

            scoresTextView.setText(Integer.toString(score));
            killsTextView.setText(Integer.toString(kills));
            deathsTextView.setText(Integer.toString(deaths));
        }

        @Override
        public int compareTo(ScoreListEntryHolder scoreListEntryHolder) {
            if (this.score > scoreListEntryHolder.score)
            {
                return -1;
            }
            if (this.score == scoreListEntryHolder.score)
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }

        @Override
        public boolean equals(Object obj) {
            ScoreListEntryHolder rhs = (ScoreListEntryHolder) obj;

            boolean result = true;
            result = result && rhs.playerName.equals(this.playerName);
            result = result && rhs.team.equals(this.team);
            result = result && (rhs.ranking == this.ranking);
            result = result && (rhs.kills == this.kills);
            result = result && (rhs.deaths == this.deaths);
            result = result && (rhs.score == this.score);
            result = result && (rhs.isAlive == this.isAlive);
            result = result && (rhs.isHighlighted == this.isHighlighted);
            return result;
        }
    }

    private ArrayList<ScoreListEntryHolder> populateScoresArray(Map<RCSP.DeviceAddress, CausticDevice> devices) {

        ArrayList<ScoreListEntryHolder> array = new ArrayList<>();
        for (Map.Entry<RCSP.DeviceAddress, CausticDevice> entry : devices.entrySet()) {

            CausticDevice dev = entry.getValue();
            if (dev.areDeviceRelatedParametersAdded()&&dev.areParametersSync())
            {
                ScoreListEntryHolder player = new ScoreListEntryHolder(dev);
                array.add(player);
            }
        }

        if (array.size()>0) {
            //Sort by score
            Collections.sort(array);
        }
        for (int i=0; i < array.size(); i++)
        {
            array.get(i).ranking=i+1;
        }
        return array;
    }

    private void refreshScoreListAdapter() {
        synchronized (adapter) {
            //This array contains the player list sorted by team. First entry of each team has a visible separator in its View
            final ArrayList<ScoreListEntryHolder> newScoresList = populateScoresArray(getHeadSensorsMap(devMan.devices));

                FragmentActivity activity = getActivity();
                //Check if the activity hasn't been destroyed in the meantime
                if (activity != null) {
                    activity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {

                            //Replacing list
                            scoreEntries = newScoresList;
                            adapter.clear();

                            for (ScoreListEntryHolder entry : scoreEntries) {
                                adapter.addItem(entry);
                            }
                            Log.d(TAG, "Score list adapter refreshed");
                            adapter.notifyDataSetChanged();
                            Log.d(TAG, "Notifying adapter to redraw list");
                        }
                    });
                }
        }
    }

    public void onStart() {
        super.onStart();
        if (!devStateUpdaterRunning) {
            startDevStateUpdater();
        }
    }

    @Override
    public void onStop() {
        super.onStop();
        if (devStateUpdaterRunning) stopDevStateUpdater();
    }

    private void startDevStateUpdater() {
        mListener=synchronizationEndListener;
        devStateUpdateHandler.post(devStateUpdater);
        devStateUpdaterRunning = true;
    }

    private void stopDevStateUpdater() {
        devStateUpdateHandler.removeCallbacks(devStateUpdater);
        devStateUpdaterRunning = false;
    }

}
