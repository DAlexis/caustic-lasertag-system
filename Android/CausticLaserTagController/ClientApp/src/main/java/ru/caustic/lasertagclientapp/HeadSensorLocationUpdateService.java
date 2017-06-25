package ru.caustic.lasertagclientapp;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.IBinder;
import android.preference.PreferenceManager;
import android.support.v4.app.ActivityCompat;
import android.util.Log;

import com.caustic.rcspcore.CausticController;
import com.caustic.rcspcore.CausticDevice;
import com.caustic.rcspcore.DevicesManager;
import com.caustic.rcspcore.RCSP;

public class HeadSensorLocationUpdateService extends Service {

    private LocationListener listener;
    private LocationManager locManager;
    private static final String TAG = "HeadSensorLocationUpdat";

    public HeadSensorLocationUpdateService() {
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {


        listener = new LocationListener() {
            @Override
            public void onLocationChanged(Location location) {
                //Update location data in the app
                SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
                if (sharedPreferences.getBoolean("gps_tracking", true)) {
                    pushLocationToAssociatedHeadSensor(location);
                    Log.d(TAG, "Pushed location to head sensor");
                }
            }

            @Override
            public void onProviderDisabled(String arg0) {
            }

            @Override
            public void onProviderEnabled(String arg0) {
            }

            @Override
            public void onStatusChanged(String arg0, int arg1, Bundle bundle) {
            }
        };

        locManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
        locManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000, 1, listener);
        Log.d(TAG, "Service started");
        return START_STICKY;
    }

    private boolean pushLocationToAssociatedHeadSensor(Location location) {
        String latString = Double.toString(location.getLatitude());
        String lonString = Double.toString(location.getLongitude());
        DevicesManager devMan = CausticController.getInstance().getDevicesManager();
        CausticDevice headSensor = devMan.devices.get(devMan.associatedHeadSensorAddress);
        if (headSensor!=null) {
            headSensor.parameters.get(RCSP.Operations.HeadSensor.Configuration.playerLat.getId())
                    .setValue(latString);
            headSensor.parameters.get(RCSP.Operations.HeadSensor.Configuration.playerLon.getId())
                    .setValue(lonString);
            //Pushing values to the head sensor
            headSensor.pushToDevice(RCSP.Operations.HeadSensor.Configuration.playerLat.getId());
            headSensor.pushToDevice(RCSP.Operations.HeadSensor.Configuration.playerLon.getId());
            return true;
        }
        else {
            return false;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }
}
