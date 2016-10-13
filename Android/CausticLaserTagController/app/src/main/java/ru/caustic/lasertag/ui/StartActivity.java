package ru.caustic.lasertag.ui;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Handler;
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
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import ru.caustic.lasertag.core.CausticController;

public class StartActivity extends AppCompatActivity {

    private static final String TAG = "CC.CausticMain";

    private LinearLayout selectDeviceLayout;
    private LinearLayout connectingNowLayout;
    private LinearLayout connectionEstablishedLayout;
    private Switch switchRememberDevice;
    private Button buttonStartMainControls;

    private TextView textViewConnectedTo;

    private Handler uiHandler = null;
    private static boolean isFirstRun = true; // If we returned to this activity, we should not atomatically run MainControlsActivity
    private boolean needAutoRunMainControlsActivity = false;

    static final private int CHOOSE_BT_DEVICE = 0;

    static final int MSG_UPDATE_BLUETOOTH_UI = 1;

    public StartActivity() {
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
        setContentView(R.layout.activity_start);
        // @todo Refactor without signleton usage
        CausticController.getInstance().systemInit();
        Log.d(TAG, "Starting main activity");

        selectDeviceLayout = (LinearLayout) findViewById(R.id.selectDeviceLayout);
        connectingNowLayout = (LinearLayout) findViewById(R.id.connectingNowLayout);
        connectionEstablishedLayout = (LinearLayout) findViewById(R.id.connectionEstablishedLayout);
        textViewConnectedTo = (TextView) findViewById(R.id.textViewConnectedTo);

        switchRememberDevice = (Switch) findViewById(R.id.switchRememberDevice);

        buttonStartMainControls = (Button) findViewById(R.id.buttonStartMainControls);

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

        // Reading configuration
        SharedPreferences sharedPref = getPreferences(MODE_PRIVATE);
        String bridgeBluetoothAddress = sharedPref.getString("bridge_bluetooth_address", "");
        String bridgeBluetoothName = sharedPref.getString("bridge_bluetooth_name", "");
        boolean autoConnect = sharedPref.getBoolean("bridge_bluetooth_autoconnect", false);

        switchRememberDevice.setChecked(autoConnect);

        if (autoConnect && isFirstRun && !BluetoothManager.getInstance().isConnected()) {
            needAutoRunMainControlsActivity = true;
            isFirstRun = false;
            connectToBluetoothDevice(bridgeBluetoothAddress, bridgeBluetoothName);
        }

        //infoTextView.setText(BluetoothManager.getInstance().getDeviceName());
    }

    @Override
    protected void onPause() {
        super.onPause();
        SharedPreferences sharedPref = getPreferences(MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.putBoolean("bridge_bluetooth_autoconnect", switchRememberDevice.isChecked());
        editor.commit();
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

                String device = data.getStringExtra(BluetoothDevicesListActivity.BT_DEVICE);
                String mac = device.split("\\n")[1];

                connectToBluetoothDevice(mac, device);
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
                buttonStartMainControls.setEnabled(false);
                break;
            case BluetoothManager.BT_ESTABLISHING:
                selectDeviceLayout.setVisibility(View.GONE);
                connectionEstablishedLayout.setVisibility(View.GONE);
                connectingNowLayout.setVisibility(View.VISIBLE);
                buttonStartMainControls.setEnabled(false);
                break;
            case BluetoothManager.BT_CONNECTED:
                selectDeviceLayout.setVisibility(View.GONE);
                connectingNowLayout.setVisibility(View.GONE);
                connectionEstablishedLayout.setVisibility(View.VISIBLE);
                textViewConnectedTo.setText(BluetoothManager.getInstance().getDeviceName());
                buttonStartMainControls.setEnabled(true);
                if (needAutoRunMainControlsActivity) {
                    needAutoRunMainControlsActivity = false;
                    runMainControlsActivity();
                }
                break;
        }

    }

    private void connectToBluetoothDevice(final String mac, final String deviceName) {
        selectDeviceLayout.setVisibility(View.GONE);
        connectingNowLayout.setVisibility(View.VISIBLE);
        BluetoothManager.ConnectionDoneListener connectionDoneListener = new BluetoothManager.ConnectionDoneListener() {
            @Override
            public void onConnectionDone(boolean result) {
                runOnUiThread(new Runnable() {
                    public void run() {
                        updateBluetoothStatusUI();
                    }
                });

                if (result) {
                    runOnUiThread(new Runnable() {
                        public void run() {
                            SharedPreferences sharedPref = getPreferences(MODE_PRIVATE);
                            SharedPreferences.Editor editor = sharedPref.edit();

                            // We have successful connection
                            if (switchRememberDevice.isChecked())
                            {
                                // We can use it as default, so reconnecting
                                editor.putString("bridge_bluetooth_address", mac);
                                editor.putString("bridge_bluetooth_name", deviceName);
                                editor.putBoolean("bridge_bluetooth_autoconnect", true);

                            } else {
                                editor.putBoolean("bridge_bluetooth_autoconnect", false);
                            }
                            editor.commit();
//                            buttonScanClick(null);
                        }
                    });
                }
            }
        };
        BluetoothManager.getInstance().connect(mac, deviceName, connectionDoneListener);
    }

    private void errorExit(String title, String message){
        Log.d(TAG, "Exiting with error: " + message);
        Toast.makeText(getBaseContext(), title + " - " + message, Toast.LENGTH_LONG).show();
        finish();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    public void buttonSelectBridgeClick(View view) {
        Intent intent = new Intent (StartActivity.this, BluetoothDevicesListActivity.class);
        startActivityForResult(intent, CHOOSE_BT_DEVICE);
    }

    public void buttonDisconnectClick(View view) {
        BluetoothManager.getInstance().disconnect();
        updateBluetoothStatusUI();
    }

    public void buttonStartMainControlsClick(View view) {
        runMainControlsActivity();
    }

    private void runMainControlsActivity()
    {
        Intent intent = new Intent (StartActivity.this, MainControlsActivity.class);
        //Intent intent = new Intent (StartActivity.this, CausticDeviceSettingActivity.class);
        startActivity(intent);
    }
}
