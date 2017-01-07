package ru.caustic.lasertagclientapp;


import android.content.Context;
import android.os.Bundle;
import android.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;


/**
 * A simple {@link Fragment} subclass.
 */
public class StartButtonsFragment extends Fragment {


    public StartButtonsFragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View layout = inflater.inflate(R.layout.fragment_start_buttons, container, false);

        Button newPlayerButton = (Button) layout.findViewById(R.id.newPlayerButtonMain);
        Button logInButton = (Button) layout.findViewById(R.id.logInButtonMain);
        Button offlineButton = (Button) layout.findViewById(R.id.offlineModeButtonMain);

        View.OnClickListener activity = (View.OnClickListener) getActivity();

        newPlayerButton.setOnClickListener(activity);
        logInButton.setOnClickListener(activity);
        offlineButton.setOnClickListener(activity);

        return layout;
    }

}
