package org.ltcaustic.gamecontroller;

import android.app.Activity;
import android.content.Context;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ListView;

import org.ltcaustic.rcspandroid.CausticInitializer;
import org.ltcaustic.rcspcore.CausticDevice;
import org.ltcaustic.rcspcore.DevicesManager;

import java.util.ArrayList;
import java.util.List;


/**
 * A simple {@link Fragment} subclass.
 * create an instance of this fragment.
 */
public class FragmentConfigureGameDevicesList extends Fragment {

    private int mode = FragmentConfigureGameDevices.CONFIG_RIFLES;
    DevicesListAdapter adapter = null;
    ListView listViewDevicesToEdit = null;

    public FragmentConfigureGameDevicesList() {
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
        View v = inflater.inflate(R.layout.fragment_configure_game_devices_list, container, false);
        listViewDevicesToEdit = v.findViewById(R.id.listViewDevicesToEdit);
        adapter = new DevicesListAdapter(CausticInitializer.getInstance().controller().getDevicesManager(), getActivity());
        listViewDevicesToEdit.setAdapter(adapter);
        return v;
    }

    public class DevicesListAdapter extends BaseAdapter {
        private LayoutInflater inflater;
        private List<FragSetConfigureGameDevices.UIDevicesListElement> uiDevices = new ArrayList<>();

        public DevicesListAdapter(DevicesManager mgr, Activity activity) {
            inflater = (LayoutInflater) activity.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            updateUIList(mgr);

        }

        public void updateUIList(DevicesManager mgr) {
            for (CausticDevice dev: mgr.devices.values()) {
                if (
                        (mode == FragmentConfigureGameDevices.CONFIG_RIFLES && dev.isRifle())
                                || (mode == FragmentConfigureGameDevices.CONFIG_PLAYERS && dev.isHeadSensor())
                        ) {
                    uiDevices.add(new FragSetConfigureGameDevices.UIDevicesListElement(inflater, dev));
                }
            }
        }

        @Override
        public int getCount() {
            return uiDevices.size();
        }

        @Override
        public String getItem(int position) {
            return uiDevices.get(position).dev.getName();
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            return uiDevices.get(position).convertView;

/*            Log.i("getView", "position: "+ position+ " convertView: "+ convertView);

            SettingsEditorContext.ParameterEntry entry = devices.get(position);
            UIListElement uiListElement = (UIListElement) entry.getUIDataPicker();

            if (uiListElement.convertView == null) {
                // If real ui list element not initialized, initialize it
                //entry.initUiListElement(inflater);
                uiListElement.init(inflater, entry);

                uiListElement.update();
                //convertView.setTag(entry.getUiListElement());
            }

            uiListElement.update();

            return uiListElement.convertView;
*/

        }

    }
}
