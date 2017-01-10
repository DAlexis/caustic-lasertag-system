package ru.caustic.lasertagclientapp;

import android.os.Handler;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.support.v4.view.MenuItemCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.support.v7.widget.ShareActionProvider;
import android.widget.Toast;

import ru.caustic.rcspcore.BluetoothManager;

public class MainActivity extends AppCompatActivity implements BluetoothManager.ConnectionDoneListener {

    private static final String TAG = "MainActivity";
    private static final String EXTRAS_MODE = "mode";
    private static final String EXTRAS_BT_CONNECTED = "btConnected";
    private static final String EXTRAS_BT_HANDLER_RUNNING = "monitorRunning";

    public static boolean activityVisible = false;
    public boolean monitorRunning = false;

    private ListView drawerList;
    private String modeTitles[];
    private DrawerLayout drawerLayout;
    private ActionBarDrawerToggle drawerToggle;
    private int currentMode = 0; //for determining fragment to display
    private ShareActionProvider shareActionProvider;

    private static Handler btConnectionMonitorHandler = new Handler();
    private Runnable btConnMon = new Runnable(){
        //Time interval in msecs for checking BT connection status
        public static final long BT_CONNECTION_MONITOR_PERIODICITY = 1000;

        @Override
        public void run() {
            boolean btCurrentStatus = false;
            if (BluetoothManager.getInstance().getStatus() == BluetoothManager.BT_CONNECTED)
            {
                btCurrentStatus = true;
            }
            boolean btStatusChanged = setBtConnected(btCurrentStatus);
            Log.d(TAG, "BTConnMon: btStatus " + isBtConnected() + ", btStatusChanged " + btStatusChanged);
            if (btStatusChanged)
            {
                //Update fragment view if BT status changed
                selectMode(currentMode, false);
                //Show notification
                if (isBtConnected())
                {
                    Toast.makeText(MainActivity.this, "Bluetooth connection established", Toast.LENGTH_SHORT).show();

                }
                else {
                    Toast.makeText(MainActivity.this, "Bluetooth connection failed", Toast.LENGTH_SHORT).show();
                }

                //Redraw action bar to show/hide disconnect option
                invalidateOptionsMenu();
            }
            btConnectionMonitorHandler.postDelayed(this, BT_CONNECTION_MONITOR_PERIODICITY);
        }
    };

    public boolean isBtConnected() {
        return btConnected;
    }

    public boolean setBtConnected(boolean btConnected) {
        //Returns true if BT connection status changed, and false if did not change
        if (this.btConnected != btConnected) {
            this.btConnected = btConnected;
            return true;
        }
        else return false;
    }

    private boolean btConnected = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        //Initialize activity members
        drawerList = (ListView) findViewById(R.id.drawer);
        drawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);

        //Get array of options titles in the navigation drawer
        modeTitles = getResources().getStringArray(R.array.mode_titles);


        if (savedInstanceState != null)
        {
            currentMode = (int) savedInstanceState.getInt(EXTRAS_MODE);
            setActionBarTitle(currentMode);
            setBtConnected(savedInstanceState.getBoolean(EXTRAS_BT_CONNECTED));
            monitorRunning =savedInstanceState.getBoolean(EXTRAS_BT_HANDLER_RUNNING);
        }
        else
        {
            currentMode = 0;
            selectMode(currentMode, false);
            setActionBarTitle(currentMode);
        }


        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setHomeButtonEnabled(true);
        }
        else {
            Log.d(TAG, "Action bar not found onCreate");
        }



        drawerList.setOnItemClickListener(new DrawerClickListener());
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(
                this,
                android.R.layout.simple_list_item_activated_1,
                modeTitles);
        drawerList.setAdapter(adapter);

        drawerToggle = new ActionBarDrawerToggle(this, drawerLayout, R.string.open_drawer, R.string.close_drawer)
        {
            @Override
            public void onDrawerOpened(View drawerView) {

                invalidateOptionsMenu();
                super.onDrawerOpened(drawerView);
            }

            @Override
            public void onDrawerClosed(View drawerView) {
                invalidateOptionsMenu();
                super.onDrawerClosed(drawerView);
            }
        };
        drawerLayout.addDrawerListener(drawerToggle);

        getSupportFragmentManager().addOnBackStackChangedListener(
                new FragmentManager.OnBackStackChangedListener() {
                    public void onBackStackChanged() {
                        FragmentManager fragMan = getSupportFragmentManager();
                        Fragment fragment = fragMan.findFragmentByTag("visible_fragment");
                        if (fragment instanceof LobbyFragment)
                        {
                            currentMode = 0;
                        }
                        if (fragment instanceof PersonalFragment)
                        {
                            currentMode = 1;
                        }
                        if (fragment instanceof GlobalFragment)
                        {
                            currentMode = 2;
                        }
                        if (fragment instanceof HUDFragment)
                        {
                            currentMode = 3;
                        }
                        setActionBarTitle(currentMode);
                        drawerList.setItemChecked(currentMode, true);
                    }
                }
        );
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        MenuItem menuItem = (MenuItem) menu.findItem(R.id.action_share);
        shareActionProvider = (ShareActionProvider) MenuItemCompat.getActionProvider(menuItem);
        setShareIntent("Default text");
        MenuItem disconnectMenu = (MenuItem) menu.findItem(R.id.action_disconnect);
        disconnectMenu.setVisible(isBtConnected());
        return super.onCreateOptionsMenu(menu);
    }

    private void setShareIntent(String text) {
        Intent intent = new Intent(Intent.ACTION_SEND);
        intent.setType("text/plain");
        intent.putExtra(Intent.EXTRA_TEXT, text);
        shareActionProvider.setShareIntent(intent);
    }

    @Override
    public void onConnectionDone(final boolean result) {

    }

    private class DrawerClickListener implements ListView.OnItemClickListener
    {
        @Override
        public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
            selectMode(i, true);
        }
    }

    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        drawerToggle.syncState(); //Required for correct drawer toggle behaviour
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {

        super.onConfigurationChanged(newConfig);
        drawerToggle.onConfigurationChanged(newConfig); //Ensures correct drawer behaviour on device orientation change
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {

        boolean drawerOpen = drawerLayout.isDrawerOpen(drawerList);
        menu.findItem(R.id.action_share).setVisible(!drawerOpen);
        return super.onPrepareOptionsMenu(menu);
    }

    public boolean onOptionsItemSelected(MenuItem item) {
        if (drawerToggle.onOptionsItemSelected(item))
        {
            return true;
        }
        else
        {
            switch (item.getItemId()){
                //case R.id.action_create_order:
                //    Intent intent = new Intent(this, OrderActivity.class);
                //    startActivity(intent);
                //    return true;
                case R.id.action_settings:
                    Intent intent = new Intent(this, SettingsActivity.class);
                    startActivity(intent);
                    return true;
                case R.id.action_disconnect:
                    BluetoothManager.getInstance().cancel();
                    return true;
                default:
                    return super.onOptionsItemSelected(item);
            }
        }
    }

    private void selectMode(int position, boolean pushToBackStack)
    {
        currentMode = position;
        Fragment fragment;
        switch (position)
        {

            case 1:
                fragment = new PersonalFragment();
                break;
            case 2:
                fragment = new GlobalFragment();
                break;
            case 3:
                fragment = new HUDFragment();
                break;
            default:
                fragment = new LobbyFragment();
                break;
        }

//        //If in lobby, BT monitor should be running, if not, stop it
//        if (position == 0 && !monitorRunning)
//        {
//            startBTMonitor();
//        }
//        else if (monitorRunning && isBtConnected() == false)
//        {
//            stopBTMonitor();
//        }

        FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
        ft.replace(R.id.content_frame, fragment, "visible_fragment");
        if (pushToBackStack) {
            ft.addToBackStack(null);
        }
        ft.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
        ft.commit();

        setActionBarTitle(position);
        drawerLayout.closeDrawer(drawerList);
    }

    private void startBTMonitor() {
        btConnectionMonitorHandler.post(btConnMon);
        monitorRunning = true;
    }

    private void stopBTMonitor() {
        btConnectionMonitorHandler.removeCallbacks(btConnMon);
        monitorRunning = false;
    }

    private void setActionBarTitle(int position) {
        String title;
        title = modeTitles[position];

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setTitle(title);
        }
        else {
            Log.d(TAG, "Action bar not found");
        }
    }


    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putInt(EXTRAS_MODE, currentMode);
        outState.putBoolean(EXTRAS_BT_CONNECTED, isBtConnected());
        outState.putBoolean(EXTRAS_BT_HANDLER_RUNNING, monitorRunning);
        activityVisible = false;
    }

    @Override
    protected void onStop() {
        activityVisible = false;
        stopBTMonitor();
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        activityVisible = false;
        super.onDestroy();
    }

    @Override
    protected void onStart() {
        super.onStart();
        activityVisible = true;
        if (!monitorRunning){
            startBTMonitor();
        }
    }
}
