#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <map>
#include <vector>


using namespace std;

const int boarder = 1; //create a border of null CUBES so we know when we reach an edge
const int COLS = 4 + (boarder*2);
const int BOARD_SIZE = COLS * COLS;
const int NUM_NEIGHBORS = 8;
const int MAX_INPUT = 50; //longest word in dictionary

// neighbors layout
// 012
// 3*4
// 567
const int NEIGHBORS[NUM_NEIGHBORS] = {-1-COLS,-COLS,1-COLS,-1,1,COLS-1,COLS,COLS+1};


char board[BOARD_SIZE];
map <string,bool> dict;
map <string,bool> prefixes;
map <string,bool> words; //found words

//based on http://boardgamegeek.com/thread/300883/letter-distribution
const int NUM_GROUPS = 11;
const string LETTER_GROUPS[NUM_GROUPS] = {"e","t","ar","ino","s","d","chl","fmpu","gy","w","bjkqvxz"};
const int LETTER_GROUP_FREQ[NUM_GROUPS] = {19,13,12,11,9,6,5,4,3,2,1};


void init(){
    string group;
    char letter;
    int neighborIndex;

    srand( time(NULL) );

    //setup random board
    for(int i = 0; i < BOARD_SIZE; i++){
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
    for(int i = 0; i < BOARD_SIZE; i++){
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
    in_file.open("bogwords.txt"); //Official Scrabble Players Dictionary
    string word;
    string prefix;
    int i, j = 0;
    int len;
    //extra paragraph returns in dictionary will break this
    while( getline(in_file,word) )
    {
        len = word.length();
        dict[word] = true;


        prefix = "";
        for(i = 0; i < len; i++){
            prefix += word[i];
            prefixes[prefix] = true;
        }
        // if( dict["blah"] == 0 ) cout << "not found" << endl;
        // if( j > 1 ) break;

        // j++;
    }
    cout << "words: " << dict.size() << endl;
    cout << "prefixes: " << prefixes.size() << endl;
}

// i = board index
// searched = used cubes on the board
void find(int i, string str, vector<bool> searched, int depth){
    if( depth > 3 ) return;
    if( searched[i] || (board[i] == '*')   ) return; //|| (prefixes[str] == 0)
    
    str += board[i];
    if( dict[str] ) words[str] = true;
    searched[i] = true;
    depth++;

    //recursion
    for(int j = 0; j < NUM_NEIGHBORS; j++){
        find(NEIGHBORS[j] + i, str, searched, depth);
    }
    
}

int main(){
    string str;
    vector <bool> searched;

    for(int i = 0; i < BOARD_SIZE; i++) searched.push_back(false);

    init();
    cout << endl;
    printboard();
    cout << endl;
    // printNeighbors( 6 );

   buildDict();
   for(int i = 0; i < BOARD_SIZE; i++){
       find(i, str, searched, 0);
   }
   cout << "Words found: " << words.size() << endl;
   map<string, bool>::iterator p;
   // for(p = words.begin(); p != words.end(); p++) {
   //     cout << p->first << endl;
   //   }
}
