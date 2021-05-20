#pragma once

#include <string>
#include <vector>
#include <stdio.h>
#include <ostream>
#include <assert.h>  
#include <regex>

const static int numChords = 11;
const static int maxChordLength = 6;
const static int chord_table[numChords][maxChordLength] = {
    {0, 4, 7, -1,-1,-1},   // Major
    {0, 3, 7, -1,-1,-1},   // Minor
    {0, 3, 6,- 1,-1,-1},   // Diminished
    {0, 4, 7, 11, -1,-1},  // Maj7
    {0, 3, 7, 10, -1,-1},  // Minor7
    {0, 4, 7, 10, -1,-1},  // Dom7
    {0, 2, 7, -1,-1,-1},   // Sus2
    {0, 5, 7, -1,-1,-1},   // Sus4
    {0, 4, 8, -1,-1,-1},   // Aug
    {0, 4, 7, 10, 14, -1}, // Dom9
    {0, 4, 7, 11, 14, 17}, // Maj11
};

const static int numScales = 3;
const static int maxScaleLength = 8;
const static int scale_table[numScales][maxScaleLength] = {
    {0, 2, 4, 5, 7, 9, 11, 12}, // Major
    {0, 2, 3, 5, 7, 8, 10, 12}, // Minor
    {0, 2, 4, 7, 9, 12, -1, -1} // Pentatonic          
};

const static int numIntervals = 26;
const static int interval_table[numIntervals] = {
    0, 5, 7, 12,          // Perfect
    1, 3, 8, 10,          // Minor
    2, 4, 9, 11,          // Major
    0, 2, 4, 6, 7, 9, 11, // Diminished
    1, 3, 5, 6, 8, 10, 12 // Augmented
};

class Note {
    public:
        char signPref;
        int index;

        struct parsed_string{
            char note, sign;
            int octave;
        };

        // Type enums must be in same order as table declarations
        enum scale_type {Major, Minor, Pent};

        enum chord_type {Maj, min, Dim, Maj7, min7, Dom7, sus2, sus4, Aug, Dom9, Maj11};
        
        enum interval_type {
            P1, P4, P5, P8, 
            m2, m3, m6, m7, 
            M2, M3, M6, M7,
            d2, d3, d4, d5, d6, d7, d8,
            A1, A2, A3, A4, A5, A6, A7
        };
        
        // default constructor
        //  Note(), Note('C'), Note('C', '#'), Note('C', 2), Note('C', '#', 2)
        Note(char note='A', char sign='n', int octave=4, char signPref='b'){ 
            parsed_string parsed = {note, sign, octave};
            init(parsedToMidi(parsed), signPref);
        }

        // string constructor
        //  Note("C"), Note("C2"), Note("G#5") = G#5
        Note(std::string input, char signPref='n'){ 
            this->set(input, signPref);
        }

        // midi constructor
        //  Note(69), Note(70, '#' or 'b')
        Note(int midi, char signPref='b'){ init(midi, signPref); }

        // Main initializer
        void init(int midi, char signPref){
            if(midi > 127 || midi <0){
                throw std::out_of_range("Note(midi) : midi index ("+std::to_string(midi)+") is out of range");
            }
            
            this->index = midi;
            this->signPref = signPref;
        }

        // Re-initializers
        void set(int midi, char signPref='b'){ init(midi, signPref); }

        void set(std::string input, char signPref='n'){ 
            parsed_string parsed = parseString(input);
            if(signPref == 'n'){
                if(parsed.sign == '#') { init(parsedToMidi(parsed), '#'); }
                else{ init(parsedToMidi(parsed), 'b'); }
            }
            else{ init(parsedToMidi(parsed), signPref); } 
        }
        
        void set(char note='A', char sign='n', int octave=4, char signPref='b'){ 
            parsed_string parsed = {note, sign, octave};
            init(parsedToMidi(parsed), signPref);
        }

        void setSignPref(char s){ 
            std::regex sign_regex("[#nb]");
            assert(regex_match(std::to_string(s), sign_regex));

            this->signPref = s; 
        }

        void setOctave(int octave=4){
            assert(octave > -2 && octave < 10);
            int noteIdx = this->index%12;
            int offset = (octave+1)*12;

            this->index = noteIdx + offset;

        }

// ------------------------------------------------------------------
//      Access / Translation
// ------------------------------------------------------------------     

        // returns full note name (e.g. "Db6")
        std::string name(){
            return midiToString(this->index, this->signPref);
        }

        // returns key without octave (e.g. "Db")
        std::string key(){
            return midiToString(this->index, this->signPref, false);
        }

        // returns midi index
        int midi(){
            return this->index;
        }

        // returns frequency (based on root)
        float frequency(float root=440.0){
            int distance = this->index - 69;
            double multiplier = pow(2.0, 1.0/12);

            return (float)(root*pow(multiplier, distance));
        }

        int octave(){
            return (this->midi()/12)-1;
        }

        // returns distance (in semitones) to another note
        int distanceTo(Note* b){
            return b->midi() - this->index;
        }

// ------------------------------------------------------------------
//      Intervals / Chords / Scales
// ------------------------------------------------------------------

        // returns note at specified interval above/below current note
        // direction = 1 for up
        // direction = -1 for down
        Note interval(interval_type type, int direction=1){
            int interval = interval_table[type] * direction;
            Note n = Note(this->index + interval);
            return n;
        }
        
        // returns vector of Notes corresponding to root note, chord type, and inversion
        std::vector<Note> chord(chord_type type, int inv=0){
            std::vector<Note> ret;

            // loop through chord intervals to build list of notes
            for(int i=0; i<maxChordLength; i++){
                int interval = chord_table[type][i];
                if(interval >= 0 ){ // variable length chords, fixed length array, filled space with -1s
                    int idx = this->index + interval;
                    Note cnote = Note(idx, signPref);
                    ret.push_back(cnote);
                }
            }

            // 1st inversion - move root up an octave
            if(inv == 1){
                Note root = ret[0];
                root.set(root.midi()+12);
                std::rotate(ret.begin(), ret.begin() + 1, ret.end());
            }

            // 2nd inversion - move first two notes up an octave
            else if(inv == 2){
                Note root = ret[0];
                Note one = ret[1];
                root.set(root.midi()+12);
                one.set(one.midi()+12);
                std::rotate(ret.begin(), ret.begin() + 2, ret.end());
            }
            return ret;
        }

        std::vector<Note> scale(scale_type type=Major){
            std::vector<Note> ret;
            for(int i=0; i<maxScaleLength; i++){
                int interval = scale_table[type][i];
                if(interval == -1) break;
                else{
                    Note snote = Note(this->index + interval, signPref);
                    ret.push_back(snote);
                }
            }
            return ret;
        }

// ------------------------------------------------------------------
//      Utility
// ------------------------------------------------------------------
        
        // takes string input
        // returns Midi index if valid
        static int stringToMidi(std::string str){
            parsed_string parsed = parseString(str);
            
            return parsedToMidi(parsed);
        }

        // takes string input
        // returns {note, sign, octave} if valid
        static Note::parsed_string parseString(std::string str){
            std::regex note_regex("[a-gA-G]");
            std::regex sign_regex("[#nb]");
            std::regex octave_regex("\\-1|[0-9]");
            
            std::string toParse = str;
            parsed_string ret;

            if(str.length() < 1 ){
                throw std::out_of_range("Note(string) : input string ("+str+") is too short");
            }

            if(regex_match(toParse.substr(0,1), note_regex)){
                ret.note = toParse[0];
                if(toParse.length() == 1){
                    ret.sign = 'n';
                    ret.octave = 4;
                    return ret;
                }
                toParse = toParse.substr(1);
            }
            else{
                throw std::out_of_range("Note(string) : input string ("+str+") is invalid (First char is not valid note)");
            }

            if(regex_match(toParse.substr(0,1), sign_regex)){
                ret.sign = toParse[0];
                if(toParse.length() == 1){
                    ret.octave = 4;
                    return ret;
                }
                
                toParse = toParse.substr(1);
            }

            if(regex_match(toParse, octave_regex)){
                if(toParse[0] == '-'){
                    ret.octave = -1*atoi(&toParse[1]);
                    return ret;
                }
                else{
                    ret.octave = atoi(&toParse[0]);
                    return ret;
                }
            }

            throw std::out_of_range("Note(string) : input string ("+str+") is invalid EoF");
        }

        // takes parsed string input
        // returns midi index if valid
        static int parsedToMidi(Note::parsed_string parsed){
            std::regex note_regex("[a-gA-G]");
            std::regex sign_regex("[#nb]");
            
            // Validate input
            assert(regex_match(std::to_string(parsed.note), note_regex));
            assert(regex_match(std::to_string(parsed.sign), sign_regex));
            assert(parsed.octave > -1 && parsed.octave < 9);
            
            int octDist = (parsed.octave-4)*12;
            int noteDist = 0;

            switch(parsed.note){
                case 'C': case 'c':
                    noteDist = -9;
                    break;
                case 'D': case 'd':
                    noteDist = -7;
                    break;
                case 'E': case 'e':
                    noteDist = -5;
                    break;
                case 'F': case 'f':
                    noteDist = -4;
                    break;
                case 'G': case 'g':
                    noteDist = -2;
                    break;
                case 'A': case 'a':
                    noteDist = 0;
                    break;
                case 'B': case 'b':
                    noteDist = 2;
                    break;
            }
            if(parsed.sign == 'b') noteDist -= 1;
            else if(parsed.sign == '#') noteDist += 1;

            return octDist + noteDist + 69;
        }

        static std::string midiToString(int midi, char signPref='b', bool withOctave=true){
            if(midi > 127 || midi <0){
                throw std::out_of_range("Note(midi) : midi index ("+std::to_string(midi)+") is out of range");
            } 
            int noteIdx = midi%12;
            int octave = ((midi-noteIdx)/12)-1;
            char letter;
            char sign = 'n';
            
            switch(noteIdx){
                case 0:
                    letter = 'C';
                    break;
                case 1:
                    if(signPref=='#'){
                        letter='C';
                        sign='#';
                    }
                    else{
                        letter='D';
                        sign='b';
                    }
                    break;
                case 2:
                    letter = 'D';
                    break;
                case 3:
                    if(signPref=='#'){
                        letter='D';
                        sign='#';
                    }
                    else{
                        letter='E';
                        sign='b';
                    }
                    break;
                case 4:
                    letter = 'E';
                    break;
                case 5:
                    letter = 'F';
                    break;
                case 6:
                    if(signPref=='#'){
                        letter='F';
                        sign='#';
                    }
                    else{
                        letter='G';
                        sign='b';
                    }
                    break;
                case 7:
                    letter = 'G';
                    break;
                case 8:
                    if(signPref=='#'){
                        letter='G';
                        sign='#';
                    }
                    else{
                        letter='A';
                        sign='b';
                    }
                    break;
                case 9:
                    letter = 'A';
                    break;
                case 10:
                    if(signPref=='#'){
                        letter='A';
                        sign='#';
                    }
                    else{
                        letter='B';
                        sign='b';
                    }
                    break;
                case 11:
                    letter = 'B';
                    break;
            }

            std::string ret;
            ret += letter;
            if(sign == 'b' || sign=='#') ret += sign;
            if(withOctave) ret += std::to_string(octave);

            return ret;
        }

        

};

/*--------------------------------------------------

example: translation

note = Note("Db5") or Note(63) or Note('G', '#', 2)

note.midi()      > [0, 127]
note.octave()    > [-1, 9]
note.name()      > e.g. "Eb4"
note.key()       > e.g. "Eb"
note.frequency() >  622.254

note.distanceTo(note2)       > (int) num semitones
note.interval(type)          > (Note) 
note.chord(type, inversion)  > returns vector<Note> 
note.scale(type)             > returns vector<Note> 

scales: 
    Major, Minor, Pentatonic

chords:
    Maj, min, 
    Aug, Dim, 
    Maj7, min7, 
    Dom7, Dom9,
    sus2, sus4,
    Maj11

intervals: 
    perfect: P1, P5, P8
    minor: m2, m3, m6, m7 
    major: M2, M3, M6, M7 
    diminished: d2, d3, d4, d5, d6, d7, d8 
    augmented: A1, A2, A3, A4, A5, A6, A7

--------------------------------------------------

example use: modulate key and octave

playSong(key, sign, octave){
    root = Note(key, sign, octave)
    third = root.interval("M3")
}

--------------------------------------------------

example use: 2-5-1 progression

root = Note("C")
two = root.interval("M2")  < defined by interval so root is flexible
five = root.interval("P5")

rootChord[] = root.chord(Maj7)
twoChord[] = two.chord(min7)
fiveChord[] = five.chord(Dom7)

rootEmbellish[] = root.scale(Pent)
twoEmbellish[] = two.scale(Pent)

playSequence(time){
    time = playChord(twoChord, time)
    time = playChord(fiveChord, time)
    time = playChord(rootChord, time)
    time = playChord(rootChord, time)
}

playChord(chord, time){
    for note in chord
        playSound(time, note.frequency(), duration, ...)
    return time+length
}

--------------------------------------------------*/