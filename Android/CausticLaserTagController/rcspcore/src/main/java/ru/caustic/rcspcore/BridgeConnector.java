package ru.caustic.rcspcore;

import android.util.Log;

import java.util.ArrayList;
import java.util.Arrays;

/**
 * Created by alexey on 18.09.15.
 */
public class BridgeConnector {
    public interface IBluetoothListener {
        void receiveBluetoothMessage(byte[] buffer, int size);
    }

    public interface IBluetoothManager {
        int RECEIVE_MESSAGE = 1;

        void setListener(IBluetoothListener listener);

        boolean sendData(byte[] message);

    }
    public static class Broadcast {
        public static DeviceAddress anyDevice = new DeviceAddress(255, 255, 255);
    }

    public static class DeviceAddress extends Object {

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;

            DeviceAddress that = (DeviceAddress) o;

            return Arrays.equals(address, that.address);

        }

        @Override
        public int hashCode() {
            return Arrays.hashCode(address);
        }

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
            address[1] = MemoryUtils.byteToUnsignedByte(memory[position + 1]);
            address[2] = MemoryUtils.byteToUnsignedByte(memory[position + 2]);
        }

        public void deserialize(ArrayList<Byte> memory, int position) {
            address[0] = MemoryUtils.byteToUnsignedByte(memory.get(position));
            address[1] = MemoryUtils.byteToUnsignedByte(memory.get(position + 1));
            address[2] = MemoryUtils.byteToUnsignedByte(memory.get(position + 2));
        }

        public int serialize(byte[] memory, int offset) {
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
            for (int i = 0; i < 3; i++) {
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

    public static final int MESSAGE_LEN_MAX = 250;
    public static final int MESSAGE_OUT_LEN_MAX = 23;

    public void setReceiver(IncomingPackagesListener packagesReceiver) {
        this.packagesReceiver = packagesReceiver;
    }

    public void sendMessage(DeviceAddress addr, byte[] data, int size) {
        BluetoothMessage msg = new BluetoothMessage();
        msg.address = addr;
        msg.setPayload(data, size);
        msg.setProperChecksum();
        byte[] serialized = msg.serialize();
        bluetoothManager.sendData(serialized);

        /* int messageSize = size + 1 + DeviceAddress.sizeof();
        byte[] message = new byte[messageSize];

        // Putting message size
        message[0] = (byte) messageSize;
        // Then putting target address
        addr.serialize(message, 1);
        // Then putting message
        for (int i = 0; i < size; i++) {
            message[1 + DeviceAddress.sizeof() + i] = data[i];
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
        public static final DeviceAddress any = new DeviceAddress("255.255.255");
        public static final DeviceAddress anyGameDevice = new DeviceAddress("255.255.254");
        public static final DeviceAddress headSensors = new DeviceAddress("255.255.4");
        public static final DeviceAddress headSensorsRed = new DeviceAddress("255.255.0");
        public static final DeviceAddress headSensorsBlue = new DeviceAddress("255.255.1");
        public static final DeviceAddress headSensorsYellow = new DeviceAddress("255.255.2");
        public static final DeviceAddress headSensorsGreen = new DeviceAddress("255.255.3");


        public static boolean isBroadcast(DeviceAddress addr) {
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
        BridgeConnector.DeviceAddress address = new BridgeConnector.DeviceAddress();
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

        public static int headerSize = 2 + DeviceAddress.sizeof();
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
                DeviceAddress addr = new DeviceAddress();

                // Creating static array from ArrayList to process it
                byte receivedData[] = new byte[incoming.size()];
                for (int j = 0; j < incoming.size(); j++) {
                    receivedData[j] = incoming.get(j);
                }

                addr.deserialize(receivedData, 2);
                if (packagesReceiver != null) {
                    int headerSize = 2 + DeviceAddress.sizeof();
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
    private static String TAG = "BridgeConnector";
    //private byte[] incoming = new byte[MESSAGE_LEN_MAX];
    private ArrayList<Byte> incoming = new ArrayList<>();
    private int position = 0;
    private int currentMsgLength = 0;
    private long lastMessageTime = 0;

    private IncomingPackagesListener packagesReceiver = null;
    private static IBluetoothManager bluetoothManager = null;
}
