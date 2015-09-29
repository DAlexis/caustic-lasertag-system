package ru.caustic.lasertag.causticlasertagcontroller;

import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Map;
import java.util.TreeMap;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "CC.CausticMain";

    private Button connectButton;
    private TextView infoTextView;

    private Intent runServiceIntent;

    boolean bound = false;
    ServiceConnection sConn;

    static final private int CHOOSE_BT_DEVICE = 0;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.d(TAG, "Starting main activity");

        infoTextView = (TextView) findViewById(R.id.selectedDeviceText);

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

        sConn = new ServiceConnection() {
            public void onServiceConnected(ComponentName name, IBinder binder) {
                Log.d(TAG, "MainActivity onServiceConnected");
                bound = true;
            }

            public void onServiceDisconnected(ComponentName name) {
                Log.d(TAG, "MainActivity onServiceDisconnected");
                bound = false;
            }
        };

        infoTextView.setText(BluetoothManager.getInstance().getDeviceName());

        // This is not used yet
        //runServiceIntent = new Intent("ru.caustic.lasertag.causticlasertagcontroller.CausticConnectorService");
        runServiceIntent = new Intent(MainActivity.this, CausticConnectorService.class);
        startService(runServiceIntent);
        bindService(runServiceIntent, sConn, BIND_AUTO_CREATE);
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
                infoTextView.setText("Connecting...");
                //connectButton.setClickable(true);

                String device = data.getStringExtra(BluetoothDevicesList.BT_DEVICE);
                String mac = device.split("\\n")[1];

                if (!BluetoothManager.getInstance().connect(mac, device)) {
                    Toast.makeText(getBaseContext(), "Cannot connect to device!", Toast.LENGTH_LONG).show();
                }

                infoTextView.setText(BluetoothManager.getInstance().getDeviceName());
            } else {
                infoTextView.setText("");
            }
        }
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

    public void disconnectClick(View view) {
        BluetoothManager.getInstance().disconnect();
        infoTextView.setText(BluetoothManager.getInstance().getDeviceName());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopServiceClick(null);
    }

    public void stopServiceClick(View view) {
        if (bound) {
            unbindService(sConn);
            bound = false;
        }
        stopService(runServiceIntent);
    }

    public void respawnClick(View view) {
        CausticDevicesManager.getInstance().remoteCall(
                BridgeConnector.Broadcasts.headSensors,
                CausticDevicesManager.headSensor,
                RCSProtocol.RCSPOperationCodes.HeadSensor.Functions.playerRespawn,
                ""
        );
    }

    public void respRedClick(View view) {
        CausticDevicesManager.getInstance().remoteCall(
                BridgeConnector.Broadcasts.headSensorsRed,
                CausticDevicesManager.headSensor,
                RCSProtocol.RCSPOperationCodes.HeadSensor.Functions.playerRespawn,
                ""
        );
    }

    public void respBlueClick(View view) {
        CausticDevicesManager.getInstance().remoteCall(
                BridgeConnector.Broadcasts.headSensorsBlue,
                CausticDevicesManager.headSensor,
                RCSProtocol.RCSPOperationCodes.HeadSensor.Functions.playerRespawn,
                ""
        );
    }

    public void causticDevsListClicked(View view) {
        Intent intent = new Intent (MainActivity.this, CausticDevicesList.class);
        startActivityForResult(intent, CHOOSE_BT_DEVICE);
    }


}
