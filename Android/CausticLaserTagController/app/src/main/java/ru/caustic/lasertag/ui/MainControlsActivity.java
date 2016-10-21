package ru.caustic.lasertag.ui;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import java.util.Map;

import ru.caustic.lasertag.core.BridgeConnector;
import ru.caustic.lasertag.core.CausticController;
import ru.caustic.lasertag.core.DevicesManager;


public class MainControlsActivity extends AppCompatActivity {

    private static final String TAG = "CC.MainControls";
    private TextView textDevsInArea;
    private int devsInArea = 0;

    private DevicesManager devicesManager;
    public MainControlsActivity()
    {
        super();
        // @todo Refactor without signleton usage
        devicesManager = CausticController.getInstance().getDevicesManager();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "Starting main controls activity");
        setContentView(R.layout.activity_main_controls);
        textDevsInArea = (TextView) findViewById(R.id.textDevsInArea);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        doScan();
    }

    public void buttonRescanClick(View view) {
        doScan();
    }

    public void buttonSettingsClick(View view) {
        Intent intent = new Intent (MainControlsActivity.this, DevicesListActivity.class);
        //Intent intent = new Intent (StartActivity.this, CausticDeviceSettingActivity.class);
        startActivity(intent);
    }

    public void buttonOpenGameStatsClick(View v) {
        Intent intent = new Intent (MainControlsActivity.this, GameStatsActivity.class);
        startActivity(intent);
    }

    private void doScan()
    {
        devsInArea = 0;
        updateDevsCountInAreaUI();
        devicesManager.setDeviceListUpdatedListener(new DevicesManager.DeviceListUpdatedListener() {
            @Override
            public void onDevicesListUpdated() {
                devsInArea = devicesManager.devices.size();

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        updateDevsCountInAreaUI();
                    }
                });
            }
        });
        devicesManager.updateDevicesList();
    }

    private void updateDevsCountInAreaUI() {
        textDevsInArea.setText(Integer.toString(devsInArea));
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main_controls, menu);
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

        switch (item.getItemId()) {
            case android.R.id.home:
                finish();
                return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
