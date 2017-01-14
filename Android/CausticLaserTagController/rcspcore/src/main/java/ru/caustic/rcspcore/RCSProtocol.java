package ru.caustic.rcspcore;

import android.content.Context;
import android.preference.Preference;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.concurrent.ConcurrentSkipListMap;

/**
 * Created by alexey on 18.09.15.
 */
public class RCSProtocol {

    // Public
    // Classes
    public enum OperationCodeType { SET_OBJECT, OBJECT_REQUEST, CALL_REQUEST, RESERVED, UNKNOWN }

    public static class RCSPOperation {
        public final static int MIN_SIZE = 3;
        public int id = 0;
        public OperationCodeType type = OperationCodeType.UNKNOWN;
        public byte[] argument = null;
        public int argumentSize = 0;

        public boolean isNOP() {
            return id == 0;
        }

        public int size() {
            return argumentSize + 3;
        }
        public static RCSPOperation parse(byte[] memory, int position, int size)
        {
            int read = 0;
            int argSize = MemoryUtils.byteToUnsignedByte(memory[position]);
            if (size < argSize + 3) {
                Log.e(TAG, "binary stream seems to be broken: unexpected end of stream");
                return null;
            }
            RCSPOperation result = new RCSPOperation();
            read++;
            int operationId = MemoryUtils.bytesArrayToUint16(memory, position+read);
            read += 2;
            result.type = dispatchOperationCodeType(operationId);
            result.id = removeOperationTypeBits(operationId);
            result.argumentSize = argSize;
            result.argument = new byte[argSize];
            for (int i=0; i<argSize; i++, read++) {
                result.argument[i] = memory[position + read];
            }
            return result;
        }
        public static List<RCSPOperation> parseStream(byte[] memory, int position, int size)
        {
            List<RCSPOperation> result = new ArrayList<>();
            RCSPOperation next;
            do {
                next = parse(memory, position, size);
                if (next == null) {
                    // This means continuation of stream is broken
                    break;
                }
                result.add(next);
                position += next.size();
            } while (size-position >= RCSPOperation.MIN_SIZE);
            return result;
        }
    }

    // Descriptions
    public static abstract class AnyDescription implements IDescription {
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
    public static abstract class ParameterDescription extends AnyDescription implements ISerializerCreator {
        private final boolean editable;
        public ParameterDescription(IDescriptionsHolder descrSet, int id, String name, boolean editable) {
            super(id, name);
            this.editable = editable;
            if (descrSet != null)
                descrSet.register(this);
        }
        public boolean isEditable() {
            return editable;
        }
    }

    public static class IntParameter extends ParameterDescription {
        public final int minValue;
        public final int maxValue;

        public AnyParameterSerializer createSerializer() { return new Serializer(this); }

        public static class Serializer extends AnyParameterSerializer {
            public Serializer(ParameterDescription descr) {
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

        public IntParameter(IDescriptionsHolder descrSet, int id, String name, int minValue, int maxValue) {
            super(descrSet, id, name, true);
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public IntParameter(IDescriptionsHolder descrSet, int id, String name) {
            super(descrSet, id, name, false);
            this.minValue = 0;
            this.maxValue = 0;
        }
    }
    public static class UintParameter extends ParameterDescription {
        public final int minValue;
        public final int maxValue;

        public AnyParameterSerializer createSerializer() { return new Serializer(this); }

        public static class Serializer extends AnyParameterSerializer{
            public Serializer(ParameterDescription descr) {
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

        public UintParameter(IDescriptionsHolder descrSet, int id, String name, int minValue, int maxValue) {
            super(descrSet, id, name, true);
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public UintParameter(IDescriptionsHolder descrSet, int id, String name) {
            super(descrSet, id, name, false);
            this.minValue = 0;
            this.maxValue = 0;
        }
    }
    public static class UByteParameter extends ParameterDescription {
        public final int minValue;
        public final int maxValue;

        public AnyParameterSerializer createSerializer() { return new Serializer(this); }

        public static class Serializer extends AnyParameterSerializer{
            public Serializer(ParameterDescription descr) {
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

        public UByteParameter(IDescriptionsHolder descrSet, int id, String name, int minValue, int maxValue) {
            super(descrSet, id, name, true);
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public UByteParameter(IDescriptionsHolder descrSet, int id, String name, boolean editable) {
            super(descrSet, id, name, editable);
            this.minValue = 0;
            this.maxValue = 0;
        }
    }
    public static class TimeIntervalParameter extends ParameterDescription {
        public final long minValue;
        public final long maxValue;

        public AnyParameterSerializer createSerializer() { return new Serializer(this); }

        public static class Serializer extends AnyParameterSerializer{
            public Serializer(ParameterDescription descr) {
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

        public TimeIntervalParameter(IDescriptionsHolder descrSet, int id, String name, long minValue, long maxValue) {
            super(descrSet, id, name, true);
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public TimeIntervalParameter(IDescriptionsHolder descrSet, int id, String name) {
            super(descrSet, id, name, false);
            this.minValue = 0;
            this.maxValue = 0;
        }
    }


    public static class ColorParameter extends ParameterDescription {
        //This is basically int32
        public final long minValue;
        public final long maxValue;

        public AnyParameterSerializer createSerializer() { return new Serializer(this); }

        public static class Serializer extends AnyParameterSerializer{
            public Serializer(ParameterDescription descr) {
                super(descr);
                super.value = "0";
            }

            public void deserialize(byte[] memory, int offset) {
                isSynchronized = true;
                long result = MemoryUtils.bytesArrayToInt32(memory, offset);
                super.value = Long.toString(result);
            }

            public int serialize(byte[] memory, int offset) {
                isSynchronized = true;
                long l = Long.parseLong(super.value);
                MemoryUtils.int32ToByteArray(memory, offset, l);
                return size();
            }

            public int size() {
                return 4;
            }
        }

        public ColorParameter(IDescriptionsHolder descrSet, int id, String name, long minValue, long maxValue) {
            super(descrSet, id, name, true);
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public ColorParameter(IDescriptionsHolder descrSet, int id, String name) {
            super(descrSet, id, name, false);
            this.minValue = 0;
            this.maxValue = 0;
        }
    }


    public static class BooleanParameter extends ParameterDescription {

        public AnyParameterSerializer createSerializer() { return new Serializer(this); }

        public static class Serializer extends AnyParameterSerializer {
            public Serializer(ParameterDescription descr) {
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

        public BooleanParameter(IDescriptionsHolder descrSet, int id, String name, boolean editable) {
            super(descrSet, id, name, editable);

        }

    }
    public static class MT2IdParameter extends ParameterDescription {
        public final int minValue;
        public final int maxValue;

        public AnyParameterSerializer createSerializer() { return new Serializer(this); }

        public static class Serializer extends AnyParameterSerializer {
            public Serializer(ParameterDescription descr) {
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

        public MT2IdParameter(IDescriptionsHolder descrSet, int id, String name, boolean editable, int minValue, int maxValue) {
            super(descrSet, id, name, editable);
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public MT2IdParameter(IDescriptionsHolder descrSet, int id, String name, boolean editable) {
            super(descrSet, id, name, editable);
            this.minValue = 1;
            this.maxValue = 127;
        }
    }
    public static class FloatParameter extends ParameterDescription {
        public final float minValue;
        public final float maxValue;

        public AnyParameterSerializer createSerializer() { return new Serializer(this); }

        public static class Serializer extends AnyParameterSerializer {
            public Serializer(ParameterDescription descr) {
                super(descr);
                super.value = "0.0";
            }

            public void deserialize(byte[] memory, int offset) {
                isSynchronized = true;
                byte tmp[] = Arrays.copyOfRange(memory, offset, offset + 4);
                float f = ByteBuffer.wrap(tmp).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                super.value = Float.toString(f);
            }

            public int serialize(byte[] memory, int offset) {
                isSynchronized = true;
                byte tmp[] = new byte[4];
                ByteBuffer.wrap(tmp).order(ByteOrder.LITTLE_ENDIAN).putFloat(Float.parseFloat(super.value));
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


        public FloatParameter(IDescriptionsHolder descrSet, int id, String name, boolean editable, float minValue, float maxValue) {
            super(descrSet, id, name, editable);
            this.minValue = minValue;
            this.maxValue = maxValue;
        }

        public FloatParameter(IDescriptionsHolder descrSet, int id, String name, boolean editable) {
            super(descrSet, id, name, editable);
            this.minValue = 0.0f;
            this.maxValue = 0.0f;
        }
    }
    public static class DevNameParameter extends ParameterDescription {

        public AnyParameterSerializer createSerializer() { return new Serializer(this); }

        public static class Serializer extends AnyParameterSerializer {

            public static final String defaultName = "Name unavailable";
            private boolean isInitialized = false;

            public Serializer(ParameterDescription descr) {
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

        public DevNameParameter(IDescriptionsHolder descrSet, int id, String name, boolean editable) {
            super(descrSet, id, name, editable);
        }
    }
    public static class DevAddrParameter extends ParameterDescription {

        public AnyParameterSerializer createSerializer() { return new Serializer(this); }

        public static class Serializer extends AnyParameterSerializer {
            public Serializer(ParameterDescription descr) {
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

        public DevAddrParameter(IDescriptionsHolder descrSet, int id, String name, boolean editable) {
            super(descrSet, id, name, editable);
        }
    }

    // @todo Should inherit uint8_t parameter
    public static abstract class EnumParameter extends UByteParameter {
        public Map<String, Integer> entries = new HashMap<>();
        public ArrayList<String> namesOrdered = new ArrayList<>();

        public EnumParameter(IDescriptionsHolder descrSet, int id, String name, boolean editable) {
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

    public static class teamEnum extends EnumParameter {

        public teamEnum(IDescriptionsHolder descrSet, int id, String name) {
            super(descrSet, id, name, true);
            addElement("Red", 0);
            addElement("Blue", 1);
            addElement("Yellow", 2);
            addElement("Green", 3);
        }
    }

    // Serializers
    public static abstract class AnySerializer extends Object implements ISerializer {

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
    public static abstract class FunctionDescription extends AnyDescription implements ISerializer {
        public AnySerializer argumentSerializer = null;

        public FunctionDescription(IDescriptionsHolder container, int id, String name) {
            super(id, name);

            if (container != null)
                container.register(this);
        }

        public abstract void setArgument(String argument);
    }
    public static class FunctionDescriptionNoArg extends FunctionDescription {
        public FunctionDescriptionNoArg(IDescriptionsHolder container, int id, String name) {
            super(container, id, name);
        }

        public void setArgument(String argument) {}

        public void deserialize(byte[] memory, int offset) {}

        public int size() {
            return 0;
        }
        public int serialize(byte[] memory, int offset) {
            return size();
        }
    }
    public static class FunctionDesctiptionWithArg extends FunctionDescription {
        AnyParameterSerializer argumentSerializer;

        public FunctionDesctiptionWithArg(IDescriptionsHolder container, int id, String name, AnyParameterSerializer argumentSerializer)
        {
            super(container, id, name);
            this.argumentSerializer = argumentSerializer;
        }

        public void setArgument(String argument)
        {
            argumentSerializer.setValue(argument);
        }

        public void deserialize(byte[] memory, int offset) {}

        public int size() {
            return argumentSerializer.size();
        }

        public int serialize(byte[] memory, int offset) {
            return argumentSerializer.serialize(memory, offset);
        }
    }

    public static abstract class AnyParameterSerializer extends AnySerializer {
        protected boolean isSynchronized = false;
        protected String value = "";

        public boolean isSync() {
            return isSynchronized;
        }

        public void invalidate() {
            isSynchronized = false;
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

    // Containers
    public static class ParametersDescriptionsContainer implements IDescriptionsHolder {
        public Map<Integer, ParameterDescription> descriptions = new TreeMap<>();
        public ArrayList<Integer> orderedIds = new ArrayList<>();

        public void register(IDescription descr)
        {
            ParameterDescription parDescr = null;
            if (descr instanceof ParameterDescription) {
                parDescr = (ParameterDescription) descr;
            } else {
                return;
            }
            descriptions.put(parDescr.getId(), parDescr);
            orderedIds.add(parDescr.getId());
        }

        /**
         * Create parameters serializers by all descriptions and put it to container
         * @param container where to put parameter serializers
         */
        public void addParameters(IParameterSerializersHolder container) {
            for (int id : orderedIds) {
                AnyParameterSerializer serializer = descriptions.get(id).createSerializer();
                container.register(serializer);
            }
        }

        public boolean hasParameter(int id) {
            return descriptions.get(id) != null;
        }
    }
    public static class ParametersContainer implements IParameterSerializersHolder {
        public Map<Integer, AnyParameterSerializer> allParameters = new ConcurrentSkipListMap<>();
        public ArrayList<Integer> orderedIds = new ArrayList<>();

        @Override
        public void register(AnyParameterSerializer par) {
            allParameters.put(par.description.getId(), par);
            orderedIds.add(par.description.getId());
        }

        public AnyParameterSerializer get(int id) {
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

        public int deserializeOneParamter(RCSPOperation operation)
        {
            if (operation == null) {
                // @todo use exception there: stream is broken
                return 0;
            }
            if (operation.type != OperationCodeType.SET_OBJECT)
            {
                Log.e(TAG, "Trying to dispatch " + removeOperationTypeBits(operation.id) + " with invalid operation code type bits");
                return 0;
            }

            AnyParameterSerializer par = allParameters.get(operation.id);
            if (par != null) {
                if (operation.argumentSize != par.size()) {
                    Log.e(TAG, "Invalid argument length " + operation.argumentSize + " instead of " + par.size() + " for operation id " + operation.id);
                    return 0;
                }
                par.deserialize(operation.argument, 0);
            } else {
                Log.e(TAG, "Unknown parameter id: " + operation.id);
                return 0;
            }
            return operation.size();
        }

        @Deprecated // Only for testing now, because stream may contain not only parameters records
        public void deserializeStream(byte[] memory, int offset, int size)
        {
            // @todo Do we really need this function?
            int notParsed = size;
            for (int i = 0; i<size; )
            {
                RCSPOperation operation = RCSPOperation.parse(memory, offset + i, notParsed);
                int result = deserializeOneParamter(operation);
                if (result == 0) {
                    // @todo may be skip?
                    break;
                }
                i += result;
                notParsed -= result; /// @todo test it
            }
        }
    }
    public static class FunctionsContainer implements IDescriptionsHolder {
        public Map<Integer, FunctionDescription> functions = new TreeMap<>();

        public void register(IDescription descr) {
            FunctionDescription funcDescr = null;
            if (descr instanceof FunctionDescription) {
                funcDescr = (FunctionDescription) descr;
            } else {
                return;
            }
            functions.put(funcDescr.getId(), funcDescr);
        }

        public int serializeCall(int id, String argument, byte[] memory, int position, int maxPosition)
        {
            FunctionDescription func = functions.get(id);
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
        private static boolean isInitialised = false;
        public static void init() {
            if (isInitialised)
                return;
            try {
                Class.forName(Operations.class.getName());
                Class.forName(Operations.AnyDevice.class.getName());
                Class.forName(Operations.AnyDevice.Configuration.class.getName());
                Class.forName(AnyDevice.Functions.class.getName());
                Class.forName(Operations.Rifle.class.getName());
                Class.forName(Operations.Rifle.Configuration.class.getName());
                Class.forName(Rifle.Functions.class.getName());
                Class.forName(Operations.HeadSensor.class.getName());
                Class.forName(Operations.HeadSensor.Configuration.class.getName());
                Class.forName(HeadSensor.Functions.class.getName());
                isInitialised = true;
            } catch (ClassNotFoundException e) {
                throw new AssertionError(e);  // Can't happen
            }
        }
        public static class AnyDevice {
            public static ParametersDescriptionsContainer parametersDescriptions = new ParametersDescriptionsContainer();
            public static FunctionsContainer functionsSerializers = new FunctionsContainer();
            public static class Configuration {
                public static final int DEV_TYPE_UNDEFINED = 100;
                public static final int DEV_TYPE_RIFLE = 1;
                public static final int DEV_TYPE_HEAD_SENSOR = 2;
                public static final int DEV_TYPE_BLUETOOTH_BRIDGE = 3;

                public static final ParameterDescription deviceType
                        = new UintParameter(parametersDescriptions, 2002, "Device id");
                public static final ParameterDescription deviceName
                        = new DevNameParameter(parametersDescriptions, 2001, "Device name", false);
            }

            public static class Functions {
                public static final FunctionDescription resetToDefaults
                        = new FunctionDescriptionNoArg(functionsSerializers, 2100, "Reset device to default");
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
            public static FunctionsContainer functionsSerializers = new FunctionsContainer();
            public static class Configuration {
                // Parameters order is important for output

                public static final ParameterDescription teamMT2Id
                        = new teamEnum(parametersDescriptions, 1032, "Team");
                public static final ParameterDescription playerGameId
                        = new UByteParameter(parametersDescriptions, 1031, "Player Id in game", 1, 125);
                public static final ParameterDescription healthMax
                        = new UintParameter(parametersDescriptions, 1000, "Maximal player health", 1, 200);
                public static final ParameterDescription healthStart
                        = new UintParameter(parametersDescriptions, 1003, "Player health at start", 1, 200);
                public static final ParameterDescription isHealable
                        = new BooleanParameter(parametersDescriptions, 1010, "Is player healable", true);
                public static final ParameterDescription lifesCount
                        = new UintParameter(parametersDescriptions, 1011, "Players life count", 1, 1000);

                public static final ParameterDescription frendlyFireCoeff
                        = new FloatParameter(parametersDescriptions, 1015, "Friendly fire coefficient", true, 0.0f, 1.0f);
                public static final ParameterDescription selfShotCoeff
                        = new FloatParameter(parametersDescriptions, 1016, "Self shot coefficient", true, 0.0f, 1.0f);

                public static final ParameterDescription playerLat
                        = new FloatParameter(parametersDescriptions, 1033, "Player's latitude", true, -90.0f, 90.0f);
                public static final ParameterDescription playerLon
                        = new FloatParameter(parametersDescriptions, 1034, "Player's longitude", true, -180.0f, 180.0f);


                public static final ParameterDescription markerColor
                        = new ColorParameter(parametersDescriptions, 1035, "On-map marker color", -2147483648, -2147483648);
            }

            public static class Functions {
                public static final FunctionDescription playerTurnOff
                        = new FunctionDescriptionNoArg(functionsSerializers, 1201, "Turn off player");
                public static final FunctionDescription playerTurnOn
                        = new FunctionDescriptionNoArg(functionsSerializers, 1202, "Turn on player");
                public static final FunctionDescription playerReset
                        = new FunctionDescriptionNoArg(functionsSerializers, 1203, "Reset players configuration to default");
                public static final FunctionDescription playerRespawn
                        = new FunctionDescriptionNoArg(functionsSerializers, 1204, "Respawn player");
                public static final FunctionDescription playerKill
                        = new FunctionDescriptionNoArg(functionsSerializers, 1205, "Kill player");
                public static final FunctionDescription resetStats
                        = new FunctionDescriptionNoArg(functionsSerializers, 1210, "Reset statistics at head sensor");
                public static final FunctionDescription readStats
                        = new FunctionDescriptionNoArg(functionsSerializers, 1211, "Read statistics from head sensor");

            }
        }

        public static class Rifle {
            public static ParametersDescriptionsContainer parametersDescriptions = new ParametersDescriptionsContainer();
            public static FunctionsContainer functionsSerializers = new FunctionsContainer();
            public static class Configuration {
                public static final ParameterDescription damageMin
                        = new UintParameter(parametersDescriptions, 5, "Minimal damage", 0, 100);
                public static final ParameterDescription damageMax
                        = new UintParameter(parametersDescriptions, 6, "Maximal damage", 0, 100);

                public static final ParameterDescription magazinesCountStart
                        = new UintParameter(parametersDescriptions, 30, "Start magazines count", 0, 100);

                /*
                public static final ParameterDescription magazinesCountMax
                        = new UintParameter(parametersDescriptions, 31, "Max magazines count", 0, 100);
                */

                public static final ParameterDescription bulletsInMagazineStart
                        = new UintParameter(parametersDescriptions, 32, "Bullets in mag. after respawn", 0, 100);
                public static final ParameterDescription bulletsInMagazineMax
                        = new UintParameter(parametersDescriptions, 33, "Bullets in magazine", 0, 100);


                public static final ParameterDescription firePeriod
                        = new TimeIntervalParameter(parametersDescriptions, 7, "Fire period", 0, 2_000_000);

                public static final ParameterDescription reloadingTime
                        = new TimeIntervalParameter(parametersDescriptions, 34, "Reloading imitation time", 0, 30_000_000);

                public static final ParameterDescription automaticAllowed
                        = new BooleanParameter(parametersDescriptions, 13, "Allow automatic fire", true);

                public static final ParameterDescription outputPower
                        = new UintParameter(parametersDescriptions, 90, "IR power", 0, 100);


            }

            public static class Functions {

            }
        }
    }

    // Methods
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
    public static int nextCommandSize(byte[] memory, int position) {
        int readed = 0;
        int argSize = MemoryUtils.byteToUnsignedByte(memory[position]);
        return argSize+1+2;
    }

    // Private
    // Interfaces
    private interface IDescriptionsHolder {
        void register(IDescription description);
    }
    private interface IParameterSerializersHolder {
        void register(AnyParameterSerializer serializer);
    }
    private interface ISerializerCreator {
        AnyParameterSerializer createSerializer();
    }
    private interface IDescription {
        String getName();
        int getId();
    }
    private interface ISerializer {
        void deserialize(byte[] memory, int offset);
        int size();
        int serialize(byte[] memory, int offset);
    }

    private static final String TAG = "CC.RCSProtocol";
    private static final int OperationCodeMask = 0b0011_1111_1111_1111;
}
