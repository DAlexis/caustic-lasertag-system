package ru.caustic.lasertag.causticlasertagcontroller;

import android.app.Activity;
import android.content.Context;
import android.net.Uri;
import android.os.Bundle;
import android.app.Fragment;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Map;
import java.util.TreeMap;


public class CausticDevicesListFragment extends Fragment {

    private DevicesListAdapter adapter;
    private ListView devicesList;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_caustic_devices_list, container, false);

        devicesList = (ListView) view.findViewById(R.id.listViewDevices);
        adapter = new DevicesListAdapter();
        devicesList.setAdapter(adapter);

        CausticDevicesManager.getInstance().updateDevicesList2(
                new Handler() {
                    public void handleMessage(android.os.Message msg) {
                        switch (msg.what) {
                            case CausticDevicesManager.DEVICES_LIST_UPDATED:
                                Map<BridgeConnector.DeviceAddress, CausticDevicesManager.CausticDevice2> devs =
                                        (Map<BridgeConnector.DeviceAddress, CausticDevicesManager.CausticDevice2>) msg.obj;

                                adapter.clear();

                                for (Map.Entry<BridgeConnector.DeviceAddress, CausticDevicesManager.CausticDevice2> entry : devs.entrySet()) {
                                    adapter.addItem(new DevicesListElementHolder(entry.getValue()));
                                }

                                adapter.notifyDataSetChanged();
                                break;
                        }
                    }
                });

        return view;
    }

    private class DevicesListElementHolder {
        public CheckBox deviceName = null;
        public TextView deviceSummary = null;
        public CausticDevicesManager.CausticDevice2 device;

        DevicesListElementHolder(CausticDevicesManager.CausticDevice2 device) {
            this.device = device;
        }

        public void initView(View convertView) {
            deviceName = (CheckBox) convertView.findViewById(R.id.checkBoxDeviceSelected);
            deviceSummary = (TextView) convertView.findViewById(R.id.textViewDeviceSummary);
            deviceName.setText(device.getName());
            deviceSummary.setText(device.getType() + "\n" + device.address.toString());

            deviceName.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    checkAndAddToSettingsEditorContext();
                    Toast.makeText(getActivity(), deviceName.getText().toString(), Toast.LENGTH_SHORT).show();
                }
            });
        }

        public void checkAndAddToSettingsEditorContext() {
            if (deviceName.isChecked()) {
                DeviceSettingsFragment.editorContext.add(device.address);
            } else {
                DeviceSettingsFragment.editorContext.devices.remove(device.address);
            }
        }
    }


    private class DevicesListAdapter extends BaseAdapter {

        private ArrayList<DevicesListElementHolder> mHolders = new ArrayList();
        private LayoutInflater mInflater;

        public DevicesListAdapter() {
            mInflater = (LayoutInflater) getActivity().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        public void addItem(DevicesListElementHolder item) {
            mHolders.add(item);
            //notifyDataSetChanged();
        }

        public void clear() {
            mHolders.clear();
        }

        @Override
        public int getCount() {
            return mHolders.size();
        }

        @Override
        public String getItem(int position) {
            return null;
            //return mHolders.get(position).device.address.toString();
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            //System.out.println("getView " + position + " " + convertView);
            DevicesListElementHolder holder;
            if (convertView == null) {
                // We are creating view now
                convertView = mInflater.inflate(R.layout.devices_list_item, null);
                holder = mHolders.get(position);
                convertView.setTag(holder);
            } else {
                holder = (DevicesListElementHolder)convertView.getTag();
            }
            holder.initView(convertView);
            return convertView;
        }

    }
}
