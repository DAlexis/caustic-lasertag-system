package org.ltcaustic.gamecontroller;

import android.content.Context;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.Switch;

import org.ltcaustic.rcspandroid.CausticInitializer;


/**
 * A simple {@link Fragment} subclass.
 */
public class FragmentGameControls extends Fragment {

    Switch switchRed;
    Switch switchBlue;
    Switch switchYellow;
    Switch switchGreen;
    Switch switchAllTeams;

    Button buttonRespawn;
    Button buttonKill;
    Button buttonNewGame;

    public FragmentGameControls() {
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
        // Inflate the layout for this fragment
        View v = inflater.inflate(R.layout.fragment_game_controls, container, false);
        switchRed = v.findViewById(R.id.switchRed);
        switchBlue = v.findViewById(R.id.switchBlue);
        switchYellow = v.findViewById(R.id.switchYellow);
        switchGreen = v.findViewById(R.id.switchGreen);
        switchAllTeams = v.findViewById(R.id.switchAllTeams);

        buttonRespawn = v.findViewById(R.id.buttonRespawn);
        buttonKill = v.findViewById(R.id.buttonKill);
        buttonNewGame = v.findViewById(R.id.buttonNewGame);

        switchAllTeams.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                switchRed.setChecked(b);
                switchBlue.setChecked(b);
                switchYellow.setChecked(b);
                switchGreen.setChecked(b);
            }
        });

        buttonRespawn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                CausticInitializer.getInstance().controller().getBroadcastCalls().respawn(getSelectedTeams());
            }
        });
        buttonKill.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                CausticInitializer.getInstance().controller().getBroadcastCalls().kill(getSelectedTeams());
            }
        });
        buttonNewGame.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                CausticInitializer.getInstance().controller().getBroadcastCalls().resetPlayers(getSelectedTeams());
            }
        });
        return v;

    }

    private int getSelectedTeams()
    {
        int result = 0;
        if (switchRed.isChecked())
            result |= CausticInitializer.getInstance().controller().getBroadcastCalls().RED;
        if (switchBlue.isChecked())
            result |= CausticInitializer.getInstance().controller().getBroadcastCalls().BLUE;
        if (switchYellow.isChecked())
            result |= CausticInitializer.getInstance().controller().getBroadcastCalls().YELLOW;
        if (switchGreen.isChecked())
            result |= CausticInitializer.getInstance().controller().getBroadcastCalls().GREEN;
        return result;
    }
}
