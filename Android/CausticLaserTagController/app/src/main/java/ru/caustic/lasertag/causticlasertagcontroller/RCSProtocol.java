package ru.caustic.lasertag.causticlasertagcontroller;

import android.util.Log;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Map;
import java.util.TreeMap;

/**
 * Created by alexey on 18.09.15.
 */
public class RCSProtocol {

    public static final int DEVICE_TYPE_RIFLE = 1;
    public static final int DEVICE_TYPE_HEAD_SENSOR = 2;

    private static final String TAG = "CC.RCSProtocol";

    public enum OperationCodeType { SET_OBJECT, OBJECT_REQUEST, CALL_REQUEST, RESERVED }

    private static final int OperationCodeMask = 0b0011_1111_1111_1111;

    public static int removeOperationTypeBits(int id) {
        return id & OperationCodeMask;
    }

    public static int makeOperationCodeType(int id, OperationCodeType type) {
        id &= OperationCodeMask;
        switch (type) {
            case SET_OBJECT:
                id |= (1<<14);
                break;
            case OBJECT_REQUEST:
                id |= (1<<15);
                break;
            case CALL_REQUEST:
                break;
            case RESERVED:
                id |= (1<<14) | (1<<15);
                break;
        }
        return id;
    }

    public static OperationCodeType dispatchOperationCodeType(int id) {
        boolean b14 = (id & (1<<14)) != 0;
        boolean b15 = (id & (1<<15)) != 0;

        if (!b14 && !b15)
            return OperationCodeType.CALL_REQUEST;
        if (!b14 && b15)
            return OperationCodeType.OBJECT_REQUEST;
        if (b14 && !b15)
            return OperationCodeType.SET_OBJECT;

        // if (b14 && b15)
        return OperationCodeType.RESERVED;
    }

    public static class ParametersContainer {
        private Map<Integer, RCSPParameterGroup> allParameters = new TreeMap<Integer, RCSPParameterGroup>();

        public void add(RCSPParameterGroup par) {
            allParameters.put(par.getId(), par);
        }

        public RCSPParameterGroup get(int id){
            return allParameters.get(id);
        }

        public int serializeOneParameter(int id, byte[] memory, int position, int maxPosition) {
            RCSPAnyGroup par = allParameters.get(id);
            if (par == null)
                return 0;
            int size = par.size();
            if (maxPosition - position < size + 3)
                return 0;
            memory[position++] = (byte) size;
            MemoryUtils.uint16ToByteArray(memory, position, makeOperationCodeType(id, OperationCodeType.SET_OBJECT));
            position += 2;
            par.serialize(memory, position);
            return size + 3;
        }

        public static int serializeParameterRequest(int id, byte[] memory, int position, int maxPosition) {
            if (maxPosition - position < 3)
                return 0;
            memory[position++] = 0;
            MemoryUtils.uint16ToByteArray(memory, position, makeOperationCodeType(id, OperationCodeType.OBJECT_REQUEST));
            return 3;
        }

        public int serializeSetObject(int id, byte[] memory, int position, int maxPosition) {
            RCSPParameterGroup par = allParameters.get(id);
            int size = par.size();
            int totalSize = size + 3;

            if (maxPosition - position < totalSize)
                return 0;

            memory[position++] = 0;
            MemoryUtils.uint16ToByteArray(memory, position, makeOperationCodeType(id, OperationCodeType.SET_OBJECT));
            position += 2;
            par.serialize(memory, position);
            return totalSize;
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

            if (dispatchOperationCodeType(id) != OperationCodeType.SET_OBJECT)
            {
                Log.e(TAG, "Trying to dispatch " + removeOperationTypeBits(id) + " with invalid operation code type bits");
                return 0;
            }

            id = removeOperationTypeBits(id);

            RCSPAnyGroup par = allParameters.get(id);
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

    public static class RemoteFunctionsContainer {
        private Map<Integer, RCSPFunctionCallGroup> allFucntions = new TreeMap<Integer, RCSPFunctionCallGroup>();

        public void add(RCSPFunctionCallGroup par) {
            allFucntions.put(par.getId(), par);
        }

        public int serializeCall(int id, String argument, byte[] memory, int position, int maxPosition)
        {
            RCSPFunctionCallGroup func = allFucntions.get(id);
            if (func == null)
                return 0;
            int size = func.size();
            if (maxPosition - position < size + 3)
                return 0;
            memory[position++] = (byte) size;
            MemoryUtils.uint16ToByteArray(memory, position, makeOperationCodeType(id, OperationCodeType.CALL_REQUEST));
            position += 2;
            func.setArgument(argument);
            func.serialize(memory, position);
            return size + 3;
        }
    }

    public static abstract class RCSPAnyGroup extends Object {
        private int id;
        private String name;


        public RCSPAnyGroup(int _id, String _name) {
            id = _id;
            name = _name;
        }

        public abstract void deserialize(byte[] memory, int offset);
        public abstract int size();
        public abstract int serialize(byte[] memory, int offset);

        public String getName() {
            return name;
        }

        public int getId() {
            return id;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;

            RCSPAnyGroup that = (RCSPAnyGroup) o;

            return id == that.id;

        }

        @Override
        public int hashCode() {
            return id;
        }
    }

    public static abstract class RCSPParameterGroup extends RCSPAnyGroup {
        private String value;

        public RCSPParameterGroup(int _id, String _name) {
            super(_id, _name);
        }

        public String getValue() {
            return value;
        }

        public void setValue(String value) {
            this.value = value;
        }
    }

    public static abstract class RCSPFunctionCallGroup extends RCSPAnyGroup {
        public RCSPFunctionCallGroup(int _id, String _name) {
            super(_id, _name);
        }

        public abstract void setArgument(String argument);
    }

    public static class UintGroupRCSP extends RCSPParameterGroup {
        public UintGroupRCSP(int _id, String _name) {
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
    public static class IntGroupRCSP extends RCSPParameterGroup {
        public IntGroupRCSP(int _id, String _name) {
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
    public static class DevNameGroupRCSP extends RCSPParameterGroup {
        public static final String defaultName = "Name unavailable";

        public DevNameGroupRCSP(int _id, String _name) {
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
    public static class MT2IdGroupRCSP extends RCSPParameterGroup {
        public MT2IdGroupRCSP(int _id, String _name) {
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
    public static class FloatGroupRCSP extends RCSPParameterGroup {
        public FloatGroupRCSP(int _id, String _name) {
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
    public static class DevAddrGroupRCSP extends RCSPParameterGroup {
        public DevAddrGroupRCSP(int _id, String _name) {
            super(_id, _name);
            super.value = "0.0.0";
        }

        public void deserialize(byte[] memory, int offset) {
            super.value = MemoryUtils.byteToUnsignedByte(memory[offset])
                    + "." + MemoryUtils.byteToUnsignedByte(memory[offset+1])
                    + "." + MemoryUtils.byteToUnsignedByte(memory[offset+2]);
        }

        public int serialize(byte[] memory, int offset) {
            String[] address = super.value.split("\\.");
            memory[offset] = (byte) Integer.parseInt(address[0]);
            memory[offset+1] = (byte) Integer.parseInt(address[1]);
            memory[offset+2] = (byte) Integer.parseInt(address[2]);
            return size();
        }

        public int size() {
            return 3;
        }
    }

    public static class FunctionNoParsGroupRCSP extends RCSPFunctionCallGroup {
        public FunctionNoParsGroupRCSP(int _id, String _name)
        {
            super(_id, _name);
        }

        public void deserialize(byte[] memory, int offset) {

        }

        public int size() {
            return 0;
        }

        public int serialize(byte[] memory, int offset) {
            return size();
        }
        public void setArgument(String argument) {  }
    }

    public static class RCSPOperationCodes
    {
        public static class AnyDevice {
            public static class Configuration {
                public static final int DEV_TYPE_UNDEFINED = 100;
                public static final int DEV_TYPE_RIFLE = 1;
                public static final int DEV_TYPE_HEAD_SENSOR = 2;
                public static final int DEV_TYPE_BLUETOOTH_BRIDGE = 3;

                public static final int devAddr = 2000;
                public static final int deviceName = 2001;
                public static final int deviceType = 2002;
            }

            public static class Functions {
                public static final int resetToDefaults = 2100;
            }

            public static void registerParameters(ParametersContainer pars) {
                pars.add(new DevNameGroupRCSP(Configuration.deviceName, "Device name"));
                pars.add(new UintGroupRCSP(Configuration.deviceType, "Device type"));
                pars.get(Configuration.deviceType).setValue(Integer.toString(Configuration.DEV_TYPE_UNDEFINED));
            }
            public static void registerFunctions(RemoteFunctionsContainer funcs) {
                funcs.add(new FunctionNoParsGroupRCSP(Functions.resetToDefaults, "Reset to default"));
            }
        }

        public static class HeadSensor {
            public static class Functions
            {
                public static final int playerTurnOff =     1201;
                public static final int playerTurnOn =      1202;
                public static final int playerReset =       1203;
                public static final int playerRespawn =     1204;
                public static final int playerKill =        1205;
                /*
                FUNC_CODE_1P(addMaxHealth, IntParameter, 1220)
                FUNC_CODE_1P(setTeam,      TeamMT2Id,    1221)

                FUNC_CODE_1P(registerWeapon,  DeviceAddress,      1300)
                FUNC_CODE_1P(notifyIsDamager, DamageNotification, 1400)
                */
                public static final int rifleToHeadSensorHeartbeat = 1500;
            }

            public static void registerParameters(ParametersContainer pars) {

            }

            public static void registerFunctions(RemoteFunctionsContainer funcs) {
                funcs.add(new FunctionNoParsGroupRCSP(Functions.playerTurnOff, "Turn off player"));
                funcs.add(new FunctionNoParsGroupRCSP(Functions.playerTurnOn, "Turn on player"));
                funcs.add(new FunctionNoParsGroupRCSP(Functions.playerReset, "Reset players configuration to default"));
                funcs.add(new FunctionNoParsGroupRCSP(Functions.playerRespawn, "Respawn player"));
                funcs.add(new FunctionNoParsGroupRCSP(Functions.playerKill, "Kill player"));
            }
        }
        public static class Rifle {
            public static class Functions
            {

            }

            public static void registerParameters(ParametersContainer pars) {

            }

            public static void registerFunctions(RemoteFunctionsContainer funcs) {

            }
        }


    }

}
