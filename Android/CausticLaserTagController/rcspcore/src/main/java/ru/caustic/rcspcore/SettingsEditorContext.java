package ru.caustic.rcspcore;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;

/**
 * This class represent anything related to devices settings editor. It store list of devices to edit
 * and provide data picking from UI entries and pushing to device
 */
public class SettingsEditorContext {

    public SettingsEditorContext(DevicesManager devicesManager)
    {
        this.devicesManager = devicesManager;
    }

    /**
     * Interface to communicate with GUI element representing parameter
     */
    public static abstract class UIDataPicker {
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

    /**
     * Abstract factory to build realizations of UIDataPicker
     */
    public interface IUIDataPickerFactory {
        UIDataPicker create(RCSP.ParameterDescription description);
    }

    /**
     * This class represent ONE parameter for ALL devices selected to edit.
     * It store all information to create UI list entry, current state of editing,
     * reference to UIDataPicker implementer in Activiry(Fragment) class.
     */
    public class ParameterEntry {
        public SettingsEditorContext context;
        public RCSP.ParameterDescription description;
        // If all the devices has the same parameters value, it should be shown. Otherwise "Different" will be shown
        public boolean hasInitialValue = false;
        public boolean wasChanged = false;
        final String differentValueStr = "Different";
        public String currentValue = differentValueStr;
        String initialValue = currentValue;

        public UIDataPicker uiListElement = null;


        public String getValue() {
            return currentValue;
        }

        public String getInitialValue() {
            return initialValue;
        }

        public ParameterEntry(SettingsEditorContext context, RCSP.ParameterDescription description) {
            this.context = context;
            this.description = description;
        }

        /**
         * Interrogate all devices included to current SettingsEditorContext abd determine if they
         * have the same or different value
         */
        public void init(IUIDataPickerFactory factory) {
            boolean valueInitialized = false;
            hasInitialValue = true; // We suppose that all values are the same
            for (RCSP.DeviceAddress addr : context.devices) {
                String thisDeviceValue
                        = devicesManager.devices
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

            for (RCSP.DeviceAddress addr : context.devices) {
                RCSP.AnyParameterSerializer par
                        = devicesManager.devices
                        .get(addr).parameters
                        .get(description.getId());
                par.setValue(currentValue);
            }
        }
    }


    /**
     * Set of devices selected to editing
     */
    private Set<RCSP.DeviceAddress> devices = new HashSet<>();

    /**
     * Set of parameters avaliable to edit
     */
    public ArrayList<ParameterEntry> parameters = new ArrayList<>();

    private boolean entriesCreated = false;

    /**
     * Remove all devices from list to edit
     */
    public void clearSelectedToEdit() {
        devices.clear();
    }

    /**
     * Add device to list for editing. Changed parameters will be pulled/pushed to/from device
     * @param addr device address to edit
     */
    public void selectForEditing(RCSP.DeviceAddress addr) {
        devices.add(addr);
        entriesCreated = false;
    }

    /**
     * Remove from list to edit
     * @param addr device address should not be edited
     */
    public void deselectForEditing(RCSP.DeviceAddress addr) {
        if (devices.contains(addr))
            devices.remove(addr);
    }

    /**
     * Check containment of list for editing
     * @return true if list for editing not empty
     */
    public boolean isSomethingSelectedToEdit() {
        return !devices.isEmpty();
    }
    public final Set<RCSP.DeviceAddress> getDevicesSelectedToEdit() {
        return devices;
    }
    public void asyncPopParametersFromSelectedDevices(DevicesManager.SynchronizationEndListener endHandler)
    {
        devicesManager.asyncPullAllParameters(endHandler, devices);
    }
    // End of accessors for devices

    /**
     * Get any one of addresses of devices seslected to edit
     * @return device address
     */
    public RCSP.DeviceAddress getAnyAddress() {
        for (RCSP.DeviceAddress addr : devices) {
            return addr;
        }
        return null;
    }

    /**
     * Get type of devices that are selected to edit to the present momentum
     * @return See RCSP.Operations.AnyDevice.Configuration: DEV_TYPE_UNDEFINED, DEV_TYPE_RIFLE, DEV_TYPE_HEAD_SENSOR, ...
     */
    public int getDeviceType() {
        if (devices.isEmpty()) {
            return RCSP.Operations.AnyDevice.Configuration.DEV_TYPE_UNDEFINED;
        }

        return Integer.parseInt(
                devicesManager.devices.get(getAnyAddress()).parameters.get(
                        RCSP.Operations.AnyDevice.Configuration.deviceType.getId()
                ).getValue()
        );
    }

    /**
     * Create parameters list entries
     * @param factory Realization of IUIDataPickerFactory that builds real UI list elements
     *                that implements UIDataPicker
     */
    public void createEntries(IUIDataPickerFactory factory) {
        /// @todo add check that all devices are homogeneous
        if (devices.isEmpty())
            return;

            /*if (entriesCreated)
                return;*/
        parameters.clear();

        RCSP.DeviceAddress someAddress = getAnyAddress();

        DevicesManager.CausticDevice dev = devicesManager.devices.get(someAddress);

        // We need to output parameters sorted by original order
        for (int id : dev.parameters.orderedIds) {
            RCSP.ParameterDescription descr = dev.parameters.allParameters.get(id).getDescription();

            if (!descr.isEditable())
                continue;

            ParameterEntry newParameter = new ParameterEntry(this, descr);
            newParameter.init(factory);

            /// @todo Combine this to containers and classes those contain
            parameters.add(newParameter);
        }
        entriesCreated = true;
    }

    /**
     * Pushing all changes made by user to devices
     */
    public void pushValues() {
        // Reading modified values
        for (ParameterEntry entry : parameters) {
            entry.uiListElement.pickValue();
            entry.pushValue();
        }
        // Sending values to devices
        for (RCSP.DeviceAddress address : devices) {
            devicesManager.devices.get(address).pushToDevice();
        }
    }

    private DevicesManager devicesManager;
}
