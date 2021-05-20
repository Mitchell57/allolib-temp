#include <cstdio> // for printing to stdout

#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Oscillator.h"

#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/scene/al_PolySynth.hpp"
#include "al/scene/al_SynthSequencer.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"

#include "note.h"
#include "tempo.h"

// using namespace gam;
using namespace al;

// This example shows how to use SynthVoice and SynthManagerto create an audio
// visual synthesizer. In a class that inherits from SynthVoice you will
// define the synth's voice parameters and the sound and graphic generation
// processes in the onProcess() functions.

class SquareWave : public SynthVoice
{
public:
  // Unit generators
  gam::Pan<> mPan;
  gam::Sine<> mOsc1;
  gam::Sine<> mOsc3;
  gam::Sine<> mOsc5;
  gam::Sine<> mOsc7;

  gam::Env<3> mAmpEnv;

  // Initialize voice. This function will only be called once per voice when
  // it is created. Voices will be reused if they are idle.
  void init() override
  {
    // Intialize envelope
    mAmpEnv.curve(0); // make segments lines
    mAmpEnv.levels(0, 1, 1, 0);
    mAmpEnv.sustainPoint(2); // Make point 2 sustain until a release is issued

    createInternalTriggerParameter("amplitude", 0.8, 0.0, 1.0);
    createInternalTriggerParameter("frequency", 440, 20, 5000);
    createInternalTriggerParameter("attackTime", 0.1, 0.01, 3.0);
    createInternalTriggerParameter("releaseTime", 0.1, 0.1, 10.0);
    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
  }

  // The audio processing function
  void onProcess(AudioIOData &io) override
  {
    // Get the values from the parameters and apply them to the corresponding
    // unit generators. You could place these lines in the onTrigger() function,
    // but placing them here allows for realtime prototyping on a running
    // voice, rather than having to trigger a new voice to hear the changes.
    // Parameters will update values once per audio callback because they
    // are outside the sample processing loop.
    float f = getInternalParameterValue("frequency");
    mOsc1.freq(f);
    mOsc3.freq(f * 3);
    mOsc5.freq(f * 5);
    mOsc7.freq(f * 7);

    float a = getInternalParameterValue("amplitude");
    mAmpEnv.lengths()[0] = getInternalParameterValue("attackTime");
    mAmpEnv.lengths()[2] = getInternalParameterValue("releaseTime");
    mPan.pos(getInternalParameterValue("pan"));
    while (io())
    {
      float s1 = mAmpEnv() * (mOsc1() * a +
                              mOsc3() * (a / 3.0) +
                              mOsc5() * (a / 5.0) +
                              mOsc7() * (a / 7.0));

      float s2;
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // We need to let the synth know that this voice is done
    // by calling the free(). This takes the voice out of the
    // rendering chain
    if (mAmpEnv.done())
      free();
  }

  // The triggering functions just need to tell the envelope to start or release
  // The audio processing function checks when the envelope is done to remove
  // the voice from the processing chain.
  void onTriggerOn() override { mAmpEnv.reset(); }
  void onTriggerOff() override { mAmpEnv.release(); }
};

// We make an app.
class MyApp : public App
{
public:
  // GUI manager for SquareWave voices
  // The name provided determines the name of the directory
  // where the presets and sequences are stored
  SynthGUIManager<SquareWave> synthManager{"SquareWave"};

  // This function is called right after the window is created
  // It provides a grphics context to initialize ParameterGUI
  // It's also a good place to put things that should
  // happen once at startup.
  void onCreate() override
  {
    navControl().active(false); // Disable navigation via keyboard, since we
                                // will be using keyboard for note triggering

    // Set sampling rate for Gamma objects from app's audio
    gam::sampleRate(audioIO().framesPerSecond());

    imguiInit();

    // Play example sequence. Comment this line to start from scratch
    // synthManager.synthSequencer().playSequence("synth1.synthSequence");
    synthManager.synthRecorder().verbose(true);
  }

  // The audio callback function. Called when audio hardware requires data
  void onSound(AudioIOData &io) override
  {
    synthManager.render(io); // Render audio
  }

  void onAnimate(double dt) override
  {
    // The GUI is prepared here
    imguiBeginFrame();
    // Draw a window that contains the synth control panel
    synthManager.drawSynthControlPanel();
    imguiEndFrame();
  }

  // The graphics callback function.
  void onDraw(Graphics &g) override
  {
    g.clear();
    // Render the synth's graphics
    synthManager.render(g);

    // GUI is drawn here
    imguiDraw();
  }

  // Whenever a key is pressed, this function is called
  bool onKeyDown(Keyboard const &k) override
  {
    if (ParameterGUI::usingKeyboard())
    { // Ignore keys if GUI is using
      // keyboard
      return true;
    }

    switch (k.key())
    {
    case 'a':
      playHappyBirthday(Note("C4"), 90);
      return false;
    case 's':
      playHappyBirthday(Note("G3"), 120);
      return false;

    
    }

    return true;
  }

  // Whenever a key is released this function is called
  bool
  onKeyUp(Keyboard const &k) override
  {
    int midiNote = asciiToMIDI(k.key());
    if (midiNote > 0)
    {
      synthManager.triggerOff(midiNote);
    }
    return true;
  }

  void onExit() override { imguiShutdown(); }

  // New code: a function to play a note A

  float playNote(float time, Note note, float duration = 0.5, float amp = 0.2, float attack = 0.1, float decay = 0.5)
  {
    auto *voice = synthManager.synth().getVoice<SquareWave>();
    // amp, freq, attack, release, pan
    voice->setTriggerParams({amp, note.frequency(), 0.1, 0.1, 0.0});
    synthManager.synthSequencer().addVoiceFromNow(voice, time, duration*0.9);

    return time+duration;
  }

  float playChord(float time, std::vector<Note> chord, float duration){
      for(int i=0; i<chord.size(); i++){
          playNote(time, chord[i], duration, 0.05);
      }
      return time+duration;
  }

  void playHappyBirthday(Note root, float tempo)
  {
    // Happy birthday uses: P1(C), M2(D), M3(E), P4(F), P5(G), M6(A), and m7(Bb)
    std::vector<Note> majScale = root.scale(Note::Major);
    Note M2 = majScale[1];
    Note M3 = majScale[2];
    Note P4 = majScale[3];
    Note P5 = majScale[4];
    Note M6 = majScale[5];
    Note m7 = root.interval(Note::m7);
    Note P8 = majScale[7];

    // For chords it needs: F Maj, C Dom7, Bb Maj, F/C (2nd inversion)
    std::vector<Note> chord1 = P4.chord(Note::Maj);
    std::vector<Note> chord2 = root.chord(Note::Dom7);
    
    // This m7 is pretty high so we'll drop the chord down an octave
    Note m7Low = m7.interval(Note::P8, -1);
    std::vector<Note> chord3 = m7Low.chord(Note::Maj);
    std::vector<Note> chord4 = P4.chord(Note::Maj, 2); // 2nd inversion

    // Now lets set up a tempo
    //  syntax: Tempo(bpm, time signature top, time signature bottom)
    float time = 0;
    Tempo t(tempo, 3, 4); 
    // this allows us to say get exact durations for common note types

    time = playNote(time, root, t.duration(Tempo::eighth, true)); // true = dotted note
    time = playNote(time, root, t.duration(Tempo::sixteenth)); 

    playChord(time, chord1, t.duration(Tempo::half));
    time = playNote(time, M2, t.duration(Tempo::quarter)); 
    time = playNote(time, root, t.duration(Tempo::quarter)); 
    time = playNote(time, P4, t.duration(Tempo::quarter)); 

    playChord(time, chord2, t.duration(Tempo::half));
    time = playNote(time, root, t.duration(Tempo::half)); 
    time = playNote(time, root, t.duration(Tempo::eighth, true)); // true = dotted note
    time = playNote(time, root, t.duration(Tempo::sixteenth));

    time = playNote(time, M2, t.duration(Tempo::quarter)); 
    time = playNote(time, root, t.duration(Tempo::quarter)); 
    time = playNote(time, P5, t.duration(Tempo::quarter)); 

    playChord(time, chord1, t.duration(Tempo::half));
    time = playNote(time, P4, t.duration(Tempo::half)); 
    time = playNote(time, root, t.duration(Tempo::eighth, true)); // true = dotted note
    time = playNote(time, root, t.duration(Tempo::sixteenth));

    time = playNote(time, P8, t.duration(Tempo::quarter)); 
    time = playNote(time, M6, t.duration(Tempo::quarter)); 
    time = playNote(time, P4, t.duration(Tempo::quarter)); 

    playChord(time, chord3, t.duration(Tempo::half));
    time = playNote(time, M3, t.duration(Tempo::quarter)); 
    time = playNote(time, M2, t.duration(Tempo::quarter)); 
    time = playNote(time, m7, t.duration(Tempo::eighth, true)); // true = dotted note
    time = playNote(time, m7, t.duration(Tempo::sixteenth));

    playChord(time, chord4, t.duration(Tempo::half));
    time = playNote(time, M6, t.duration(Tempo::quarter)); 
    time = playNote(time, P4, t.duration(Tempo::quarter)); 
    playChord(time, chord2, t.duration(Tempo::quarter));   
    time = playNote(time, P5, t.duration(Tempo::quarter));  

    playChord(time, chord1, t.duration(Tempo::half));
    time = playNote(time, P4, t.duration(Tempo::half)); 

  }



  };

  

int main()
{
  // Create app instance
  MyApp app;

  // Set up audio
  app.configureAudio(48000., 512, 2, 0);

  app.start();

  return 0;
}
