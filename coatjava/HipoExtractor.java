import java.util.List;
import java.util.ArrayList;

public class Test {
	public static void main(String[] args) {
		short[] samples = {1,2,3,4,5,6,7,6,5,4,3,2,1};
		//short[] samples = {1,2,3,4,5,6,4095,4095,4095,4095,4095,4,3,2,1};
		
		System.out.println("=====> Test HipoExtractor");
		HipoExtractor H = new HipoExtractor();
		//System.out.println("samples : " + samples.toString());
		System.out.println("=====> Setting parameters");
		short adcOffset = 0;
		if (samples.length > 5) {
			for (int bin=0; bin<5; ++bin)
				adcOffset += samples[bin];
		}
		adcOffset = Integer.valueOf(adcOffset / 5).shortValue();
		adcOffset = 0;
		H.adcOffset = adcOffset; // adc
		H.samplingTime = 1; // ns // 44 ns
		H.sparseSample = 0;
		H.timeStamp = 0;
		H.fineTimeStampResolution = 0;
		H.amplitudeFractionCFA = 0.5f;
		H.binDelayCFD = 5;
		H.fractionCFD = 0.3f;
		System.out.println("adcOffset    = " + H.adcOffset + " adc");
		System.out.println("samplingTime = " + H.samplingTime + " ns");
		System.out.println("sparseSample = " + H.sparseSample);
		System.out.println("binOffset = " + H.binOffset);
		System.out.println("timeStample = " + H.timeStamp);
		System.out.println("fineTimeStampleResolution = " + H.fineTimeStampResolution);
		
		System.out.println("=====> Application of the HipoExtractor");
		List<Pulse> pulses = H.extract(samples);
		System.out.println("binMax = " + H.binMax);
		System.out.println("adcMax = " + H.adcMax);
		System.out.println("timeMax = " + H.timeMax + " ns");
		System.out.println("timeRiseCFA = " + H.timeRiseCFA + " ns");
		System.out.println("timeFallCFA = " + H.timeFallCFA + " ns");
		System.out.println("timeOverThreshold = " + H.timeOverThresholdCFA + " ns");
		System.out.println("timeCFD = " + H.timeCFD + " ns");
		System.out.println(pulses.get(0));
	}
}

public class Pulse {
	public float integral;
	public float time;

	public Pulse(float integral, float time) {
		this.integral = integral;
		this.time = time;
	}

	@Override
	public String toString() {
		return String.format("pulse: integral=%f time=%f", integral, time);
	}
}


//public abstract class HipoExtractor implements IExtractor {
public class HipoExtractor {
	
	// Standard attributs to be filled
	public int binMax; //Bin of the max ADC over the pulse
	public int binOffset; //Offset due to sparse sample
	public float adcMax; //Max value of ADC over the pulse (fitted)
	public float timeMax; //Time of the max ADC over the pulse (fitted)
	public float integral; //Sum of ADCs over the pulse (not fitted)
	public long timestamp;

	private short[] samplesCorr; //Waveform after offset (pedestal) correction
	private int binNumber; //Number of bins in one waveform

	// New attributs to be filled
	public float timeRiseCFA; // moment when the signal reaches a Constant Fraction of its Amplitude uphill (fitted)
	public float timeFallCFA; // moment when the signal reaches a Constant Fraction of its Amplitude downhill (fitted)
	public float timeOverThresholdCFA; // is equal to (timeFallCFA - timeRiseCFA)
	public float timeCFD; // time extracted using the Constant Fraction Discriminator (CFD) algorithm (fitted)

	// Setting parameters // Should ideally be arguments in the extarct() methods by comparison to MVTFitter.java
	public float samplingTime;
	public int sparseSample;
	public short adcOffset; // à discuter avec Raphaël, est-ce que je le définis moi-même comme en simulation ?
	long timeStamp;
	float fineTimeStampResolution;
	public static final short ADC_LIMIT = 4095; // 2^12-1
	public float amplitudeFractionCFA;
	public int binDelayCFD;
	public float fractionCFD;
	

	/**
	 * @param samples : waveform samples stored in an array of short
	 * @param id : 
	 * @param pars : 
	 */
	public List<Pulse> extract(short... samples){
		waveformCorrection(adcOffset,samplingTime,samples,sparseSample);
		fitAverage(samplingTime);
		computeTimeAtConstantFractionAmplitude(samplingTime,amplitudeFractionCFA);
		computeTimeUsingConstantFractionDiscriminator(samplingTime,fractionCFD,binDelayCFD);
		fineTimeStampCorrection(timeStamp,fineTimeStampResolution);	
		// output
		Pulse pulse = new Pulse(integral,timeMax);
		List<Pulse> output = new ArrayList<>();
		output.add(pulse);
		return output;
	}

	/**
	 * This method subtracts the pedestal (noise) from samples and stores it in : samplesCorr
	 * It also computes a first value for : adcMax, binMax, timeMax and integral
	 * This code is inspired by the one of MVTFitter.java
	 * @param adcOffset : pedestal or noise level
	 * @param samplingTime : time between two adc bins
	 * @param sparseSample : to define binOffset
	 */
	private void waveformCorrection(short adcOffset, float samplingTime, short[] samples, int sparseSample){
		binNumber = samples.length;
		binMax = 0;
		adcMax = (short) (samples[0] - adcOffset);
		integral = 0;
		samplesCorr = new short[binNumber];
		for (int bin = 0; bin < binNumber; bin++){
			samplesCorr[bin] = (short) (samples[bin] - adcOffset);
			if (adcMax < samplesCorr[bin]){
				adcMax = samplesCorr[bin];
				binMax = bin;
			}
			integral += samplesCorr[bin];
		}
		/**
		 * If adcMax == ADC_LIMIT, that means there is saturation
		 * In that case, binMax is the middle of the first plateau
		 * This convention can be changed
		 */
		if ((short) adcMax == ADC_LIMIT) { 
			int binMax2 = binMax;
			for (int bin = binMax; bin < binNumber; bin++){
				if (samplesCorr[bin] == ADC_LIMIT) {
					binMax2 = bin;
				}
				else {
					break;
				}
			}
			binMax = (binMax + binMax2)/2;	
		}
		binOffset = sparseSample*binMax;
		timeMax = (binMax + binOffset)*samplingTime;
	}

	/**
	 * This method gives a more precise value of the max of the waveform by computing the average of five points around the binMax
	 * It is an alternative to fitParabolic()
	 * The suitability of one of these fits can be the subject of a study
	 * Remark : This method updates adcMax but doesn't change timeMax
	 * @param samplingTime : time between 2 ADC bins
	 */
	private void fitAverage(float samplingTime){
		if ((binMax - 2 >= 0) && (binMax + 2 <= binNumber - 1)){
			adcMax = 0;
			for (int bin = binMax - 2; bin <= binMax + 2; bin++){
				adcMax += samplesCorr[bin];
			}
			adcMax = adcMax/5; 
		}	
	}

	/**
	 * Fit the max of the pulse using parabolic fit, this method updates the timeMax and adcMax values
	 * @param samplingTime : time between 2 ADC bins
	 */
	private void fitParabolic(float samplingTime) {
	
	}

	/**
	 * From MVTFitter.java
	 * Make fine timestamp correction (using dream (=electronic chip) clock)
	 * @param timeStamp : timing informations (used to make fine corrections)
	 * @param fineTimeStampResolution : precision of dream clock (usually 8)
	 */
	private void fineTimeStampCorrection (long timeStamp, float fineTimeStampResolution) {
		this.timestamp = timeStamp;
		String binaryTimeStamp = Long.toBinaryString(timeStamp); //get 64 bit timestamp in binary format
		if (binaryTimeStamp.length()>=3){
			byte fineTimeStamp = Byte.parseByte(binaryTimeStamp.substring(binaryTimeStamp.length()-3,binaryTimeStamp.length()),2); //fineTimeStamp : keep and convert last 3 bits of binary timestamp
			timeMax += (float) ((fineTimeStamp+0.5) * fineTimeStampResolution); //fineTimeStampCorrection
		}
	}

	/**
	 * This method determines the moment when the signal reaches a Constant Fraction of its Amplitude (i.e fraction*adcMax)
	 * It fills the attributs : timeRiseCFA, timeFallCFA, timeOverThresholdCFA 
	 * @param samplingTime : time between 2 ADC bins
	 * @param amplitudeFraction : a float between 0 and 1
	 */
	private void computeTimeAtConstantFractionAmplitude(float samplingTime, float amplitudeFractionCFA){
		float threshold = amplitudeFractionCFA*adcMax;
		// timeRiseCFA
		int binRise = 0;
		for (int bin = 0; bin < binMax; bin++){
			if (samplesCorr[bin] < threshold)
				binRise = bin;  // last pass below threshold and before adcMax
		} // at this stage : binRise < timeRiseCFA/samplingTime <= binRise + 1 // timeRiseCFA is determined by assuming a linear fit between binRise and binRise + 1
		float slopeRise = 0;
		if (binRise + 1 <= binNumber-1)
			slopeRise = samplesCorr[binRise+1] - samplesCorr[binRise];
		float fittedBinRise = (slopeRise == 0) ? binRise : binRise + (threshold - samplesCorr[binRise])/slopeRise;
		timeRiseCFA = (fittedBinRise + binOffset)*samplingTime; // binOffset is determined in wavefromCorrection() // must be the same for all time ? // or must be defined using fittedBinRise*sparseSample 
		System.out.println("**** binRise : " + binRise);
		System.out.println("**** fittedBinRise : " + fittedBinRise);
		System.out.println("**** slopeRise : " + slopeRise);	
		
		// timeFallCFA
		int binFall = binMax;
		for (int bin = binMax; bin < binNumber; bin++){
			if (samplesCorr[bin] > threshold){
				binFall = bin;
			}
			else {
				binFall = bin;
				break; // first pass below the threshold
			}
		} // at this stage : binFall - 1 <= timeRiseCFA/samplingTime < binFall // timeFallCFA is determined by assuming a linear fit between binFall - 1 and binFall
		float slopeFall = 0;
		if (binFall - 1 >= 0)
			slopeFall = samplesCorr[binFall] - samplesCorr[binFall-1];
		float fittedBinFall = (slopeFall == 0) ? binFall : binFall + (threshold - samplesCorr[binFall-1])/slopeFall;
		timeFallCFA = (fittedBinFall + binOffset)*samplingTime;	
		
		// timeOverThreshold
		timeOverThresholdCFA = timeFallCFA - timeRiseCFA;
	}

	/**
	 * This methods extracts a time using the Constant Fraction Discriminator (CFD) algorithm
	 * It fills the attribut : timeCFD
	 * @param samplingTime : time between 2 ADC bins
	 * @param amplitudeFraction : a float between 0 and 1, CFD fraction parameter
	 * @param binDelay : CFD delay parameter
	 */
	private void computeTimeUsingConstantFractionDiscriminator(float samplingTime, float fractionCFD, int binDelayCFD){
		float[] signal = new float[binNumber];
		// signal generation
		for (int bin = 0; bin < binNumber; bin++){
			signal[bin] = (1 - fractionCFD)*samplesCorr[bin]; // we fill it with a fraction of the original signal
			if (bin < binNumber - binDelayCFD) 
				signal[bin] += -1*fractionCFD*samplesCorr[bin + binDelayCFD]; // we advance and invert a complementary fraction of the original signal and superimpose it to the previous signal 
		}
		// determine the two humps
		int binHumpSup = 0;
		int binHumpInf = 0;
		for (int bin = 0; bin < binNumber; bin++){
			if (signal[bin] > signal[binHumpSup])
				binHumpSup = bin;
		}
		for (int bin = 0; bin < binHumpSup; bin++){ // this loop has been added to be sure : binHumpInf < binHumpSup
			if (signal[bin] < signal[binHumpInf])
				binHumpInf = bin;
		}
		// research for zero
		int binZero = 0;
		for (int bin = binHumpInf; bin <= binHumpSup; bin++){
			if (signal[bin] < 0)
				binZero = bin; // last pass below zero
		} // at this stage : binZero < timeCFD/samplingTime <= binZero + 1 // timeCFD is determined by assuming a linear fit between binZero and binZero + 1
		float slopeCFD = 0;
		if (binZero + 1 <= binNumber)
			slopeCFD = signal[binZero+1] - signal[binZero];
		float fittedBinZero = (slopeCFD == 0) ? binZero : binZero + (0 - signal[binZero])/slopeCFD;
		timeCFD = (fittedBinZero + binOffset)*samplingTime;
		
	}
}
