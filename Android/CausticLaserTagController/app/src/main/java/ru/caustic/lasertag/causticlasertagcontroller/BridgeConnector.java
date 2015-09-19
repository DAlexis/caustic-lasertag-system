package ru.caustic.lasertag.causticlasertagcontroller;

import android.os.Handler;

/**
 * Created by alexey on 18.09.15.
 */
public class BridgeConnector {

    private StringBuilder sb = new StringBuilder();

    BridgeConnector() {
        BluetoothManager.getInstance().setRXHandler(new Handler() {
            public void handleMessage(android.os.Message msg) {
                switch (msg.what) {
                    case BluetoothManager.RECIEVE_MESSAGE:                                         // если приняли сообщение в Handler
                        byte[] readBuf = (byte[]) msg.obj;
                        /*
                        String strIncom = new String(readBuf, 0, msg.arg1);
                        sb.append(strIncom);                                                // формируем строку
                        int endOfLineIndex = sb.indexOf("q");                            // определяем символы конца строки
                        if (endOfLineIndex > 0) {                                            // если встречаем конец строки,
                            String sbprint = sb.substring(0, endOfLineIndex);               // то извлекаем строку
                            sb.delete(0, sb.length());                                      // и очищаем sb
                            incomingTextView.setText(sbprint);             // обновляем TextView
                        }
                        //Log.d(TAG, "...Строка:"+ sb.toString() +  "Байт:" + msg.arg1 + "...");
                        */
                        break;
                }
            }
        });
    }

    public void requestProperty(int propertyCode) {

    }

    public void setProperty(int propertyCode, String propertyValue) {

    }
}
