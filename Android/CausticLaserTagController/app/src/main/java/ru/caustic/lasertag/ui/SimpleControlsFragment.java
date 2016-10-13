package ru.caustic.lasertag.ui;

import android.os.Bundle;
import android.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;

import ru.caustic.lasertag.core.BridgeConnector;
import ru.caustic.lasertag.core.CausticController;
import ru.caustic.lasertag.core.CausticDevicesManager;
import ru.caustic.lasertag.core.RCSProtocol;

public class SimpleControlsFragment extends Fragment {

    private Button buttonRespawn;
    private Button buttonKill;

    private CheckBox checkBoxControlRed;
    private CheckBox checkBoxControlBlue;
    private CheckBox checkBoxControlYellow;
    private CheckBox checkBoxControlGreen;
    private CheckBox checkBoxControlAllTeams;

    private CausticDevicesManager causticDevicesManager;

    public SimpleControlsFragment()
    {
        super();
        // @todo Refactor without signleton usage
        causticDevicesManager = CausticController.getInstance().getCausticDevicesManager();
    }
    //private boolean workaroundDisableAutoChecking
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_simple_controls, container, false);

        buttonRespawn = (Button) view.findViewById(R.id.buttonRespawn);
        buttonKill = (Button) view.findViewById(R.id.buttonKill);

        checkBoxControlRed = (CheckBox) view.findViewById(R.id.checkBoxControlRed);
        checkBoxControlBlue = (CheckBox) view.findViewById(R.id.checkBoxControlBlue);
        checkBoxControlYellow = (CheckBox) view.findViewById(R.id.checkBoxControlYellow);
        checkBoxControlGreen = (CheckBox) view.findViewById(R.id.checkBoxControlGreen);
        checkBoxControlAllTeams = (CheckBox) view.findViewById(R.id.checkBoxControlAllTeams);

        checkBoxControlAllTeams.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                checkBoxControlAllTeamsClick(v);
            }
        });
        checkBoxControlRed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                checkBoxAnyTeamClick(v);
            }
        });
        checkBoxControlBlue.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                checkBoxAnyTeamClick(v);
            }
        });
        checkBoxControlYellow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                checkBoxAnyTeamClick(v);
            }
        });
        checkBoxControlGreen.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                checkBoxAnyTeamClick(v);
            }
        });

        buttonRespawn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                buttonRespawnClick(v);
            }
        });
        buttonKill.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                buttonKillClick(v);
            }
        });

        return view;
    }

    private void checkBoxControlAllTeamsClick(View v) {
        checkBoxControlRed.setChecked(checkBoxControlAllTeams.isChecked());
        checkBoxControlBlue.setChecked(checkBoxControlAllTeams.isChecked());
        checkBoxControlYellow.setChecked(checkBoxControlAllTeams.isChecked());
        checkBoxControlGreen.setChecked(checkBoxControlAllTeams.isChecked());
    }

    private void checkBoxAnyTeamClick(View v) {
        if (
                checkBoxControlRed.isChecked() &&
                checkBoxControlBlue.isChecked() &&
                checkBoxControlYellow.isChecked() &&
                checkBoxControlGreen.isChecked()
            )
            checkBoxControlAllTeams.setChecked(true);
        else
            checkBoxControlAllTeams.setChecked(false);
    }

    private void remoteCallForSelection(RCSProtocol.FunctionsContainer functionsContainer, int operationId, String argument)
    {
        // If we need to respawn all players, we can use single broadcast
        if (checkBoxControlAllTeams.isChecked()) {
            causticDevicesManager.remoteCall(
                    BridgeConnector.Broadcasts.headSensors,
                    functionsContainer,
                    operationId,
                    argument
            );
            return;
        }

        // else we should check each checkbox if checked
        if (checkBoxControlRed.isChecked()) {
            causticDevicesManager.remoteCall(
                    BridgeConnector.Broadcasts.headSensorsRed,
                    functionsContainer,
                    operationId,
                    argument
            );
        }
        if (checkBoxControlBlue.isChecked()) {
            causticDevicesManager.remoteCall(
                    BridgeConnector.Broadcasts.headSensorsBlue,
                    functionsContainer,
                    operationId,
                    argument
            );
        }
        if (checkBoxControlYellow.isChecked()) {
            causticDevicesManager.remoteCall(
                    BridgeConnector.Broadcasts.headSensorsYellow,
                    functionsContainer,
                    operationId,
                    argument
            );
        }
        if (checkBoxControlGreen.isChecked()) {
            causticDevicesManager.remoteCall(
                    BridgeConnector.Broadcasts.headSensorsGreen,
                    functionsContainer,
                    operationId,
                    argument
            );
        }

    }

    private void buttonRespawnClick(View v) {
        remoteCallForSelection(
                RCSProtocol.Operations.HeadSensor.functionsSerializers,
                RCSProtocol.Operations.HeadSensor.Functions.playerRespawn.getId(),
                ""
        );
    }

    private void buttonKillClick(View v) {
        remoteCallForSelection(
                RCSProtocol.Operations.HeadSensor.functionsSerializers,
                RCSProtocol.Operations.HeadSensor.Functions.playerKill.getId(),
                ""
        );
    }
}
