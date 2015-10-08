package ru.caustic.lasertag.causticlasertagcontroller;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

public class BluetoothDevicesList extends AppCompatActivity {

    public final static String BT_DEVICE = "ru.caustic.lasertag.causticlasertagcontroller.BT_DEVICE";

    // @todo Move this to another place
    public static final String APP_PREFERENCES = "settings";

    SharedPreferences sharedPreferences;
    ListView devicesList = null;
    CheckBox checkBoxAutoConnect;

    Set<BluetoothDevice> pairedDevicesSet = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_bluetooth_devices_list);



        devicesList = (ListView) findViewById(R.id.devicesList);
        checkBoxAutoConnect = (CheckBox) findViewById(R.id.checkBoxAutoConnect);

        sharedPreferences = getSharedPreferences(APP_PREFERENCES, Context.MODE_PRIVATE);

        boolean autoConnect = sharedPreferences.getBoolean("autoConnect", false);
        checkBoxAutoConnect.setChecked(autoConnect);

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
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_bluetooth_devices_list, menu);
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
    public void onResume() {
        pairedDevicesSet = BluetoothManager.getInstance().getPairedDevicesSet();

        List<String> your_array_list = new ArrayList<String>();


        // This is the array adapter, it takes the context of the activity as a
        // first parameter, the type of list view as a second parameter and your
        // array as a third parameter.
        ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>(
                this,
                android.R.layout.simple_list_item_1,
                your_array_list );

        devicesList.setAdapter(arrayAdapter);
        arrayAdapter.notifyDataSetChanged();


        for(BluetoothDevice device: pairedDevicesSet){
        //  Добавляем имена и адреса в mArrayAdapter, чтобы показать
        // через ListView
            your_array_list.add(device.getName()+"\n"+ device.getAddress());
        }
        super.onResume();

    }
}
