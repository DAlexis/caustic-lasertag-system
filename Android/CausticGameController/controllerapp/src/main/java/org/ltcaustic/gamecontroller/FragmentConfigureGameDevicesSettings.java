package org.ltcaustic.gamecontroller;


import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;


/**
 * A simple {@link Fragment} subclass.
 */
public class FragmentConfigureGameDevicesSettings extends Fragment {

    private int mode = FragmentConfigureGameDevices.CONFIG_RIFLES;

    public FragmentConfigureGameDevicesSettings() {
        // Required empty public constructor
    }

    /**
     * Use fragment to configure rifles or players
     * @param mode
     */
    public void setMode(int mode) {
        this.mode = mode;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_configure_game_devices_settings, container, false);
    }

}
