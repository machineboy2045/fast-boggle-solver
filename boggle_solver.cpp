#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <map>

using namespace std;

const int boarder = 1; //create a border of null CUBES so we know when we reach an edge
const int COLS = 4 + (boarder*2);
const int board_SIZE = COLS * COLS;
const int NUM_NEIGHBORS = 8;
const int MAX_INPUT = 50; //longest word in dictionary

// neighbors layout
// 012
// 3*4
// 567
const int NEIGHBORS[NUM_NEIGHBORS] = {-1-COLS,-COLS,1-COLS,-1,1,COLS-1,COLS,COLS+1};


char board[board_SIZE];
map <string,int> dict;

//based on http://boardgamegeek.com/thread/300883/letter-distribution
const int NUM_GROUPS = 11;
const string LETTER_GROUPS[NUM_GROUPS] = {"E","T","AR","INO","S","D","CHL","FMPU","GY","W","BJKQVXZ"};
const int LETTER_GROUP_FREQ[NUM_GROUPS] = {19,13,12,11,9,6,5,4,3,2,1};


void init(){
    string group;
    char letter;
    int neighborIndex;

    srand( time(NULL) );

    //setup random board
    for(int i = 0; i < board_SIZE; i++){
        if( (i < COLS) ||           //top
        ((i+1) % COLS == 0) ||      //right
        (i > COLS * (COLS -1)) ||   //bot
        (i % COLS == 0) ){             //left
            board[i] = '*';
        }else{
            group = LETTER_GROUPS[rand() % NUM_GROUPS];
            letter = group[rand() % group.length()];
            board[i] = letter;
        }
    }

}   

void printboard(){
    for(int i = 0; i < board_SIZE; i++){
        char c = board[i];
        if( c == 'Q' ){ 
            cout << c << 'u';
        }else{
            cout << c << ' ';
        }
        if( (i+1) % COLS == 0 ) cout << endl;     
    }
}

void printNeighbors( int x ){
    char neighbor;
    int nIndex;

    if( board[x] == '*' ){
        cout << "Can't print neighbors of border piece";
        return;
    }

    for(int i = 0; i < NUM_NEIGHBORS; i++){
        nIndex = NEIGHBORS[i] + x;
        neighbor = board[nIndex];
        cout << neighbor << ' ';
        if( i == 3 ) cout << board[x] << ' ';
        if( (i == 2) || (i == 4) ) cout << endl;     
    }
}

// load dictionary into hash table
void buildDict()
{
    ifstream in_file;
    in_file.open("ospd.txt");
    string temp;
    int i = 0;
    //extra paragraph returns in dictionary will break this
    while( getline(in_file,temp) )
    {
        dict[temp] = 1;
        // if( dict["blah"] == 0 ) cout << "not found" << endl;
        // if( i > 1 ) break;

        // i++;
    }
    cout << "words: " << dict.size() << endl;
}


int main(){
    
    // init();
    // cout << endl;
    // printboard();
    // cout << endl;
    // printNeighbors( 6 );

   buildDict();
}
