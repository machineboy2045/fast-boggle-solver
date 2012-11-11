#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string>

using namespace std;

const int BOARDER = 1; //create a border of null CUBES so we know when we reach an edge
const int COLS = 4 + (BOARDER*2);
const int BOARD_SIZE = COLS * COLS;
const int NUM_NEIGHBORS = 8;
// neighbors layout
// 012
// 3*4
// 567
const int NEIGHBORS[NUM_NEIGHBORS] = {-1-COLS,-COLS,1-COLS,-1,1,COLS-1,COLS,COLS+1};


struct CUBE{
    char letter;
    int index;
    CUBE* neighbors[NUM_NEIGHBORS];
};


CUBE BOARD[BOARD_SIZE];

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
    for(int i = 0; i < BOARD_SIZE; i++){
        if( (i < COLS) ||           //top
        ((i+1) % COLS == 0) ||      //right
        (i > COLS * (COLS -1)) ||   //bot
        (i % COLS == 0) ){             //left
            BOARD[i] = *;
        }else{
            group = LETTER_GROUPS[rand() % NUM_GROUPS];
            letter = group[rand() % group.length()];
            BOARD[i] = letter;
        }
    }

    for(int i = 0; i < BOARD_SIZE; i++){
        if( BOARD[i] != NULL ){
            //setup neighbors
            for(int j = 0; j < NUM_NEIGHBORS; j++){
                neighborIndex = i + NEIGHBORS[j];
                if( neighborIndex >= 0 ){ 
                    BOARD[i].neighbors[j] = &BOARD[neighborIndex];
                }else{
                    BOARD[i].neighbors[j] = NULL;
                }
            }
        }
    }

}   

void printBoard(){
    for(int i = 0; i < BOARD_SIZE; i++){
        // cout << BOARD[i].letter << ' ';
        if( i < 10 ) cout << '0';
        cout << i << ' ';
        if( (i+1) % COLS == 0 ) cout << endl;     
    }
}

void printNeighbors( CUBE cube ){
    CUBE *neighbor;
    for(int i = 0; i < NUM_NEIGHBORS; i++){
        neighbor = cube.neighbors[i];
        if( neighbor == NULL ){ 
            cout << "**";
        }else{
            // cout << neighbor->letter;
            if( neighbor->index < 10 ) cout << '0';
            cout << neighbor->index;
        }
        cout << ' ';
        // if( i == 3 ) cout << "!! ";
        if( i == 3 ) cout << cube.index << " ";
        if( (i == 2) || (i == 4) ) cout << endl;     
    }
}


int main(){
    
    init();
    cout << endl;
    printBoard();
    cout << endl;
    printNeighbors( BOARD[3] );
}
