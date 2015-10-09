package ru.caustic.lasertag.causticlasertagcontroller;

import android.app.Activity;
import android.net.Uri;
import android.os.Bundle;
import android.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;


public class SimpleControlsFragment extends Fragment {

    private Button buttonRespawnAll;
    private Button buttonRespawnRed;
    private Button buttonRespawnBlue;
    private Button buttonKillAll;
    private Button buttonKillRed;
    private Button buttonKillBlue;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_simple_controls, container, false);

        buttonRespawnAll = (Button) view.findViewById(R.id.buttonRespawnAll);
        buttonRespawnRed = (Button) view.findViewById(R.id.buttonRespawnRed);
        buttonRespawnBlue = (Button) view.findViewById(R.id.buttonRespawnBlue);
        buttonKillAll = (Button) view.findViewById(R.id.buttonKillAll);
        buttonKillRed = (Button) view.findViewById(R.id.buttonKillRed);
        buttonKillBlue = (Button) view.findViewById(R.id.buttonKillBlue);

        buttonRespawnAll.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CausticDevicesManager.getInstance().remoteCall2(
                        BridgeConnector.Broadcasts.headSensors,
                        RCSProtocol.Operations.HeadSensor.functionsSerializers,
                        RCSProtocol.Operations.HeadSensor.Funcitons.playerRespawn.description.getId(),
                        ""
                );
            }
        });

        buttonRespawnRed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CausticDevicesManager.getInstance().remoteCall2(
                        BridgeConnector.Broadcasts.headSensorsRed,
                        RCSProtocol.Operations.HeadSensor.functionsSerializers,
                        RCSProtocol.Operations.HeadSensor.Funcitons.playerRespawn.description.getId(),
                        ""
                );
            }
        });

        buttonRespawnBlue.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CausticDevicesManager.getInstance().remoteCall2(
                        BridgeConnector.Broadcasts.headSensorsBlue,
                        RCSProtocol.Operations.HeadSensor.functionsSerializers,
                        RCSProtocol.Operations.HeadSensor.Funcitons.playerRespawn.description.getId(),
                        ""
                );
            }
        });

        buttonKillAll.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CausticDevicesManager.getInstance().remoteCall2(
                        BridgeConnector.Broadcasts.headSensors,
                        RCSProtocol.Operations.HeadSensor.functionsSerializers,
                        RCSProtocol.Operations.HeadSensor.Funcitons.playerKill.description.getId(),
                        ""
                );
            }
        });

        buttonKillRed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CausticDevicesManager.getInstance().remoteCall2(
                        BridgeConnector.Broadcasts.headSensorsRed,
                        RCSProtocol.Operations.HeadSensor.functionsSerializers,
                        RCSProtocol.Operations.HeadSensor.Funcitons.playerKill.description.getId(),
                        ""
                );
            }
        });

        buttonKillBlue.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CausticDevicesManager.getInstance().remoteCall2(
                        BridgeConnector.Broadcasts.headSensorsBlue,
                        RCSProtocol.Operations.HeadSensor.functionsSerializers,
                        RCSProtocol.Operations.HeadSensor.Funcitons.playerKill.description.getId(),
                        ""
                );
            }
        });

        return view;
    }

}
