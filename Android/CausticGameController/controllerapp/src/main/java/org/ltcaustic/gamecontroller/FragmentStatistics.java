package org.ltcaustic.gamecontroller;

import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;

import org.ltcaustic.rcspandroid.CausticInitializer;
import org.ltcaustic.rcspcore.CausticDevice;
import org.ltcaustic.rcspcore.DevicesManager;
import org.ltcaustic.rcspcore.GameStatistics;

import java.util.Set;

/**
 * A simple {@link Fragment} subclass.
 */
public class FragmentStatistics extends Fragment {

    TableLayout tableLayoutStats;

    GameStatistics statistics;
    DevicesManager devicesManager;

    public FragmentStatistics() {
        // Required empty public constructor
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {

        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        statistics = CausticInitializer.getInstance().controller().getGameStatistics();
        devicesManager = CausticInitializer.getInstance().controller().getDevicesManager();

        View v = inflater.inflate(R.layout.fragment_statistics, container, false);
        tableLayoutStats = v.findViewById(R.id.tableLayoutStats);

        statistics.updateStats();
        updateStatistics();
        subscribe();
        return v;
    }

    @Override
    public void onResume() {
        super.onResume();
        isActive = true;
    }

    @Override
    public void onPause() {
        super.onPause();
        isActive = false;
    }

    @Override
    public void onDetach() {
        super.onDetach();
    }

    void addTextView(TableRow row, String text, boolean center) {
        TextView tv = new TextView(getActivity());
        tv.setText(text);
        if (center)
            tv.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
        row.addView(tv);
    }

    void updateStatistics() {
        Set<Integer> ids = statistics.getPlayerIds();
        while (tableLayoutStats.getChildCount() > 1)
            tableLayoutStats.removeView(tableLayoutStats.getChildAt(tableLayoutStats.getChildCount() - 1));

        for (Integer id : ids)
        {
            CausticDevice dev = devicesManager.getDeviceById(id);
            String name;
            String lives;
            if (dev == null) {
                name = "Unknown player " + Integer.toString(id);
                lives = "-";
            } else {
                name = dev.getName();
                lives = Integer.toString(dev.getLives());
            }

            String kills = Integer.toString(statistics.getStatForPlayerID(id, GameStatistics.ENEMY_KILLS_COUNT));
            String damage = Integer.toString(statistics.getStatForPlayerID(id, GameStatistics.DAMAGE));
            String hits = Integer.toString(statistics.getStatForPlayerID(id, GameStatistics.HITS_COUNT));
            String ff = Integer.toString(statistics.getStatForPlayerID(id, GameStatistics.FRIENDLY_KILLS_COUNT));

            TableRow row = new TableRow(getActivity());

            addTextView(row, name, false);
            addTextView(row, lives, true);
            addTextView(row, kills, true);
            addTextView(row, damage, true);
            addTextView(row, hits, true);
            addTextView(row, ff, true);

            tableLayoutStats.addView(row);
        }
    }

    void subscribe() {
        if (updatedSubscriber != null)
            return;
        updatedSubscriber = new GameStatistics.StatsUpdatedSubscriber() {
            @Override
            public void onStatsUpdated() {
                if (isActive) {
                    getActivity().runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            updateStatistics();
                        }
                    });
                }
            }
        };
        statistics.subscribeStatsUpdated(updatedSubscriber);
    }

    GameStatistics.StatsUpdatedSubscriber updatedSubscriber = null;
    boolean isActive = false;
}
