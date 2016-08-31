package ru.caustic.lasertag.controlcore;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;

/**
 * This class represent anything related to devices settings editor. It store list of devices to edit
 * and provide data picking from UI entries and pushing to device
 */
public class SettingsEditorContext {

    /**
     * This class provide data picking from interface elements to ParameterEntry
     */
    public static abstract class UIDataPicker {
        public void setParameterEntry(ParameterEntry parameterEntry) {
            this.parameterEntry = parameterEntry;
        }

        /**
         * Update UI element when containment changed
         */
        public abstract void update();

        /**
         * Get value from UI element and call parameterEntry.setValue with new value
         */
        public abstract void pickValue();

        protected ParameterEntry parameterEntry = null;
    }

    public interface UIDataPickerFactory {
        UIDataPicker create(RCSProtocol.ParameterDescription description);
    }

    /**
     * This class represent ONE parameter for ALL devices selected to edit
     */
    public static class ParameterEntry {
        SettingsEditorContext context;
        RCSProtocol.ParameterDescription description;
        // If all the devices has the same parameters value, it should be shown. Otherwise "Different" will be shown
        boolean hasInitialValue = false;
        boolean wasChanged = false;
        final String differentValueStr = "Different";
        String currentValue = differentValueStr;
        String initialValue = currentValue;

        private UIDataPicker uiListElement = null;


        public String getValue() {
            return currentValue;
        }

        public String getInitialValue() {
            return initialValue;
        }

        public ParameterEntry(SettingsEditorContext context, RCSProtocol.ParameterDescription description) {
            this.context = context;
            this.description = description;
        }

        /**
         * Interrogate all devices included to current SettingsEditorContext abd determine if they
         * have the same or different value
         */
        public void init(UIDataPickerFactory factory) {
            boolean valueInitialized = false;
            hasInitialValue = true; // We suppose that all values are the same
            for (BridgeConnector.DeviceAddress addr : context.devices) {
                String thisDeviceValue
                        = CausticDevicesManager.getInstance().devices
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
            initialValue = currentValue;
            wasChanged = false;
            uiListElement = factory.create(description);
        }

        /*
        public void initUiListElement(LayoutInflater inflater) {
            // Here will be parameter type specification code
            uiListElement.init(inflater, this);
        }*/

        public UIDataPicker getUIDataPicker() {
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
                        = CausticDevicesManager.getInstance().devices
                        .get(addr).parameters
                        .get(description.getId());
                par.setValue(currentValue);
            }
        }
    }

    public Set<BridgeConnector.DeviceAddress> devices = new HashSet<>();
    public ArrayList<ParameterEntry> parameters = new ArrayList<>();

    private boolean entriesCreated = false;
    public void clear() {
        devices.clear();
    }
    public void selectForEditing(BridgeConnector.DeviceAddress addr) {
        devices.add(addr);
        entriesCreated = false;
    }
    public void deselectForEditing(BridgeConnector.DeviceAddress addr) {
        devices.remove(addr);
    }
    public BridgeConnector.DeviceAddress getAnyAddress() {
        for (BridgeConnector.DeviceAddress addr : devices) {
            return addr;
        }
        return null;
    }
    public int getDeviceType() {
        if (devices.isEmpty()) {
            return RCSProtocol.Operations.AnyDevice.Configuration.DEV_TYPE_UNDEFINED;
        }

        return Integer.parseInt(
                CausticDevicesManager.getInstance().devices.get(getAnyAddress()).parameters.get(
                        RCSProtocol.Operations.AnyDevice.Configuration.deviceType.getId()
                ).getValue()
        );
    }
    public void createEntries(UIDataPickerFactory factory) {
        /// @todo add check that all devices are homogeneous
        if (devices.isEmpty())
            return;

            /*if (entriesCreated)
                return;*/
        parameters.clear();

        BridgeConnector.DeviceAddress someAddress = getAnyAddress();

        CausticDevicesManager.CausticDevice dev = CausticDevicesManager.getInstance().devices.get(someAddress);

        // We need to output parameters sorted by original order
        for (int id : dev.parameters.orderedIds) {
            RCSProtocol.ParameterDescription descr = dev.parameters.allParameters.get(id).getDescription();

            if (!descr.isEditable())
                continue;

            ParameterEntry newParameter = new ParameterEntry(this, descr);
            newParameter.init(factory);

            /// @todo Combine this to containers and classes those contain
            parameters.add(newParameter);
        }
        entriesCreated = true;
    }
    public void pushValues() {
        // Reading modified values
        for (ParameterEntry entry : parameters) {
            entry.uiListElement.pickValue();
            entry.pushValue();
        }
        // Sending values to devices
        for (BridgeConnector.DeviceAddress address : devices) {
            CausticDevicesManager.getInstance().devices.get(address).pushToDevice();
        }
    }
}
