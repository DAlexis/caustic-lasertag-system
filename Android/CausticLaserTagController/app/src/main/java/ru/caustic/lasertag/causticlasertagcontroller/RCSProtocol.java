package ru.caustic.lasertag.causticlasertagcontroller;

import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;
import java.util.Map;
import java.util.TreeMap;

/**
 * Created by alexey on 18.09.15.
 */
public class RCSProtocol {
    public static final int DEVICE_TYPE_RIFLE = 1;
    public static final int DEVICE_TYPE_HEAD_SENSOR = 2;

    private static final String TAG = "RCSProtocol";

    public static class ParametersContainer {
        private Map<Integer, Parameter> allParameters = new TreeMap<Integer, Parameter>();

        public void add(String name, int type, int id) {
            allParameters.put(id, new Parameter(name, type, id));
        }

        public Parameter get(int id){
            return allParameters.get(id);
        }

        public int serialize(int id, byte[] memory, int position, int maxPosition) {
            Parameter par = allParameters.get(id);
            if (par == null)
                return 0;
            int size = par.size();
            if (maxPosition - position < size + 3)
                return 0;
            memory[position++] = (byte) size;
            MemoryUtils.uint16ToByteArray(memory, position, id);
            position += 2;
            par.serialize(memory, position);
            return size + 3;
        }

        /**
         * Read one parameter from binary stream
         * @param memory Binary stream
         * @param position start position for reading
         * @param size maximal index value allowed for memory
         * @return Count of bytes parsed. If parsing is impossible, 0. If id is not registered, nothing will be added
         */
        public int readOneParamter(byte[] memory, int position, int size)
        {
            int readed = 0;
            int argSize = MemoryUtils.byteToUnsignedByte(memory[position]);
            if (size < argSize + 3) {
                Log.e(TAG, "binary stream seems to be broken: unexpected end of stream");
                return 0;
            }
            readed++;
            int id = MemoryUtils.bytesArrayToUint16(memory, position+readed);
            readed += 2;

            // @todo Add here id 'set' bits removing

            Parameter par = allParameters.get(id);
            if (par != null) {
                par.parse(memory, position + readed);
            } else {
                Log.e(TAG, "Unknown parameter id: " + id);
            }
            return argSize + 3;
        }

        public void disatchStream(byte[] memory, int offset, int size)
        {
            int notParsed = size;
            for (int i = 0; i<size; )
            {
                int result = readOneParamter(memory, offset+i, notParsed);
                if (result == 0)
                    break;
                i += result;
                notParsed -= result; /// @todo test it
            }
        }
    }

    public static class Parameter {

        public static final int TYPE_UINT_PARAMETER  = 1; // uint16_t
        public static final int TYPE_INT_PARAMETER   = 2; // int16_t
        public static final int TYPE_TIME_INTERVAL   = 3; // uint32_t
        public static final int TYPE_FLOAT_PARAMETER = 4; // float
        public static final int TYPE_BOOL_PARAMETER  = 6; // bool
        public static final int TYPE_DEVICE_NAME     = 7; // char[20]
        public static final int TYPE_DEVICE_ADDRESS  = 8; // uint8_t[3]
        public static final int TYPE_MT2_ID          = 9; // uin8_t


        public static final int DEVICE_NAME_LENGTH   = 20;

        private final String name;
        private final int type;
        private final int id;

        private String value;
/*
        // This is for support of big/little endian for uint
        private static int majorDigit = 0;
        private static int leastDigit = 1;
*/

        public Parameter(String _name, int _type, int _id) {
            name = _name;
            type = _type;
            id = _id;
            switch (type) {
                case TYPE_UINT_PARAMETER:
                    value = "0";
                    break;
                case TYPE_INT_PARAMETER:
                    value = "0";
                    break;
                case TYPE_TIME_INTERVAL:
                    value = "0";
                    break;
                case TYPE_FLOAT_PARAMETER:
                    value = "0.0";
                    break;
                case TYPE_BOOL_PARAMETER:
                    value = "false";
                    break;
                case TYPE_DEVICE_NAME:
                    value = "undefined name";
                    break;
                case TYPE_DEVICE_ADDRESS:
                    value = "0.0.0";
                    break;
                case TYPE_MT2_ID:
                    value = "0";
                    break;
            }
        }

        public void parse(byte[] memory, int position) {
            switch (type) {
                case TYPE_UINT_PARAMETER: {
                    int result = MemoryUtils.bytesArrayToUint16(memory, position);
                    value = Integer.toString(result);
                }
                    break;
                case TYPE_INT_PARAMETER: {
                    int result = MemoryUtils.bytesArrayToInt16(memory, position);
                    value = Integer.toString(result);
                }
                    break;
                case TYPE_TIME_INTERVAL:
                    break;
                case TYPE_FLOAT_PARAMETER: {
                    byte tmp[] = Arrays.copyOfRange(memory, position, position + 4);
                    float f = ByteBuffer.wrap(tmp).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                    value = Float.toString(f);
                }
                    break;
                case TYPE_BOOL_PARAMETER:
                    break;
                case TYPE_DEVICE_NAME:
                    value = "";
                    for (int i=0; i<19; i++) {
                        if (memory[position+i] == 0)
                            break;
                        value += (char) MemoryUtils.byteToUnsignedByte(memory[position+i]);
                    }
                    break;
                case TYPE_DEVICE_ADDRESS:

                    break;
                case TYPE_MT2_ID:
                    int id = MemoryUtils.byteToUnsignedByte(memory[position]);
                    value = Integer.toString(id);
                    break;
            }
        }

        public void setValue(String value) {
            this.value = value;
        }

        public String getValue() {
            return value;
        }

        public void serialize(byte[] memory, int position)
        {
            switch (type) {
                case TYPE_UINT_PARAMETER: {
                    int i = Integer.parseInt(value);
                    MemoryUtils.uint16ToByteArray(memory, position, i);
                }
                    break;
                case TYPE_INT_PARAMETER: {
                    int val = Integer.parseInt(value);
                    MemoryUtils.int16ToByteArray(memory, position, val);
                }
                    break;
                case TYPE_TIME_INTERVAL:
                    break;
                case TYPE_FLOAT_PARAMETER:

                    break;
                case TYPE_BOOL_PARAMETER:
                    break;
                case TYPE_DEVICE_NAME:
                    int j = 0;
                    for (char ch : value.toCharArray()){
                        memory[position + j++] = (byte) ch;
                        if (j == 19)
                            break;
                    }
                    memory[position + j] = 0;
                    break;
                case TYPE_DEVICE_ADDRESS:

                    break;
                case TYPE_MT2_ID:
                    memory[position] = (byte) Integer.parseInt(value);
                    break;
            }
        }

        public int size()
        {
            switch (type) {
                case TYPE_UINT_PARAMETER:
                    return 2;
                case TYPE_INT_PARAMETER:
                    return 2;
                case TYPE_TIME_INTERVAL:
                    return 4;
                case TYPE_FLOAT_PARAMETER:
                    return 4;
                case TYPE_BOOL_PARAMETER:
                    break;
                case TYPE_DEVICE_NAME:
                    return 20;
                case TYPE_DEVICE_ADDRESS:
                    return 3;
                case TYPE_MT2_ID:
                    return 1;

            }
            return 0;
        }

        public String getName() {
            return name;
        }

        public int getId() {
            return id;
        }
    }

    class AnyDevice {

    }
}
