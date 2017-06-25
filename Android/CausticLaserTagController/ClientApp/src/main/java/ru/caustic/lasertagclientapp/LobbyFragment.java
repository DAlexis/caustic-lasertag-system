package ru.caustic.lasertagclientapp;


import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.caustic.androidshared.BluetoothManager;


/**
 * A simple {@link Fragment} subclass.
 */
public class LobbyFragment extends Fragment {


    public LobbyFragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View layout = inflater.inflate(R.layout.fragment_lobby, container, false);

        MainActivity mainActivity = (MainActivity) getActivity();

        Fragment fragment;

        if (BluetoothManager.getInstance().getStatus()==BluetoothManager.BT_CONNECTED)
        {
            fragment = new PlayerListFragment();
        }
        else {
            fragment = new BluetoothConnectFragment();
        }

        FragmentTransaction ft = getChildFragmentManager().beginTransaction();
        ft.replace(R.id.lobby_fragment_container, fragment);
        ft.addToBackStack(null);
        ft.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
        ft.commit();

        return layout;
    }

}
