package ru.caustic.lasertag.causticlasertagcontroller;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.res.Configuration;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceFragment;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.preference.RingtonePreference;
import android.text.TextUtils;
import android.util.Log;
import android.view.MenuItem;
import android.support.v4.app.NavUtils;
import android.widget.Toast;

import java.util.List;


public class CausticDeviceSettings extends PreferenceActivity {

    public final String TAG = "CC.CausticDeviceSettings";
    public void onBuildHeaders(List<Header> target) {

        //loadHeadersFromResource(R.xml.pref_header, target);
        //Toast.makeText(getBaseContext(), target.get(0).fragment, Toast.LENGTH_LONG).show();

        Header h = new Header();
        h.title = "Test1";
        h.fragment = "ru.caustic.lasertag.causticlasertagcontroller.CausticDeviceSettings$CausticDeviceSettingsFragment";
        h.fragmentArguments = null;
        h.breadCrumbShortTitle = null;
        h.breadCrumbTitle = null;
        h.summary = "Summmmmary";
        h.extras = null;
        h.intent = null;
        h.id = -1;
        h.iconRes = 0;
        h.breadCrumbTitleRes = 0;
        h.breadCrumbShortTitleRes = 0;
        h.summaryRes = 0;
        h.titleRes = 0;

        //h.fragment = CausticDeviceSettingsFragment.class.toString();
        //h.fragment = "ru.caustic.lasertag.causticlasertagcontroller.CausticDeviceSettings.CausticDeviceSettingsFragment";
        target.add(h);

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //getFragmentManager().beginTransaction().replace(android.R.id.content, new CausticDeviceSettingsFragment()).commit();
    }

    public static class CausticDeviceSettingsFragment extends PreferenceFragment {

        private PreferenceScreen screen = null;
        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            addPreferencesFromResource(R.xml.device_preference_stub);
            // создаем экран

            screen = this.getPreferenceScreen(); // "null". See onViewCreated.

            // Create the Preferences Manually - so that the key can be set programatically.
            PreferenceCategory category = new PreferenceCategory(screen.getContext());
            category.setTitle("Channel Configuration");
            screen.addPreference(category);
/*
            CheckBoxPreference checkBoxPref = new CheckBoxPreference(screen.getContext());
            checkBoxPref.setKey(channelConfig.getName() + "_ENABLED");
            checkBoxPref.setTitle(channelConfig.getShortname() + "Enabled");
            checkBoxPref.setSummary(channelConfig.getDescription());
            checkBoxPref.setChecked(channelConfig.isEnabled());

            category.addPreference(checkBoxPref);*/



            PreferenceScreen rootScreen = getPreferenceManager().createPreferenceScreen(screen.getContext());
            // говорим Activity, что rootScreen - корневой
            setPreferenceScreen(rootScreen);

            // даллее создаем элементы, присваиваем атрибуты и формируем иерархию

            CheckBoxPreference chb1 = new CheckBoxPreference(screen.getContext());
            chb1.setKey("chb1");
            chb1.setTitle("CheckBox 1");
            chb1.setSummaryOn("Description of checkbox 1 on");
            chb1.setSummaryOff("Description of checkbox 1 off");

            rootScreen.addPreference(chb1);

    /*
            ListPreference list = new ListPreference(this);
            list.setKey("list");
            list.setTitle("List");
            list.setSummary("Description of list");
            list.setEntries(R.array.entries);
            list.setEntryValues(R.array.entry_values);

            rootScreen.addPreference(list);*/


            CheckBoxPreference chb2 = new CheckBoxPreference(screen.getContext());
            chb2.setKey("chb2");
            chb2.setTitle("CheckBox 2");
            chb2.setSummary("Description of checkbox 2");

            rootScreen.addPreference(chb2);


            //PreferenceScreen screen = getPreferenceManager().createPreferenceScreen(this);
            screen.setKey("screen");
            screen.setTitle("Screen");
            screen.setSummary("Description of screen");


            final CheckBoxPreference chb3 = new CheckBoxPreference(screen.getContext());
            chb3.setKey("chb3");
            chb3.setTitle("CheckBox 3");
            chb3.setSummary("Description of checkbox 3");

            screen.addPreference(chb3);


            PreferenceCategory categ1 = new PreferenceCategory(screen.getContext());
            categ1.setKey("categ1");
            categ1.setTitle("Category 1");
            categ1.setSummary("Description of category 1");

            screen.addPreference(categ1);

            CheckBoxPreference chb4 = new CheckBoxPreference(screen.getContext());
            chb4.setKey("chb4");
            chb4.setTitle("CheckBox 4");
            chb4.setSummary("Description of checkbox 4");

            categ1.addPreference(chb4);


            final PreferenceCategory categ2 = new PreferenceCategory(screen.getContext());
            categ2.setKey("categ2");
            categ2.setTitle("Category 2");
            categ2.setSummary("Description of category 2");

            screen.addPreference(categ2);


            CheckBoxPreference chb5 = new CheckBoxPreference(screen.getContext());
            chb5.setKey("chb5");
            chb5.setTitle("CheckBox 5");
            chb5.setSummary("Description of checkbox 5");

            categ2.addPreference(chb5);


            CheckBoxPreference chb6 = new CheckBoxPreference(screen.getContext());
            chb6.setKey("chb6");
            chb6.setTitle("CheckBox 6");
            chb6.setSummary("Description of checkbox 6");

            categ2.addPreference(chb6);

            rootScreen.addPreference(screen);

            //list.setDependency("chb1");
            screen.setDependency("chb2");

            // код из прошлого урока для связи активности categ2 и значения chb3
            categ2.setEnabled(chb3.isChecked());
            chb3.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener() {
                public boolean onPreferenceClick(Preference preference) {
                    categ2.setEnabled(chb3.isChecked());
                    return false;
                }
            });
        }
    }
}