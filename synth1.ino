// Simple DAC sine wave test on Teensy 3.1

float phase = 0.0;
float twopi = 3.14159 * 2;
elapsedMicros usec = 0;


const int length = 512 * 2;
double sineTable[length];
double lineTable[length];
double *currentBuffer = sineTable;
int currentBufferIndex = 0;
int currentSample;

double testSample = 0;
double frequency = 440;

float lpA = .1;
float lpHist = 0;
float amp = 0;

IntervalTimer myTimer;

void audioCallback(){
  
  //float val = sin(phase) * 2000.0 / 20 + 2050.0;
  //val += sin(phase * 2) * 2000. / 20.;
  //analogWrite(A14, (int)val);
  phase += .000022 * frequency;
  if (phase >= 1.){
    phase -= 1;
  }
  
  int index = length * phase;
  
  float val = currentBuffer[index] * 2000. / 20. * amp   + 2050.;
  amp *= .9995;
  lpHist = val = val * lpA + lpHist * (1. - lpA);
  analogWrite(A14, (int)val);
  if (++currentSample > length){
    currentSample = 0;
  //  if (currentBufferIndex){
  //    currentBuffer = sineTable;
  //    currentBufferIndex = 0;
  //  }
  //  else{
  //    currentBuffer = lineTable;
  //    currentBufferIndex = 1;
  //  }
  }
}

void setup() {
  analogWriteResolution(12);
  
  for (int i = 0; i < length; ++i){
    lineTable[i] = (double)i / (double)length;
    sineTable[i] = .5 - cos((double)i / (double)length * 2. * 3.1415) / 2.;
  }
  
  
  Serial.begin(115200);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  
  
  myTimer.begin(audioCallback, 22 * 1.2);  // audiocallback 
  //Apparently the teensy does not handle 44100 hz
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  currentBuffer = lineTable;
 
  frequency = 440. * pow (2., (double)(note - 64) / 12.);
  amp = 1;
}


void OnNoteOff(byte channel, byte note, byte velocity) {
  //currentBuffer = sineTable;
}

void loop() {
  //float val = sin(phase) * 2000.0 / 20 + 2050.0;
  //val += sin(phase * 2) * 2000. / 20.;
  //analogWrite(A14, (int)val);
  //phase = phase + 0.02 * 2;
  //if (phase >= twopi) phase = 0;
  
  //audioCallback();
  //int samplePeriod = 22;
  //while (usec < samplePeriod) ; // wait
  //usec = usec - samplePeriod;
  
  //int waitTime = 100000;
  //while (usec < waitTime);
  //if (currentBuffer == sineTable){
  //  currentBuffer = lineTable;
  //}
  //else{
  //  currentBuffer = sineTable;
  //}
  //usec -= waitTime;
  //frequency += 1;
  usbMIDI.read();
  
}
