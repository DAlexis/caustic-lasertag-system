package org.ltcaustic.rcspandroid;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.content.SharedPreferences;
import android.util.Log;

import org.ltcaustic.rcspcore.BridgeDriver;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

import static android.content.Context.MODE_PRIVATE;


/**
 * Created by alexey on 15.09.15.
 */
public class BluetoothManager implements BridgeDriver.IBluetoothManager {

    public static final String PREFERENCE_AUTOCONNECT = "bluetooth_autoconnect";
    public static final String PREFERENCE_DEFAULT_ADDRESS = "bluetooth_default_address";

    public static final int BLUETOOTH_ENABLED = 0;
    public static final int BLUETOOTH_DISABLED = 1;
    public static final int BLUETOOTH_NOT_SUPPORTED = 2;

    public static final int RECEIVE_MESSAGE = 1;

    public static final int REQUEST_ENABLE_BT = 1;

    public static final int BT_NOT_CONNECTED = 0;
    public static final int BT_ESTABLISHING  = 1;
    public static final int BT_CONNECTED     = 2;

    public int getStatus() {
        return this.status;
    }

    @Override
    public void setListener(BridgeDriver.IBluetoothListener listener) {
        incomingMsgListener = listener;
    }
    @Override
    public boolean sendData(byte[] message) {
        if (!isConnected()) {
            Log.e(TAG, "Attempt to send data without a connection!");
            return false;
        }
        mConnectedThread.write(message);
        return true;
    }

    public Intent getIntentEnable() {
        return new Intent(btAdapter.ACTION_REQUEST_ENABLE);
    }

    public Set<BluetoothDevice> getPairedDevicesSet()
    {
        return btAdapter.getBondedDevices();
    }

    /**
     * This function will proceed connection to bluetooth device if:
     * - shared preferences tells we need it,
     * - we are not connected yet
     *
     * @param activity
     * @param connectionProcessListener New listener. Previous listener will not be restored!
     */
    public void doAutoconnectIfNeeded(Activity activity, ConnectionProcessListener connectionProcessListener)
    {
        if (CausticInitializer.getInstance().bluetooth().isConnected())
            return;
        SharedPreferences sharedPref = activity.getPreferences(MODE_PRIVATE);
        boolean autoconnect = sharedPref.getBoolean(BluetoothManager.PREFERENCE_AUTOCONNECT, false);
        String defaultDeviceAddress = sharedPref.getString(BluetoothManager.PREFERENCE_DEFAULT_ADDRESS, "");
        if (!autoconnect || defaultDeviceAddress == "")
            return;

        // Finding name
        String name = "";

        for(BluetoothDevice device: getPairedDevicesSet()) {
            if (device.getAddress().equals(defaultDeviceAddress))
                name = device.getName();
        }
        setConnectionProcessListener(connectionProcessListener);
        connect(defaultDeviceAddress, name);
    }

    public boolean isConnected() {
        return status == BT_CONNECTED;
    }
    public void setConnectionProcessListener(ConnectionProcessListener listener) {
        this.listener = listener;
    }
    public boolean connect(String address, String deviceName) {
        if (address == "")
            return false;
        if (status == BT_ESTABLISHING)
            return false;
        if (status == BT_CONNECTED) {
            // Check if connection is already done
            if (this.address == address) {
                if (listener != null)
                    listener.onConnectionDone(true);
                return true;
            }
            disconnect();
        }

        this.status = BT_ESTABLISHING;
        this.deviceName = deviceName;
        this.address = address;

        Thread asyncConnect = new AsyncConnect();
        asyncConnect.start();
        return true;
    }
    public boolean disconnect() {
        if (!isConnected())
            return false;
        Log.d(TAG, "Disconnecting");

        try {
            btSocket.close();
        } catch (IOException e) {
            Log.e(TAG, "Fatal: failed to close socket." + e.getMessage() + ".");
            return false;
        }
        onConnectionClosed();
        return true;
    }

    public int checkBluetoothState() {
        // Check for Bluetooth support and then check to make sure it is turned on
        // Emulator doesn't support Bluetooth and will return null
        if (btAdapter == null) {
            Log.e(TAG, "Bluetooth not supported");
            return BLUETOOTH_NOT_SUPPORTED;
        } else {
            if (btAdapter.isEnabled()) {
                Log.d(TAG, "Bluetooth enabled.");
                return BLUETOOTH_ENABLED;
            } else {
                Log.d(TAG, "Bluetooth disabled, so need to be turned on");
                return BLUETOOTH_DISABLED;
            }
        }
    }

    /* Call this from the main activity to shutdown the connection */
    public void cancel() {
        if (btSocket == null)
            return;
        try {
            btSocket.close();
        } catch (IOException e) { }
    }

    public String getConnectedDeviceName() {
        return deviceName;
    }
    public String getConnectedAddress() { return address; }

    public interface ConnectionProcessListener {
        void onConnecting(final String deviceName);
        void onConnectionDone(boolean result);
    }

    // Private
    private class AsyncConnect extends Thread {

        public AsyncConnect() {
        }

        @Override
        public void run() {
            if (listener != null)
                listener.onConnecting(deviceName);
            BluetoothDevice device = btAdapter.getRemoteDevice(address);

            // Two things are needed to make a connection:
            //   A MAC address, which we got above.
            //   A Service ID or UUID.  In this case we are using the
            //     UUID for SPP.
            try {
                Thread.sleep(100);
                // THIS code ony works on ZTE!!
                // This code block is for solving "[JSR82] write: write() failed" problem.
                // See https://stackoverflow.com/questions/20078457/android-bluetoothsocket-write-fails-on-4-2-2
/*
                btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
                Field f = btSocket.getClass().getDeclaredField("mFdHandle");
                f.setAccessible(true);
                f.set(btSocket, 0x8000);
                btSocket.close();
                Thread.sleep(1000); // Just in case the socket was really connected
                */
                // end of that block

                btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
            /*Method m = device.getClass().getMethod("createRfcommSocket", new Class[] { int.class });
            BluetoothSocket connection = (BluetoothSocket)m.invoke(device, MY_UUID);*/

            } catch (IOException e) {
                onConnectionClosed();
                Log.e(TAG, "Fatal: socket creation failed: " + e.getMessage() + ".");
                if (listener != null)
                    listener.onConnectionDone(false);
                return;
            } catch (Exception e1) {
                onConnectionClosed();
                Log.e(TAG, "Reset Failed", e1);
                if (listener != null)
                    listener.onConnectionDone(false);
                return;
            }

            // Discovery is resource intensive.  Make sure it isn't going on
            // when you attempt to connect and pass your message.
            btAdapter.cancelDiscovery();

            // Establish the connection.  This will block until it connects.
            Log.d(TAG, "Connecting...");
            try {
                btSocket.connect();
                Log.d(TAG, "Connection established and ready to data transfer");
            } catch (IOException e) {
                onConnectionClosed();
                Log.e(TAG, "Error during btSocket.connect(): " + e.getMessage());
                try {
                    btSocket.close();
                    if (listener != null)
                        listener.onConnectionDone(false);
                    return;
                } catch (IOException e2) {
                    Log.e(TAG, "Fatal: unable to close socket during connection failure" + e2.getMessage() + ".");
                    if (listener != null)
                        listener.onConnectionDone(false);
                    return;
                }
            }

            // Create a data stream so we can talk to server.
            Log.d(TAG, "Creating data stream...");

            mConnectedThread = new ConnectedThread(btSocket);
            mConnectedThread.start();
            status = BT_CONNECTED;
            if (listener != null)
                listener.onConnectionDone(true);
        }
    }

    private class ConnectedThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        public ConnectedThread(BluetoothSocket socket) {
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            // Get the input and output streams, using temp objects because
            // member streams are final
            try {
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) { }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        public void run() {
            byte[] buffer = new byte[256];  // buffer store for the stream

            // Keep listening to the InputStream until an exception occurs
            for (;;) {
                try {
                    // Read from the InputStream
                    int size = mmInStream.read(buffer);
                    byte toReceiver[] = buffer.clone();
                    if (incomingMsgListener != null) {
                        incomingMsgListener.receiveBluetoothMessage(toReceiver, size);
                    } else {
                        Log.e(TAG, "Incoming listener is not set!");
                    }
                    /*
                    if (handler != null) {
                        handler.obtainMessage(RECEIVE_MESSAGE, bytes, -1, toReceiver).sendToTarget();
                    } else {
                        Log.e(TAG, "Handler is not set!");
                    }*/

                } catch (IOException e) {
                    Log.d(TAG, "run died due to exception!");
                    onConnectionClosed();
                    if (listener != null)
                        listener.onConnectionDone(false);
                    break;
                }
            }
        }

        /* Call this from the main activity to send data to the remote device */
        public void write(byte[] message) {
            try {

                mmOutStream.write(message);
                Log.d(TAG, "Data seems sent");
            } catch (IOException e) {
                Log.e(TAG, "Bluetooth data sending error: " + e.getMessage() + "...");
            }
        }

        /* Call this from the main activity to shutdown the connection */
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) { }
        }
    }

    public BluetoothManager() {
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        onConnectionClosed();
    }

    private void onConnectionClosed() {
        address = "";
        deviceName = "Bridge not connected";
        status = BT_NOT_CONNECTED;
    }

    // Private variables

    private static BluetoothManager ourInstance = new BluetoothManager();
    private static final String TAG = "CC.BluetoothManager";

    private int status = BT_NOT_CONNECTED;

    private BluetoothAdapter btAdapter = null;
    private BluetoothSocket btSocket = null;
    private ConnectedThread mConnectedThread;

    private BridgeDriver.IBluetoothListener incomingMsgListener = null;
    private ConnectionProcessListener listener = null;

    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");

    private String address = "";
    private String deviceName = "";
}
