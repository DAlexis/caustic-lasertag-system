package ru.caustic.lasertag.causticlasertagcontroller;

import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class CausticDevicesList extends AppCompatActivity {

    ListView devicesList = null;

    List<String> devicesArrayList = new ArrayList<String>();
    ArrayAdapter<String> arrayAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_caustic_devices_list);
/*
        if (!BluetoothManager.getInstance().isConnected()) {
            finish();
        }

        devicesList = (ListView) findViewById(R.id.causticDevsList);

        arrayAdapter = new ArrayAdapter<>(
                this,
                android.R.layout.simple_list_item_1,
                devicesArrayList
        );
        devicesList.setAdapter(arrayAdapter);
        arrayAdapter.notifyDataSetChanged();*/
    }

    @Override
    protected void onResume() {
        super.onResume();
/*
        CausticDevicesManager.getInstance().updateDevicesList(
                new Handler() {
                    public void handleMessage(android.os.Message msg) {
                        int qq = 0;
                        switch (msg.what) {
                            case CausticDevicesManager.DEVICES_LIST_UPDATED:
                                Map<BridgeConnector.DeviceAddress, CausticDevicesManager.CausticDevice> devs =
                                        (Map<BridgeConnector.DeviceAddress, CausticDevicesManager.CausticDevice>) msg.obj;

                                devicesArrayList.clear();

                                for (Map.Entry<BridgeConnector.DeviceAddress, CausticDevicesManager.CausticDevice> entry : devs.entrySet())
                                {
                                    devicesArrayList.add(entry.getValue().getName() + "\nAddress: " + entry.getKey().toString());
                                }

                                arrayAdapter.notifyDataSetChanged();
                                break;
                        }
                    }
                });
*/
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_caustic_devices_list, menu);
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
}
