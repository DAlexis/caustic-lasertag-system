package ru.caustic.rcspcore;

import android.support.annotation.NonNull;
import android.util.Log;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.TimeUnit;

/**
 * This class responsible for all communication with devices of Caustic Lasertag System.
 * All received or sended messages are going iver this class
 */

public class LTSCommunicator implements BridgeDriver.IncomingPackagesListener, RCSP.OperationDispatcherUser {

    public LTSCommunicator(BridgeDriver bridgeDriver) {
        this.bridgeDriver = bridgeDriver;
        workingThread = new Thread(new Runnable() {
            @Override
            public void run() {
                threadFunc();
            }
        },
        "LTSCommunicator i/o thread");
        workingThread.start();
    }

    public void stop() {
        workingThreadShouldWork = false;
        try {
            workingThread.join();
        } catch (Exception ex) {

        }
    }

    @Override
    public void getData(RCSP.DeviceAddress address, byte[] data, int offset, int size) {
        List<RCSP.RCSPOperation> operations = RCSP.RCSPOperation.parseStream(data, offset, size);
        for (RCSP.RCSPOperation operation : operations) {
            if (operation.isNOP())
                continue;

            RCSP.OperationDispatcher dispatcher = dispByCode.get(operation.id);
            if (dispatcher != null) {
                // We found dispatcher in map, so use it
                dispatcher.dispatchOperation(address, operation);
                continue;
            }

            // We have not found dispatcher in map, lets try to get it inside list
            for (RCSP.OperationDispatcher disp : dispNoCode) {
                if (disp.dispatchOperation(address, operation))
                    continue;
            }
            continue;
        }
    }

    @Override
    public void addOperationDispatcher(int code, RCSP.OperationDispatcher disp) {
        dispByCode.put(code, disp);
    }

    @Override
    public void addOperationDispatcher(RCSP.OperationDispatcher disp) {
        dispNoCode.add(disp);
    }

    public void sendMessage(RCSP.DeviceAddress addr, byte[] data, int size) {
        try {
            outMessagesQueue.put(new Message(addr, data, size));
        } catch (InterruptedException ex)
        {
            Log.e(TAG, "Exception while outMessagesQueue.take();");
        }
        //bridgeDriver.sendMessage(addr, data, size);
    }

    private static class Message {
        RCSP.DeviceAddress addr;
        byte[] data = null;
        int size = 0;
        Message(RCSP.DeviceAddress addr, byte[] data, int size) {
            this.addr = addr; this.data = data; this.size = size;
        }
    }

    private void threadFunc() {
        while (workingThreadShouldWork) {
            try {
                Message m = outMessagesQueue.take();
                bridgeDriver.sendMessage(m.addr, m.data, m.size);
            } catch (InterruptedException ex) {
                Log.e(TAG, "Exception while outMessagesQueue.take();");
                return;
            }
        }
    }

    private Map<Integer, RCSP.OperationDispatcher> dispByCode = new TreeMap<>();
    private List<RCSP.OperationDispatcher> dispNoCode = new ArrayList<>();
    private BridgeDriver bridgeDriver;
    private BlockingQueue<Message> outMessagesQueue = new ArrayBlockingQueue<Message>(500);
    private Thread workingThread;
    private boolean workingThreadShouldWork = true;
    private final String TAG = "CC.LTSCommunicator";
}
