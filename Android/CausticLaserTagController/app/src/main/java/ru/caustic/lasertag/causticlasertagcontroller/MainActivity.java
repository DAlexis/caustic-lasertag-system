package ru.caustic.lasertag.causticlasertagcontroller;

import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Map;
import java.util.TreeMap;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "CC.CausticMain";

    private Button connectButton;

    private Intent runServiceIntent;

    private LinearLayout selectDeviceLayout;
    private LinearLayout connectingNowLayout;
    private LinearLayout connectionEstablishedLayout;
    private LinearLayout operatingLayout;

    private TextView textViewConnectedTo;

    private TextView textViewDevicesInAreaCount;

    private int causticDevicesCountInArea = 0;

    private Handler uiHandler = null;

    boolean bound = false;
    ServiceConnection sConn;

    SharedPreferences sp;

    static final private int CHOOSE_BT_DEVICE = 0;

    static final int MSG_UPDATE_BLUETOOTH_UI = 1;

    public MainActivity() {
        uiHandler = new Handler(Looper.getMainLooper()) {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case MSG_UPDATE_BLUETOOTH_UI:
                        updateBluetoothStatusUI();
                        break;
                }
            }
        };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        RCSProtocol.Operations.init();
        Log.d(TAG, "Starting main activity");

        selectDeviceLayout = (LinearLayout) findViewById(R.id.selectDeviceLayout);
        connectingNowLayout = (LinearLayout) findViewById(R.id.connectingNowLayout);
        connectionEstablishedLayout = (LinearLayout) findViewById(R.id.connectionEstablishedLayout);

        operatingLayout = (LinearLayout) findViewById(R.id.operatingLayout);

        textViewConnectedTo = (TextView) findViewById(R.id.textViewConnectedTo);
        textViewDevicesInAreaCount = (TextView) findViewById(R.id.textViewDevicesInAreaCount);

/*
        infoTextView = (TextView) findViewById(R.id.selectedDeviceText);
        textViewDevsCount = (TextView) findViewById(R.id.textViewDevsCount);
*/
        ProgressBar spinner = (ProgressBar) findViewById(R.id.progressBar1);
        /*spinner.setVisibility(View.GONE);
        spinner.setVisibility(View.VISIBLE);*/

        switch(BluetoothManager.getInstance().checkBluetoothState()) {
            case BluetoothManager.BLUETOOTH_NOT_SUPPORTED:
                errorExit("Fatal Error", "Your device does not support bluetooth :(");
                break;
            case BluetoothManager.BLUETOOTH_DISABLED:
                startActivityForResult(BluetoothManager.getInstance().getIntentEnable(), BluetoothManager.REQUEST_ENABLE_BT);
                break;
            case BluetoothManager.BLUETOOTH_ENABLED:
                break;
        }

        updateBluetoothStatusUI();
/*
        sConn = new ServiceConnection() {
            public void onServiceConnected(ComponentName name, IBinder binder) {
                Log.d(TAG, "MainActivity onServiceConnected");
                bound = true;
            }

            public void onServiceDisconnected(ComponentName name) {
                Log.d(TAG, "MainActivity onServiceDisconnected");
                bound = false;
            }
        };*/

        //infoTextView.setText(BluetoothManager.getInstance().getDeviceName());

        // This is not used yet
        //runServiceIntent = new Intent("ru.caustic.lasertag.causticlasertagcontroller.CausticConnectorService");
        /*
        runServiceIntent = new Intent(MainActivity.this, CausticConnectorService.class);
        startService(runServiceIntent);
        bindService(runServiceIntent, sConn, BIND_AUTO_CREATE);*/
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == CHOOSE_BT_DEVICE) {
            if (resultCode == RESULT_OK) {
                //infoTextView.setText("Connecting...");
                //connectButton.setClickable(true);

                String device = data.getStringExtra(BluetoothDevicesList.BT_DEVICE);
                String mac = device.split("\\n")[1];

                connectToBluetoothDevice(mac, device);
                 /*
                if (!BluetoothManager.getInstance().connect(mac, device, null)) {
                    Toast.makeText(getBaseContext(), "Cannot connect to device!", Toast.LENGTH_LONG).show();
                }*/

                //infoTextView.setText(BluetoothManager.getInstance().getDeviceName());
            } else {
                //infoTextView.setText("");
            }
        }
    }

    private void updateBluetoothStatusUI() {
        switch(BluetoothManager.getInstance().getStatus()) {
            case BluetoothManager.BT_NOT_CONNECTED:
                connectingNowLayout.setVisibility(View.GONE);
                connectionEstablishedLayout.setVisibility(View.GONE);
                selectDeviceLayout.setVisibility(View.VISIBLE);
                operatingLayout.setVisibility(View.GONE);
                break;
            case BluetoothManager.BT_ESTABLISHING:
                selectDeviceLayout.setVisibility(View.GONE);
                connectionEstablishedLayout.setVisibility(View.GONE);
                connectingNowLayout.setVisibility(View.VISIBLE);
                operatingLayout.setVisibility(View.GONE);
                break;
            case BluetoothManager.BT_CONNECTED:
                selectDeviceLayout.setVisibility(View.GONE);
                connectingNowLayout.setVisibility(View.GONE);
                connectionEstablishedLayout.setVisibility(View.VISIBLE);
                operatingLayout.setVisibility(View.VISIBLE);
                textViewConnectedTo.setText(BluetoothManager.getInstance().getDeviceName());
                break;
        }

    }

    private void updateDevsCountInAreaUI() {
        textViewDevicesInAreaCount.setText(Integer.toString(causticDevicesCountInArea));
    }

    private void connectToBluetoothDevice(String mac, String deviceName) {
        selectDeviceLayout.setVisibility(View.GONE);
        connectingNowLayout.setVisibility(View.VISIBLE);
        BluetoothManager.getInstance().connect(mac, deviceName, new BluetoothManager.ConnectionDoneListener() {
            @Override
            public void onConnectionDone(boolean result) {
                //connectingNowLayout.setVisibility(View.GONE);
                /*
                Handler h = connectingNowLayout.getHandler();
                h.post(new Runnable() {
                    public void run() {
                        updateBluetoothStatusUI();
                    }
                });*/

                runOnUiThread(new Runnable() {
                    public void run() {
                        updateBluetoothStatusUI();
                    }
                });

                if (result) {
                    runOnUiThread(new Runnable() {
                        public void run() {
                            buttonScanClick(null);
                        }
                    });
                }
                //uiHandler.obtainMessage(MSG_UPDATE_BLUETOOTH_UI).sendToTarget();
            }
        });
    }

    public void selectBtDeviceClick(View view) {
        Intent intent = new Intent (MainActivity.this, BluetoothDevicesList.class);
        startActivityForResult(intent, CHOOSE_BT_DEVICE);
    }

    private void errorExit(String title, String message){
        Log.d(TAG, "Exiting with error: " + message);
        Toast.makeText(getBaseContext(), title + " - " + message, Toast.LENGTH_LONG).show();
        finish();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        //stopServiceClick(null);
    }

    public void stopServiceClick(View view) {
        if (bound) {
            unbindService(sConn);
            bound = false;
        }
        stopService(runServiceIntent);
    }

    public void causticDevsListClicked(View view) {
        Intent intent = new Intent (MainActivity.this, CausticDevicesListActivity.class);
        //Intent intent = new Intent (MainActivity.this, CausticDeviceSettingActivity.class);
        startActivityForResult(intent, 0);
    }


    public void testPrefsClick(View view) {
        Intent settingsActivity = new Intent(getBaseContext(), CausticDeviceSettings.class);
        startActivity(settingsActivity);
    }


    public void buttonSelectBridgeClick(View view) {
        Intent intent = new Intent (MainActivity.this, BluetoothDevicesList.class);
        startActivityForResult(intent, CHOOSE_BT_DEVICE);
    }

    public void buttonDisconnectClick(View view) {
        BluetoothManager.getInstance().disconnect();
        updateBluetoothStatusUI();
    }

    public void buttonScanClick(View view) {
        textViewDevicesInAreaCount.setText("0");
        CausticDevicesManager.getInstance().updateDevicesList2(
                new Handler() {
                    public void handleMessage(android.os.Message msg) {
                        switch (msg.what) {
                            case CausticDevicesManager.DEVICES_LIST_UPDATED:
                                Map<BridgeConnector.DeviceAddress, CausticDevicesManager.CausticDevice2> devs =
                                        (Map<BridgeConnector.DeviceAddress, CausticDevicesManager.CausticDevice2>) msg.obj;

                                causticDevicesCountInArea = devs.size();

                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        updateDevsCountInAreaUI();
                                    }
                                });
                                break;
                        }
                    }
                });
    }

    public void buttonSimpleControlsClick(View view) {
        Intent settingsActivity = new Intent(getBaseContext(), SimpleControlsActivity.class);
        startActivity(settingsActivity);
    }

    public void buttonDevicesSettingsClick(View view) {
        Intent intent = new Intent (MainActivity.this, CausticDevicesListActivity.class);
        //Intent intent = new Intent (MainActivity.this, CausticDeviceSettingActivity.class);
        startActivity(intent);
    }
}
