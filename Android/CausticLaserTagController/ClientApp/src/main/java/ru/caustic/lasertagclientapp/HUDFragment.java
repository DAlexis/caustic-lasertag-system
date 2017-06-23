package ru.caustic.lasertagclientapp;


import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTabHost;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;


/**
 * A simple {@link Fragment} subclass.
 */
public class HUDFragment extends Fragment{

    private FragmentTabHost mTabHost;

    public HUDFragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment

        mTabHost = new FragmentTabHost(getActivity());
        mTabHost.setup(getActivity(), getChildFragmentManager(), R.id.fragment_map);

        mTabHost.addTab(mTabHost.newTabSpec("map").setIndicator("Map"),
                MapFragment.class, null);
        mTabHost.addTab(mTabHost.newTabSpec("scores").setIndicator("Scores"),
                GameScoresFragment.class, null);

        return mTabHost;

    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        mTabHost = null;
    }
}
