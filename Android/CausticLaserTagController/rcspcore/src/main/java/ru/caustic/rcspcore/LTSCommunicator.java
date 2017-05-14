package ru.caustic.rcspcore;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

/**
 * This class responsible for all communication with devices of Caustic Lasertag System.
 * It receive all RCSP messages
 */

public class LTSCommunicator implements BridgeDriver.IncomingPackagesListener, RCSP.OperationDispatcherUser {

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

    private Map<Integer, RCSP.OperationDispatcher> dispByCode = new TreeMap<>();
    private List<RCSP.OperationDispatcher> dispNoCode = new ArrayList<>();
}
