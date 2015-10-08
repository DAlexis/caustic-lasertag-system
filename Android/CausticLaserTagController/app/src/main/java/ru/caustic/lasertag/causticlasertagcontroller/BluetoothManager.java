package ru.caustic.lasertag.causticlasertagcontroller;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Handler;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.Set;
import java.util.UUID;

/**
 * Created by alexey on 15.09.15.
 */
public class BluetoothManager {

    public static final int BLUETOOTH_ENABLED = 0;
    public static final int BLUETOOTH_DISABLED = 1;
    public static final int BLUETOOTH_NOT_SUPPORTED = 2;

    public static final int RECIEVE_MESSAGE = 1;

    public static final int REQUEST_ENABLE_BT = 1;

    private static BluetoothManager ourInstance = new BluetoothManager();
    private static final String TAG = "CC.BluetoothManager";

    private BluetoothAdapter btAdapter = null;
    private BluetoothSocket btSocket = null;
    private ConnectedThread mConnectedThread;

    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");

    private String address = "";
    private String deviceName = "";

    public static final int BT_NOT_CONNECTED = 0;
    public static final int BT_ESTABLISHING  = 1;
    public static final int BT_CONNECTED     = 2;

    private int status = BT_NOT_CONNECTED;

    private BluetoothManager() {
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        onConnectionClosed();
    }

    private void onConnectionClosed()
    {
        address = "";
        deviceName = "Bridge not connected";
        status = BT_NOT_CONNECTED;
    }

    public static BluetoothManager getInstance() {
        return ourInstance;
    }

    public int getStatus() {
        return this.status;
    }

    public void setRXHandler(Handler _handler) {
        mConnectedThread.handler = _handler;
    }

    public Intent getIntentEnable() {
        return new Intent(btAdapter.ACTION_REQUEST_ENABLE);
    }

    public Set<BluetoothDevice> getPairedDevicesSet()
    {
        return btAdapter.getBondedDevices();
    }

    public boolean isConnected() {
        return address != "";
    }

    public boolean connect(String _address, String _deviceName, ConnectionDoneListener listener) {
        if (_address == "")
            return false;
        if (status == BT_ESTABLISHING)
            return false;
        if (status == BT_CONNECTED)
            disconnect();

        status = BT_ESTABLISHING;
        deviceName = _deviceName;
        address = _address;

        Thread asyncConnect = new AsyncConnect(listener);
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

    public boolean sendData(byte[] message) {
        if (!isConnected()) {
            Log.e(TAG, "Attempt to send data without a connection!");
            return false;
        }
        mConnectedThread.write(message);
        return true;
    }

    /* Call this from the main activity to shutdown the connection */
    public void cancel() {
        try {
            btSocket.close();
        } catch (IOException e) { }
    }

    public String getDeviceName() {
        return deviceName;
    }

    public interface ConnectionDoneListener {
        void onConnectionDone(boolean result);
    }

    private class AsyncConnect extends Thread {

        private final ConnectionDoneListener listener;

        private class DefaultListener implements ConnectionDoneListener {
            @Override
            public void onConnectionDone(boolean result) {

            }
        }

        public AsyncConnect(ConnectionDoneListener listener) {
            if (listener != null)
                this.listener = listener;
            else
                this.listener = new DefaultListener();
        }

        @Override
        public void run() {
            BluetoothDevice device = btAdapter.getRemoteDevice(address);

            // Two things are needed to make a connection:
            //   A MAC address, which we got above.
            //   A Service ID or UUID.  In this case we are using the
            //     UUID for SPP.
            try {

                // THIS code ony works on ZTE!!
                // This code block is for solving "[JSR82] write: write() failed" problem.
                // See https://stackoverflow.com/questions/20078457/android-bluetoothsocket-write-fails-on-4-2-2
                btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
                Field f = btSocket.getClass().getDeclaredField("mFdHandle");
                f.setAccessible(true);
                f.set(btSocket, 0x8000);
                btSocket.close();
                Thread.sleep(1000); // Just in case the socket was really connected
                // end of that block

                btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
            /*Method m = device.getClass().getMethod("createRfcommSocket", new Class[] { int.class });
            BluetoothSocket connection = (BluetoothSocket)m.invoke(device, MY_UUID);*/

            } catch (IOException e) {
                onConnectionClosed();
                Log.e(TAG, "Fatal: socket creation failed: " + e.getMessage() + ".");
                listener.onConnectionDone(false);
                return;
            } catch (Exception e1) {
                onConnectionClosed();
                Log.e(TAG, "Reset Failed", e1);
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
                    listener.onConnectionDone(false);
                    return;
                } catch (IOException e2) {
                    Log.e(TAG, "Fatal: unable to close socket during connection failure" + e2.getMessage() + ".");
                    listener.onConnectionDone(false);
                    return;
                }
            }

            // Create a data stream so we can talk to server.
            Log.d(TAG, "Creating data stream...");

            mConnectedThread = new ConnectedThread(btSocket);
            mConnectedThread.start();
            status = BT_CONNECTED;
            listener.onConnectionDone(true);
        }
    }

    private class ConnectedThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        public Handler handler;

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
            int bytes; // bytes returned from read()

            // Keep listening to the InputStream until an exception occurs
            for (;;) {
                try {
                    // Read from the InputStream
                    bytes = mmInStream.read(buffer);
                    byte toReceiver[] = buffer.clone();
                    if (handler != null) {
                        handler.obtainMessage(RECIEVE_MESSAGE, bytes, -1, toReceiver).sendToTarget();
                    } else {
                        Log.e(TAG, "Handler is not set!");
                    }

                } catch (IOException e) {
                    Log.d(TAG, "run died due to exception!");
                    break;
                }
            }
        }

        /* Call this from the main activity to send data to the remote device */
        public void write(byte[] message) {
            try {
                mmOutStream.write(message);
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
}
