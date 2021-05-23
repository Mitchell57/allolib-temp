#include <string>
/*
    Stores constants / names / labels for Note
*/



/*
    scale_type: holds scale intervals, names, and degree names

    Degrees:
        Tonic, Supertonic, Mediant, Subdominant, 
        Dominant, Submediant, Subtonic, Leading, Tonic2

    Scales:
        Aeolian / Minor, Ionian / Major, 
        Dorian, Phrygian, Locrian,
        Lydian, Mixolydian

        Pentatonic Major, Pentatonic Minor, 
        Harmonic Major, Harmonic Minor,
        Melodic Minor (asc), Melodic Minor (desc), 
        Bebop Dominant, Bebop Major,
        Major Minor, Half Diminished, 

        Algerian, Augmented, Blues, 
        Chromatic, Prometheus, Tritone,
        Locrian Major, Altered / Super Locrian, 
        Phrygian Dominant, Lydian Augmented / Acoustic,  
        Enigmatic, Double Harmonic / Flamenco, 
        
        Neapolitan Major, Neapolitan Minor, 
        Hungarian Minor, Hungarian Major
    
        Hirajoshi, In, Insen, Iwato, Persian
*/
namespace scale_type
{
    const static int numScales = 37;
    const static int maxLength = 13;

    enum name {
        Chromatic=0, 
        Aeolian=1, Minor=1, Locrian=2, Ionian=3, Major=3, 
        Dorian=4, Phrygian=5, Lydian=6, Mixolydian=7,
        MelodicMinorDesc=7, MajorMinor=8, HalfDim = 9, LocrianMajor=10, Altered=11, SuperLocrian=11,
        PhrygianDom=12, LydianAug=13, Acoustic=13, 
        HarmonicMajor=14, HarmonicMinor=15, Enigmatic=16, DoubleHarmonic=17, Flamenco=17, 
        MelodicMinorAsc=18, NeapolitanMajor=19, NeapolitanMinor=20, HungarianMinor=21, HungarianMajor=22, 
        PentMajor=23, PentMinor=24,Algerian=25, Augmented=26, BebopDom=27, BebopMaj=28, Blues=29, Prometheus=30, Tritone=31,
        Hirajoshi=32, In=33, Insen=34, Iwato=35, Persian=36
    };

    enum degree {
        Tonic=0, Supertonic=1, Mediant=2, Subdominant=3, Dominant=4, Submediant=5, Subtonic=6, Leading=6, Tonic2=7,
        I=0, II=1, III=2, IV=3, V=4, VI=5, VIIb=6, VII=6, VIII=7,
    };

    const static int table[numScales][maxLength] = {
        {0,1,2,3,4,5,6,7,8,9,10,11,12},       // 0 Chromatic

        // Diatonic modes
        {0,2,3,5,7,8,10,12,-1,-1,-1,-1,-1},   // 1 Aeolian / Minor 
        {0,1,3,5,6,8,10,12,-1,-1,-1,-1,-1},   // 2 Locrian 
        {0,2,4,5,7,9,11,12,-1,-1,-1,-1,-1},   // 3 Ionian / Major   
        {0,2,3,5,7,9,10,12,-1,-1,-1,-1,-1},   // 4 Dorian
        {0,1,3,5,7,8,10,12,-1,-1,-1,-1,-1},   // 5 Phrygian 
        {0,2,4,6,7,9,11,12,-1,-1,-1,-1,-1},   // 6 Lydian 
        {0,2,4,5,7,9,10,12,-1,-1,-1,-1,-1},   // 7 Mixolydian / Melodic minor (descending)

        // Secondary Diatonic
        {0,2,4,5,7,8,10,12,-1,-1,-1,-1,-1},   // 8  MajorMinor
        {0,2,3,5,6,8,10,12,-1,-1,-1,-1,-1},   // 9  Half diminished
        {0,2,4,5,6,8,10,12,-1,-1,-1,-1,-1},   // 10 Locrian major 
        {0,1,3,4,6,8,10,12,-1,-1,-1,-1,-1},   // 11 Altered / Super Locrian
        {0,1,4,5,7,8,10,12,-1,-1,-1,-1,-1},   // 12 Phrygian dominant
        {0,2,4,6,8,9,11,12,-1,-1,-1,-1,-1},   // 13 Lydian Augmented / Acoustic

        // Misc Heptanonic (7 notes)
        {0,2,4,5,7,8,11,12,-1,-1,-1,-1,-1},   // 14 Harmonic Major   
        {0,2,3,5,7,8,11,12,-1,-1,-1,-1,-1},   // 15 Harmonic Minor  
        {0,1,4,6,8,10,11,12,-1,-1,-1,-1,-1},  // 16 Enigmatic
        {0,1,4,5,7,8,11,12,-1,-1,-1,-1,-1},   // 17 Double Harmonic / Flamenco
        {0,2,3,5,7,9,11,12,-1,-1,-1,-1,-1},   // 18 Melodic minor (ascending)   
        {0,1,3,5,7,9,11,12,-1,-1,-1,-1,-1},   // 19 Neapolitan major 
        {0,1,3,5,7,8,11,12,-1,-1,-1,-1,-1},   // 20 Neapolitan minor 
        {0,2,3,6,7,8,11,12,-1,-1,-1,-1,-1},   // 21 Hungarian Minor  
        {0,3,4,6,7,9,10,12,-1,-1,-1,-1,-1},   // 22 Hungarian Major

        // Misc Western
        {0,2,4,7,9,12,-1,-1,-1,-1,-1,-1,-1},  // 23 Major Pentatonic 
        {0,3,5,7,10,12,-1,-1,-1,-1,-1,-1,-1}, // 24 Minor Pentatonic 
        {0,2,3,6,7,8,11,12,14,15,17,-1,-1},   // 25 Algerian
        {0,3,4,7,8,11,12,-1,-1,-1,-1,-1,-1},  // 26 Augmented          
        {0,2,4,5,7,9,10,11,12,-1,-1,-1,-1},   // 27 Bebop dominant
        {0,2,4,5,7,8,9,11,12,-1,-1,-1,-1},    // 28 Bebop major      
        {0,3,5,6,7,10,12,-1,-1,-1,-1,-1,-1},  // 29 Blues            
        {0,2,4,6,9,10,12,-1,-1,-1,-1,-1,-1},  // 30 Prometheus 
        {0,1,4,6,7,10,12,-1,-1,-1,-1,-1,-1},  // 31 Tritone   

        // Japanese
        {0,4,6,7,11,12,-1,-1,-1,-1,-1,-1,-1}, // 32 Hirajoshi        
        {0,1,5,7,8,12,-1,-1,-1,-1,-1,-1,-1},  // 33 In               
        {0,1,5,7,10,12,-1,-1,-1,-1,-1,-1,-1}, // 34 Insen            
        {0,1,5,6,10,12,-1,-1,-1,-1,-1,-1,-1}, // 35 Iwato            

        // Middle East
        {0,1,4,5,6,8,11,12,-1,-1,-1,-1,-1},   // 36 Persian 

        // TODO: Add more world, jazz scales 
    };

    const static int numLabels = numScales+6;  // 6 scales with two names
    const static std::string label[numLabels] = {
        "Chromatic", 
        "Aeolian", "Minor", "Locrian", "Ionian", "Major", 
        "Dorian", "Phrygian", "Lydian", "Mixolydian",
        "MelodicMinorDesc", "MajorMinor", "HalfDim", "LocrianMajor", "Altered", "SuperLocrian",
        "PhrygianDom", "LydianAug", "Acoustic", 
        "HarmonicMajor", "HarmonicMinor", "Enigmatic", "DoubleHarmonic", "Flamenco", 
        "MelodicMinorAsc", "NeapolitanMajor", "NeapolitanMinor", "HungarianMinor", "HungarianMajor", 
        "PentMajor", "PentMinor","Algerian", "Augmented", "BebopDom", "BebopMaj", "Blues", "Prometheus", "Tritone",
        "Hirajoshi", "In", "Insen", "Iwato", "Persian"
    };

    const static std::string degree_labels[9] = {
        "Tonic", "Supertonic", "Mediant", "Subdominant", "Dominant", "Submediant", "Subtonic", "Leading", "Tonic2"
    };
}


/*
    chord_type: holds chord intervals and names
 
    Maj, min, 
    Maj7, min7,
    dim, Aug, 
    Dom7, Dom9, 
    sus2, sus4, 
    Maj11
*/
namespace chord_type
{
    const static int num = 7;
    const static int maxLength = 7;

    // enum name {
    //     Maj, 
    //     min, 
    //     Aug,
    //     dim, 
    //     Maj7, 
    //     min7, 
    //     Dom7, 
    //     Dom9,
    //     sus2, 
    //     sus4, 
    //     Maj11
    // };

    enum quality {
        M, m, aug, dim, dom, sus2, sus4
    };

    enum degree {
        root, third, fifth, seventh, ninth, eleventh, thirteenth
    };

    const static int table[num][maxLength] = {
        {0, 4, 7, 11, 14, 17, 21}, // Major
        {0, 3, 7, 10, 14, 17, 20}, // Minor
        {0, 4, 8, 10, 15, 19, 23}, // Aug
        {0, 3, 6, 9, 14, 17, 20},  // Diminished
        {0, 4, 7, 10, 13, 17, 20}, // Dom
        {0, 2, 7, -1,-1,-1},       // Sus2
        {0, 5, 7, -1,-1,-1},       // Sus4
    };

    // const static std::string label[num] = {
    //     "Major", "min", "Dim", "Maj7", "min7", "Dom7", "sus2", "sus4", "Aug", "Dom9", "Maj11"
    // };
}


/*
    interval_type: holds intervals and names

        Perfect    - P1, P4, P5, P8, 
        Minor      - m2, m3, m6, m7, 
        Major      - M2, M3, M6, M7,
        Diminished - d2, d3, d4, d5, d6, d7, d8,
        Augmented  - A1, A2, A3, A4, A5, A6, A7
*/
namespace interval_type
{
    const static int numIntervals = 26;
    enum name {
        P1, P4, P5, P8, 
        m2, m3, m6, m7, 
        M2, M3, M6, M7,
        d2, d3, d4, d5, d6, d7, d8,
        A1, A2, A3, A4, A5, A6, A7
    };
    const static int table[numIntervals] = {
        0, 5, 7, 12,          // Perfect
        1, 3, 8, 10,          // Minor
        2, 4, 9, 11,          // Major
        0, 2, 4, 6, 7, 9, 11, // Diminished
        1, 3, 5, 6, 8, 10, 12 // Augmented
    };
    const static std::string label[numIntervals] = {
        "P1", "P4", "P5", "P8", 
        "m2", "m3", "m6", "m7", 
        "M2", "M3", "M6", "M7",
        "d2", "d3", "d4", "d5", "d6", "d7", "d8",
        "A1", "A2", "A3", "A4", "A5", "A6", "A7"
    };
    
}