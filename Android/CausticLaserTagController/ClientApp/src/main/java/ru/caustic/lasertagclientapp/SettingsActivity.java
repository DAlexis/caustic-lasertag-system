package ru.caustic.lasertagclientapp;


import android.annotation.TargetApi;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.support.v7.app.ActionBar;
import android.preference.PreferenceFragment;
import android.preference.PreferenceManager;
import android.preference.RingtonePreference;
import android.support.annotation.ColorInt;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.util.Log;
import android.view.MenuItem;

import com.enrico.colorpicker.colorDialog;

import com.caustic.rcspcore.CausticController;
import com.caustic.rcspcore.CausticDevice;
import com.caustic.rcspcore.DevicesManager;
import com.caustic.rcspcore.RCSP;

/**
 * A {@link PreferenceActivity} that presents a set of application settings. On
 * handset devices, settings are presented as a single list. On tablets,
 * settings are split by category, with category headers shown to the left of
 * the list of settings.
 * <p>
 * See <a href="http://developer.android.com/design/patterns/settings.html">
 * Android Design: Settings</a> for design guidelines and the <a
 * href="http://developer.android.com/guide/topics/ui/settings.html">Settings
 * API Guide</a> for more information on developing a Settings UI.
 */
public class SettingsActivity extends AppCompatActivity implements colorDialog.ColorSelectedListener {
    /**
     * A preference value change listener that updates the preference's summary
     * to reflect its new value.
     */
    private static final String TAG = "SettingsActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setupActionBar();
        getFragmentManager().beginTransaction()
                .replace(android.R.id.content, new GeneralPreferenceFragment())
                .commit();

    }

    /**
     * Set up the {@link android.app.ActionBar}, if the API is available.
     */
    private void setupActionBar() {
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            // Show the Up button in the action bar.
            actionBar.setDisplayHomeAsUpEnabled(true);
        }
    }


    /**
     * This method stops fragment injection in malicious applications.
     * Make sure to deny any unknown fragments here.
     */
    protected boolean isValidFragment(String fragmentName) {
        return PreferenceFragment.class.getName().equals(fragmentName)
                || GeneralPreferenceFragment.class.getName().equals(fragmentName);
    }



    /**
     * This fragment shows general preferences only. It is used when the
     * activity is showing a two-pane settings UI.
     */
    @TargetApi(Build.VERSION_CODES.HONEYCOMB)
    public static class GeneralPreferenceFragment extends PreferenceFragment {

        private Preference.OnPreferenceChangeListener sBindPreferenceSummaryToValueListener = new Preference.OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object value) {
                String stringValue = value.toString();

                if (preference instanceof ListPreference) {
                    // For list preferences, look up the correct display value in
                    // the preference's 'entries' list.
                    ListPreference listPreference = (ListPreference) preference;
                    int index = listPreference.findIndexOfValue(stringValue);

                    // Set the summary to reflect the new value.
                    preference.setSummary(
                            index >= 0
                                    ? listPreference.getEntries()[index]
                                    : null);

                } else if (preference instanceof RingtonePreference) {
                    // For ringtone preferences, look up the correct display value
                    // using RingtoneManager.
                    if (TextUtils.isEmpty(stringValue)) {
                        // Empty values correspond to 'silent' (no ringtone).
                        preference.setSummary(R.string.pref_ringtone_silent);

                    } else {
                        Ringtone ringtone = RingtoneManager.getRingtone(
                                preference.getContext(), Uri.parse(stringValue));

                        if (ringtone == null) {
                            // Clear the summary if there was a lookup error.
                            preference.setSummary(null);
                        } else {
                            // Set the summary to reflect the new ringtone display
                            // name.
                            String name = ringtone.getTitle(preference.getContext());
                            preference.setSummary(name);
                        }
                    }

                } else {
                    // For all other preferences, set the summary to the value's
                    // simple string representation.
                    preference.setSummary(stringValue);
                }

                pushLocalSettingsToAssociatedHeadSensor(getActivity());
                return true;
            }
        };

        /**
         * Helper method to determine if the device has an extra-large screen. For
         * example, 10" tablets are extra-large.
         */
        private static boolean isXLargeTablet(Context context) {
            return (context.getResources().getConfiguration().screenLayout
                    & Configuration.SCREENLAYOUT_SIZE_MASK) >= Configuration.SCREENLAYOUT_SIZE_XLARGE;
        }

        /**
         * Binds a preference's summary to its value. More specifically, when the
         * preference's value is changed, its summary (line of text below the
         * preference title) is updated to reflect the value. The summary is also
         * immediately updated upon calling this method. The exact display format is
         * dependent on the type of preference.
         *
         * @see #sBindPreferenceSummaryToValueListener
         */
        private void bindPreferenceSummaryToValue(Preference preference) {
            // Set the listener to watch for value changes.
            preference.setOnPreferenceChangeListener(sBindPreferenceSummaryToValueListener);

            // Trigger the listener immediately with the preference's
            // current value.
            sBindPreferenceSummaryToValueListener.onPreferenceChange(preference,
                    PreferenceManager
                            .getDefaultSharedPreferences(preference.getContext())
                            .getString(preference.getKey(), ""));
        }


        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            addPreferencesFromResource(R.xml.pref_general);
            setHasOptionsMenu(true);

            // Bind the summaries of EditText/List/Dialog/Ringtone preferences
            // to their values. When their values change, their summaries are
            // updated to reflect the new value, per the Android Design
            // guidelines.
            bindPreferenceSummaryToValue(findPreference("display_name_key"));


            Preference colorPreference = findPreference("marker_color_key");
            SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(getActivity());
            int color = sharedPref.getInt("marker_color_key", 0);
            colorDialog.setColorPreferenceSummary(colorPreference, color, getActivity(), getResources());
            colorPreference.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener() {

                @Override

                public boolean onPreferenceClick(Preference preference) {

                    colorDialog.showColorPicker((AppCompatActivity) getActivity(), 0);

                    return false;
                }
            });
        }


    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == android.R.id.home)
        {
            onBackPressed();
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onColorSelection(DialogFragment dialogFragment, @ColorInt int selectedColor) {

        //Set start color for the picker
        colorDialog.setPickerColor(SettingsActivity.this, 0, selectedColor);

        //Set preference summary after picking new color
        GeneralPreferenceFragment prefFrag = (GeneralPreferenceFragment) getFragmentManager().findFragmentById(android.R.id.content);
        Preference colorPreference = prefFrag.findPreference("marker_color_key");
        colorDialog.setColorPreferenceSummary(colorPreference, selectedColor, this, getResources());

        //Update related shared preference
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(SettingsActivity.this);
        prefs.edit().putInt("marker_color_key", selectedColor).apply();
        Log.d(TAG, "Color is " + prefs.getInt("marker_color_key", 0));

        //Push changes to head sensor
        pushLocalSettingsToAssociatedHeadSensor(SettingsActivity.this);
    }

    private static void pushLocalSettingsToAssociatedHeadSensor(Context context) {
        DevicesManager devMan = CausticController.getInstance().getDevicesManager();
        CausticDevice headSensor = devMan.devices.get(devMan.associatedHeadSensorAddress);
        if (headSensor!=null) {
            if (headSensor.areDeviceRelatedParametersAdded()) {
                SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(context);

                //Setting displayed name
                //@todo Make separate fields for device name and player name
                String playerName = sharedPref.getString("display_name_key", "Default Player2");
                headSensor.parameters.get(RCSP.Operations.AnyDevice.Configuration.deviceName.getId())
                        .setValue(playerName);
                headSensor.pushToDevice(RCSP.Operations.AnyDevice.Configuration.deviceName.getId());

                //Setting on-map marker color
                String markerColor = Integer.toString(sharedPref.getInt("marker_color_key", -1));
                headSensor.parameters.get(RCSP.Operations.HeadSensor.Configuration.markerColor.getId())
                        .setValue(markerColor);
                headSensor.pushToDevice(RCSP.Operations.HeadSensor.Configuration.markerColor.getId());
            }
        }
    }
}
