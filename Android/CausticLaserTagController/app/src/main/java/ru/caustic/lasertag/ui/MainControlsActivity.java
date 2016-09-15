package ru.caustic.lasertag.ui;

import android.content.Intent;
import android.os.Bundle;
import android.app.Activity;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.util.Map;

import ru.caustic.lasertag.core.BridgeConnector;
import ru.caustic.lasertag.core.CausticDevicesManager;


public class MainControlsActivity extends AppCompatActivity {

    private static final String TAG = "CC.MainControls";
    private TextView textDevsInArea;
    private int devsInArea = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "Starting main controls activity");
        setContentView(R.layout.activity_main_controls);
        textDevsInArea = (TextView) findViewById(R.id.textDevsInArea);
    }

    public void buttonRescanClick(View view) {
        doScan();
    }

    public void buttonSettingsClick(View view) {
        Intent intent = new Intent (MainControlsActivity.this, DevicesListActivity.class);
        //Intent intent = new Intent (StartActivity.this, CausticDeviceSettingActivity.class);
        startActivity(intent);
    }

    private void doScan()
    {
        devsInArea = 0;
        updateDevsCountInAreaUI();
        CausticDevicesManager.getInstance().updateDevicesList(
                new Handler() {
                    public void handleMessage(android.os.Message msg) {
                        switch (msg.what) {
                            case CausticDevicesManager.DEVICES_LIST_UPDATED:
                                Map<BridgeConnector.DeviceAddress, CausticDevicesManager.CausticDevice> devs =
                                        (Map<BridgeConnector.DeviceAddress, CausticDevicesManager.CausticDevice>) msg.obj;

                                devsInArea = devs.size();

                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        updateDevsCountInAreaUI();
                                    }
                                });
                                break;
                        }
                    }
                }
        );
    }

    private void updateDevsCountInAreaUI() {
        textDevsInArea.setText(Integer.toString(devsInArea));
    }
}
