package com.caustic.rcspcore;

/**
 * Created by dalexies on 14.05.17.
 */

public class Utils {
    public static class SimpleWaiter
    {
        SimpleWaiter(long timeout) {
            this.timeout = timeout;
        }

        SimpleWaiter(long timeout, long period, Runnable runnable) {
            this.timeout = timeout;
            this.runnable = runnable;
            this.period = period;
            waitingThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    threadBody();
                }
            });
            waitingThread.start();
        }

        public boolean isTimeout() {
            return System.currentTimeMillis() - lastTouchTime >= timeout;
        }

        public void touch() {
            lastTouchTime = System.currentTimeMillis();
            CallFromThreadDone = false;
        }

        public void enableCallback(boolean isEnabled) {
            callbackEnabled = isEnabled;
        }

        public void stopThread() {
            needStopThread = true;
            try {
                waitingThread.join();
            } catch (Exception e1) {
            }
        }

        private void threadBody() {
            while(true) {
                try {
                    Thread.sleep(period);
                } catch (Exception e1) {
                    return;
                }
                if (isTimeout() && !CallFromThreadDone) {
                    CallFromThreadDone = true;
                    if (callbackEnabled)
                        runnable.run();
                }
                if (needStopThread) {
                    return;
                }
            }
        }

        private long lastTouchTime = 0;
        private long timeout;
        private Runnable runnable = null;
        private long period;
        private Thread waitingThread = null;
        private boolean CallFromThreadDone = true; // Initial is true to prevent first call without touch().
        private boolean needStopThread = false;
        private boolean callbackEnabled = false;
    }
}
