package ru.caustic.lasertagclientapp;

import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.ShareActionProvider;

public class MainActivity extends Activity {

    private static final String TAG = "MainActivity";
    private static final String EXTRAS_MODE = "mode";

    private ListView drawerList;
    private String modeTitles[];
    private DrawerLayout drawerLayout;
    private ActionBarDrawerToggle drawerToggle;
    private int currentMode = 0; //for determining fragment to display
    private ShareActionProvider shareActionProvider;

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
        }
        else
        {
            selectItem(0);
        }

        getActionBar().setDisplayHomeAsUpEnabled(true);
        getActionBar().setHomeButtonEnabled(true);


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

        getFragmentManager().addOnBackStackChangedListener(
                new FragmentManager.OnBackStackChangedListener() {
                    public void onBackStackChanged() {
                        FragmentManager fragMan = getFragmentManager();
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
        shareActionProvider = (ShareActionProvider) menuItem.getActionProvider();
        setShareIntent("Default text");
        return super.onCreateOptionsMenu(menu);
    }

    private void setShareIntent(String text) {
        Intent intent = new Intent(Intent.ACTION_SEND);
        intent.setType("text/plain");
        intent.putExtra(Intent.EXTRA_TEXT, text);
        shareActionProvider.setShareIntent(intent);
    }

    private class DrawerClickListener implements ListView.OnItemClickListener
    {
        @Override
        public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
            selectItem(i);
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
        //menu.findItem(R.id.action_share).setVisible(!drawerOpen);
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
                default:
                    return super.onOptionsItemSelected(item);
            }
        }
    }

    private void selectItem(int position)
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

        FragmentTransaction ft = getFragmentManager().beginTransaction();
        ft.replace(R.id.content_frame, fragment, "visible_fragment");
        ft.addToBackStack(null);
        ft.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
        ft.commit();

        setActionBarTitle(position);
        drawerLayout.closeDrawer(drawerList);
    }

    private void setActionBarTitle(int position)
    {
        String title;
        if (position == 0)
        {
            title = getResources().getString(R.string.app_name);
        }
        else {
            title = modeTitles[position];
        }
        getActionBar().setTitle(title);
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putInt(EXTRAS_MODE, currentMode);
    }

}
