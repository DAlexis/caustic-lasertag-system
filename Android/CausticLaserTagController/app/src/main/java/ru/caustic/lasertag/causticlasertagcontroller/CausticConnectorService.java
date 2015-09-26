package ru.caustic.lasertag.causticlasertagcontroller;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.widget.Toast;

public class CausticConnectorService extends Service {
    private static String TAG = "CC.CausticConnectorService";

    public CausticConnectorService() {
    }
    @Override
    public void onCreate() {
        super.onCreate();
        Toast.makeText(this, "Service created",
                Toast.LENGTH_SHORT).show();
    }
    @Override
    public IBinder onBind(Intent intent) {
        Toast.makeText(this, "Service bind", Toast.LENGTH_SHORT).show();
        return new Binder();
    }
    @Override
    public void onRebind(Intent intent) {
        super.onRebind(intent);
        Toast.makeText(this, "Service rebind", Toast.LENGTH_SHORT).show();
    }
    @Override
    public boolean onUnbind(Intent intent) {
        Toast.makeText(this, "Service unbind", Toast.LENGTH_SHORT).show();
        return super.onUnbind(intent);
    }
    @Override
    public void onDestroy() {
        super.onDestroy();
        Toast.makeText(this, "Service stopped", Toast.LENGTH_SHORT).show();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Toast.makeText(this, "Service run", Toast.LENGTH_SHORT).show();
        return super.onStartCommand(intent, flags, startId);
    }

}
