package ru.caustic.lasertag.core;

/**
 * Created by alexey on 19.09.15.
 */
public class MemoryUtils {
    // @todo Check endian problem for other (not uint16_t) types
    public static int byteToUnsignedByte(byte b) {
        return b < 0 ? 256 + b : b;
    }

    public static byte unsignedByteToByte(int b) {
        return (byte) b;
    }

    public static int bytesArrayToUint16(byte[] memory, int position) {
        int result = 0;
        result = MemoryUtils.byteToUnsignedByte(memory[position+1]);
        result *= 256;
        result += MemoryUtils.byteToUnsignedByte(memory[position]);
        return result;
    }

    public static void uint16ToByteArray(byte[] memory, int position, int uint16) {
        memory[position+1] = (byte) (uint16 / 256);
        memory[position] = (byte) (uint16 % 256);
    }

    public static long bytesArrayToUint32(byte[] memory, int position) {
        long result = 0;
        result = MemoryUtils.byteToUnsignedByte(memory[position+3]);
        result *= 256;
        result += MemoryUtils.byteToUnsignedByte(memory[position+2]);
        result *= 256;
        result += MemoryUtils.byteToUnsignedByte(memory[position+1]);
        result *= 256;
        result += MemoryUtils.byteToUnsignedByte(memory[position]);
        return result;
    }

    public static void uint32ToByteArray(byte[] memory, int position, long uint32) {
        memory[position] = (byte) (uint32 % 256);
        uint32 /= 256;
        memory[position+1] = (byte) (uint32 % 256);
        uint32 /= 256;
        memory[position+2] = (byte) (uint32 % 256);
        uint32 /= 256;
        memory[position+3] = (byte) (uint32 % 256);
    }

    public static int bytesArrayToInt16(byte[] memory, int position) {
        int result = bytesArrayToUint16(memory, position);
        if (result > 32767)
            result -= 65535;
        return result;
    }

    public static void int16ToByteArray(byte[] memory, int position, int int16) {
        if (int16 < 0)
            int16 += 65535;
        uint16ToByteArray(memory, position, int16);
    }

    public static boolean bytesArrayToBool(byte[] memory, int position) {
        if (memory[position] == 0)
            return false;
        else
            return true;
    }

    public static void boolToBytesArray(byte[] memory, int position, boolean val) {
        if (val) {
            memory[position] = (byte) 1;
        } else {
            memory[position] = 0;
        }
    }

    public static void zerify(byte[] memory) {
        for (int i=0; i<memory.length; i++)
            memory[i] = 0;
    }
}
