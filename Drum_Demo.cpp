
#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Gamma.h"
#include "Gamma/Oscillator.h"
#include "Gamma/Types.h"
#include "Gamma/SamplePlayer.h"

#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/scene/al_PolySynth.hpp"
#include "al/scene/al_SynthSequencer.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"

// using namespace gam;
using namespace al;
using namespace std;

class Kick : public SynthVoice {
 public:
  // Unit generators
  gam::Pan<> mPan;
  gam::Sine<> mOsc;
  gam::Decay<> mDecay; // Added decay envelope for pitch
  gam::AD<> mAmpEnv; // Changed amp envelope from Env<3> to AD<>

  void init() override {
    // Intialize amplitude envelope
    // - Minimum attack (to make it thump)
    // - Short decay
    // - Maximum amplitude
    mAmpEnv.attack(0.01);
    mAmpEnv.decay(0.3);
    mAmpEnv.amp(1.0);

    // Initialize pitch decay 
    mDecay.decay(0.3);

    createInternalTriggerParameter("amplitude", 0.3, 0.0, 1.0);
    createInternalTriggerParameter("frequency", 60, 20, 5000);
  }

  // The audio processing function
  void onProcess(AudioIOData& io) override {
    mOsc.freq(getInternalParameterValue("frequency"));
    mPan.pos(0);
    // (removed parameter control for attack and release)

    while (io()) {
      mOsc.freqMul(mDecay()); // Multiply pitch oscillator by next decay value
      float s1 = mOsc() *  mAmpEnv() * getInternalParameterValue("amplitude");
      float s2;
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }

    if (mAmpEnv.done()) free();
  }

  void onTriggerOn() override { mAmpEnv.reset(); mDecay.reset(); }

  void onTriggerOff() override { mAmpEnv.release(); mDecay.finish(); }
};

/* ---------------------------------------------------------------- */

class Hihat : public SynthVoice {
 public:
  // Unit generators
  gam::Pan<> mPan;
  gam::AD<> mAmpEnv; // Changed amp envelope from Env<3> to AD<>
  
  gam::Burst mBurst;

  void init() override {
    // Initialize burst 
    mBurst = gam::Burst(20000, 15000, 0.05);

  }

  // The audio processing function
  void onProcess(AudioIOData& io) override {
    while (io()) {
      float s1 = mBurst();
      float s2;
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // Left this in because I'm not sure how to tell when a burst is done
    if (mAmpEnv.done()) free();
  }
  void onTriggerOn() override { mBurst.reset(); }
  //void onTriggerOff() override {  }
};

/* ---------------------------------------------------------------- */

class Snare : public SynthVoice {
 public:
  // Unit generators
  gam::Pan<> mPan;
  gam::AD<> mAmpEnv;
  gam::Sine<> mOsc;
  gam::Decay<> mDecay;

  
  // Noise to simulate chains
  gam::Burst mBurst;


  void init() override {
    // Initialize burst 
    mBurst = gam::Burst(10000, 5000, 0.3);

    mAmpEnv.attack(0.01);
    mAmpEnv.decay(0.01);
    mAmpEnv.amp(1.0);

    // Initialize pitch decay 
    mDecay.decay(0.8);

  }

  // The audio processing function
  void onProcess(AudioIOData& io) override {
    mOsc.freq(250);

    while (io()) {
      mOsc.freqMul(mDecay());
      float s1 = mBurst() + (mOsc() * mAmpEnv() * 0.1);
      float s2;
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    
    if (mAmpEnv.done()) free();
  }
  void onTriggerOn() override { mBurst.reset(); mAmpEnv.reset(); mDecay.reset();}
  
  void onTriggerOff() override { mAmpEnv.release(); mDecay.finish(); }
};

/* ---------------------------------------------------------------- */

class MyApp : public App {
 public:
  SynthGUIManager<Kick> synthManager{"Kick"};

  // Added SamplePlayer to mix in external audio clips
  // gam::SamplePlayer<> samplePlayer;
  // bool paused = true;

  ParameterMIDI parameterMIDI;
  int midiNote;

  gam::Burst mBurst();

  void onInit() override {
    // Set sampling rate for Gamma objects from app's audio
    gam::sampleRate(audioIO().framesPerSecond());
  }

  void onCreate() override {
    imguiInit();

    navControl().active(false);  // Disable navigation via keyboard, since we
                                 // will be using keyboard for note triggering

    // Play example sequence. Comment this line to start from scratch
    //    synthManager.synthSequencer().playSequence("synth4.synthSequence");
    synthManager.synthRecorder().verbose(true);

    // Load audio sample
    // samplePlayer.load("test2.wav");
    // samplePlayer.loop();
  }

  void onSound(AudioIOData& io) override {
    synthManager.render(io);  // Render audio
    
    // After rendering synths, 
    // while(io() && !paused){  
    //   float s = samplePlayer();
    //   io.out(0) +=  s*0.6;
    //   io.out(1) += s*0.6;
	  // }
  }

  void onAnimate(double dt) override {
    imguiBeginFrame();
    synthManager.drawSynthControlPanel();
    imguiEndFrame();
  }

  void onDraw(Graphics& g) override {
    g.clear();
    synthManager.render(g);

    imguiDraw();
  }

  bool onKeyDown(Keyboard const& k) override {
    // pressing space starts audio clip
    //if(k.key() == ' ') paused = !paused;
    
    // testing grounds
    if(k.key() == 'e') playHihat(0, 0.1);
    if(k.key() == 'w') playSnare(0, 0.2);
    if(k.key() == 'q') playKick(100, 0, 0.4, 0.9);
    if(k.key() == 'd') playBeat(110);
    
    
    return true;
  }

  bool onKeyUp(Keyboard const& k) override {
    int midiNote = asciiToMIDI(k.key());
    if (midiNote > 0) {
      synthManager.triggerOff(midiNote);
      synthManager.triggerOff(midiNote - 24);  // Trigger both off for safety
    }
    return true;
  }

  void onExit() override { imguiShutdown(); }

  void playKick(float freq, float time, float duration = 0.5, float amp = 0.2, float attack = 0.01, float decay = 0.1)
  {
      auto *voice = synthManager.synth().getVoice<Kick>();
      // amp, freq, attack, release, pan
      voice->setTriggerParams({amp, freq, 0.01, 0.1, 0.0});
      voice->setInternalParameterValue("freq", freq);
      synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }

  void playHihat(float time, float duration = 0.3)
  {
      auto *voice = synthManager.synth().getVoice<Hihat>();
      // amp, freq, attack, release, pan
      synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }

  void playSnare(float time, float duration = 0.3)
  {
      auto *voice = synthManager.synth().getVoice<Snare>();
      // amp, freq, attack, release, pan
      synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }

  void playBeat(float tempo){
    float beat = 60./tempo;


    
    playKick(100, 0*beat, 0.4, 0.9);
    playHihat(0*beat);

    playHihat(0.5*beat);

    playSnare(1*beat, 0.1);
    playHihat(1*beat);

    playHihat(1.5*beat);

    playKick(100, 2*beat, 0.4, 0.9);
    playHihat(2*beat);

    playKick(100, 2.5*beat, 0.4, 0.9);
    playHihat(2.5*beat);

    playSnare(3*beat, 0.1);
    playHihat(3*beat);

    playHihat(3.5*beat);
  }


};

int main() {
  MyApp app;

  // Set up audio
  app.configureAudio(48000., 512, 2, 0);

  app.start();
}
