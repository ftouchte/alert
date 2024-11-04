import java.util.List;
import java.util.ArrayList;

public class Test {
	public static void main(String[] args) {
		short[] samples = {1,2,3,4,5,6,7,1,1};
		//short[] samples = {1,2,3,4,5,6,4095,4095,4095,4095,4095,4,3,2,1};

		System.out.println("=====> Test HipoExtractor");
		HipoExtractor H = new HipoExtractor();

		System.out.println("=====> Setting parameters");
		short adcOffset = 0;
		if (samples.length > 5) {
			for (int bin=0; bin<5; ++bin)
				adcOffset += samples[bin];
		}
		adcOffset = Integer.valueOf(adcOffset / 5).shortValue();
		adcOffset = 0;
		H.adcOffset = adcOffset; // adc
		H.samplingTime = 44; // ns
		H.sparseSample = 0;
		H.timeStamp = 0;
		H.fineTimeStampResolution = 0;
		System.out.println("adcOffset    = " + H.adcOffset + " adc");
		System.out.println("samplingTime = " + H.samplingTime + " ns");
		System.out.println("sparseSample = " + H.sparseSample);
		System.out.println("timeStample = " + H.timeStamp);
		System.out.println("fineTimeStampleResolution = " + H.fineTimeStampResolution);
		
		System.out.println("=====> Application of the HipoExtractor");
		List<Pulse> pulses = H.extract(samples);
		System.out.println("binMax = " + H.binMax);
		System.out.println("adcMax = " + H.adcMax);
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

	// Setting parameters
	public float samplingTime;
	public int sparseSample;
	public short adcOffset; // à discuter avec Raphaël, est-ce que je le définis moi-même comme en simulation ?
	long timeStamp;
	float fineTimeStampResolution;
	public static final short ADC_LIMIT = 4095; // 2^12-1
	

	/**
	 * @param samples : waveform samples stored in an array of short
	 * @param id : 
	 * @param pars : 
	 */
	public List<Pulse> extract(short... samples){
		waveformCorrection(adcOffset,samplingTime,samples,sparseSample);
		fitAverage(samplingTime);
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
	private void getTimeAtConstantFractionAmplitude(float samplingTime, float amplitudeFraction){
		
	}

	/**
	 * This methods extracts a time using the Constant Fraction Discriminator (CFD) algorithm
	 * It fills the attribut : timeCFD
	 * @param samplingTime : time between 2 ADC bins
	 * @param amplitudeFraction : a float between 0 and 1, CFD fraction parameter
	 * @param binDelay : CFD delay parameter
	 */
	private void getTimeUsingConstantFractionDiscriminator(float samplingTime, float amplitudeFraction, int binDelay){

	}
}