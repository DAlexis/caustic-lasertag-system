package ru.caustic.rcspcore;

import android.util.Log;

import java.util.ArrayList;
import java.util.Arrays;

/**
 * This class contains realization of Android < = > Caustic bridge protocol
 * It produce bluetooth packages with its header and parse this packages
 */
public class BridgeDriver {
    public interface IBluetoothListener {
        void receiveBluetoothMessage(byte[] buffer, int size);
    }

    public interface IBluetoothManager {
        int RECEIVE_MESSAGE = 1;

        void setListener(IBluetoothListener listener);

        boolean sendData(byte[] message);

    }
    public static class Broadcast {
        public static RCSP.DeviceAddress anyDevice = new RCSP.DeviceAddress(255, 255, 255);
    }

    public interface IncomingPackagesListener {
        void getData(RCSP.DeviceAddress address, byte[] data, int offset, int size);
    }

    public static final int MESSAGE_LEN_MAX = 250;
    public static final int MESSAGE_OUT_LEN_MAX = 23;

    public void setReceiver(IncomingPackagesListener packagesReceiver) {
        this.packagesReceiver = packagesReceiver;
    }

    public void sendMessage(RCSP.DeviceAddress addr, byte[] data, int size) {
        BluetoothMessage msg = new BluetoothMessage();
        msg.address = addr;
        msg.setPayload(data, size);
        msg.setProperChecksum();
        byte[] serialized = msg.serialize();
        bluetoothManager.sendData(serialized);

        /* int messageSize = size + 1 + RCSP.DeviceAddress.sizeof();
        byte[] message = new byte[messageSize];

        // Putting message size
        message[0] = (byte) messageSize;
        // Then putting target address
        addr.serialize(message, 1);
        // Then putting message
        for (int i = 0; i < size; i++) {
            message[1 + RCSP.DeviceAddress.sizeof() + i] = data[i];
        }

        bluetoothManager.sendData(message);*/
    }

    public void init(IBluetoothManager bluetoothManager) {
        this.bluetoothManager = bluetoothManager;
        bluetoothManager.setListener(new IBluetoothListener() {
            @Override
            public void receiveBluetoothMessage(byte[] buffer, int size) {
                parseBluetoothMessage(buffer, size);
            }
        });
    }

    public static class Broadcasts {
        public static final RCSP.DeviceAddress any = new RCSP.DeviceAddress("255.255.255");
        public static final RCSP.DeviceAddress anyGameDevice = new RCSP.DeviceAddress("255.255.254");
        public static final RCSP.DeviceAddress headSensors = new RCSP.DeviceAddress("255.255.4");
        public static final RCSP.DeviceAddress headSensorsRed = new RCSP.DeviceAddress("255.255.0");
        public static final RCSP.DeviceAddress headSensorsBlue = new RCSP.DeviceAddress("255.255.1");
        public static final RCSP.DeviceAddress headSensorsYellow = new RCSP.DeviceAddress("255.255.2");
        public static final RCSP.DeviceAddress headSensorsGreen = new RCSP.DeviceAddress("255.255.3");


        public static boolean isBroadcast(RCSP.DeviceAddress addr) {
            return addr == any
                    || addr == anyGameDevice
                    || addr == headSensors
                    || addr == headSensorsRed
                    || addr == headSensorsBlue
                    || addr == headSensorsYellow
                    || addr == headSensorsGreen;
        }
    }

    // Private
    private static class BluetoothMessage {
        RCSP.DeviceAddress address = new RCSP.DeviceAddress();
        int totalLength = headerSize;
        int checksum = 0;
        byte[] payload;

        int payloadLength() { return totalLength - headerSize; }

        boolean parse(ArrayList<Byte> data) {
            if (data.size() < 1)
                return false;
            int len = MemoryUtils.byteToUnsignedByte(data.get(0));
            if (data.size() != len)
                return false;
            totalLength = len;
            checksum = MemoryUtils.byteToUnsignedByte(data.get(1));
            address.deserialize(data, 2);

            payload = new byte[data.size() - headerSize];
            for (int i = headerSize; i < data.size(); i++)
                payload[i - headerSize] = data.get(i);

            return true;
        }

        byte[] serialize() {
            byte[] message = new byte[totalLength];
            // Putting message size
            message[0] = (byte) totalLength;
            message[1] = MemoryUtils.unsignedByteToByte(checksum);
            // Then putting target address
            address.serialize(message, 2);
            // Then putting message
            for (int i = 0; i < payloadLength(); i++) {
                message[headerSize + i] = payload[i];
            }
            return message;
        }

        void setProperChecksum() {
            checksum = getChecksum();
        }

        int getChecksum()
        {
            int result = 0;
            result += totalLength;
            result += address.address[0];
            result += address.address[1];
            result += address.address[2];
            result %= 256;
            for (int i=0; i<payloadLength(); i++) {
                result += payload[i];
            }
            result %= 256;
            return result;
        }

        boolean isChecksumCorrect()
        {
            return checksum == getChecksum();
        }

        void setPayload(byte[] data, int size) {
            totalLength = headerSize + size;
            payload = new byte[size];
            for (int i=0; i<size; i++)
                payload[i] = data[i];
        }

        public static int headerSize = 2 + RCSP.DeviceAddress.sizeof();
    }

    // Private functions
    private void parseBluetoothMessage(byte[] buffer, int size) {
        if (size >= MESSAGE_LEN_MAX) {
            Log.e(TAG, "Too long bluetooth message");
            return;
        }

        long now = System.currentTimeMillis();
        if (now - lastMessageTime > 1000)
            resetReceiver();
        lastMessageTime = now;

        BluetoothMessage msg = new BluetoothMessage();
        for (int i = 0; i < size; i++) {
            incoming.add(buffer[i]);
            if (msg.parse(incoming)) {
                if (msg.isChecksumCorrect())
                    packagesReceiver.getData(msg.address, msg.payload, 0, msg.payloadLength());
                resetReceiver();
            }
        }
        /*
        for (int i = 0; i < size; ) {


            // Iterating by incoming buffer. One iteration ends when incoming message is full

            if (position == 0) {
                // Incoming is empty at the moment
                currentMsgLength = MemoryUtils.byteToUnsignedByte(buffer[i]);
                // @todo Add check for incorrect totalLength -> is this enough?
                if (currentMsgLength > MESSAGE_LEN_MAX)
                    return;
            }

            for (; i < size && position < currentMsgLength; i++, position++) {
                incoming.add(buffer[i]);
            }

            if (position == currentMsgLength) {
                // We have received full message now
                RCSP.DeviceAddress addr = new RCSP.DeviceAddress();

                // Creating static array from ArrayList to process it
                byte receivedData[] = new byte[incoming.size()];
                for (int j = 0; j < incoming.size(); j++) {
                    receivedData[j] = incoming.get(j);
                }

                addr.deserialize(receivedData, 2);
                if (packagesReceiver != null) {
                    int headerSize = 2 + RCSP.DeviceAddress.sizeof();
                    int checksum = MemoryUtils.byteToUnsignedByte(incoming.get(2));
                    byte[] toReceiver = Arrays.copyOfRange(receivedData, headerSize, currentMsgLength);
                    packagesReceiver.getData(addr, toReceiver, 0, currentMsgLength - headerSize);
                } else {
                    Log.e(TAG, "Packages receiver is not set!");
                }
                resetReceiver();
            }
        }*/
    }

    private void resetReceiver() {
        position = 0;
        incoming.clear();
    }

    // Private variables
    private static String TAG = "BridgeDriver";
    //private byte[] incoming = new byte[MESSAGE_LEN_MAX];
    private ArrayList<Byte> incoming = new ArrayList<>();
    private int position = 0;
    private int currentMsgLength = 0;
    private long lastMessageTime = 0;

    private IncomingPackagesListener packagesReceiver = null;
    private static IBluetoothManager bluetoothManager = null;
}
