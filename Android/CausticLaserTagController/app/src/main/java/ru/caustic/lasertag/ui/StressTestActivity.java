package ru.caustic.lasertag.ui;

import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import java.util.Set;

import ru.caustic.rcspcore.BridgeConnector;
import ru.caustic.rcspcore.CausticController;
import ru.caustic.rcspcore.DeviceUtils;
import ru.caustic.rcspcore.DevicesManager;

public class StressTestActivity extends AppCompatActivity {
    private static int DELAY=500;
    private static final String TAG = "StressTestActivity";
    private Handler testHandler = new Handler();
    DevicesManager devMan = CausticController.getInstance().getDevicesManager();
    private Runnable stressTest = new Runnable(){
        @Override
        public void run() {
            devsInArea=devMan.devices.size();
            devMan.setDeviceListUpdatedListener(devListener);
            devMan.updateDevicesList();
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    final TextView devs = (TextView) findViewById(R.id.devs_in_area);
                    if (devs!=null) {
                        devs.setText(Integer.toString(devsInArea));
                    }
                }
            });
            testHandler.postDelayed(this, DELAY);
        }
    };
    private static int devsInArea = 0;

    private DevicesManager.SynchronizationEndListener syncListener = new DevicesManager.SynchronizationEndListener() {
        @Override
        public void onSynchronizationEnd(boolean isSuccess, Set<BridgeConnector.DeviceAddress> notSynchronized) {
            Log.d(TAG, "Sync completed, result: " + isSuccess + ", not synchronized: " + notSynchronized.size());
            //List population occurs here.


        }
    };

    private DevicesManager.DeviceListUpdatedListener devListener = new DevicesManager.DeviceListUpdatedListener() {
        @Override
        public void onDevicesListUpdated() {
            Log.d(TAG, "Updated devices list");

            Log.d(TAG, "Starting parameters sync");
            devMan.asyncPopParametersFromDevices(syncListener, DeviceUtils.getHeadSensorsMap(devMan.devices).keySet());

        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_stress_test);
        SeekBar bar = (SeekBar) findViewById(R.id.seekBar);
        final TextView value = (TextView) findViewById(R.id.test_delay);
        final TextView devs = (TextView) findViewById(R.id.devs_in_area);

        value.setText(Integer.toString(DELAY));
        devs.setText(Integer.toString(devsInArea));
        bar.setProgress(DELAY);
        bar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress,
                                          boolean fromUser) {
                // TODO Auto-generated method stub
                value.setText(String.valueOf(progress));
                DELAY = progress;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    public void buttonStartStressTest(View view) {
        SeekBar bar = (SeekBar) findViewById(R.id.seekBar);
        DELAY = bar.getProgress();
        testHandler.post(stressTest);
    }

    public void buttonStopStressTest(View view) {
        SeekBar bar = (SeekBar) findViewById(R.id.seekBar);
        DELAY = bar.getProgress();
        testHandler.removeCallbacks(stressTest);
    }

}
