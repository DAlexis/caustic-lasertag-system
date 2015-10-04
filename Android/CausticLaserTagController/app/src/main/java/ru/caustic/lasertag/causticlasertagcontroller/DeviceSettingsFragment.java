package ru.caustic.lasertag.causticlasertagcontroller;

import android.content.Context;
import android.os.Bundle;
import android.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;


public class DeviceSettingsFragment extends Fragment {

    public static SettingsEditorContext editorContext = new SettingsEditorContext();


    public static class ParametersListElementBase {
        TextView parameterName;
        TextView parameterValue;
        ParameterEntry parameterEntry;

        public void init(View convertView, ParameterEntry parameterEntry) {
            this.parameterEntry = parameterEntry;
            parameterName = (TextView) convertView.findViewById(R.id.parameterName);
            parameterValue = (TextView) convertView.findViewById(R.id.parameterValue);
            parameterName.setText(parameterEntry.description.getName());
            parameterValue.setText(parameterEntry.getValue());
        }
    }

    public static class ParameterEntry {
        SettingsEditorContext context;
        RCSProtocol.ParameterDescription description;
        // If all the devices has the same parameters value, it should be shown. Otherwise "Different" will be shown
        boolean hasInitialValue = false;
        boolean wasChanged = false;
        final String differentValueStr = "Different";
        String currentValue = differentValueStr;

        private ParametersListElementBase uiListElement = null;

        public String getValue() {
            return currentValue;
        }

        public ParameterEntry(SettingsEditorContext context, RCSProtocol.ParameterDescription description) {
            this.context = context;
            this.description = description;
        }

        /**
         * Interrogate all devices included to current SettingsEditorContext abd determine if they
         * have the same or different value
         */
        public void init() {
            boolean valueInitialized = false;
            hasInitialValue = true; // We suppose that all values are the same
            for (BridgeConnector.DeviceAddress addr : context.devices) {
                String thisDeviceValue
                        = CausticDevicesManager.getInstance().devices2
                        .get(addr).parameters
                        .get(description.getId()).getValue();
                if (!valueInitialized) {
                    currentValue = thisDeviceValue;
                    valueInitialized = true;
                }

                if (hasInitialValue && !currentValue.equals(thisDeviceValue)) {
                    hasInitialValue = false;
                    currentValue = differentValueStr;
                    break;
                }
            }
            wasChanged = false;
        }

        public void initUiListElement(View convertView) {
            // Here will be parameter type specification code
            uiListElement = new ParametersListElementBase();
            uiListElement.init(convertView, this);
        }

        public ParametersListElementBase getUiListElement() {
            return uiListElement;
        }
    }

    public static class SettingsEditorContext {
        public Set<BridgeConnector.DeviceAddress> devices = new HashSet<>();
        public ArrayList<ParameterEntry> parameters = new ArrayList<>();

        public void clear() {
            devices.clear();
        }

        public void add(BridgeConnector.DeviceAddress addr) {
            devices.add(addr);
        }

        public void createEntries() {
            /// @todo add check that all devices are homogeneous
            if (devices.isEmpty())
                return;

            parameters.clear();

            BridgeConnector.DeviceAddress someAddress = null;
            for (BridgeConnector.DeviceAddress addr : devices) {
                someAddress = addr;
                break;
            }

            CausticDevicesManager.CausticDevice2 dev = CausticDevicesManager.getInstance().devices2.get(someAddress);

            for (Map.Entry<Integer, RCSProtocol.AnyParameterSerializer> par : dev.parameters.entrySet()) {
                RCSProtocol.ParameterDescription descr = par.getValue().getDescription();

                ParameterEntry newParameter = new ParameterEntry(this, descr);
                newParameter.init();

                /// @todo Combine this to containers and classes those contain
                parameters.add(newParameter);
            }
        }
    }

    private ParametersListAdapter mAdapter;
    ListView parsList;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_device_settings, container, false);
        parsList = (ListView) view.findViewById(R.id.paramtersList);
        // Inflate the layout for this fragment
        editorContext.createEntries();
        mAdapter = new ParametersListAdapter();

        parsList.setAdapter(mAdapter);
        mAdapter.notifyDataSetChanged();
        return view;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }

    private class ParametersListAdapter extends BaseAdapter {

        private LayoutInflater mInflater;

        public ParametersListAdapter() {
            mInflater = (LayoutInflater) getActivity().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        @Override
        public int getCount() {
            return editorContext.parameters.size();
        }

        @Override
        public String getItem(int position) {
            return editorContext.parameters.get(position).description.getName();
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            ParameterEntry entry = editorContext.parameters.get(position);
            if (convertView == null) {
                // We are creating view now
                convertView = mInflater.inflate(R.layout.parameters_list_base_item, null);
                // Initializing holder
                convertView.setTag(entry.getUiListElement());
            }
            entry.initUiListElement(convertView);

            return convertView;
        }

    }
}
