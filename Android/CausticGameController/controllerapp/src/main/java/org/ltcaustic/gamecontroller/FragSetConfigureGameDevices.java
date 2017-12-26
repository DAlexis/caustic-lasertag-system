package org.ltcaustic.gamecontroller;

import android.app.Activity;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.TextView;

import org.ltcaustic.rcspcore.CausticDevice;
import org.ltcaustic.rcspcore.DevicesManager;
import org.ltcaustic.rcspcore.SettingsEditorContext;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by dalexies on 24.12.17.
 */

public class FragSetConfigureGameDevices {

    public static final int CONFIG_RIFLES = 1;
    public static final int CONFIG_PLAYERS = 2;

    private int mode = CONFIG_RIFLES;

    public FragmentConfigureGameDevicesList fragmentConfigureGameDevicesList;
    public FragmentConfigureGameDevicesSettings fragmentConfigureGameDevicesSettings;

    FragSetConfigureGameDevices(int mode) {
        this.mode = mode;
        fragmentConfigureGameDevicesList = new FragmentConfigureGameDevicesList();
        fragmentConfigureGameDevicesList.setMode(mode);
        fragmentConfigureGameDevicesSettings = new FragmentConfigureGameDevicesSettings();
        fragmentConfigureGameDevicesSettings.setMode(mode);

    }
}
