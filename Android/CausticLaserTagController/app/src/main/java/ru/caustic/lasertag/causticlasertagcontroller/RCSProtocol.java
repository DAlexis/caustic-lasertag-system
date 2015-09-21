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
        private Map<Integer, AnyParameter> allParameters = new TreeMap<Integer, AnyParameter>();

        public void add(AnyParameter par) {
            allParameters.put(par.getId(), par);
        }

        public AnyParameter get(int id){
            return allParameters.get(id);
        }

        public int serializeOneParameter(int id, byte[] memory, int position, int maxPosition) {
            AnyParameter par = allParameters.get(id);
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
        public int deserializeOneParamter(byte[] memory, int position, int size)
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

            AnyParameter par = allParameters.get(id);
            if (par != null) {
                par.deserialize(memory, position + readed);
            } else {
                Log.e(TAG, "Unknown parameter id: " + id);
            }
            return argSize + 3;
        }

        public void deserializeStream(byte[] memory, int offset, int size)
        {
            int notParsed = size;
            for (int i = 0; i<size; )
            {
                int result = deserializeOneParamter(memory, offset + i, notParsed);
                if (result == 0)
                    break;
                i += result;
                notParsed -= result; /// @todo test it
            }
        }
    }

    public static abstract class AnyParameter {
        private int id;
        private String name;
        private String value;

        public AnyParameter(int _id, String _name) {
            id = _id;
            name = _name;
        }

        public abstract void deserialize(byte[] memory, int offset);
        public abstract int size();
        public abstract int serialize(byte[] memory, int offset);

        public String getValue() {
            return value;
        }

        public void setValue(String value) {
            this.value = value;
        }

        public String getName() {
            return name;
        }

        public int getId() {
            return id;
        }
    }

    public static class UintParameter extends AnyParameter {
        public UintParameter(int _id, String _name) {
            super(_id, _name);
            super.value = "0";
        }

        public void deserialize(byte[] memory, int offset) {
            int result = MemoryUtils.bytesArrayToUint16(memory, offset);
            super.value = Integer.toString(result);
        }

        public int serialize(byte[] memory, int offset) {
            int i = Integer.parseInt(super.value);
            MemoryUtils.uint16ToByteArray(memory, offset, i);
            return size();
        }

        public int size() {
            return 2;
        }
    }
    public static class IntParameter extends AnyParameter {
        public IntParameter(int _id, String _name) {
            super(_id, _name);
            super.value = "0";
        }

        public void deserialize(byte[] memory, int offset) {
            int result = MemoryUtils.bytesArrayToInt16(memory, offset);
            super.value = Integer.toString(result);
        }

        public int serialize(byte[] memory, int offset) {
            int val = Integer.parseInt(super.value);
            MemoryUtils.int16ToByteArray(memory, offset, val);
            return size();
        }

        public int size() {
            return 2;
        }
    }
    public static class DevNameParameter extends AnyParameter {
        public static final String defaultName = "Name unavailable";

        public DevNameParameter(int _id, String _name) {
            super(_id, _name);
            super.value = defaultName;
        }

        public void deserialize(byte[] memory, int offset) {
            super.value = "";
            for (int i=0; i<19; i++) {
                if (memory[offset+i] == 0)
                    break;
                super.value += (char) MemoryUtils.byteToUnsignedByte(memory[offset+i]);
            }
        }

        public int serialize(byte[] memory, int offset) {
            int j = 0;
            for (char ch : super.value.toCharArray()){
                memory[offset + j++] = (byte) ch;
                if (j == 19)
                    break;
            }
            memory[offset + j] = 0;
            return size();
        }

        public int size() {
            return 20;
        }
    }
    public static class MT2IdParameter extends AnyParameter {
        public MT2IdParameter(int _id, String _name) {
            super(_id, _name);
            super.value = "0";
        }

        public void deserialize(byte[] memory, int offset) {
            int id = MemoryUtils.byteToUnsignedByte(memory[offset]);
            super.value = Integer.toString(id);
        }

        public int serialize(byte[] memory, int offset) {

            memory[offset] = (byte) Integer.parseInt(super.value);
            return size();
        }

        public int size() {
            return 1;
        }
    }
    public static class FloatParameter extends AnyParameter {
        public FloatParameter(int _id, String _name) {
            super(_id, _name);
            super.value = "0.0";
        }

        public void deserialize(byte[] memory, int offset) {
            byte tmp[] = Arrays.copyOfRange(memory, offset, offset + 4);
            float f = ByteBuffer.wrap(tmp)/*.order(ByteOrder.LITTLE_ENDIAN)*/.getFloat();
            super.value = Float.toString(f);
        }

        public int serialize(byte[] memory, int offset) {
            byte tmp[] = new byte[4];
            ByteBuffer.wrap(tmp).putFloat(Float.parseFloat(super.value));
            for (int i=0; i<4; i++)
            {
                memory[offset+i] = tmp[i];
            }
            return size();
        }

        public int size() {
            return 4;
        }
    }

}
