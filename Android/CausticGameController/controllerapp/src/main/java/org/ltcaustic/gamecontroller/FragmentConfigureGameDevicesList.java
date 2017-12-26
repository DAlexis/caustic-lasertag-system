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
import android.widget.CheckBox;
import android.widget.ListView;
import android.widget.TextView;

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
    TextView textViewSelectDevice = null;

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
        textViewSelectDevice = v.findViewById(R.id.textViewSelectDevice);
        if (mode == FragmentConfigureGameDevices.CONFIG_RIFLES) {
            textViewSelectDevice.setText("Select rifle to configure:");
        } else {
            textViewSelectDevice.setText("Select player to configure:");
        }

        adapter = new DevicesListAdapter(CausticInitializer.getInstance().controller().getDevicesManager(), getActivity());
        listViewDevicesToEdit.setAdapter(adapter);
        return v;
    }

    public static class UIDevicesListElement {
        public View convertView = null;
        CheckBox checkBoxDeviceName = null;
        TextView textViewDeviceComment = null;
        public CausticDevice dev = null;

        UIDevicesListElement(LayoutInflater inflater, CausticDevice dev) {
            convertView = inflater.inflate(org.ltcaustic.rcspandroid.R.layout.devices_list_selectable_item, null);
            this.dev = dev;
            checkBoxDeviceName = convertView.findViewById(R.id.checkBoxDeviceName);
            textViewDeviceComment = convertView.findViewById(R.id.textViewDeviceComment);
            update();
        }

        void update() {
            checkBoxDeviceName.setText(dev.getName());
            textViewDeviceComment.setText(dev.address.toString());
        }

        public boolean isSelected() {
            return checkBoxDeviceName.isSelected();
        }
    }

    public class DevicesListAdapter extends BaseAdapter {
        private LayoutInflater inflater;
        private List<UIDevicesListElement> uiDevices = new ArrayList<>();

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
                    uiDevices.add(new UIDevicesListElement(inflater, dev));
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
            uiDevices.get(position).update();
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
