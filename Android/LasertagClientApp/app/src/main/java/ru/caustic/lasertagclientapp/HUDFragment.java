package ru.caustic.lasertagclientapp;


import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.drawable.ShapeDrawable;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentTabHost;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.graphics.drawable.shapes.OvalShape;
import android.os.Handler;

import com.google.android.gms.maps.*;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.LatLngBounds;
import com.google.android.gms.maps.model.MapStyleOptions;
import com.google.android.gms.maps.model.MarkerOptions;

import java.util.ArrayList;
import java.util.Map;
import java.util.Set;

import ru.caustic.rcspcore.BridgeConnector;
import ru.caustic.rcspcore.CausticController;
import ru.caustic.rcspcore.DeviceUtils;
import ru.caustic.rcspcore.DevicesManager;
import ru.caustic.rcspcore.RCSProtocol;

import static ru.caustic.rcspcore.DeviceUtils.getDeviceName;
import static ru.caustic.rcspcore.DeviceUtils.getDeviceTeam;
import static ru.caustic.rcspcore.DeviceUtils.getMarkerColor;


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
                GameStatsFragment.class, null);

        return mTabHost;

    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        mTabHost = null;
    }
}
