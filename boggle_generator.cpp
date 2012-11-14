#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>


using namespace std;

//based on http://boardgamegeek.com/thread/300883/letter-distribution
const int NUM_GROUPS = 11;
const string LETTER_GROUPS[NUM_GROUPS] = {"e","t","ar","ino","s","d","chl","fmpu","gy","w","bjkqvxz"};
const int LETTER_GROUP_FREQ[NUM_GROUPS] = {19,13,12,11,9,6,5,4,3,2,1};

void generate(int board_size, string fname){
    string tileBag;
    char letter;
    ofstream file;
    file.open( fname.c_str() );


    for(int i = 0; i < NUM_GROUPS; i++){
        int freq = LETTER_GROUP_FREQ[i];
        for(int j = 0; j < freq; j++){
            tileBag += LETTER_GROUPS[i];
        }
    }

    srand( time(NULL) );

    //setup random board
    for(int i = 0; i < board_size; i++){
        letter = tileBag[rand() % tileBag.length()];
        file << letter;
    }
    file.close();
}   


int main(int argc, char* argv[]){
    int size = 4;
    string fname = "boggle.txt";
    
    if( argc > 1 ) size = atoi(argv[1]);
    if( argc > 2 ) fname = argv[2];
    
    int board_size = size * size;

    generate(board_size, fname);

    cout << "Created boggle board " << fname << endl;
}
