#include <string>
#include <vector>
#include <stdio.h>
#include <ostream>
#include <assert.h>  
#include <regex>

const static int numNotes = 6;
const static float note_length[numNotes] = {4, 2, 1, 0.5, 0.25, 0.125};

class Tempo{
    public:
        struct timeSignature{
            int top;
            int bottom;
        };

        enum note_type{
            whole=0, w=0,
            half=1, h=1,
            quarter=2, q=2,
            eighth=3, e=3,
            sixteenth=4, s=4,
            thirtysecond=5, t=5,
        };

        timeSignature timeSig;
        note_type beatType;

        float bpm;
        float beatLength;
        float barLength;

        Tempo(float bpm=80, int sigTop=4, int sigBottom=4){
            assert(sigBottom%4 ==0);
            assert(bpm > 0);
            
            this->bpm = bpm;
            this->timeSig.top = sigTop;
            this->timeSig.bottom = sigBottom;

            this->beatLength = 1 / (bpm / 60);
            this->barLength = beatLength*(4.0/sigBottom)*sigTop;
        }

        float duration(note_type type, bool dot=false){
            float duration = note_length[type] * beatLength;
            if(dot) duration *= 1.5;
            return duration;
        }
};