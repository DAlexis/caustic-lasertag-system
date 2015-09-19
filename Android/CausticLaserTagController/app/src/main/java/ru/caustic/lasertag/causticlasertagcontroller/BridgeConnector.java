package ru.caustic.lasertag.causticlasertagcontroller;

import android.os.Handler;
import android.util.Log;

/**
 * Created by alexey on 18.09.15.
 */
public class BridgeConnector {

    public static class DeviceAddress {
        public int[] address = new int[3];

        public void deserialize(byte[] memory, int position) {
            address[0] = MemoryUtils.byteToUnsignedByte(memory[position]);
            address[1] = MemoryUtils.byteToUnsignedByte(memory[position+1]);
            address[2] = MemoryUtils.byteToUnsignedByte(memory[position+2]);
        }

        public int serialize(byte[] memory, int offset)
        {
            address[offset + 0] = (byte) address[0];
            address[offset + 1] = (byte) address[1];
            address[offset + 2] = (byte) address[2];
            return sizeof();
        }

        public String toString() {
            return address[0] + "." + address[1] + "." + address[2];
        }

        public static int sizeof() {
            return 3;
        }
    }

    public interface IncomingPackagesListener {
        void getData(DeviceAddress address, byte[] data, int offset, int size);
    }

    public static final int MESSAGE_LEN_MAX = 50;
    public static final int MESSAGE_OUT_LEN_MAX = 23;


    private static String TAG = "BridgeConnector";
    private byte[] incoming = new byte[MESSAGE_LEN_MAX];
    private int position = 0;
    private int currentMsgLength = 0;

    private IncomingPackagesListener packagesReceiver = null;

    public void setReceiver(IncomingPackagesListener packagesReceiver) {
        this.packagesReceiver = packagesReceiver;
    }

    public void sendMessage(DeviceAddress addr, byte[] data, int size) {
        int messageSize = size + 1 + DeviceAddress.sizeof();
        byte[] message = new byte[messageSize];
        addr.serialize(message, 1);
        BluetoothManager.getInstance().sendData(message);
    }

    BridgeConnector() {
        BluetoothManager.getInstance().setRXHandler(new Handler() {
            public void handleMessage(android.os.Message msg) {
                switch (msg.what) {
                    case BluetoothManager.RECIEVE_MESSAGE:                    // если приняли сообщение в Handler
                        byte[] readBuf = (byte[]) msg.obj;
                        if (position == 0) {
                            currentMsgLength = MemoryUtils.byteToUnsignedByte(readBuf[0]);
                            // @todo Add check for incorrect length
                        }

                        // iterating by incoming data. It may contain many messages, so we should process it one by one
                        for (int i = 1; i < msg.arg1; ) {
                            // Copying message to incoming
                            for (; i < msg.arg1 && position < currentMsgLength; i++, position++) {
                                incoming[position] = readBuf[i];
                            }

                            // If we have the whole message, we need call
                            if (position == currentMsgLength) {
                                DeviceAddress addr = new DeviceAddress();
                                addr.deserialize(incoming, 0);
                                if (packagesReceiver != null) {
                                    int headerSize = 1 + DeviceAddress.sizeof();
                                    packagesReceiver.getData(addr, incoming, headerSize, currentMsgLength-headerSize);
                                } else {
                                    Log.e(TAG, "Packages receiver is not set!");
                                }
                            }
                        }
                        break;
                }
            }
        });
    }



}