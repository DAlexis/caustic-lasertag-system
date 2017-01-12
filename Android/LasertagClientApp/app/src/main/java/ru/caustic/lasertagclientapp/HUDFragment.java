package ru.caustic.lasertagclientapp;


import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.drawable.ShapeDrawable;
import android.location.Location;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.graphics.drawable.shapes.OvalShape;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapFragment;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;

import org.w3c.dom.Text;

import java.util.ArrayList;
import java.util.Map;

import ru.caustic.rcspcore.BridgeConnector;
import ru.caustic.rcspcore.CausticController;
import ru.caustic.rcspcore.DevicesManager;
import ru.caustic.rcspcore.RCSProtocol;

import static ru.caustic.lasertagclientapp.DeviceUtils.getDeviceName;
import static ru.caustic.lasertagclientapp.DeviceUtils.getDeviceTeam;


/**
 * A simple {@link Fragment} subclass.
 */
public class HUDFragment extends Fragment implements OnMapReadyCallback{

    private DevicesManager devMan = CausticController.getInstance().getDevicesManager();
    ArrayList<PlayerOnMap> playerOnMapList = new ArrayList<>();
    private GoogleMap map;
    private static final String TAG = "HUDFragment";

    private static final int MARKER_BORDER_WIDTH_SP = 4;
    private static final int MARKER_RADIUS_SP = 20;

    public HUDFragment() {
        // Required empty public constructor
    }

    @Override
    public void onMapReady(GoogleMap googleMap) {
        map = googleMap;
        for (PlayerOnMap player : playerOnMapList)
        {
            Log.d(TAG, "Setting marker at " + player.lat + ", " + player.lon);
            LatLng latLng = new LatLng(player.lat, player.lon);
            //map.moveCamera(CameraUpdateFactory.newLatLng(latLng));
            Bitmap icon = createMarkerIcon(player.markerColor, player.team);
            MarkerOptions options =new MarkerOptions()
                    .position(latLng).title(player.name).anchor(0.5f, 0.5f).icon(BitmapDescriptorFactory.fromBitmap(icon));
            map.addMarker(options);
        }
    }

    private Bitmap createMarkerIcon(int markerColor, String team) {

        int pxRadius = MARKER_RADIUS_SP * Math.round(getActivity().getResources().getDisplayMetrics().scaledDensity);
        int pxBorderWidth = MARKER_BORDER_WIDTH_SP * Math.round(getActivity().getResources().getDisplayMetrics().scaledDensity);


        Bitmap b = Bitmap.createBitmap(pxRadius, pxRadius, Bitmap.Config.ARGB_8888);
        Canvas c = new Canvas(b);
        ShapeDrawable mDrawable = new ShapeDrawable(new OvalShape());

        //Drawing circle with team color first and then a smaller circle with marker color second
        int teamColor = getTeamColor(team);
        mDrawable.getPaint().setColor(teamColor);
        mDrawable.setBounds(0, 0, c.getWidth(), c.getHeight());
        mDrawable.draw(c);

        mDrawable.getPaint().setColor(markerColor);
        mDrawable.setBounds(pxBorderWidth, pxBorderWidth, c.getWidth()-pxBorderWidth, c.getHeight()-pxBorderWidth);
        mDrawable.draw(c);

        return b;
    }

    private int getTeamColor(String team) {
        switch (team)
        {
            case "Red":
                return ContextCompat.getColor(getContext(), R.color.red);
            case "Blue":
                return ContextCompat.getColor(getContext(), R.color.blue);
            case "Green":
                return ContextCompat.getColor(getContext(), R.color.green);
            case "Yellow":
                return ContextCompat.getColor(getContext(), R.color.yellow);
            default:
                return ContextCompat.getColor(getContext(), R.color.grey);
        }

    }

    private class PlayerOnMap
    {
        float lat=0;
        float lon=0;
        String name = "Unknown";
        String team = "Unknown";
        int markerColor = 0xFFFFFFFF;
        boolean isVisible = true;
        public PlayerOnMap(DevicesManager.CausticDevice device)
        {

            if (device.areDeviceRelatedParametersAdded()&&device.areParametersSync())
            {
                lat = Float.parseFloat(device.parameters.get(RCSProtocol.Operations.HeadSensor.Configuration.playerLat.getId()).getValue());
                lon = Float.parseFloat(device.parameters.get(RCSProtocol.Operations.HeadSensor.Configuration.playerLon.getId()).getValue());
                if (lat>90 || lat<-90 || lon>180 || lon<-180) isVisible = false;
                team = getDeviceTeam(device);
                name = getDeviceName(device);
            }
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View layout = inflater.inflate(R.layout.fragment_hud, container, false);

        TextView locs = (TextView) layout.findViewById(R.id.locs_test);
        Map<BridgeConnector.DeviceAddress, DevicesManager.CausticDevice> devsToLocate = DeviceUtils.getHeadSensorsMap(devMan.devices);
        playerOnMapList = populatePlayerOnMapList(devsToLocate);
        String testDisplay = "";
        for (PlayerOnMap player : playerOnMapList)
        {
            testDisplay = testDisplay + player.name + " - team " + player.team + ", lat: " + player.lat +
                    ", lon: " + player.lon + " , marker:" + player.markerColor + "\n";
        }
        locs.setText(testDisplay);

        Fragment mMapFragment = SupportMapFragment.newInstance();
        FragmentTransaction fragmentTransaction =
                getChildFragmentManager().beginTransaction();
        fragmentTransaction.add(R.id.map_fragment_container, mMapFragment);
        fragmentTransaction.commit();

        //Need to call getMapAsync to receive a OnMapReady callback
        SupportMapFragment mapFrag = (SupportMapFragment) mMapFragment;
        mapFrag.getMapAsync(this);

        return layout;
    }

    private ArrayList<PlayerOnMap> populatePlayerOnMapList(Map<BridgeConnector.DeviceAddress, DevicesManager.CausticDevice> headSensorsMap) {
        ArrayList<PlayerOnMap> returnList = new ArrayList<>();
        for (Map.Entry<BridgeConnector.DeviceAddress, DevicesManager.CausticDevice> entry : headSensorsMap.entrySet()) {

            DevicesManager.CausticDevice dev = entry.getValue();
            PlayerOnMap newPlayer = new PlayerOnMap(dev);
            returnList.add(newPlayer);
        }
        return returnList;
    }


    public static class MarkerMapFragment extends SupportMapFragment {


        public MarkerMapFragment() {
            // Required empty public constructor
        }


        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                 Bundle savedInstanceState) {

            return null;
        }

    }


}
