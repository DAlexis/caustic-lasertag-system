package org.ltcaustic.rcspandroid;

import android.bluetooth.BluetoothDevice;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CompoundButton;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import static android.content.Context.MODE_PRIVATE;


/**
 * A simple {@link Fragment} subclass.
 * Activities that contain this fragment must implement the
 * {@link FragmentSelectBluetoothDevice.OnFragmentInteractionListener} interface
 * to handle interaction events.
 */
public class FragmentSelectBluetoothDevice extends Fragment {

    //private final String TAG = "fragment_select_bt_device";


    ListView devicesList = null;
    Switch switchAutoConnect = null;
    SharedPreferences sharedPref = null;
    String defaultDeviceAddress = "";
    String lastSelectedAddress = "";

    Set<BluetoothDevice> pairedDevicesSet = null;
    BluetoothManager.ConnectionProcessListener connectionProcessListener = null;
    BtConnectionListener ourConnectionListener = new BtConnectionListener();

    // Is this fragment now on screen
    boolean isActive = false;

    private OnFragmentInteractionListener mListener;

    /**
     * This class listen {@link BluetoothManager}'s connection process,
     * redraw list and forward calls to {@link BluetoothManager.ConnectionProcessListener}
     * given by main activity
     */
    private class BtConnectionListener implements BluetoothManager.ConnectionProcessListener
    {
        @Override
        public void onConnecting(String deviceName) {
            if (connectionProcessListener != null)
                connectionProcessListener.onConnecting(deviceName);
        }

        @Override
        public void onConnectionDone(boolean result) {
            if (isActive) {
                getActivity().runOnUiThread(new Runnable() {
                    public void run() {
                        redrawDevicesList();
                    }
                });
            }
            if (connectionProcessListener != null)
                connectionProcessListener.onConnectionDone(result);

        }
    }

    public FragmentSelectBluetoothDevice() {
        // Required empty public constructor
    }

    void initSwitch()
    {
        boolean autoconnect = sharedPref.getBoolean(BluetoothManager.PREFERENCE_AUTOCONNECT, false);
        switchAutoConnect.setChecked(autoconnect);
        switchAutoConnect.setOnCheckedChangeListener(new Switch.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                SharedPreferences.Editor editor = sharedPref.edit();
                editor.putBoolean(BluetoothManager.PREFERENCE_AUTOCONNECT, b);
                editor.commit();
            }
        });
    }

    void readDefaultDevice()
    {
        defaultDeviceAddress = sharedPref.getString(BluetoothManager.PREFERENCE_DEFAULT_ADDRESS, defaultDeviceAddress);
    }

    void saveDefaultDevice(String address)
    {
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.putString(BluetoothManager.PREFERENCE_DEFAULT_ADDRESS, address);
        editor.commit();
    }



    public void setConnectionProcessListener(BluetoothManager.ConnectionProcessListener listener)
    {
        connectionProcessListener = listener;
    }

    private void redrawDevicesList()
    {
        if (!isActive)
            return;

        devicesList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View itemClicked, int position, long id) {
                String device = (String) ((TextView) itemClicked).getText();
                String mac = device.split("\\n")[1];
                saveDefaultDevice(mac);
                CausticInitializer.getInstance().bluetooth().cancel();
                CausticInitializer.getInstance().bluetooth().connect(mac, device);
            }
        });

        pairedDevicesSet = CausticInitializer.getInstance().bluetooth().getPairedDevicesSet();

        List<String> availableBluetoothDevices = new ArrayList<String>();

        ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>(
                getActivity(),
                android.R.layout.simple_list_item_1,
                availableBluetoothDevices
        );

        devicesList.setAdapter(arrayAdapter);
        arrayAdapter.notifyDataSetChanged();

        for(BluetoothDevice device: pairedDevicesSet)
        {
            if (device.getAddress().equals(CausticInitializer.getInstance().bluetooth().getConnectedAddress())) {
                availableBluetoothDevices.add(device.getName() + " (connected)\n" + device.getAddress());
            } else if (device.getAddress().equals(defaultDeviceAddress)) {
                availableBluetoothDevices.add(device.getName() + " (last used)\n" + device.getAddress());
            } else {
                availableBluetoothDevices.add(device.getName() + "\n" + device.getAddress());
            }
        }
        //doAutoconnectIfNeeded();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            /*
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);*/
        }

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        isActive = true;
        // Inflate the layout for this fragment
        View v = inflater.inflate(R.layout.fragment_select_bluetooth_device, container, false);
        devicesList = (ListView) v.findViewById(R.id.bluetoothDevicesList);
        switchAutoConnect = (Switch) v.findViewById(R.id.switchAutoConnect);

        CausticInitializer.getInstance().bluetooth().setConnectionProcessListener(ourConnectionListener);

        sharedPref = getActivity().getPreferences(MODE_PRIVATE);

        readDefaultDevice();
        initSwitch();
        redrawDevicesList();

        return v;
    }

    @Override
    public void onPause() {
        isActive = false;
        super.onPause();
    }

    /*
        @Override
        public void onAttach(Context context) {
            super.onAttach(context);
            if (context instanceof OnFragmentInteractionListener) {
                mListener = (OnFragmentInteractionListener) context;
            } else {
                throw new RuntimeException(context.toString()
                        + " must implement OnFragmentInteractionListener");
            }
        }
    */
    @Override
    public void onDetach() {
        super.onDetach();
        mListener = null;
    }

    /**
     * This interface must be implemented by activities that contain this
     * fragment to allow an interaction in this fragment to be communicated
     * to the activity and potentially other fragments contained in that
     * activity.
     * <p>
     * See the Android Training lesson <a href=
     * "http://developer.android.com/training/basics/fragments/communicating.html"
     * >Communicating with Other Fragments</a> for more information.
     */
    public interface OnFragmentInteractionListener {
        // TODO: Update argument type and name
        void onFragmentInteraction(Uri uri);
    }
}
