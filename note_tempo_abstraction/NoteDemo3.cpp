#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/scene/al_PolySynth.hpp"
#include "al/scene/al_SynthSequencer.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"

#include "note.h"

// using namespace gam;
using namespace al;
using namespace std;


class MyApp : public App {
 public:
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
    //synthManager.synthRecorder().verbose(true);

    // Load audio sample (files go in bin folder)
    //if(hasSample) samplePlayer.load("guitartest.wav");

  }

  void onSound(AudioIOData& io) override {
    // synthManager.render(io);  // Render audio
    
    // // After rendering synths, 
    // while(io() && !paused && hasSample){  
    //   float s = samplePlayer();
    //   io.out(0) +=  s;
    //   io.out(1) += s;
	  // }
  }

  void onAnimate(double dt) override {
    imguiBeginFrame();
    //synthManager.drawSynthControlPanel();
    imguiEndFrame();
  }

  void onDraw(Graphics& g) override {
    g.clear();
    //synthManager.render(g);

    imguiDraw();
  }

  bool onKeyDown(Keyboard const& k) override {
    std::string chord;
    while(true){
        cout << "Enter chord: ";
        cin >> chord;
        Note::getChord(chord);
    }
    

    return true;
  }

  
  bool onKeyUp(Keyboard const& k) override {
    // int midiNote = asciiToMIDI(k.key());
    // if (midiNote > 0) {
    //   synthManager.triggerOff(midiNote);
    //   synthManager.triggerOff(midiNote - 24);  // Trigger both off for safety
    // }
    return true;
  }

  void onExit() override { imguiShutdown(); }

};

int main() {
  MyApp app;

  // Set up audio
  app.configureAudio(48000., 512, 2, 0);

  app.start();
}
