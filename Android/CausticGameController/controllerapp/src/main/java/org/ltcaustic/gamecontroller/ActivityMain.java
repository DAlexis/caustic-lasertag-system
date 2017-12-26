package org.ltcaustic.gamecontroller;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.design.widget.Snackbar;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.ViewGroup;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import org.ltcaustic.rcspandroid.BluetoothManager;
import org.ltcaustic.rcspandroid.CausticInitializer;
import org.ltcaustic.rcspandroid.FragmentSelectBluetoothDevice;
import org.ltcaustic.rcspcore.DevicesManager;

public class ActivityMain extends AppCompatActivity
        implements NavigationView.OnNavigationItemSelectedListener {

    private static final String PREFERENCE_LAST_FRAGMENT = "last_active_fragment";
    private static final String PREFERENCE_LAST_FRAGMENT_QUICK_CONTROLS = "quick_controls";
    private static final String PREFERENCE_LAST_FRAGMENT_STATISTICS = "statistics";
    private static final String PREFERENCE_LAST_FRAGMENT_CONFIGURE_PLAYERS = "configure_players";
    private static final String PREFERENCE_LAST_FRAGMENT_CONFIGURE_RIFLES = "configure_rifles";
    private static final String PREFERENCE_LAST_FRAGMENT_SELECT_BLUETOOTH_DEVICE = "select_bluetooth_device";

    FragmentStatistics fragmentStatistics;
    FragSetConfigureGameDevices configPlayers;
    FragSetConfigureGameDevices configRifles;

    FragmentGameControls fragmentGameControls;
    FragmentSelectBluetoothDevice selectBluetoothFragment;
    BtConnectingInformer btConnectingInformer = new BtConnectingInformer();
    TextView textViewDevicesCount;

    SharedPreferences sharedPref = null;

    Toolbar toolbar;
    boolean isActive = false;

    public class SnackbarInformer
    {
        public void show(String text, boolean finite, boolean progressBar)
        {
            int length = finite ? Snackbar.LENGTH_SHORT : Snackbar.LENGTH_INDEFINITE;
            snackbar = Snackbar.make(toolbar, text, length);
            if (progressBar) {
                ViewGroup contentLay = (ViewGroup) snackbar.getView().findViewById(android.support.design.R.id.snackbar_text).getParent();
                contentLay.addView(new ProgressBar(toolbar.getContext()), 0);
            }
            snackbar.show();
        }

        public void hide()
        {
            snackbar.dismiss();
        }

        private Snackbar snackbar;
    }

    private void errorExit(String title, String message) {
        Toast.makeText(getBaseContext(), title + " - " + message, Toast.LENGTH_LONG).show();
        finish();
    }

    public void testBluetoothEnabled() {
        switch(CausticInitializer.getInstance().bluetooth().checkBluetoothState()) {
            case BluetoothManager.BLUETOOTH_NOT_SUPPORTED:
                errorExit("Fatal Error", "Your device does not support bluetooth :(");
                break;
            case BluetoothManager.BLUETOOTH_DISABLED:
                startActivityForResult(CausticInitializer.getInstance().bluetooth().getIntentEnable(), BluetoothManager.REQUEST_ENABLE_BT);
                break;
            case BluetoothManager.BLUETOOTH_ENABLED:
                break;
        }
    }

    /**
     * Class
     */
    public class BtConnectingInformer extends SnackbarInformer implements BluetoothManager.ConnectionProcessListener {
        @Override
        public void onConnecting(final String deviceName) {
            runOnUiThread(new Runnable() {
                public void run() {
                    show("Connecting to " + deviceName + "...", false, true);
                }
            });
        }

        @Override
        public void onConnectionDone(boolean result) {
            if (result) {
                runOnUiThread(new Runnable() {
                    public void run() {
                        hide();
                    }
                });
            } else {
                runOnUiThread(new Runnable() {
                    public void run() {
                        hide();
                        show("Connection error", true, false);
                        commitFragment(selectBluetoothFragment, getSupportFragmentManager());
                    }
                });
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        /*
        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });*/


        /*SnackbarInformer si = new SnackbarInformer();
        si.show("Hello");*/

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        textViewDevicesCount = findViewById(R.id.textViewDevicesCount);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.addDrawerListener(toggle);
        toggle.syncState();

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);

        sharedPref = getPreferences(MODE_PRIVATE);

        fragmentStatistics = new FragmentStatistics();
        fragmentGameControls = new FragmentGameControls();
        configPlayers = new FragSetConfigureGameDevices(FragSetConfigureGameDevices.CONFIG_PLAYERS);
        configRifles = new FragSetConfigureGameDevices(FragSetConfigureGameDevices.CONFIG_RIFLES);
        selectBluetoothFragment = new FragmentSelectBluetoothDevice();

        testBluetoothEnabled();

        CausticInitializer.getInstance().controller();
        // We use FragmentSelectBluetoothDevice's proxy to allow him redraw elements according new connection state
        selectBluetoothFragment.setConnectionProcessListener(btConnectingInformer);
        CausticInitializer.getInstance().bluetooth().doAutoconnectIfNeeded(this, btConnectingInformer);
        CausticInitializer.getInstance().controller().getDevicesListUpdater().subscribe(new DevicesCountUpdater());
        commitFragment(getLastFragment(), getSupportFragmentManager());
    }

    @Override
    protected void onResume() {
        super.onResume();
        isActive = true;
    }

    @Override
    protected void onPause() {
        super.onPause();
        isActive = false;
    }

    @Override
    public void onBackPressed() {
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        // Handle navigation view item clicks here.
        int id = item.getItemId();


        FragmentManager fm = getSupportFragmentManager();

        if (id == R.id.nav_game_controls) {
            commitFragment(fragmentGameControls, fm);
        } else if (id == R.id.nav_statistics) {
            commitFragment(fragmentStatistics, fm);
        } else if (id == R.id.nav_config_players) {
            commitFragment(configPlayers.fragmentConfigureGameDevicesList, fm);
        } else if (id == R.id.nav_config_rifles) {
            commitFragment(configPlayers.fragmentConfigureGameDevicesList, fm);
        } else if (id == R.id.nav_bluetooth_device) {
            commitFragment(selectBluetoothFragment, fm);
        } else if (id == R.id.nav_other_settings) {

        }

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;
    }

    public void commitFragment(Fragment fragment, FragmentManager fragmentManager) {
        fragmentManager.popBackStack();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
        fragmentTransaction.replace(R.id.fragment, fragment);
        fragmentTransaction.commit();
        saveLastFragment(fragment);
    }

    private class DevicesCountUpdater implements DevicesManager.DeviceListUpdatedListener {
        @Override
        public void onDevicesListUpdated() {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    textViewDevicesCount.setText(Integer.toString(CausticInitializer.getInstance().controller().getDevicesManager().devicesCount()));
                }
            });
        }
    }

    void saveLastFragment(Fragment fragment) {
        SharedPreferences.Editor editor = sharedPref.edit();
        if (fragment == fragmentGameControls)
            editor.putString(PREFERENCE_LAST_FRAGMENT, PREFERENCE_LAST_FRAGMENT_QUICK_CONTROLS);
        else if (fragment == fragmentStatistics)
            editor.putString(PREFERENCE_LAST_FRAGMENT, PREFERENCE_LAST_FRAGMENT_STATISTICS);
        else if (fragment == configPlayers.fragmentConfigureGameDevicesList)
            editor.putString(PREFERENCE_LAST_FRAGMENT, PREFERENCE_LAST_FRAGMENT_CONFIGURE_PLAYERS);
        else if (fragment == configRifles.fragmentConfigureGameDevicesList)
            editor.putString(PREFERENCE_LAST_FRAGMENT, PREFERENCE_LAST_FRAGMENT_CONFIGURE_RIFLES);
        else if (fragment == selectBluetoothFragment)
            editor.putString(PREFERENCE_LAST_FRAGMENT, PREFERENCE_LAST_FRAGMENT_SELECT_BLUETOOTH_DEVICE);
        editor.commit();
    }

    Fragment getLastFragment() {
        String f = sharedPref.getString(PREFERENCE_LAST_FRAGMENT, PREFERENCE_LAST_FRAGMENT_SELECT_BLUETOOTH_DEVICE);
        if (f.equals(PREFERENCE_LAST_FRAGMENT_QUICK_CONTROLS))
            return fragmentGameControls;
        else if (f.equals(PREFERENCE_LAST_FRAGMENT_STATISTICS))
            return fragmentStatistics;
        else if (f.equals(PREFERENCE_LAST_FRAGMENT_CONFIGURE_PLAYERS))
            return configPlayers.fragmentConfigureGameDevicesList;
        else if (f.equals(PREFERENCE_LAST_FRAGMENT_CONFIGURE_RIFLES))
            return configRifles.fragmentConfigureGameDevicesList;
        else if (f.equals(PREFERENCE_LAST_FRAGMENT_SELECT_BLUETOOTH_DEVICE))
            return selectBluetoothFragment;
        return selectBluetoothFragment;
    }
}
