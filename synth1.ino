// Overclocked to 96MHz
// Mattias Larsson Sköld

float phase = 0.0;
float twopi = 3.14159 * 2;
elapsedMicros usec = 0;


const int length = 512 * 2;
float sineTable[length];
float lineTable[length];
float *currentBuffer = sineTable;
int currentBufferIndex = 0;
int currentSample;

double testSample = 0;
double frequency = 440;

float lpA = .1;
float lpHist = 0;
float amp = 0;

IntervalTimer myTimer;

const int BufferSize = 512 * 2;
float buffer1[BufferSize];
float buffer2[BufferSize];
bool bufferEmpty = true;
float* nextBuffer = buffer1;
float* writingToBuffer = buffer1;
float envelope;

inline float* otherBuffer(float *buffer){
  if (buffer == buffer1){
    return buffer2;
  }
  else{
    return buffer1;
  }
}

inline float generateWave(float *table, float phase){
  return table[((int)(length * (phase - (int) phase)))];
}

float lfoPhase = 0;
float phase2 = 0;

void process(float *buffer){
  bufferEmpty = false;
  for (int i = 0; i < BufferSize; ++i){
    phase += .000022 * frequency;
    phase2 += .00002201 * frequency;
    if (phase >= 1.){
      phase -= 1;
    }
    //if (phase2 >= 1.){
    //  phase2 -= 1;
    //}
    
    float val = generateWave(currentBuffer, phase) * amp;
    //val += generateWave(currentBuffer, phase2) * amp;
    
    lfoPhase += .0004;
    if (lfoPhase > 1.){
      lfoPhase = 0;
    }
    float lfo = generateWave(sineTable, lfoPhase);
    
    amp *= .9999;
    if (envelope < .99){
      envelope += .00002;
    }
    
    lpA = envelope * .5 + lfo / 4.;
    lpHist = val = val * lpA * lfoPhase + lpHist * (1. - lpA);
    nextBuffer[i] = val;
  }
}

void audioCallback(){
  if (++currentSample >= length){
    currentSample = 0;
    bufferEmpty = true;
    writingToBuffer = otherBuffer(writingToBuffer);
  }
  
  int val = 200 * writingToBuffer[currentSample] + 2050;

  analogWrite(A14, val);
}

void setup() {
  analogWriteResolution(12);
  
  for (int i = 0; i < length; ++i){
    lineTable[i] = (double)i / (double)length - .5;
    sineTable[i] = .5 - cos((double)i / (double)length * 2. * 3.1415) / 2.;
  }
  
  
  Serial.begin(115200);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  
  process(buffer1);
  process(buffer2);
  
  myTimer.begin(audioCallback, 22 * 2);  // audiocallback 
  //Apparently the teensy does not handle 44100 hz
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  currentBuffer = lineTable;
 
  frequency = 440. * pow (2., (double)(note - 64) / 12.);
  amp = (float)velocity / 255.;
  envelope = 0;
}


void OnNoteOff(byte channel, byte note, byte velocity) {
  //currentBuffer = sineTable;
}

void loop() {
  while (!bufferEmpty){
    usbMIDI.read();
  }
  
  process(nextBuffer);
  nextBuffer = otherBuffer(nextBuffer);
}
