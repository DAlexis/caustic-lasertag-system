package org.ltcaustic.rcspcore;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by dalexies on 14.05.17.
 */

public class RCSPStream {
    private class RCSPStreamSegment {
        int requestSize = 23;
        byte[] request = new byte[requestSize];
        int cursor = 0;

        public RCSPStreamSegment(LTSCommunicator communicator)
        {
            this.communicator = communicator;
            MemoryUtils.zerify(request);
        }

        public boolean addObjectRequest(int id) {
            int size = RCSP.ParametersContainer.serializeParameterRequest(
                    id,
                    request,
                    cursor,
                    requestSize
            );

            if (size != 0) {
                cursor += size;
                return true;
            } else {
                return false;
            }
        }

        public boolean addSetObject(CausticDevice dev, int id) {
            RCSP.ParametersContainer pars = dev.parameters;

            int size = pars.serializeSetObject(
                    id,
                    request,
                    cursor,
                    requestSize
            );

            if (size != 0) {
                cursor += size;
                return true;
            } else {
                return false;
            }
        }

        public boolean addFunctionCall(RCSP.FunctionsContainer functionsContainer, int id, String argument) {
            int size = functionsContainer.serializeCall(
                    id,
                    argument,
                    request,
                    cursor,
                    requestSize
            );

            if (size != 0) {
                cursor += size;
                return true;
            }
            return false;
        }

        public void send(RCSP.DeviceAddress addr) {
            if (cursor != 0)
                communicator.sendMessage(addr, request, cursor);
        }
        LTSCommunicator communicator;
    }

    List<RCSPStreamSegment> streams = new ArrayList<>();

    public RCSPStream(LTSCommunicator communicator) {
        this.communicator = communicator;
        streams.add(new RCSPStreamSegment(this.communicator));
    }

    public void addObjectRequest(int id) {
        if (!streams.get(streams.size()-1).addObjectRequest(id)) {
            streams.add(new RCSPStreamSegment(communicator));
            streams.get(streams.size()-1).addObjectRequest(id);
        }
    }
    public void addSetObject(CausticDevice dev, int id) {
        if (!streams.get(streams.size()-1).addSetObject(dev, id)) {
            streams.add(new RCSPStreamSegment(communicator));
            streams.get(streams.size()-1).addSetObject(dev, id);
        }
    }
    public void addFunctionCall(RCSP.FunctionsContainer functionsContainer, int id, String argument) {
        if (!streams.get(streams.size()-1).addFunctionCall(functionsContainer, id, argument)) {
            streams.add(new RCSPStreamSegment(communicator));
            streams.get(streams.size()-1).addFunctionCall(functionsContainer, id, argument);
        }
    }
    public void send(RCSP.DeviceAddress addr) {
        for (RCSPStreamSegment stream : streams) {
            stream.send(addr);
        }
    }

    private LTSCommunicator communicator;
}
