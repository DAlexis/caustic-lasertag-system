package ru.caustic.lasertag.causticlasertagcontroller;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Map;

/**
 * Created by alexey on 18.09.15.
 */
public class RCSProtocol {

    private Map<Integer, Parameter> allParameters;

    class Parameter {

        public static final int TYPE_UINT_PARAMETER  = 1;
        public static final int TYPE_INT_PARAMETER   = 2;
        public static final int TYPE_TIME_INTERVAL   = 3;
        public static final int TYPE_FLOAT_PARAMETER = 4;
        public static final int TYPE_BOOL_PARAMETER  = 6;
        public static final int TYPE_DEVICE_NAME     = 7;
        public static final int TYPE_DEVICE_ADDRESS  = 8;
        public static final int TYPE_MT2_ID          = 9;

        private final String name;
        private final int type;
        private final int id;

        private String value;

        Parameter(String _name, int _type, int _id) {
            name = _name;
            type = _type;
            id = _id;
        }

        void parse(byte[] memory) {
            switch (type) {
                case TYPE_UINT_PARAMETER:
                    int result = 0;
                    result = memory[0] < 0 ? 255 + memory[0] : memory[0];
                    result *= 256;
                    result += memory[1] < 0 ? 255 + memory[1] : memory[1];
                    value = Integer.toString(result);
                    break;
                case TYPE_INT_PARAMETER:
                    break;
                case TYPE_TIME_INTERVAL:
                    break;
                case TYPE_FLOAT_PARAMETER:
                    float f = ByteBuffer.wrap(memory).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                    value = Float.toString(f);
                    break;
                case TYPE_BOOL_PARAMETER:
                    break;
                case TYPE_DEVICE_NAME:
                    break;
                case TYPE_DEVICE_ADDRESS:

                    break;
                case TYPE_MT2_ID:
                    break;
            }
        }

        public void setValue(String value) {
            this.value = value;
        }

        public String getValue() {
            return value;
        }
    }

    class AnyDevice {

    }
}
