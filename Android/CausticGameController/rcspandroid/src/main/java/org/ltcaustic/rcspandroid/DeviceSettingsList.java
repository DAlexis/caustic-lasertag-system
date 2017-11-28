package org.ltcaustic.rcspandroid;

import android.app.Activity;
import android.content.Context;
import android.text.Editable;
import android.text.TextWatcher;
import android.text.method.DigitsKeyListener;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;

import org.ltcaustic.rcspcore.DevicesManager;
import org.ltcaustic.rcspcore.RCSP;
import org.ltcaustic.rcspcore.SettingsEditorContext;

import java.util.List;
import java.util.Set;

/**
 * Created by dalexies on 28.11.17.
 */

public class DeviceSettingsList {
    private static final String TAG = "CC.DevSetFragment";

    /**
     * UI superstructure over UIDataPicker -- adds field used by any UI list element and init() method
     */
    private static abstract class UIListElement extends SettingsEditorContext.UIDataPicker {
        public View convertView = null;
        public abstract void init(LayoutInflater inflater, final SettingsEditorContext.ParameterEntry _parameterEntry);
    }

    private static abstract class UIListElementSeekBar extends UIListElement {
        TextView parameterName = null;
        TextView parameterSummary = null;
        SeekBar seekBar = null;
        EditText parameterValue = null;
        TextView notEqual = null;
        ImageButton revert = null;

        boolean workaroundIgnoreInput = false;
        RCSP.ParameterDescription descr = null;

        protected abstract String progressToValue(int progress);
        protected abstract int valueToProgress(String value);
        protected abstract int maxProgress();
        protected abstract String textToValue(String text);
        protected abstract String valueToText(String value);
        protected abstract String getSummary();

        private void updateProgress(String value) {
            seekBar.setProgress(valueToProgress(value));
        }

        private void updateText(String value) {
            parameterValue.setText(valueToText(value));
        }

        @Override
        public void init(LayoutInflater inflater, final SettingsEditorContext.ParameterEntry _parameterEntry) {
            this.parameterEntry = _parameterEntry;
            descr = parameterEntry.description;

            this.convertView = inflater.inflate(R.layout.parameters_list_base_item, null);
            parameterName = (TextView) convertView.findViewById(R.id.parameterName);
            parameterSummary = (TextView) convertView.findViewById(R.id.parameterSummary);
            seekBar = (SeekBar) convertView.findViewById(R.id.seekBarParameterValue);
            parameterValue = (EditText) convertView.findViewById(R.id.editTextParameterValue);
            notEqual = (TextView) convertView.findViewById(R.id.notEqual);
            revert = (ImageButton) convertView.findViewById(R.id.revert);


            seekBar.setMax(maxProgress());

            // First setting values for SeekBar and TextView, than setting listeners to prevent picking values before
            // user enter it manually. When many devices selected for editing this cause setting not equal parameters
            // to zero and picking it to device.
            parameterName.setText(parameterEntry.description.getName());
            parameterSummary.setText(getSummary());
            if (parameterEntry.hasInitialValue) {
                parameterValue.setText(valueToText(parameterEntry.getValue()));
                notEqual.setVisibility(View.GONE);
                updateProgress(parameterEntry.getValue());
            } else {
                parameterValue.setText("");
                seekBar.setProgress(0);
                notEqual.setVisibility(View.VISIBLE);
            }

            seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    if (workaroundIgnoreInput)
                        return;
                    if (fromUser) {
                        updateText(progressToValue(progress));
                        Log.d(TAG, "onProgressChanged from user");
                        pickValue();
                    }
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                }
            });

            parameterValue.addTextChangedListener(new TextWatcher() {
                @Override
                public void beforeTextChanged(CharSequence s, int start, int count, int after) {

                }

                @Override
                public void onTextChanged(CharSequence s, int start, int before, int count) {

                }

                @Override
                public void afterTextChanged(Editable s) {
                    Log.d(TAG, "afterTextChanged");
                    if (workaroundIgnoreInput)
                        return;
                    String str = s.toString();
                    if (s.toString().isEmpty())
                        return;

                    pickValue();
                    workaroundIgnoreInput = true;
                    updateProgress(parameterEntry.getValue());
                    workaroundIgnoreInput = false;
                }
            });

            revert.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (parameterEntry.hasInitialValue) {
                        updateText(parameterEntry.getInitialValue());
                        updateProgress(parameterEntry.getInitialValue());
                    } // TODO add here code for multiply selection
                }
            });


        }

        public void update() {/*
            parameterName.setText(parameterEntry.description.getName());
            if (parameterEntry.hasInitialValue) {
                parameterValue.setText(valueToText(parameterEntry.getValue()));
                notEqual.setVisibility(View.GONE);
            } else {
                parameterValue.setText("");
                notEqual.setVisibility(View.VISIBLE);
            }

            parameterSummary.setText(getSummary());
            if (parameterEntry.hasInitialValue) {
                updateProgress(parameterEntry.getValue());
            }*/
        }

        public void pickValue() {
            if (parameterValue == null)
                return;
            String text = parameterValue.getText().toString();
            if (text.isEmpty()) {
                parameterEntry.setValue(progressToValue(0));
            } else {
                parameterEntry.setValue(textToValue(text));
            }
        }
    }

    private static class UIListElementInteger extends UIListElementSeekBar {
        protected int min() {
            return ((RCSP.UintParameter) descr).minValue;
        }

        protected int max() {
            return ((RCSP.UintParameter) descr).maxValue;
        }

        @Override
        public void init(LayoutInflater inflater, final SettingsEditorContext.ParameterEntry _parameterEntry) {
            super.init(inflater, _parameterEntry);
            parameterValue.setKeyListener(DigitsKeyListener.getInstance("0123456789"));
        }

        @Override
        protected String progressToValue(int progress) {
            return Integer.toString(progress + min());
        }

        @Override
        protected int valueToProgress(String value) {
            return Integer.parseInt(value) - min();
        }

        @Override
        protected int maxProgress() {
            return max() - min();
        }

        @Override
        protected String textToValue(String text) {
            int val;
            try {
                //Update Seekbar value after entering a number
                val = Integer.parseInt(text);
            } catch(Exception ex) {
                val = min();
            }

            if (val > max())
                val = max();
            if (val < min())
                val = min();

            return Integer.toString(val);
        }

        @Override
        protected String valueToText(String value) {
            return value;
        }

        @Override
        protected String getSummary() {
            return "From " + min() + " to " + max();
        }
    }
    private static class UIListElementUByte extends UIListElementInteger {
        @Override
        protected int min() {
            return ((RCSP.UByteParameter) descr).minValue;
        }

        @Override
        protected int max() {
            return ((RCSP.UByteParameter) descr).maxValue;
        }
    }
    private static class UIListElementTimeInterval extends UIListElementSeekBar {
        private static int k = 1000;
        private long min() {
            return ((RCSP.TimeIntervalParameter) descr).minValue;
        }

        private long max() {
            return ((RCSP.TimeIntervalParameter) descr).maxValue;
        }

        @Override
        protected String progressToValue(int progress) {
            long value = progress * k + min();
            return Long.toString(value);
        }

        @Override
        protected int valueToProgress(String value) {
            long v = (Long.parseLong(value) - min()) / k;
            return (int) v;
        }

        @Override
        protected int maxProgress() {
            return (int) ((max() - min()) / k);
        }

        @Override
        protected String textToValue(String text) {
            return Long.toString(Long.parseLong(text)*k);
        }

        @Override
        protected String valueToText(String value) {
            return Long.toString(Long.parseLong(value)/k);
        }

        @Override
        protected String getSummary() {
            return "From " + min()/k + "ms to " + max()/k + "ms";
        }
    }
    private static class UIListElementFloat extends UIListElementSeekBar {
        private static int progressElements = 1000;
        private float min() {
            return ((RCSP.FloatParameter) descr).minValue;
        }

        private float max() {
            return ((RCSP.FloatParameter) descr).maxValue;
        }

        @Override
        protected String progressToValue(int progress) {
            float value = ((float) progress) / progressElements * (max() - min()) + min();
            return Float.toString(value);
        }

        @Override
        protected int valueToProgress(String value) {
            return Math.round((Float.parseFloat(value) - min()) / (max() - min()) * progressElements);
        }

        @Override
        protected int maxProgress() {
            return progressElements;
        }

        @Override
        protected String textToValue(String text) {
            return text;
        }

        @Override
        protected String valueToText(String value) {
            return value;
        }

        @Override
        protected String getSummary() {
            return "From " + min() + " to " + max();
        }
    }
    private static class UIListElementBoolean extends UIListElement {
        TextView parameterName = null;
        TextView parameterSummary = null;
        Switch boolSwitch = null;
        TextView notEqual = null;
        ImageButton revert = null;

        RCSP.BooleanParameter descr = null;

        @Override
        public void init(LayoutInflater inflater, final SettingsEditorContext.ParameterEntry _parameterEntry) {
            this.parameterEntry = _parameterEntry;
            this.convertView = inflater.inflate(R.layout.parameters_list_boolean_item, null);
            parameterName = (TextView) convertView.findViewById(R.id.parameterName);
            parameterSummary = (TextView) convertView.findViewById(R.id.parameterSummary);
            boolSwitch = (Switch) convertView.findViewById(R.id.switchBooleanValue);
            notEqual = (TextView) convertView.findViewById(R.id.notEqual);
            revert = (ImageButton) convertView.findViewById(R.id.revert);
            revert.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (parameterEntry.hasInitialValue) {
                        boolSwitch.setChecked(parameterEntry.getInitialValue().equals("true"));
                    } else {
                        boolSwitch.setChecked(false);
                    }
                }
            });
        }

        public void update() {
            parameterName.setText(parameterEntry.description.getName());
            parameterSummary.setText("");
            if (parameterEntry.hasInitialValue) {
                boolSwitch.setChecked(parameterEntry.getValue().equals("true"));
                notEqual.setVisibility(View.GONE);
            } else {
                boolSwitch.setChecked(false);
                notEqual.setVisibility(View.VISIBLE);
            }
        }

        public void pickValue() {
            if (boolSwitch == null)
                return;
            parameterEntry.setValue(boolSwitch.isChecked() ? "true" : "false");
        }
    }
    private static class UIListElementEnum extends UIListElement {
        TextView parameterName = null;
        TextView parameterSummary = null;
        Spinner variants = null;
        TextView notEqual = null;
        ImageButton revert = null;

        RCSP.EnumParameter descr = null;

        ArrayAdapter<String> itemsAdapter = null;

        int initialPosition = 0;

        @Override
        public void init(LayoutInflater inflater, final SettingsEditorContext.ParameterEntry _parameterEntry) {
            this.parameterEntry = _parameterEntry;
            descr = (RCSP.EnumParameter) parameterEntry.description;

            this.convertView = inflater.inflate(R.layout.parameters_list_enum_item, null);
            parameterName = (TextView) convertView.findViewById(R.id.parameterName);
            parameterSummary = (TextView) convertView.findViewById(R.id.parameterSummary);
            variants = (Spinner) convertView.findViewById(R.id.variants);
            notEqual = (TextView) convertView.findViewById(R.id.notEqual);
            revert = (ImageButton) convertView.findViewById(R.id.revert);

            if (itemsAdapter == null) {
                itemsAdapter = new ArrayAdapter<>(inflater.getContext(), android.R.layout.simple_list_item_1, descr.namesOrdered);
            }
            variants.setAdapter(itemsAdapter);
            if (parameterEntry.hasInitialValue) {
                initialPosition = itemsAdapter.getPosition(descr.getCurrentValueString(Integer.parseInt(parameterEntry.currentValue)));
                if (initialPosition < 0)
                    initialPosition = 0;
            } else {
                initialPosition = 0;
            }

            revert.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (parameterEntry.hasInitialValue) {
                        variants.setSelection(initialPosition);
                    }
                }
            });

            variants.post(new Runnable() {
                @Override
                public void run() {
                    variants.setSelection(initialPosition);
                }
            });
        }

        public void update() {
            parameterName.setText(parameterEntry.description.getName());
            parameterSummary.setText("");
            if (parameterEntry.hasInitialValue) {
                notEqual.setVisibility(View.GONE);

            } else {
                notEqual.setVisibility(View.VISIBLE);
            }
            itemsAdapter.notifyDataSetChanged();
        }

        public void pickValue() {
            if (variants == null)
                return;
            parameterEntry.setValue(Integer.toString(
                    descr.entries.get(
                            variants.getSelectedItem().toString()
                    )
                    )
            );
        }
    }
    private static class UIListElementUnsupported extends UIListElement {
        TextView parameterName = null;
        TextView parameterSummary = null;

        RCSP.BooleanParameter descr = null;

        @Override
        public void init(LayoutInflater inflater, final SettingsEditorContext.ParameterEntry parameterEntry) {
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

    public static class UIDataPickerFactory implements SettingsEditorContext.IUIDataPickerFactory {
        @Override
        public SettingsEditorContext.UIDataPicker create(RCSP.ParameterDescription description)
        {
            if (description instanceof RCSP.UintParameter) {
                return new UIListElementInteger();
            } else if (description instanceof RCSP.BooleanParameter) {
                return new UIListElementBoolean();
            } else if (description instanceof RCSP.TimeIntervalParameter) {
                return new UIListElementTimeInterval();
            } else if (description instanceof RCSP.EnumParameter) {
                return new UIListElementEnum();
            } else if (description instanceof RCSP.FloatParameter) {
                return new UIListElementFloat();
            } else if (description instanceof RCSP.UByteParameter) {
                return new UIListElementUByte();
            }
            return new UIListElementUnsupported();
        }
    }

    /**
     * Adapter adopts List<SettingsEditorContext.ParameterEntry> that should be get from
     * SettingsEditorContext.parameters
     */
    public class ParametersListAdapter extends BaseAdapter {
        private LayoutInflater mInflater;
        private List<SettingsEditorContext.ParameterEntry> parametersList;

        public ParametersListAdapter(List<SettingsEditorContext.ParameterEntry> parametersList, Activity activity) {
            this.parametersList = parametersList;
            mInflater = (LayoutInflater) activity.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        @Override
        public int getCount() {
            return parametersList.size();
        }

        @Override
        public String getItem(int position) {
            return parametersList.get(position).description.getName();
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {

            Log.i("getView", "position: "+ position+ " convertView: "+ convertView);

            SettingsEditorContext.ParameterEntry entry = parametersList.get(position);
            UIListElement uiListElement = (UIListElement) entry.getUIDataPicker();

            if (uiListElement.convertView == null) {
                // If real ui list element not initialized, initialize it
                //entry.initUiListElement(mInflater);
                uiListElement.init(mInflater, entry);

                uiListElement.update();
                //convertView.setTag(entry.getUiListElement());
            }

            uiListElement.update();

            return uiListElement.convertView;

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
