package ru.caustic.lasertag.causticlasertagcontroller;

import android.content.Context;
import android.preference.Preference;
import android.util.Log;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

/**
 * Created by alexey on 18.09.15.
 */
public class RCSProtocol {

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

    // Descriptions
    public static abstract class AnyDescription {
        protected int id;
        protected String name;


        public AnyDescription(int _id, String _name) {
            id = _id;
            name = _name;
        }

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

            AnyDescription that = (AnyDescription) o;

            return id == that.id;

        }

        @Override
        public int hashCode() {
            return id;
        }
    }
    public static class ParameterDescription extends AnyDescription {
        private final boolean editable;
        public ParameterSerializerFactory factory;
        public ParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, boolean editable, ParameterSerializerFactory factory) {
            super(id, name);
            this.editable = editable;
            this.factory = factory;
            if (descrSet != null)
                descrSet.register(this);
        }
        public boolean isEditable() {
            return editable;
        }
    }

    public static class IntParameterDescription extends ParameterDescription {
        public final int minValue;
        public final int maxValue;

        public IntParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, int minValue, int maxValue) {
            super(descrSet, id, name, true, IntParameterSerializer.factory);
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public IntParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name) {
            super(descrSet, id, name, false, IntParameterSerializer.factory);
            this.minValue = 0;
            this.maxValue = 0;
        }
    }
    public static class UintParameterDescription extends ParameterDescription {
        public final int minValue;
        public final int maxValue;

        public UintParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, int minValue, int maxValue) {
            super(descrSet, id, name, true, UintParameterSerializer.factory);
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public UintParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name) {
            super(descrSet, id, name, false, UintParameterSerializer.factory);
            this.minValue = 0;
            this.maxValue = 0;
        }
    }
    public static class UByteParameterDescription extends ParameterDescription {
        public final int minValue;
        public final int maxValue;

        public UByteParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, int minValue, int maxValue) {
            super(descrSet, id, name, true, UByteParameterSerializer.factory);
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public UByteParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, boolean editable) {
            super(descrSet, id, name, editable, UByteParameterSerializer.factory);
            this.minValue = 0;
            this.maxValue = 0;
        }
    }
    public static class TimeIntervalParameterDescription extends ParameterDescription {
        public final long minValue;
        public final long maxValue;

        public TimeIntervalParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, long minValue, long maxValue) {
            super(descrSet, id, name, true, TimeIntervalParameterSerializer.factory);
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public TimeIntervalParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name) {
            super(descrSet, id, name, false, TimeIntervalParameterSerializer.factory);
            this.minValue = 0;
            this.maxValue = 0;
        }
    }
    public static class BooleanParameterDescription extends ParameterDescription {
        public BooleanParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, boolean editable) {
            super(descrSet, id, name, editable, BoolParameterSerializer.factory);

        }
    }
    public static class MT2IdParameterDescription extends ParameterDescription {
        public final int minValue;
        public final int maxValue;

        public MT2IdParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, boolean editable, int minValue, int maxValue) {
            super(descrSet, id, name, editable, MT2IdParameterSerializer.factory);
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public MT2IdParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, boolean editable) {
            super(descrSet, id, name, editable, MT2IdParameterSerializer.factory);
            this.minValue = 1;
            this.maxValue = 127;
        }
    }
    public static class FloatParameterDescription extends ParameterDescription {
        public final float minValue;
        public final float maxValue;

        public FloatParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, boolean editable, float minValue, float maxValue) {
            super(descrSet, id, name, editable, FloatParameterSerializer.factory);
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public FloatParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, boolean editable) {
            super(descrSet, id, name, editable, FloatParameterSerializer.factory);
            this.minValue = 0.0f;
            this.maxValue = 0.0f;
        }
    }
    public static class DevNameParameterDescription extends ParameterDescription {
        public DevNameParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, boolean editable) {
            super(descrSet, id, name, editable, DevNameParameterSerializer.factory);
        }
    }
    public static class DevAddrParameterDescription extends ParameterDescription {
        public DevAddrParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, boolean editable) {
            super(descrSet, id, name, editable, DevAddrParameterSerializer.factory);
        }
    }

    // @todo Should inherit uint8_t parameter
    public static abstract class EnumParameterDescription extends UByteParameterDescription {
        public Map<String, Integer> entries = new HashMap<>();
        public ArrayList<String> namesOrdered = new ArrayList<>();

        public EnumParameterDescription(ParametersDescriptionsContainer descrSet, int id, String name, boolean editable) {
            super(descrSet, id, name, editable);
        }

        public String getCurrentValueString(int intValue) {
            for (Map.Entry<String, Integer> entry : entries.entrySet())
            {
                if (entry.getValue() == intValue)
                    return entry.getKey();
            }
            /// @todo make something there! Value can be out of enum range
            return "";
        }

        public void addElement(String name, int value) {
            entries.put(name, value);
            namesOrdered.add(name);
        }
    }

    public static class teamEnumDescription extends EnumParameterDescription {

        public teamEnumDescription(ParametersDescriptionsContainer descrSet, int id, String name) {
            super(descrSet, id, name, true);
            addElement("Red", 0);
            addElement("Blue", 1);
            addElement("Yellow", 2);
            addElement("Green", 3);
        }
    }

    public static class FunctionCallDescription extends AnyDescription {
        public FunctionCallSerializer serializer;
        public FunctionCallDescription(int _id, String _name) {
            super(_id, _name);
        }
    }

    // Factories
    public interface ParameterSerializerFactory {
        AnyParameterSerializer create(ParameterDescription descr);
    }

    // Srializers
    public static abstract class AnySerializer extends Object {

        public final AnyDescription description;

        public AnySerializer(AnyDescription descr) {
            description = descr;
        }

        public AnyDescription getDescription() {
            return description;
        }

        public abstract void deserialize(byte[] memory, int offset);
        public abstract int size();
        public abstract int serialize(byte[] memory, int offset);

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;

            AnyParameterSerializer that = (AnyParameterSerializer) o;

            return description.id == that.description.id;
        }

        @Override
        public int hashCode() {
            return description.id;
        }
    }

    public static abstract class FunctionCallSerializer extends AnySerializer {

        public FunctionCallSerializer(FunctionsContainer2 container, int id, String name)
        {
            super(new FunctionCallDescription(id, name));
            if (container != null)
                container.register(this);
        }

        public abstract void setArgument(String argument);
    }
    public static class FunctionCallNoParsSerializer extends FunctionCallSerializer {
        public FunctionCallNoParsSerializer(FunctionsContainer2 container, int id, String name)
        {
            super(container, id, name);
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

    public static abstract class AnyParameterSerializer extends AnySerializer {
        protected boolean isSynchronized = false;
        protected String value = "";

        public boolean isSync() {
            return isSynchronized;
        }

        public AnyParameterSerializer(ParameterDescription descr) {
            super(descr);
        }

        public String getValue() {
            return value;
        }

        public void setValue(String value) {
            if (!value.equals(this.value))
                isSynchronized = false;
            this.value = value;
        }

        public ParameterDescription getDescription() {
            return (ParameterDescription) description;
        }
    }

    // Custom parameters serializers
    public static class UintParameterSerializer extends AnyParameterSerializer{
        public static final ParameterSerializerFactory factory = new ParameterSerializerFactory() {
            @Override
            public AnyParameterSerializer create(ParameterDescription descr) {
                return new UintParameterSerializer(descr);
            }
        };

        public UintParameterSerializer(ParameterDescription descr) {
            super(descr);
            super.value = "0";
        }

        public void deserialize(byte[] memory, int offset) {
            isSynchronized = true;
            int result = MemoryUtils.bytesArrayToUint16(memory, offset);
            super.value = Integer.toString(result);
        }

        public int serialize(byte[] memory, int offset) {
            isSynchronized = true;
            int i = Integer.parseInt(super.value);
            MemoryUtils.uint16ToByteArray(memory, offset, i);
            return size();
        }

        public int size() {
            return 2;
        }
    }

    public static class UByteParameterSerializer extends AnyParameterSerializer{
        public static final ParameterSerializerFactory factory = new ParameterSerializerFactory() {
            @Override
            public AnyParameterSerializer create(ParameterDescription descr) {
                return new UByteParameterSerializer(descr);
            }
        };

        public UByteParameterSerializer(ParameterDescription descr) {
            super(descr);
            super.value = "0";
        }

        public void deserialize(byte[] memory, int offset) {
            isSynchronized = true;
            int result = MemoryUtils.byteToUnsignedByte(memory[offset]);
            super.value = Integer.toString(result);
        }

        public int serialize(byte[] memory, int offset) {
            isSynchronized = true;
            int i = Integer.parseInt(super.value);
            memory[offset] = MemoryUtils.unsignedByteToByte(i);
            return size();
        }

        public int size() {
            return 1;
        }
    }
    public static class IntParameterSerializer extends AnyParameterSerializer {
        public static final ParameterSerializerFactory factory = new ParameterSerializerFactory() {
            @Override
            public AnyParameterSerializer create(ParameterDescription descr) {
                return new IntParameterSerializer(descr);
            }
        };

        public IntParameterSerializer(ParameterDescription descr) {
            super(descr);
            super.value = "0";
        }

        public void deserialize(byte[] memory, int offset) {
            isSynchronized = true;
            int result = MemoryUtils.bytesArrayToInt16(memory, offset);
            super.value = Integer.toString(result);
        }

        public int serialize(byte[] memory, int offset) {
            isSynchronized = true;
            int val = Integer.parseInt(super.value);
            MemoryUtils.int16ToByteArray(memory, offset, val);
            return size();
        }

        public int size() {
            return 2;
        }
    }
    public static class TimeIntervalParameterSerializer extends AnyParameterSerializer{
        public static final ParameterSerializerFactory factory = new ParameterSerializerFactory() {
            @Override
            public AnyParameterSerializer create(ParameterDescription descr) {
                return new TimeIntervalParameterSerializer(descr);
            }
        };

        public TimeIntervalParameterSerializer(ParameterDescription descr) {
            super(descr);
            super.value = "0";
        }

        public void deserialize(byte[] memory, int offset) {
            isSynchronized = true;
            long result = MemoryUtils.bytesArrayToUint32(memory, offset);
            super.value = Long.toString(result);
        }

        public int serialize(byte[] memory, int offset) {
            isSynchronized = true;
            long l = Long.parseLong(super.value);
            MemoryUtils.uint32ToByteArray(memory, offset, l);
            return size();
        }

        public int size() {
            return 4;
        }
    }

    public static class BoolParameterSerializer extends AnyParameterSerializer {
        public static final ParameterSerializerFactory factory = new ParameterSerializerFactory() {
            @Override
            public AnyParameterSerializer create(ParameterDescription descr) {
                return new BoolParameterSerializer(descr);
            }
        };

        public BoolParameterSerializer(ParameterDescription descr) {
            super(descr);
            super.value = "false";
        }

        public void deserialize(byte[] memory, int offset) {
            isSynchronized = true;
            super.value = MemoryUtils.bytesArrayToBool(memory, offset) ? "true" : "false";
        }

        public int serialize(byte[] memory, int offset) {
            isSynchronized = true;
            boolean val = super.value.equals("true");
            MemoryUtils.boolToBytesArray(memory, offset, val);
            return size();
        }

        public void setBool(boolean value) {
            super.setValue(value ? "true" : "false");
        }

        public boolean getBool() {
            return super.value.equals("true");
        }

        public int size() {
            return 1;
        }
    }
    public static class DevNameParameterSerializer extends AnyParameterSerializer {
        public static final ParameterSerializerFactory factory = new ParameterSerializerFactory() {
            @Override
            public AnyParameterSerializer create(ParameterDescription descr) {
                return new DevNameParameterSerializer(descr);
            }
        };
        public static final String defaultName = "Name unavailable";
        private boolean isInitialized = false;

        public DevNameParameterSerializer(ParameterDescription descr) {
            super(descr);
            super.value = defaultName;
        }

        public boolean initialized() { return isInitialized; }

        public void deserialize(byte[] memory, int offset) {
            super.value = "";
            for (int i=0; i<19; i++) {
                if (memory[offset+i] == 0)
                    break;
                super.value += (char) MemoryUtils.byteToUnsignedByte(memory[offset+i]);
            }
            isInitialized = true;
            isSynchronized = true;
        }

        public int serialize(byte[] memory, int offset) {
            isSynchronized = true;
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
    public static class MT2IdParameterSerializer extends AnyParameterSerializer {
        public static final ParameterSerializerFactory factory = new ParameterSerializerFactory() {
            @Override
            public AnyParameterSerializer create(ParameterDescription descr) {
                return new MT2IdParameterSerializer(descr);
            }
        };

        public MT2IdParameterSerializer(ParameterDescription descr) {
            super(descr);
            super.value = "0";
        }

        public void deserialize(byte[] memory, int offset) {
            isSynchronized = true;
            int id = MemoryUtils.byteToUnsignedByte(memory[offset]);
            super.value = Integer.toString(id);
        }

        public int serialize(byte[] memory, int offset) {
            isSynchronized = true;
            memory[offset] = (byte) Integer.parseInt(super.value);
            return size();
        }

        public int size() {
            return 1;
        }

        public Preference createPreference(Context context) {
            return null;
        }
    }
    public static class FloatParameterSerializer extends AnyParameterSerializer {
        public static final ParameterSerializerFactory factory = new ParameterSerializerFactory() {
            @Override
            public AnyParameterSerializer create(ParameterDescription descr) {
                return new FloatParameterSerializer(descr);
            }
        };

        public FloatParameterSerializer(ParameterDescription descr) {
            super(descr);
            super.value = "0.0";
        }

        public void deserialize(byte[] memory, int offset) {
            isSynchronized = true;
            byte tmp[] = Arrays.copyOfRange(memory, offset, offset + 4);
            float f = ByteBuffer.wrap(tmp)/*.order(ByteOrder.LITTLE_ENDIAN)*/.getFloat();
            super.value = Float.toString(f);
        }

        public int serialize(byte[] memory, int offset) {
            isSynchronized = true;
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
    public static class DevAddrParameterSerializer extends AnyParameterSerializer {
        public static final ParameterSerializerFactory factory = new ParameterSerializerFactory() {
            @Override
            public AnyParameterSerializer create(ParameterDescription descr) {
                return new DevAddrParameterSerializer(descr);
            }
        };

        public DevAddrParameterSerializer(ParameterDescription descr) {
            super(descr);
            super.value = "0.0.0";
        }

        public void deserialize(byte[] memory, int offset) {
            isSynchronized = true;
            super.value = MemoryUtils.byteToUnsignedByte(memory[offset])
                    + "." + MemoryUtils.byteToUnsignedByte(memory[offset+1])
                    + "." + MemoryUtils.byteToUnsignedByte(memory[offset+2]);
        }

        public int serialize(byte[] memory, int offset) {
            isSynchronized = true;
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

    // Containers
    public static class ParametersDescriptionsContainer {
        public Map<Integer, ParameterDescription> descriptions = new TreeMap<>();
        public ArrayList<Integer> orderedIds = new ArrayList<>();

        public void register(ParameterDescription descr)
        {
            descriptions.put(descr.getId(), descr);
            orderedIds.add(descr.getId());
        }

        /**
         * Create parameters serializers by all descriptions and put it to container
         * @param container where to put parameter serializers
         */
        public void addParameters(ParametersContainer2 container) {
            for (Map.Entry<Integer, ParameterDescription> entry : descriptions.entrySet()) {
                AnyParameterSerializer serializer = entry.getValue().factory.create(entry.getValue());
                container.add(serializer);
            }
            for (int id : orderedIds) {
                container.orderedIds.add(id);
            }
        }

        public boolean hasParameter(int id) {
            return descriptions.get(id) != null;
        }
    }
    public static class ParametersContainer2 {
        public Map<Integer, AnyParameterSerializer> allParameters = new TreeMap<>();
        public ArrayList<Integer> orderedIds = new ArrayList<>();

        public void add(AnyParameterSerializer par) {
            allParameters.put(par.description.getId(), par);
        }

        public AnyParameterSerializer get(int id){
            return allParameters.get(id);
        }

        public Set<Map.Entry<Integer, AnyParameterSerializer>> entrySet()
        {
            return allParameters.entrySet();
        }

        public int serializeSetObject(int id, byte[] memory, int position, int maxPosition) {
            AnyParameterSerializer par = allParameters.get(id);
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

            AnyParameterSerializer par = allParameters.get(id);
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
    public static class FunctionsContainer2 {
        public Map<Integer, FunctionCallSerializer> funcions = new TreeMap<>();

        public void register(FunctionCallSerializer function) {
            funcions.put(function.description.getId(), function);
        }

        public int serializeCall(int id, String argument, byte[] memory, int position, int maxPosition)
        {
            FunctionCallSerializer func = funcions.get(id);
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

    public static class Operations {
        public static void init() {
            try {
                Class.forName(Operations.class.getName());
                Class.forName(Operations.AnyDevice.class.getName());
                Class.forName(Operations.AnyDevice.Configuration.class.getName());
                Class.forName(Operations.AnyDevice.Funcitons.class.getName());
                Class.forName(Operations.Rifle.class.getName());
                Class.forName(Operations.Rifle.Configuration.class.getName());
                Class.forName(Operations.Rifle.Funcitons.class.getName());
                Class.forName(Operations.HeadSensor.class.getName());
                Class.forName(Operations.HeadSensor.Configuration.class.getName());
                Class.forName(Operations.HeadSensor.Funcitons.class.getName());

            } catch (ClassNotFoundException e) {
                throw new AssertionError(e);  // Can't happen
            }
        }
        public static class AnyDevice {
            public static ParametersDescriptionsContainer parametersDescriptions = new ParametersDescriptionsContainer();
            public static FunctionsContainer2 functionsSerializers = new FunctionsContainer2();
            public static class Configuration {
                public static final int DEV_TYPE_UNDEFINED = 100;
                public static final int DEV_TYPE_RIFLE = 1;
                public static final int DEV_TYPE_HEAD_SENSOR = 2;
                public static final int DEV_TYPE_BLUETOOTH_BRIDGE = 3;

                public static final ParameterDescription deviceType
                        = new UintParameterDescription(parametersDescriptions, 2002, "Device id");
                public static final ParameterDescription deviceName
                        = new ParameterDescription(parametersDescriptions, 2001, "Device name", false, DevNameParameterSerializer.factory);
            }

            public static class Funcitons {
                public static final FunctionCallSerializer resetToDefaults
                        = new FunctionCallNoParsSerializer(functionsSerializers, 2100, "Reset device to default");
            }

            public static String getDevTypeString(int type) {
                switch (type) {
                    case Configuration.DEV_TYPE_RIFLE:
                        return "Rifle";
                    case Configuration.DEV_TYPE_HEAD_SENSOR:
                        return "Head sensor";
                    case Configuration.DEV_TYPE_UNDEFINED:
                        return "Type not defined";
                    default:
                        return "Unknown type";
                }
            }
        }

        public static class HeadSensor {
            public static ParametersDescriptionsContainer parametersDescriptions = new ParametersDescriptionsContainer();
            public static FunctionsContainer2 functionsSerializers = new FunctionsContainer2();
            public static class Configuration {
                // Parameters order is important for output
                public static final ParameterDescription teamMT2Id
                        = new teamEnumDescription(parametersDescriptions, 1032, "Team");
                public static final ParameterDescription healthMax
                        = new UintParameterDescription(parametersDescriptions, 1000, "Maximal player health", 1, 200);
                public static final ParameterDescription healthStart
                        = new UintParameterDescription(parametersDescriptions, 1003, "Player health at start", 1, 200);
                public static final ParameterDescription isHealable
                        = new BooleanParameterDescription(parametersDescriptions, 1010, "Is player healable", true);
                public static final ParameterDescription lifesCount
                        = new UintParameterDescription(parametersDescriptions, 1011, "Players life count", 1, 1000);
            }

            public static class Funcitons {
                public static final FunctionCallSerializer playerTurnOff
                        = new FunctionCallNoParsSerializer(functionsSerializers, 1201, "Turn off player");
                public static final FunctionCallSerializer playerTurnOn
                        = new FunctionCallNoParsSerializer(functionsSerializers, 1202, "Turn on player");
                public static final FunctionCallSerializer playerReset
                        = new FunctionCallNoParsSerializer(functionsSerializers, 1203, "Reset players configuration to default");
                public static final FunctionCallSerializer playerRespawn
                        = new FunctionCallNoParsSerializer(functionsSerializers, 1204, "Respawn player");
                public static final FunctionCallSerializer playerKill
                        = new FunctionCallNoParsSerializer(functionsSerializers, 1205, "Kill player");
            }
        }

        public static class Rifle {
            public static ParametersDescriptionsContainer parametersDescriptions = new ParametersDescriptionsContainer();
            public static FunctionsContainer2 functionsSerializers = new FunctionsContainer2();
            public static class Configuration {
                public static final ParameterDescription damageMin
                        = new UintParameterDescription(parametersDescriptions, 5, "Minimal damage", 0, 100);
                public static final ParameterDescription damageMax
                        = new UintParameterDescription(parametersDescriptions, 6, "Maximal damage", 0, 100);

                public static final ParameterDescription firePeriod
                        = new TimeIntervalParameterDescription(parametersDescriptions, 7, "Fire period", 0, 10_000_000);

                public static final ParameterDescription reloadingTime
                        = new TimeIntervalParameterDescription(parametersDescriptions, 34, "Reloading imitation time", 0, 30_000_000);

                public static final ParameterDescription automaticAllowed
                        = new BooleanParameterDescription(parametersDescriptions, 13, "Allow automatic fire", true);



            }

            public static class Funcitons {

            }
        }




    }
}
