package ru.caustic.lasertagclientapp;


import android.content.Context;
import android.graphics.Typeface;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import com.caustic.rcspcore.CausticController;
import com.caustic.rcspcore.CausticDevice;
import com.caustic.rcspcore.DevicesManager;
import com.caustic.rcspcore.RCSP;

import static com.caustic.rcspcore.DeviceUtils.allDevsSynced;
import static com.caustic.rcspcore.DeviceUtils.getDeviceTeam;
import static com.caustic.rcspcore.DeviceUtils.getHeadSensorsMap;


/**
 * A simple {@link Fragment} subclass.
 */
public class PlayerListFragment extends Fragment implements View.OnClickListener {

    private DevicesManager devMan;
    private PlayerListAdapter adapter;

    private Map<RCSP.DeviceAddress, CausticDevice> headSensors = new HashMap<>();
    private ListView playerList;
    private static final String TAG = "PlayerListFragment";

    private Handler listUpdateHandler = new Handler();
    boolean handlerRunning = false;
    private long lastAutoUpdateRun = SystemClock.elapsedRealtime();
    private static final int AUTO_UPDATE_INTERVAL_MILLIS = 2000;
    private static boolean deviceSyncRunning = false;

    private DevicesManager.SynchronizationEndListener syncListener = new DevicesManager.SynchronizationEndListener() {
        @Override
        public void onSynchronizationEnd(boolean isSuccess, Set<RCSP.DeviceAddress> notSynchronized) {
            Log.d(TAG, "Sync completed, result: " + isSuccess + ", not synchronized: " + notSynchronized.size());
            //List population occurs here.
            refreshPlayerListAdapter();
            deviceSyncRunning = false;
        }
    };

    private DevicesManager.DeviceListUpdatedListener devListener = new DevicesManager.DeviceListUpdatedListener() {
        @Override
        public void onDevicesListUpdated() {
            Log.d(TAG, "Updated devices list");
                deviceSyncRunning = true;
                Log.d(TAG, "Starting parameters sync");
                devMan.asyncPullAllParameters(syncListener, getHeadSensorsMap(devMan.devices).keySet());

        }
    };

    private boolean relatedParamsAdded(Map<RCSP.DeviceAddress, CausticDevice> headSensorsMap) {
        boolean result = true;
        for (Map.Entry<RCSP.DeviceAddress, CausticDevice> entry : headSensorsMap.entrySet()) {
            CausticDevice dev = entry.getValue();
            result = result && dev.areDeviceRelatedParametersAdded();
        }
        return result;
    }


    private Runnable listUpdateMonitor = new Runnable(){
        @Override
        public void run() {

            if (!deviceSyncRunning && allDevsSynced(getHeadSensorsMap(devMan.devices))) {
                devMan.updateDevicesList();
                Log.d(TAG, "listUpdateMonitor updating player list");
            }
            listUpdateHandler.postDelayed(this, AUTO_UPDATE_INTERVAL_MILLIS);
//            if ((SystemClock.elapsedRealtime()-lastAutoUpdateRun)>AUTO_UPDATE_INTERVAL_MILLIS) {
//                devMan.updateDevicesList();
//            }
//
        }
    };

    private void refreshPlayerListAdapter() {
        synchronized (adapter) {

            //This array contains the player list sorted by team. First entry of each team has a visible separator in its View
            final ArrayList<PlayerListEntryHolder> newPlayersList = populatePlayersArray(devMan.devices);
            if (newPlayersList.equals(players)) {
                Log.d(TAG, "New player list is equal to the old one, no refresh done");
                return;
            }
            else {


                FragmentActivity activity = getActivity();

                //Check if the activity hasn't been destroyed in the meantime
                if (activity != null) {
                    activity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {

                            //Replacing list
                            players = newPlayersList;
                            adapter.clear();

                            for (PlayerListEntryHolder entry : players) {
                                adapter.addItem(entry);
                            }


                            Log.d(TAG, "Player list adapter refreshed");

                            adapter.notifyDataSetChanged();
                            Log.d(TAG, "Notifying adapter to redraw list");
                        }
                    });
                }
            }

        }
    }

    @Override
    public void onClick(View view) {
        if (view.getId() == R.id.update_players_list_button) {
                devMan.updateDevicesList();
        }
    }

    private ArrayList<PlayerListEntryHolder> players;

    public class PlayerListAdapter extends BaseAdapter{

        private ArrayList<PlayerListEntryHolder> mHolders = new ArrayList();
        private LayoutInflater mInflater;

        public PlayerListAdapter() {
            mInflater = (LayoutInflater) getActivity().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        public void addItem(PlayerListEntryHolder item) {
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
                mHolders.get(position).setView(mInflater.inflate(R.layout.layout_player_list_entry, null));
                mHolders.get(position).updateView();
            }
            return mHolders.get(position).getView();
        }

    }

    private class PlayerListEntryHolder implements Comparable<PlayerListEntryHolder> {
        public TextView playerNameTextView = null;
        public TextView entrySeparatorTeamTag = null;

        public String playerName;
        public String team = "Default";
        public boolean needSeparator = true;

        View convertView = null;

        PlayerListEntryHolder(CausticDevice device) {
            playerName = device.getName();
            team = getDeviceTeam(device);
        }

        public View getView() {
            return convertView;
        }

        public void setView(View convertView) {
            this.convertView = convertView;
        }

        public void updateView() {

            playerNameTextView = (TextView) convertView.findViewById(R.id.player_list_entry_name);
            entrySeparatorTeamTag = (TextView) convertView.findViewById(R.id.player_list_entry_separator);

            int backColor;
            int separColor;
            switch (team)
            {
                case "Red":
                    separColor=ContextCompat.getColor(getContext(), R.color.red);
                    break;
                case "Blue":
                    separColor=ContextCompat.getColor(getContext(), R.color.blue);
                    break;
                case "Green":
                    separColor=ContextCompat.getColor(getContext(), R.color.green);
                    break;
                case "Yellow":
                    separColor=ContextCompat.getColor(getContext(), R.color.yellow);
                    break;
                default:
                    separColor=ContextCompat.getColor(getContext(), R.color.grey);
             }

            backColor=separColor-0x99000000;

            playerNameTextView.setText(playerName);
            playerNameTextView.setBackgroundColor(backColor);

            entrySeparatorTeamTag.setTypeface(null, Typeface.BOLD);
            entrySeparatorTeamTag.setText("Team " + team);
            entrySeparatorTeamTag.setTextColor(separColor);

            if (needSeparator)
            {
                entrySeparatorTeamTag.setVisibility(View.VISIBLE);
            }
            else
            {
                entrySeparatorTeamTag.setVisibility(View.GONE);
            }

        }

        @Override
        public int compareTo(PlayerListEntryHolder playerListEntryHolder) {
            return this.team.compareTo(playerListEntryHolder.team);
        }

        @Override
        public boolean equals(Object obj) {
            PlayerListEntryHolder rhs = (PlayerListEntryHolder) obj;

            boolean result = true;
            result = result && rhs.playerName.equals(this.playerName);
            result = result && rhs.team.equals(this.team);
            result = result && (rhs.needSeparator==this.needSeparator);
            return result;
        }
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                         Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View layout = inflater.inflate(R.layout.fragment_player_list, container, false);

        Button updateButton = (Button) layout.findViewById(R.id.update_players_list_button);
        updateButton.setOnClickListener(this);

        adapter= new PlayerListAdapter();
        playerList = (ListView) layout.findViewById(R.id.players_listview);
        playerList.setAdapter(adapter);

        devMan=CausticController.getInstance().getDevicesManager();
        devMan.setDeviceListUpdatedListener(devListener);
        devMan.updateDevicesList();

       // listUpdateHandler.postDelayed(listUpdateMonitor, AUTO_UPDATE_INTERVAL_MILLIS);

        return layout;
    }


    private ArrayList<PlayerListEntryHolder> populatePlayersArray(Map<RCSP.DeviceAddress, CausticDevice> devices) {

        ArrayList<PlayerListEntryHolder> array = new ArrayList<>();
        for (Map.Entry<RCSP.DeviceAddress, CausticDevice> entry : devices.entrySet()) {

            CausticDevice dev = entry.getValue();
            int devTypeInt = Integer.parseInt(
                    dev.parameters.get(RCSP.Operations.AnyDevice.Configuration.deviceType.getId()).getValue()
            );

            //Add only head sensors to be displayed as players
            if (devTypeInt == RCSP.Operations.AnyDevice.Configuration.DEV_TYPE_HEAD_SENSOR){
                if (dev.areDeviceRelatedParametersAdded()&&dev.areParametersSync())
                {
                    PlayerListEntryHolder player = new PlayerListEntryHolder(dev);
                    array.add(player);
                }
            }
        }

        if (array.size()>0) {
            //Sort by team
            Collections.sort(array);

            //Add separators between teams and in the beginning of the list

            String oldTeam = "";

            for (int i=0; i<array.size(); i++) {
                PlayerListEntryHolder current = array.get(i);
                if (current.team.equals(oldTeam)) {
                    current.needSeparator = false;
                    array.set(i, current);
                }
                oldTeam = current.team;
            }
        }
        return array;
    }


    @Override
    public void onDestroyView() {
        super.onDestroyView();
        listUpdateHandler.removeCallbacks(listUpdateMonitor);
    }
}
