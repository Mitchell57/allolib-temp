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
    if(k.key() == ' '){
      bool repeat = true;
      cout << "--- Note Abstraction Demo ---" << endl;
      while(repeat){
        cout << "Enter note as:  (s)-string  (m)-midi index   (q to quit)" << endl;
        char buf = 's';
        cin >> buf;
        Note n = Note();
        if(buf == 'q') break;
        else if(buf == 's'){
          std::string note;
          cout << "Enter note: ";
          cin >> note;
          if(note == "q") {
            repeat = false;
            break;
          }
          else{
            try{
              n.set(note);
            }
            catch (const std::out_of_range& oor) {
              std::cerr << "Out of Range error: " << oor.what() << '\n';
              break;
            }
            
          }
        }
        else if(buf == 'm'){
          int midi;
          cout << "Enter midi [0-127]: ";
          cin >> midi;
          if(midi == -1) {
            repeat = false;
            break;
          }
          else{
            try{
              n.set(midi);
            }
            catch (const std::out_of_range& oor) {
              std::cerr << "Out of Range error: " << oor.what() << '\n';
              break;
            }
            
          }
        }
        

        bool noteRepeat = true;
        while(noteRepeat){
          cout << "\nNote: " << n.name() << endl;
          cout << "Action: (a)-all [f]-freq [m]-midi [s]-scales [d]-distance [c]-chord [,]-decrement [.]-increment (q to quit)" << endl;
          cin >> buf;
          if(buf == 'q'){
            noteRepeat = false;
            break;
          }
          cout << "- - - - - - - - - - - - - -" << endl;
          if(buf== 'a'){
              cout << "Frequency: " << n.frequency() << " Hz" << endl << endl;
              cout << "Midi Index: " << n.midi() << endl << endl;
              printScale(n, Note::Major);
              printScale(n, Note::Minor);
              printScale(n, Note::Pent);

          }
          else if(buf=='f') { 
              cout << "Frequency = " << n.frequency() << endl;
          }
            else if(buf=='m') { 
              cout << "Midi Index = " << n.midi() << endl;
             }
           else  if(buf=='s') { 
              cout << "Output format: (s)-string  (m)-midi  (f)-freq" << endl;
              cin >> buf;
              printScale(n, Note::Major, buf);
              printScale(n, Note::Minor, buf);
              printScale(n, Note::Pent, buf);
              cout << endl;
            }

            else if(buf=='.') { 
              n.set(n.midi()+1);
            }
            else if(buf==',') { 
              n.set(n.midi()-1);
            }
            else if(buf == 'd'){
              std::string name;
              cout << "Enter note 2: " << endl;
              cin >> name;
              Note n2;
              try{
                n2.set(name);
              }
              catch (const std::out_of_range& oor) {
                std::cerr << "Out of Range error: " << oor.what() << '\n';
                break;
              }
              int dist = n.distanceTo(&n2);
              if(dist >= 0){
                cout << "\nDistance: " << n2.name() << " is " << dist << " semitones above " << n.name() << endl << endl;
              }
              else{
                cout << "\nDistance: " << n2.name() << " is " << dist*-1 << " semitones below " << n.name() << endl << endl;
              }
            }

            if(buf == 'c'){
              bool intervalrepeat = true;
              while(intervalrepeat)
              {
                std::string name="Maj";
                int inv=0;
                cout << "Enter chord  and inversion (l for list): ";
                cin >> name;
                if(name == "q"){
                  intervalrepeat= false;
                  break;
                }
                if(name == "l"){
                  cout << "\nMaj, min, Dim, Aug, Maj7, min7, Dom7  |  Inversion [0-2]\n" << endl;
                }

                else{
                  cin >> inv;
                  std::vector<Note> chord;
                  if(name=="Maj") chord = n.chord(Note::Maj, inv);
                  if(name=="min") chord = n.chord(Note::min, inv);
                  if(name=="Dim") chord = n.chord(Note::Dim, inv);
                  if(name=="Aug") chord = n.chord(Note::Aug, inv);
                  if(name=="Maj7") chord = n.chord(Note::Maj7, inv);
                  if(name=="min7") chord = n.chord(Note::min7, inv);
                  if(name=="Dom7") chord = n.chord(Note::Dom7, inv);

                  cout << endl << n.name() << " " << name << " chord: ";
                  for(int i=0; i<chord.size(); i++){
                    if(i != 0) cout << ", ";
                    cout << chord[i].name();
                  }
                  cout << endl << endl;
                }
              }
            }
          
          cout << "- - - - - - - - - - - - - -" << endl;
        }
        
      } 
    }

    return true;
  }

  void printScale(Note root, Note::scale_type type, char format='s'){
    std::vector<Note> scale = root.scale(type);
    std::string name;
    switch(type){
      case Note::Major:
        name = "Major";
        break;
      case Note::Minor:
        name = "Minor";
        break;
      case Note::Pent:
        name = "Pentatonic";
        break;
    }

    cout << root.key() << " " << name << " Scale:   ";
    for(int i=0; i<scale.size(); i++){
      if(i != 0) cout << ", ";
      if(format == 's') cout << scale[i].name();
      else if(format == 'm') cout << scale[i].midi();
      else if(format == 'f') cout << scale[i].frequency();

    }
    cout << endl << endl;
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
