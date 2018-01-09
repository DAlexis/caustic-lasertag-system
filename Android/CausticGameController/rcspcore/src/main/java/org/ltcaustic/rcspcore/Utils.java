package org.ltcaustic.rcspcore;

/**
 * Created by dalexies on 14.05.17.
 */

public class Utils {
    /**
     * This class waits while it is regularly touched. When it was not touched during timeout,
     * in calls runnable. It is not time-precision tool to call runnable, you must set thread period
     */
    public static class SimpleWaiter {
        /**
         * Construct @{@link SimpleWaiter} WITHOUT runnable.
         * If there is more than timeout milliseconds since last touch,
         * IsTimeout() returns true
         * @param timeout Timeout in milliseconds
         */
        SimpleWaiter(long timeout) {
            this.timeout = timeout;
        }

        /**
         * Construct @{@link SimpleWaiter} WITH runnable.
         * If there is more than timeout milliseconds since last touch,
         * @{@link SimpleWaiter} automatically calls runnable from its own thread
         * and IsTimeout() returns true returns true
         * @param timeout Timeout in milliseconds
         * @param period Thread sleeping time before timeout was checked
         * @param runnable
         */
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

        /**
         * Check if last touch() called more than timeout milliseconds ago
         * @return true if timeout
         */
        public boolean isTimeout() {
            return System.currentTimeMillis() - lastTouchTime >= timeout;
        }

        /**
         * Prevent runnable calling and true value for isTimeout() function
         * for next timeout milliseconds
         */
        public void touch() {
            lastTouchTime = System.currentTimeMillis();
            CallFromThreadDone = false;
        }

        /**
         * Enable or disable next callback in case of timeout
         * @param isEnabled true if enabled
         */
        public void enableCallback(boolean isEnabled) {
            callbackEnabled = isEnabled;
        }

        /**
         * Stop waiting thread and join it
         */
        public void stopThread() {
            needStopThread = true;
            try {
                waitingThread.join();
            } catch (Exception e1) {
            }
        }

        /**
         * Function that is running in own thread
         */
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

    public static class PeriodicRunner
    {
        PeriodicRunner(long period, Runnable runnable) {
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

                runnable.run();

                if (needStopThread) {
                    return;
                }
            }
        }

        private Runnable runnable = null;
        private long period;
        private Thread waitingThread = null;
        private boolean needStopThread = false;
    }
}
