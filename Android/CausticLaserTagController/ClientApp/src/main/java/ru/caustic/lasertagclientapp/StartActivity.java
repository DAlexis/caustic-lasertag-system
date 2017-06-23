package ru.caustic.lasertagclientapp;

import android.app.Activity;
import android.app.Fragment;
import android.content.Intent;
import android.os.Bundle;
import android.app.FragmentTransaction;
import android.util.Log;
import android.view.View;

public class StartActivity extends Activity implements View.OnClickListener {


    private static final String TAG = "StartActivity";

    private static final String MAIN_FRAGMENT_STATE_TAG = "state";
    private static final String MAIN_FRAGMENT_READY_TAG = "fragment_ready";

    @Override
    public void onClick(View view) {
        switch (view.getId())
        {
            case (R.id.newPlayerButtonMain):
                fragmentState = FragmentState.NEW_USER_DISPLAY;
                displayFragmentByState(fragmentState, true);
                break;
            case (R.id.logInButtonMain):
                fragmentState = FragmentState.LOG_IN_DISPLAY;
                displayFragmentByState(fragmentState, true);
                break;
            case (R.id.offlineModeButtonMain):
                Intent intent = new Intent(this, MainActivity.class);
                startActivity(intent);
        }
    }

    private enum FragmentState {
        START_STATE, LOG_IN_DISPLAY, NEW_USER_DISPLAY, WAITING_FOR_LOGIN, WAITING_FOR_NEW_USER
    }

    private FragmentState fragmentState;
    private boolean fragmentReady = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start);

        if (savedInstanceState == null)
        {
            fragmentState = FragmentState.START_STATE;
        }
        else
        {
            fragmentState = (FragmentState) savedInstanceState.getSerializable(StartActivity.MAIN_FRAGMENT_STATE_TAG);
            fragmentReady = savedInstanceState.getBoolean(StartActivity.MAIN_FRAGMENT_READY_TAG, false);
        }

        if (!fragmentReady) {
            displayFragmentByState(fragmentState, false);
            fragmentReady = true;
        }

    }

    private void displayFragmentByState(FragmentState state, boolean putOnBackStack)
    {
        Fragment fragment;
        switch (state)
        {
            case LOG_IN_DISPLAY:
                fragment = new LoginFragment();
                break;
            case NEW_USER_DISPLAY:
                fragment = new NewUserFragment();
                break;
            default:
                fragment = new StartButtonsFragment();
        }
        Log.d(TAG, "Created new fragment");
        FragmentTransaction transaction = getFragmentManager().beginTransaction();
        transaction.replace(R.id.fragment_main, fragment);
        transaction.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
        if (putOnBackStack) transaction.addToBackStack(null);
        transaction.commit();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putSerializable(StartActivity.MAIN_FRAGMENT_STATE_TAG, fragmentState);
        outState.putBoolean(StartActivity.MAIN_FRAGMENT_READY_TAG, fragmentReady);
    }
}
