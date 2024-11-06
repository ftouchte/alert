//package ftouchte.Pulse;

public class Pulse {

        public long timestamp;
        public float integral;
        public float time;
        public float pedestal;

        public float adcMax;
        public float timeRiseCFA;
        public float timeFallCFA;
        public float timeOverThresholdCFA;
        public float timeCFD;

        /**
        * Units are the same as the raw units of the samples.
        * @param integral pulse integral, pedestal-subtracted
        * @param time pulse time
        */
        public Pulse(float integral, float time) {
                this.integral = integral;
                this.time = time;
        }

        public Pulse(){}

        @Override
        public String toString() {
        return String.format("pulse: integral=%f time=%f",
        integral, time);
        }
}
