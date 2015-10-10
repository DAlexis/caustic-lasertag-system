package ru.caustic.lasertag.causticlasertagcontroller;

import android.content.Context;
import android.os.Bundle;
import android.app.Fragment;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;


public class DeviceSettingsFragment extends Fragment {
    private ParametersListAdapter mAdapter;
    ListView parsList;
    Button buttonApply;
    Button buttonCancel;

    public static SettingsEditorContext editorContext = new SettingsEditorContext();

    public static abstract class ParametersListElement {
        public View convertView = null;
        ParameterEntry parameterEntry = null;
        public abstract void init(LayoutInflater inflater, ParameterEntry parameterEntry);
        public abstract void update();
        public abstract void pickValue();
    }

    public static class ParametersListElementInteger extends ParametersListElement {
        TextView parameterName = null;
        TextView parameterSummary = null;
        SeekBar seekBar = null;
        EditText parameterValue = null;

        RCSProtocol.UintParameterDescription uintDescr = null;

        public boolean initialized() {
            return convertView != null;
        }

        public void init(LayoutInflater inflater, ParameterEntry parameterEntry) {
            this.parameterEntry = parameterEntry;

               this.convertView = inflater.inflate(R.layout.parameters_list_base_item, null);
                parameterName = (TextView) convertView.findViewById(R.id.parameterName);
                parameterSummary = (TextView) convertView.findViewById(R.id.parameterSummary);
                seekBar = (SeekBar) convertView.findViewById(R.id.seekBarParameterValue);
                parameterValue = (EditText) convertView.findViewById(R.id.editTextParameterValue);
        }

        public void update() {
            parameterName.setText(parameterEntry.description.getName());
            if (parameterEntry.hasInitialValue) {
                parameterValue.setText(parameterEntry.getValue().toString());
            } else {
                parameterValue.setText("");
            }
            parameterValue.addTextChangedListener(new TextWatcher() {
                @Override
                public void beforeTextChanged(CharSequence s, int start, int count, int after) {

                }

                @Override
                public void onTextChanged(CharSequence s, int start, int before, int count) {

                }

                @Override
                public void afterTextChanged(Editable s) {
                    pickValue();
                }
            });

            uintDescr = (RCSProtocol.UintParameterDescription) parameterEntry.description;
            // We have min and max values
            parameterSummary.setText("From " + uintDescr.minValue + " to " + uintDescr.maxValue);
            if (parameterEntry.hasInitialValue) {
                seekBar.setProgress(Integer.parseInt(parameterEntry.getValue()) - uintDescr.minValue);
            }
            seekBar.setMax(uintDescr.maxValue - uintDescr.minValue);
            seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    parameterValue.setText(Integer.toString(progress + uintDescr.minValue));
                    pickValue();
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) { }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) { }
            });
        }

        public void pickValue() {
            String text = parameterValue.getText().toString();
            parameterEntry.setValue(text);
        }
    }

    public static class ParametersListElementTimeInterval extends ParametersListElement {
        TextView parameterName = null;
        TextView parameterSummary = null;
        SeekBar seekBar = null;
        EditText parameterValue = null;

        RCSProtocol.TimeIntervalParameterDescription descr = null;

        public boolean initialized() {
            return convertView != null;
        }

        public void init(LayoutInflater inflater, ParameterEntry parameterEntry) {
            this.parameterEntry = parameterEntry;

               this.convertView = inflater.inflate(R.layout.parameters_list_base_item, null);
                parameterName = (TextView) convertView.findViewById(R.id.parameterName);
                parameterSummary = (TextView) convertView.findViewById(R.id.parameterSummary);
                seekBar = (SeekBar) convertView.findViewById(R.id.seekBarParameterValue);
                parameterValue = (EditText) convertView.findViewById(R.id.editTextParameterValue);
        }

        public void update() {
            parameterName.setText(parameterEntry.description.getName());
            if (parameterEntry.hasInitialValue) {
                long val = Long.parseLong(parameterEntry.getValue()) / 1000;
                parameterValue.setText(Long.toString(val));
            } else {
                parameterValue.setText("");
            }
            parameterValue.addTextChangedListener(new TextWatcher() {
                @Override
                public void beforeTextChanged(CharSequence s, int start, int count, int after) {

                }

                @Override
                public void onTextChanged(CharSequence s, int start, int before, int count) {

                }

                @Override
                public void afterTextChanged(Editable s) {
                    pickValue();
                }
            });

            descr = (RCSProtocol.TimeIntervalParameterDescription) parameterEntry.description;
            // We have min and max values
            parameterSummary.setText("From " + descr.minValue / 1000 + " to " + descr.maxValue / 1000);
            if (parameterEntry.hasInitialValue) {
                seekBar.setProgress((int) ((Long.parseLong(parameterEntry.getValue()) - descr.minValue) / 1000));
            }

            seekBar.setMax((int) ((descr.maxValue - descr.minValue)/1000) );
            seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    parameterValue.setText(Integer.toString(progress + ( (int) (descr.minValue/1000) )));
                    pickValue();
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) { }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) { }
            });
        }

        public void pickValue() {
            long value = Long.parseLong(parameterValue.getText().toString()) * 1000;
            parameterEntry.setValue(Long.toString(value));
        }
    }

    public static class ParametersListElementBoolean extends ParametersListElement {
        TextView parameterName = null;
        TextView parameterSummary = null;
        Switch boolSwitch = null;

        RCSProtocol.BooleanParameterDescription descr = null;

        public boolean initialized() {
            return convertView != null;
        }

        public void init(LayoutInflater inflater, ParameterEntry parameterEntry) {
            this.parameterEntry = parameterEntry;
            this.convertView = inflater.inflate(R.layout.parameters_list_boolean_item, null);
            parameterName = (TextView) convertView.findViewById(R.id.parameterName);
            parameterSummary = (TextView) convertView.findViewById(R.id.parameterSummary);
            boolSwitch = (Switch) convertView.findViewById(R.id.switchBooleanValue);
        }

        public void update() {
            parameterName.setText(parameterEntry.description.getName());
            parameterSummary.setText("");
            boolSwitch.setChecked(parameterEntry.hasInitialValue && parameterEntry.getValue().equals("true"));
        }

        public void pickValue() {
            parameterEntry.setValue(boolSwitch.isChecked() ? "true" : "false");
        }
    }

    public static class ParametersListElementUnsupported extends ParametersListElement {
        TextView parameterName = null;
        TextView parameterSummary = null;

        RCSProtocol.BooleanParameterDescription descr = null;

        public boolean initialized() {
            return convertView != null;
        }

        public void init(LayoutInflater inflater, ParameterEntry parameterEntry) {
            this.parameterEntry = parameterEntry;
            this.convertView = inflater.inflate(R.layout.parameters_list_unsupported_item, null);
            parameterName = (TextView) convertView.findViewById(R.id.parameterName);
            parameterSummary = (TextView) convertView.findViewById(R.id.parameterSummary);
        }

        public void update() {
            parameterName.setText(parameterEntry.description.getName());
        }

        public void pickValue() {
        }
    }

    private static ParametersListElement createListElement(RCSProtocol.ParameterDescription description) {
        if (description instanceof RCSProtocol.UintParameterDescription) {
            return new ParametersListElementInteger();
        } else if (description instanceof RCSProtocol.BooleanParameterDescription) {
            return new ParametersListElementBoolean();
        } else if (description instanceof RCSProtocol.TimeIntervalParameterDescription) {
            return new ParametersListElementTimeInterval();
        }
        return new ParametersListElementUnsupported();
    }

    public static class ParameterEntry {
        SettingsEditorContext context;
        RCSProtocol.ParameterDescription description;
        // If all the devices has the same parameters value, it should be shown. Otherwise "Different" will be shown
        boolean hasInitialValue = false;
        boolean wasChanged = false;
        final String differentValueStr = "Different";
        String currentValue = differentValueStr;

        private ParametersListElement uiListElement = null;//new ParametersListElementInteger();

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
            uiListElement = createListElement(description);
        }

        public void initUiListElement(LayoutInflater inflater) {
            // Here will be parameter type specification code
            uiListElement.init(inflater, this);
        }

        public ParametersListElement getUiListElement() {
            return uiListElement;
        }

        public void setValue(String newValue) {
            wasChanged = true;
            currentValue = newValue;
        }

        public void pushValue() {
            if (!wasChanged)
                return;

            for (BridgeConnector.DeviceAddress addr : context.devices) {
                RCSProtocol.AnyParameterSerializer par
                        = CausticDevicesManager.getInstance().devices2
                        .get(addr).parameters
                        .get(description.getId());
                par.setValue(currentValue);
            }
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

                if (!descr.isEditable())
                    continue;

                ParameterEntry newParameter = new ParameterEntry(this, descr);
                newParameter.init();

                /// @todo Combine this to containers and classes those contain
                parameters.add(newParameter);
            }
        }

        public void pushValues() {
            // Reading modified values
            for (ParameterEntry entry : parameters) {
                entry.uiListElement.pickValue();
                entry.pushValue();
            }
            // Sending values to devices
            for (BridgeConnector.DeviceAddress address : devices) {
                CausticDevicesManager.getInstance().devices2.get(address).pushToDevice();
            }
        }
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_device_settings, container, false);
        parsList = (ListView) view.findViewById(R.id.paramtersList);
        buttonApply = (Button) view.findViewById(R.id.buttonApplySettings);
        buttonCancel = (Button) view.findViewById(R.id.buttonCancelSettings);

        buttonApply.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                DeviceSettingsFragment.editorContext.pushValues();
            }
        });

        buttonCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getActivity().finish();
            }
        });

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

            Log.i("getView", "position: "+ position+ " convertView: "+ convertView);

            ParameterEntry entry = editorContext.parameters.get(position);

            if (entry.getUiListElement().convertView == null) {
                // If real ui list element not initialized, initialize it
                entry.initUiListElement(mInflater);
                entry.getUiListElement().update();
                //convertView.setTag(entry.getUiListElement());
            }

            entry.getUiListElement().update();

            return entry.getUiListElement().convertView;

/*
            if (convertView == null || entry.getUiListElement().convertView == null) {

                if (entry.getUiListElement().convertView == null) {
                    convertView = mInflater.inflate(R.layout.parameters_list_base_item, null);
                    entry.initUiListElement(convertView);
                } else {
                    convertView = entry.getUiListElement().convertView;
                }
                convertView.setTag(entry.getUiListElement());

            }
            ( (ParametersListElementBase) convertView.getTag() ).update();
            //entry.getUiListElement().update();
            return convertView;*/
        }

    }
}
