package ru.caustic.lasertagclientapp;

import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import com.caustic.androidshared.BluetoothManager;

public class BluetoothDevicesActivity extends AppCompatActivity {

    public final static String BT_DEVICE = "ru.caustic.lasertag.BT_DEVICE";

    ListView devicesList = null;

    Set<BluetoothDevice> pairedDevicesSet = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth_devices);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null){
            actionBar.setDisplayHomeAsUpEnabled(true);
        }

        devicesList = (ListView) findViewById(R.id.devicesList);
        devicesList .setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View itemClicked, int position, long id) {
                Intent answerIntent = new Intent();
                answerIntent.putExtra(BT_DEVICE, ((TextView) itemClicked).getText());
                setResult(RESULT_OK, answerIntent);
                finish();
            }
        });


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
    public void onResume() {
        pairedDevicesSet = BluetoothManager.getInstance().getPairedDevicesSet();

        List<String> availableBluetoothDevices = new ArrayList<String>();


        ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>(
                this,
                android.R.layout.simple_list_item_1,
                availableBluetoothDevices );

        devicesList.setAdapter(arrayAdapter);
        arrayAdapter.notifyDataSetChanged();


        for(BluetoothDevice device: pairedDevicesSet){

            availableBluetoothDevices.add(device.getName()+"\n"+ device.getAddress());
        }

        super.onResume();
    }
}

