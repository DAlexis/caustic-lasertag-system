package ru.caustic.lasertagclientapp;


import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import ru.caustic.rcspcore.BluetoothManager;

import static android.app.Activity.RESULT_OK;


/**
 * A simple {@link Fragment} subclass.
 */
public class BluetoothConnectFragment extends Fragment implements View.OnClickListener {


    private static final int CHOOSE_BT_DEVICE = 1337;
    LinearLayout notConnectedLayout;
    LinearLayout connectingNowLayout;
    private Button btButton;
    private Button cancelScanButton;


    public BluetoothConnectFragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View layout = inflater.inflate(R.layout.fragment_bluetooth, container, false);

        //Initialize layout element references
        btButton = (Button) layout.findViewById(R.id.bt_scan);
        cancelScanButton = (Button) layout.findViewById(R.id.cancel_bt_scan);
        notConnectedLayout = (LinearLayout) layout.findViewById(R.id.not_connected_layout);
        connectingNowLayout = (LinearLayout) layout.findViewById(R.id.connecting_now_layout);


        //Set listener for BT scan button
        btButton.setOnClickListener(this);
        cancelScanButton.setOnClickListener(this);

        return layout;

    }

    @Override
    public void onClick(View view) {
        if (view.getId() == R.id.bt_scan) {
            final int btState = BluetoothManager.getInstance().checkBluetoothState();

            switch (BluetoothManager.getInstance().checkBluetoothState()) {
                case BluetoothManager.BLUETOOTH_NOT_SUPPORTED:
                    Toast.makeText(getActivity(), "Your device does not support Bluetooth", Toast.LENGTH_LONG).show();
                    return;
                case BluetoothManager.BLUETOOTH_DISABLED:
                    startActivityForResult(BluetoothManager.getInstance().getIntentEnable(), BluetoothManager.REQUEST_ENABLE_BT);
                    break;
                case BluetoothManager.BLUETOOTH_ENABLED:
                    Intent intent = new Intent(getActivity(), BluetoothDevicesActivity.class);

                    MainActivity activity = (MainActivity) getActivity();

                    startActivityForResult(intent, CHOOSE_BT_DEVICE);
                    break;
            }
        }
        if (view.getId() == R.id.cancel_bt_scan)
        {
            BluetoothManager.getInstance().cancel();
            notConnectedLayout.setVisibility(View.VISIBLE);
            connectingNowLayout.setVisibility(View.GONE);
        }
    }


    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == BluetoothManager.REQUEST_ENABLE_BT)
        {
            if (resultCode == RESULT_OK)
            {
                Intent intent = new Intent(getActivity(), BluetoothDevicesActivity.class);
                startActivityForResult(intent, CHOOSE_BT_DEVICE);
            }
            else {
                Toast.makeText(getActivity(), "Could not enable Bluetooth", Toast.LENGTH_LONG).show();
            }
        }

        if (requestCode == CHOOSE_BT_DEVICE) {
            if (resultCode == RESULT_OK) {
                String device = data.getStringExtra(BluetoothDevicesActivity.BT_DEVICE);
                String mac = device.split("\\n")[1];

                connectToBluetoothDevice(mac, device);
            }
        }
    }

    private void connectToBluetoothDevice(final String mac, final String deviceName) {
        notConnectedLayout.setVisibility(View.GONE);
        connectingNowLayout.setVisibility(View.VISIBLE);
        BluetoothManager.ConnectionDoneListener connectionDoneListener = (BluetoothManager.ConnectionDoneListener) getActivity();
        BluetoothManager.getInstance().connect(mac, deviceName, connectionDoneListener);
    }
}
