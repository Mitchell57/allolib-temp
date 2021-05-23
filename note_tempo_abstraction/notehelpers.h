#include <string>
#include <vector>
#include <stdio.h>
#include <ostream>
#include <assert.h>  
#include <regex>

#include "noteconsts.h"

namespace helper{
    
    struct parsed_str{
        char note, sign;
        int octave;
    };

    std::regex note_regex("[a-gA-G]");
    std::regex sign_regex("[#nb]");
    std::regex octave_regex("\\-1|[0-9]");

    // takes string input
    // returns {note, sign, octave} if valid
    static parsed_str parseString(std::string str){
        std::string toParse = str;
        parsed_str ret;

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
    static int parsedToMidi(parsed_str parsed){
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

    // Takes key ( letter(+sign) )
    // Returns note index as # of semitones from A
    static int noteIndex(std::string key){
        char letter = key[0];
        int noteDist;
        switch (letter)
        {
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
        
        default:
            throw std::out_of_range("Note(string) : input letter ("+key+") is not a note");
            break;
        }
        if(key.length() == 1) return noteDist;
        else if(key.length() == 2){
            if(key[1] == 'b') noteDist -= 1;
            else if(key[1] == '#') noteDist += 1;

            return noteDist;
        }
        throw std::out_of_range("Note(string) : input letter ("+key+") is not a note");
    }

    // takes string input
    // returns Midi index if valid
    static int stringToMidi(std::string str){
        parsed_str parsed = parseString(str);
        return parsedToMidi(parsed);
    }

    // Takes midi input
    // Returns string name if valid
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

    struct parsed_chord {
        std::string key, sign, bass;
        std::vector<int> intervals; 
        chord_type::quality quality;
    };

    
    parsed_chord buildChord(parsed_chord chord, int length){
        for(int i=0; i<length; i++){
            int interval = chord_type::table[chord.quality][i];
            if(interval >= 0) chord.intervals.push_back(interval);
        }
        return chord;
    }

    static parsed_chord parseChord(std::string name){
        std::string str = name;
        
        // Key matching
        std::regex key("^[a-gA-G]");
        std::regex sign("^[bn#]");
        
        // Quality matching
        std::regex major("^((maj)|(M))");
        std::regex minor("^((min)|(m)|(-))");
        std::regex dim("^((dim)|o)");
        std::regex aug("^((aug)|(\\+)|(\\+5))");
        
        // Extensions, Alterations, Figured bass
        std::regex extended("^(7|9|(11)|(13))");
        std::regex alter("^([#b][59])");
        std::regex add("^(add[24689])");
        std::regex drop("^(add[24689])");
        std::regex bass("^(\\/[a-gA-g][b#]?)");

        std::smatch m;
        parsed_chord chord;
        int length = 0;

        // First, pop off key and sign
        if(std::regex_search(str, m, key)){
            chord.key = m.str();
            str = m.suffix().str();
        }
        if(std::regex_search(str, m, sign)){
            chord.key += m.str();
            str = m.suffix().str();
        }

        std::cout << "Chord = " << chord.key << chord.sign << " ";

        // Then determine quality
        if(std::regex_search(str, m, major) || str.length()==0){
            chord.quality = chord_type::M;
            std::cout << "Major ";
            str = m.suffix().str();
        }
        else if(std::regex_search(str, m, minor)){
            chord.quality = chord_type::m;
            std::cout << "Minor ";
            str = m.suffix().str();
        }
        else if(std::regex_search(str, m, aug)){
            chord.quality = chord_type::aug;
            std::cout << "Aug ";
            str = m.suffix().str();
        }
        else if(std::regex_search(str, m, dim)){
            chord.quality = chord_type::dim;
            std::cout << "Dim ";
            str = m.suffix().str();
        }
        else if(str.substr(0,4) == "sus2"){
            chord.quality = chord_type::sus2;
        }
        else if(str.substr(0,4) == "sus4"){
            chord.quality = chord_type::sus4;
        }
        else{
            chord.quality = chord_type::dom;
        }

        
        // then check for extensions
        if(std::regex_search(str, m, extended)){
            int extend = std::stoi(m[0]);
            std::cout << "ext=" << m.str() << " ";
            if(extend == 7) length = 4;
            if(extend == 9) length = 5;
            if(extend == 11) length = 6;
            if(extend == 13) length = 7;
            str = m.suffix().str();
        }
        else{
            length = 3;
        }

        // Build the base chord
        chord = buildChord(chord, length);
        
        // Now look for alterations
        if(std::regex_search(str, m, alter)){
            if (m.str()[0] == 'b'){
                if(m.str()[1] == '5') {
                    chord.intervals[chord_type::fifth] -= 1;
                    std::cout << "flat 5 ";
                }
                if(m.str()[1] == '9') {
                    if(chord.intervals.size() == 3 || chord.intervals.size() == 4){
                        chord.intervals.push_back(chord_type::table[chord.quality][chord_type::ninth]);
                    }
                    chord.intervals[chord.intervals.size()-1] -= 1;
                    std::cout << "flat 9 ";
                }
            }
            else if (m.str()[0] == '#'){
                if(m.str()[1] == '5') {
                    chord.intervals[chord_type::fifth] += 1;
                    std::cout << "sharp 5 ";
                    }
                if(m.str()[1] == '9') {
                    if(chord.intervals.size() == 3 || chord.intervals.size() == 4){
                        chord.intervals.push_back(chord_type::table[chord.quality][chord_type::ninth]);
                    }
                    chord.intervals[chord.intervals.size()-1] += 1;
                    std::cout << "sharp 9 ";
                }
            }

            str = m.suffix().str();
        }
        
        if(std::regex_search(str, m, add)){
            int add = std::stoi(m.str().substr(3,4));
            std::cout << "add " << add;
            int interval;
            switch(add){
                case 2:
                    interval = chord_type::table[chord.quality][chord_type::ninth] - 12;
                    break;
                case 4:
                    interval = chord_type::table[chord.quality][chord_type::eleventh] - 12;
                    break;
                case 6:
                    interval = chord_type::table[chord.quality][chord_type::thirteenth] - 12;
                    break;
                case 8:
                    interval = 12;
                    break;
                case 9:
                    interval = chord_type::table[chord.quality][chord_type::ninth];
                    break;
            }
            chord.intervals.push_back(interval);
            std::sort (chord.intervals.begin(), chord.intervals.end());
            str = m.suffix().str();
        }

         if(std::regex_search(str, m, bass)){
             chord.bass = m.str().substr(1);
             str = m.suffix().str();
         }
         else{
             chord.bass = chord.key;
         }

        if(str.length() != 0){
            throw std::out_of_range("Chord(string) : Chord ("+name+") is invalid, "+str+" was left over");
        }

        return chord;
    }

    
}

