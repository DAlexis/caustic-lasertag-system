package ru.caustic.lasertag.causticlasertagcontroller;

/**
 * Created by alexey on 19.09.15.
 */
public class MemoryUtils {

    public static int byteToUnsignedByte(byte b) {
        return b < 0 ? 256 + b : b;
    }

    public static int bytesArrayToUint16(byte[] memory, int position) {
        int result = 0;
        result = MemoryUtils.byteToUnsignedByte(memory[position]);
        result *= 256;
        result += MemoryUtils.byteToUnsignedByte(memory[position+1]);
        return result;
    }

    public static void uint16ToByteArray(byte[] memory, int position, int uint16) {
        memory[position] = (byte) (uint16 / 256);
        memory[position+1] = (byte) (uint16 % 256);
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
}
