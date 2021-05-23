#pragma once

#include <string>
#include <vector>
#include <stdio.h>
#include <ostream>
#include <assert.h>  
#include <regex>

#include "notehelpers.h"

namespace theory {
/*------------------------------------------------------------------------------

Note - an music theory abstraction for high-level composition

    Constructors --------------------------------------------------
        Note(string name): e.g. Note("Db5")

        Note(int midi): e.g. Note(63)

        Note(char key, char sign='n', int octave=4, signPref='b')
            signPref = whether scales/chords names should use sharps or flats

    Descriptors --------------------------------------------------
        note.midi()      
            > (int) [0, 127]

        note.octave()    
            > (int) [-1, 9]

        note.name()      
            > (string) "Eb4"

        note.key()       
            > (string) "Eb"

        note.frequency() 
            > (float) 622.254
        
        note.distanceTo(note2)       
            > returns num semitones (int)

    Modifiers --------------------------------------------------
        note.set(string name)
        note.set(int midi)
        note.set(char key, char sign='n', int octave=4)

        note.setOctave(int octave)
            > returns true if successful

        note.setKey(string name)
        note.setKey(char key, char sign)
            > returns true if successful

        

    Extrapolators --------------------------------------------------
        
        note.interval(interval_type, int direction) direction: 1 or -1
        note.interval(int distance)
            > returns Note at interval

        note.octaveDown()
        note.octaveUp()
            > returns Note one octave higher/lower

        note.scale_degree(scale_type, scale_degree)
        note.scale_degree(scale_type, int degree)             
            > returns Note 

        chord(Note root, string chord_name, int inversion)
        chord(string chord_name, int inversion)
            > returns chord (notelist)
        
        scale(Note tonic, scale_type)
        scale(string tonic_name, scale_type)             
            > returns scale (notelist)
        
        scale_degree(notelist scale, scale_degree)
        scale_degree(notelist scale, int degree)
            > returns Note

        scale_chord(notelist scale, scale_degree)
        scale_chord(notelist scale, int degree, int length)
            > returns chord (notelist)

             
------------------------------------------------------------------------------*/
class Note {
    public:
        char signPref;
        int index;

        typedef std::vector<Note> notelist;

        //  constructor
        Note(char note='A', char sign='n', int octave=4, char signPref='b'){ 
            helper::parsed_str parsed = {note, sign, octave};
            init(helper::parsedToMidi(parsed), signPref);
        }
        Note(std::string input, char signPref='n'){ this->set(input, signPref); }
        Note(int midi, char signPref='b'){ init(midi, signPref); }

        // Main initializer
        void init(int midi, char signPref){
            if(midi > 127 || midi <0)
            {
                throw std::out_of_range("Note(midi) : midi index ("+std::to_string(midi)+") is out of range");
            }
            
            this->index = midi;
            this->signPref = signPref;
        }
// ------------------------------------------------------------------
//      Descriptors
// ------------------------------------------------------------------     

        // returns full note name (e.g. "Db6")
        std::string name(){
            return helper::midiToString(this->index, this->signPref);
        }

        // returns key without octave (e.g. "Db")
        std::string key(){
            return helper::midiToString(this->index, this->signPref, false);
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

        // returns octave [-1, 9]
        int octave(){
            return (this->midi()/12)-1;
        }

// ------------------------------------------------------------------
//      Modifiers
// ------------------------------------------------------------------  
        
        // set note to new midi index [0-127]
        void set(int midi, char signPref='b'){ init(midi, signPref); }

        // set note to new string name
        void set(std::string input, char signPref='n')
        { 
            helper::parsed_str parsed = helper::parseString(input);
            int idx = helper::parsedToMidi(parsed);
            if(signPref == 'n'){
                if(parsed.sign == '#') { init(idx, '#'); }
                else{ init(idx, 'b'); }
            }
            else{ init(idx, signPref); } 
        }
        
        // set note to new key, sign, and octave
        void set(char key='A', char sign='n', int octave=4, char signPref='b'){ 
            helper::parsed_str parsed = {key, sign, octave};
            init(helper::parsedToMidi(parsed), signPref);
        }

        // set octave of note without changing key
        bool setOctave(int octave=4){
            if(octave < -1 && octave > 9) return false;
            int noteIdx = this->index%12;
            int offset = (octave+1)*12;
            this->index = noteIdx + offset;

            return true;
        }

        // set key of note without changing octave
        bool setKey(std::string key){
            int octave = this->octave();
            this->set(key);
            return this->setOctave(octave);
        }

        // set key of note without changing octave
        bool setKey(char key, char sign){
            int octave = this->octave();
            this->set(key, sign, octave);
            return true;
        }

        // moves note up an octave
        bool octaveUp(){
            int newIdx = this->index + 12;
            if(newIdx > 0 && newIdx < 128){
                this->set(newIdx);
                return true;
            }

            return false;
        }

        // moves note down an octave
        bool octaveDown(){
            int newIdx = this->index - 12;
            if(newIdx > 0 && newIdx < 128){
                this->set(newIdx);
                return true;
            }
            return false;
        }

// ------------------------------------------------------------------
//      Extrapolators
// ------------------------------------------------------------------

        notelist chord(std::string chord_name, int octave=3);
        
        // returns distance (in semitones) to another note
        int distanceTo(Note* b){
            return b->midi() - this->index;
        }

        // returns note at specified interval above/below current note
        // direction = 1 for up
        // direction = -1 for down
        Note interval(interval_type::name type, int direction=1){
            int interval = interval_type::table[type] * direction;
            Note n = Note(index + interval);
            return n;
        }

        // returns note at specified interval above/below current note
        Note interval(int semitones){
            Note n = Note(index + semitones);
            return n;
        }
        
        // returns vector of Notes corresponding to root note, chord type, and inversion
        // chord name structure: 
        //      [root][quality][extension][alteration][add][bass]
        //
        // quality: 
        //      maj, min, aug, dim, dom, sus2, sus4
        // extension: 
        //      7, 9, 11, or 13
        // alteration: 
        //      b5, #5, b9, #9
        // add: 
        //      add2, add4, add6, add8, add9
        // bass: 
        //      /[key] where key is a note in chord (inverts so that key is the lowest note in chord)
        //      e.g. note.getChord("maj7"), note.getChord("m7b5"), note.getChord("Madd5")   
        

        notelist getScale(scale_type::name type){
            notelist ret;

            // loop through chord intervals to build list of notes
            for(int i=0; i<scale_type::maxLength; i++){
                int interval = scale_type::table[type][i];
                if(interval >= 0 ){ // variable length scales, fixed length array, filled space with -1s
                    int idx = this->index + interval;
                    Note cnote = Note(idx, signPref);
                    ret.push_back(cnote);
                }
            }

            return ret;
        }

        // Returns note at degree on scale 
        Note scale_degree(scale_type::name type, scale_type::degree degree){
            int interval = scale_type::table[type][degree];
            
            if(interval > 0){
                return Note(this->index+interval);
            }
            else return Note(this->index);
        }

        // Returns note at degree on scale 
        Note scale_degree(scale_type::name type, int degree){
            int interval = scale_type::table[type][degree-1];
            if(interval > 0){
                return Note(this->index+interval);
            }
            else return Note(this->index);
        }

        


};

// ------------------------------------------------------------------
//      Static Methods
// ------------------------------------------------------------------     

    static Note scale_degree(Note::notelist scale, scale_type::degree degree){
            return scale[degree];
        }

    static Note::notelist dropChord(Note::notelist chord, int numOctaves=1){
        for(Note n:chord){
            n.octaveDown();
        }
        return chord;
    }
    
    static Note::notelist invertChord(Note::notelist chord, int inversion=0){
        for(int i=0; i<inversion; i++){
            int newIdx = chord[0].index+12;
            if(newIdx > 127){
                chord = dropChord(chord);
                newIdx = chord[0].index+12;
            }
            std::rotate(chord.begin(), chord.begin() + 1, chord.end());
        }

        return chord;
    }

    static Note::notelist chord(Note* root, std::string name, int octave=3){
        Note::notelist ret;

        helper::parsed_chord parsed = helper::parseChord(name);
        Note rt = Note(root->midi());
        rt.setOctave(octave);
        int rootIdx = rt.midi();

        for(int interval:parsed.intervals){
            ret.push_back(Note(rootIdx + interval));
        }

        if(parsed.bass != parsed.key){
            int bassNote = helper::noteIndex(parsed.bass);
            int bassIdx = -1;
            for(int i=0; i<ret.size(); i++){
                int noteLoc = helper::noteIndex(ret[i].key());
                if(noteLoc == bassNote){
                    bassIdx = i;
                }
            }
            if(bassIdx == -1){
                throw std::out_of_range("Chord(string) : Figured bass ("+parsed.bass+") is not in chord");
            }
            else{
                std::cout << "Inversion=" << bassIdx << std::endl;
                ret = invertChord(ret, bassIdx);
            }
        }


        std::cout << "\nNotes: ";
        for(Note n:ret){
            std::cout << n.key() << ", ";
        }

        return ret;
    }

    static Note::notelist chord(std::string name, int octave=3){
        Note::notelist ret;

        helper::parsed_chord parsed = helper::parseChord(name);
        Note root = Note(parsed.key+parsed.sign);
        root.setOctave(octave);
        int rootIdx = root.midi();

        for(int interval:parsed.intervals){
            ret.push_back(Note(rootIdx + interval));
        }

        if(parsed.bass != parsed.key){
            int bassNote = helper::noteIndex(parsed.bass);
            int bassIdx = -1;
            for(int i=0; i<ret.size(); i++){
                int noteLoc = helper::noteIndex(ret[i].key());
                if(noteLoc == bassNote){
                    bassIdx = i;
                }
            }
            if(bassIdx == -1){
                throw std::out_of_range("Chord(string) : Figured bass ("+parsed.bass+") is not in chord");
            }
            else{
                std::cout << "Inversion=" << bassIdx << std::endl;
                ret = invertChord(ret, bassIdx);
            }
        }


        std::cout << "\nNotes: ";
        for(Note n:ret){
            std::cout << n.key() << ", ";
        }

        return ret;
    }

    Note::notelist Note::chord(std::string chord_name, int octave){
        return theory::chord(this, octave);
    }
    
    // Returns chord (vector of notes) based on root name and chord type
    //  e.g. getChord("Db5", chord_type::Maj7)
    // static chord getChord(std::string note, chord_type::name type, int inversion=0){
    //     return Note(note).getChord(type, inversion);
    // }

    // // Returns chord (vector of notes) based on root name and chord type
    // //  e.g. 
    // //    Note root = Note("Db5") 
    // //    getChord(root, chord_type::Maj7)
    // static chord getChord(Note note, chord_type::name type, int inversion=0){
    //     return note.getChord(type, inversion);
    // }



    static Note::notelist getTriad(Note::notelist scale, scale_type::degree degree, int inversion){
        Note::notelist ret;
        if(degree > scale.size()){
            throw std::out_of_range("Scale Degree ("+scale_type::degree_labels[degree]+") is out of range");
        }

        Note root = Note(scale[degree].midi());
        ret.push_back(root);
        Note third = Note(scale[(degree+2)%scale.size()].midi());
        ret.push_back(third);
        Note fifth = Note(scale[(degree+4)%scale.size()].midi());
        ret.push_back(fifth);
        return ret;
    }

    

    // Returns scale (vector of notes) based on tonic note and scale type
    static Note::notelist scale(Note tonic, scale_type::name type){
        Note::notelist ret;

        // loop through chord intervals to build list of notes
        for(int i=0; i<scale_type::maxLength; i++){
            int interval = scale_type::table[type][i];
            if(interval >= 0 ){ // variable length scales, fixed length array, filled space with -1s
                int idx = tonic.index + interval;
                Note cnote = Note(idx);
                ret.push_back(cnote);
            }
        }

        return ret;
    }

    // Returns scale (vector of notes) based on tonic name and scale type
    //  e.g. getScale("A2", scale_type::HarmonicMajor)
    static Note::notelist scale(std::string note, scale_type::name type){
        return scale(Note(note), type);
    }

typedef interval_type::name intervalT;
typedef scale_type::name scaleT;
typedef scale_type::degree degree;
typedef chord_type::quality qualityT;
typedef Note::notelist notelist;

}

