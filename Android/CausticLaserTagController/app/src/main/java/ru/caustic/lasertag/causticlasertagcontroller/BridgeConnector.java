package ru.caustic.lasertag.causticlasertagcontroller;

import android.os.Handler;
import android.util.Log;

/**
 * Created by alexey on 18.09.15.
 */
public class BridgeConnector {

    public static class Broadcast {
        public static DeviceAddress anyDevice = new DeviceAddress(255, 255, 255);
    }

    public static class DeviceAddress {
        public int[] address = new int[3];

        public DeviceAddress() {
            address[0] = address[1] = address[2] = 0;
        }

        public DeviceAddress(int a1, int a2, int a3) {
            address[0] = a1;
            address[1] = a2;
            address[2] = a3;
        }

        public DeviceAddress(String addressStr) {
            String[] addressArr = addressStr.split("\\.");
            address[0] = Integer.parseInt(addressArr[0]);
            address[1] = Integer.parseInt(addressArr[1]);
            address[2] = Integer.parseInt(addressArr[2]);
            normalize();
        }

        public DeviceAddress(DeviceAddress addr) {
            address[0] = addr.address[0];
            address[1] = addr.address[1];
            address[2] = addr.address[2];
        }

        public void deserialize(byte[] memory, int position) {
            address[0] = MemoryUtils.byteToUnsignedByte(memory[position]);
            address[1] = MemoryUtils.byteToUnsignedByte(memory[position+1]);
            address[2] = MemoryUtils.byteToUnsignedByte(memory[position+2]);
        }

        public int serialize(byte[] memory, int offset)
        {
            memory[offset + 0] = (byte) address[0];
            memory[offset + 1] = (byte) address[1];
            memory[offset + 2] = (byte) address[2];
            return sizeof();
        }

        public String toString() {
            return address[0] + "." + address[1] + "." + address[2];
        }

        public static int sizeof() {
            return 3;
        }

        private void normalize() {
            for (int i=0; i<3; i++) {
                if (address[i] < 0)
                    address[i] = 0;
                else if (address[i] > 255)
                    address[i] = 255;
            }
        }
    }

    public interface IncomingPackagesListener {
        void getData(DeviceAddress address, byte[] data, int offset, int size);
    }

    private static BridgeConnector ourInstance = new BridgeConnector();
    public static BridgeConnector getInstance() {
        return ourInstance;
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

        // Putting message size
        message[0] = (byte) messageSize;
        // Then putting target address
        addr.serialize(message, 1);
        // Then putting message
        for (int i= 0; i<size; i++) {
            message[1 + DeviceAddress.sizeof() + i] = data[i];
        }
        BluetoothManager.getInstance().sendData(message);
        //BluetoothManager.getInstance().sendData("TEST\r\n".getBytes());
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

    public static class Broadcasts
    {
        public static final DeviceAddress any = new DeviceAddress("255.255.255");
        public static final DeviceAddress headSensors = new DeviceAddress("255.255.4");
        public static boolean isBroadcast(DeviceAddress addr)
        {
            return (addr == any || addr == headSensors);
        }
    }
}
