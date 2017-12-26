package org.ltcaustic.gamecontroller;

import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;


/**
 * A simple {@link Fragment} subclass.
 * Activities that contain this fragment must implement the
 * to handle interaction events.
 * create an instance of this fragment.
 */
public class FragmentConfigureGameDevices extends Fragment {

    public static final int CONFIG_RIFLES = 1;
    public static final int CONFIG_PLAYERS = 2;

    private int mode = CONFIG_RIFLES;

    FragmentConfigureGameDevicesList fragmentConfigureGameDevicesList;
    FragmentConfigureGameDevicesSettings fragmentConfigureGameDevicesSettings;

    public FragmentConfigureGameDevices() {
        // Required empty public constructor
    }

    /**
     * Use fragment to configure rifles or players
     * @param mode
     */
    public void setMode(int mode) {
        /*
        this.mode = mode;
        if (fragmentConfigureGameDevicesList != null)
            fragmentConfigureGameDevicesList.setMode(mode);
        if (fragmentConfigureGameDevicesSettings != null)
            fragmentConfigureGameDevicesSettings.setMode(mode);*/
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        /*
        if (fragmentConfigureGameDevicesList == null) {
            fragmentConfigureGameDevicesList = new FragmentConfigureGameDevicesList();
            fragmentConfigureGameDevicesList.setMode(mode);
        }

        if (fragmentConfigureGameDevicesSettings == null) {
            fragmentConfigureGameDevicesSettings = new FragmentConfigureGameDevicesSettings();
            fragmentConfigureGameDevicesSettings.setMode(mode);
        }*/
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View v = inflater.inflate(R.layout.fragment_configure_game_devices, container, false);
        return v;
    }


    @Override
    public void onDetach() {
        super.onDetach();
    }

}
