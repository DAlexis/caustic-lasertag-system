package org.ltcaustic.gamecontroller;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import org.ltcaustic.rcspandroid.CausticInitializer;
import org.ltcaustic.rcspandroid.DeviceSettingsList;
import org.ltcaustic.rcspcore.CausticDevice;
import org.ltcaustic.rcspcore.DevicesManager;
import org.ltcaustic.rcspcore.RCSP;
import org.ltcaustic.rcspcore.SettingsEditorContext;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;


/**
 * A simple {@link Fragment} subclass.
 * create an instance of this fragment.
 */
public class FragmentConfigureGameDevicesList extends Fragment {

    public static final int CONFIG_RIFLES = 1;
    public static final int CONFIG_PLAYERS = 2;


    private int mode = CONFIG_RIFLES;
    DevicesListAdapter devicesListAdapter = null;
    DeviceSettingsList.ParametersListAdapter parametersListAdapter = null;

    View layoutSelectDevice = null;
    ListView listViewDevicesToEdit = null;
    TextView textViewSelectDevice = null;
    TextView textViewSelectedDevices = null;
    Button buttonDoConfigure = null;
    Button buttonUpdateDevicesList = null;
    Spinner spinnerSelectAction = null;
    Button buttonApplyAction = null;

    View layoutParameters = null;
    ListView listViewParameters = null;
    Button buttonParametersOk = null;
    Button buttonParametersCancel = null;

    View layoutParametersLoading = null;

    SettingsEditorContext editorContext = null;
    DevicesManager devicesManager = null;

    ParametersListUpdater parametersListUpdater = new ParametersListUpdater();

    boolean isActive = false;

    public FragmentConfigureGameDevicesList() {
        // Required empty public constructor
    }

    public static FragmentConfigureGameDevicesList create(int mode) {
        FragmentConfigureGameDevicesList f = new FragmentConfigureGameDevicesList();
        f.setMode(mode);
        return f;
    }

    /**
     * Use fragment to configure rifles or players
     * @param mode
     */
    public void setMode(int mode) {
        this.mode = mode;
    }

    @Override
    public void onResume() {
        super.onResume();
        isActive = true;
    }

    @Override
    public void onPause() {
        super.onPause();
        isActive = false;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        editorContext = CausticInitializer.getInstance().controller().getSettingsEditorContext();
        devicesManager = CausticInitializer.getInstance().controller().getDevicesManager();
        // Inflate the layout for this fragment
        View v = inflater.inflate(R.layout.fragment_configure_game_devices_list, container, false);
        layoutSelectDevice = v.findViewById(R.id.layoutSelectDevice);
        listViewDevicesToEdit = v.findViewById(R.id.listViewDevicesToEdit);
        textViewSelectDevice = v.findViewById(R.id.textViewSelectDevice);
        textViewSelectedDevices = v.findViewById(R.id.textViewSelectedDevices);
        buttonDoConfigure = v.findViewById(R.id.buttonDoConfigure);
        buttonUpdateDevicesList = v.findViewById(R.id.buttonUpdateDevicesList);
        spinnerSelectAction = v.findViewById(R.id.spinnerSelectAction);
        buttonApplyAction = v.findViewById(R.id.buttonApplyAction);

        layoutParameters = v.findViewById(R.id.layoutParameters);
        listViewParameters = v.findViewById(R.id.listViewParameters);
        buttonParametersOk = v.findViewById(R.id.buttonParametersOk);
        buttonParametersCancel = v.findViewById(R.id.buttonParametersCancel);

        layoutParametersLoading = v.findViewById(R.id.layoutParametersLoading);

        parametersListAdapter = new DeviceSettingsList.ParametersListAdapter(editorContext.parameters, getActivity());
        listViewParameters.setAdapter(parametersListAdapter);

        buttonDoConfigure.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                editSelectedDevices();
            }
        });

        buttonUpdateDevicesList.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                updateDevicesList();
            }
        });

        buttonParametersOk.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                saveSettingsAndCloseEditing();
                //switchToSelectDevice();
            }
        });

        buttonParametersCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //saveSettingsAndCloseEditing();
                switchToSelectDevice();
            }
        });

        switchToSelectDevice();

        if (mode == CONFIG_RIFLES) {
            textViewSelectDevice.setText("Select rifle to configure:");
        } else {
            textViewSelectDevice.setText("Select player to configure:");
        }

        devicesListAdapter = new DevicesListAdapter(CausticInitializer.getInstance().controller().getDevicesManager(), getActivity());
        listViewDevicesToEdit.setAdapter(devicesListAdapter);
        initCustomActions();
        return v;
    }

    public static class UIDevicesListElement {
        public View convertView = null;
        CheckBox checkBoxDeviceName = null;
        TextView textViewDeviceComment = null;
        public final CausticDevice dev;

        UIDevicesListElement(LayoutInflater inflater, CausticDevice dev) {
            this.dev = dev;
            convertView = inflater.inflate(org.ltcaustic.rcspandroid.R.layout.devices_list_selectable_item, null);

            checkBoxDeviceName = convertView.findViewById(R.id.checkBoxDeviceName);
            textViewDeviceComment = convertView.findViewById(R.id.textViewDeviceComment);
            checkBoxDeviceName.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                    RCSP.DeviceAddress addr = UIDevicesListElement.this.dev.address;
                    if (isChecked) {
                        CausticInitializer.getInstance().controller().getSettingsEditorContext().selectForEditing(addr);
                    } else {
                        CausticInitializer.getInstance().controller().getSettingsEditorContext().deselectForEditing(addr);
                    }
                }
            });
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

    void initCustomActions() {
        if (mode == CONFIG_PLAYERS) {
            String[] variants = {"Respawn", "Kill", "Remove life"};
            ArrayAdapter<String> adapter = new ArrayAdapter<String>(getActivity(), android.R.layout.simple_spinner_item, variants);
            spinnerSelectAction.setAdapter(adapter);
            spinnerSelectAction.setPrompt("Actions for players");
            buttonApplyAction.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    int pos=spinnerSelectAction.getSelectedItemPosition();
                    Toast toast = null;
                    switch (pos) {
                        case 0:
                            // Respawn
                            toast = Toast.makeText(getActivity(), "Respawn", Toast.LENGTH_SHORT);
                            CausticInitializer.getInstance().controller().getCalls().respawn(editorContext.getDevices());
                            break;
                        case 1:
                            // Kill
                            toast = Toast.makeText(getActivity(), "kill", Toast.LENGTH_SHORT);
                            CausticInitializer.getInstance().controller().getCalls().kill(editorContext.getDevices());
                            break;
                        case 2:
                            // Kill
                            toast = Toast.makeText(getActivity(), "Remove life", Toast.LENGTH_SHORT);
                            CausticInitializer.getInstance().controller().getCalls().reset(editorContext.getDevices());
                            break;
                    }
                    if (toast != null)
                        toast.show();
                }
            });
        }
    }

    void switchToEditParameters() {
        /*
        layoutSelectDevice.animate().translationYBy(-1000).withEndAction(new Runnable() {
            @Override
            public void run() {
                layoutSelectDevice.setVisibility(View.GONE);
                layoutParameters.setVisibility(View.VISIBLE);
            }
        }).start();*/

        layoutParametersLoading.setVisibility(View.GONE);
        layoutSelectDevice.setVisibility(View.GONE);
        layoutParameters.setVisibility(View.VISIBLE);

        String allDevices = "";
        for (RCSP.DeviceAddress devAddr : editorContext.getDevices()) {
            if (!allDevices.equals(""))
                allDevices += ", ";
            CausticDevice dev = devicesManager.devices.get(devAddr);
            allDevices = allDevices + dev.getName();
        }
        textViewSelectedDevices.setText(allDevices);
    }

    void switchToSelectDevice() {
        layoutParameters.setVisibility(View.GONE);
        layoutParametersLoading.setVisibility(View.GONE);
        layoutSelectDevice.setVisibility(View.VISIBLE);
        updateDevicesList();
/*
        layoutSelectDevice.animate().translationYBy(1000).withEndAction(new Runnable() {
            @Override
            public void run() {
            }
        }).start();*/
    }

    void switchToLoading() {
        layoutParameters.setVisibility(View.GONE);
        layoutSelectDevice.setVisibility(View.GONE);
        layoutParametersLoading.setVisibility(View.VISIBLE);
    }

    void updateDevicesList() {
        if (!isActive || devicesListAdapter == null)
            return;

        //listViewDevicesToEdit.invalidateViews();
        devicesListAdapter.updateUIList();
        devicesListAdapter.notifyDataSetChanged();
        editorContext.clearSelectedToEdit();
    }

    void editSelectedDevices() {
        if (!editorContext.isSomethingSelectedToEdit())
            return;
        switchToLoading();
        editorContext.asyncPopParametersFromSelectedDevices(parametersListUpdater);
    }

    void saveSettingsAndCloseEditing() {
        editorContext.pushValues();
        switchToSelectDevice();
    }

    /**
     * Hold message when all settings are loaded from devices and entries ready to be created
     */
    private class ParametersListUpdater implements DevicesManager.SynchronizationEndListener {
        @Override
        public void onSynchronizationEnd(boolean isSuccess, Set<RCSP.DeviceAddress> notSynchronized) {
            if (isSuccess) {
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        switchToEditParameters();
                        editorContext.createEntries(new DeviceSettingsList.UIDataPickerFactory());
                        parametersListAdapter.notifyDataSetChanged();
                    }
                });
            } else {
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        switchToSelectDevice();
                    }
                });
            }
        }
    }

    public class DevicesListAdapter extends BaseAdapter {
        private LayoutInflater inflater;
        private List<UIDevicesListElement> uiDevices = new ArrayList<>();
        DevicesManager devicesManager;

        public DevicesListAdapter(DevicesManager mgr, Activity activity) {
            devicesManager = mgr;
            inflater = (LayoutInflater) activity.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            updateUIList();

        }

        public void updateUIList() {
            uiDevices.clear();
            for (CausticDevice dev: devicesManager.devices.values()) {
                if (
                        (mode == CONFIG_RIFLES && dev.isRifle())
                                || (mode == CONFIG_PLAYERS && dev.isHeadSensor())
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
