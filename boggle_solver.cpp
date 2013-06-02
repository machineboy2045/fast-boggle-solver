/*
FAST C++ BOGGLE SOLVER
Copyright 2012 Will Jensen

https://github.com/themachineswillwin/fast-boggle-solver

machineboy2045@gmail.com

This file is part of Fast C++ Boggle Solver.

Fast C++ Boggle Solver is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Fast C++ Boggle Solver is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Fast C++ Boggle Solver.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <locale>

using namespace std;

const int NUM_BRANCHES = 8; //max number of neighboring cubes
const int ALPHA_SIZE = 26; //number of letters in our alphabet
const int WSIZE = 20; //longest word read in dictionary
const int BORDER = ALPHA_SIZE; //surrounds the Boggle puzzle. Should be 1 > than last int in alphabet

/*
Dict is a tree (trie?).

For any prefix, it can tell you which letters
follow it and how many suffixes (children) it has.

Lets say you have a two letter prefix "AN".
If you want to know what letters could follow this prefix,
you would look at dict['A']->children['N']->children.

*/

class Trie{
    public:
        Trie* children[ALPHA_SIZE];
        Trie* parent;
        int count; //number of suffixes that share this as a root
        char* word; //if this node completes a word, store it here
        bool found; //has this word been found?

        Trie();
};

Trie::Trie(){
    count = 0;
    word = NULL;
    found = false;
    int i;
    for(i = 0; i < ALPHA_SIZE; i++){
        children[i] = NULL;
    }
}


Trie* dict;

int duplicates = 0;
int wordsParsed = 0;
int checkedNodes = 0;
int progress = 0;
int onePercentage;
char longestWord[WSIZE+1];
clock_t begin; //used to time search duration
int * board;
int board_size;
int puzzle_size;
int cols;
int * SEARCHED;
int * children;
vector<char*> found;

char* readF( char fname[] ){
    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;

    pFile = fopen ( fname , "r" );
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    // allocate memory to contain the whole file:
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    // copy the file into the buffer:
    result = fread (buffer,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

    /* the whole file is now loaded in the memory buffer. */

    // terminate
    fclose (pFile);
    // free (buffer);
    return buffer;
}

void saveResults( char fname[] ){
    FILE * file;
    file = fopen (fname,"w");

    //pretty print the board
    char c;
    for(int i = 0; i < board_size; i++){
        c = (char) (board[i] + 'a');
        if( 'q' == c ){
            fputs("Qu",file);
        }else if( BORDER == board[i] ){
            fputs("* ",file);
        }else{
            fputc(c,file);
            fputs(" ",file);
        }
        if( (i+1) % cols == 0 ) fputs("\n",file);
    }
    fputs("\n",file);

    //print found words list
    sort(found.begin(), found.end());

    for(vector<char*>::iterator it = found.begin(); it != found.end(); ++it) {
            fputs (*it,file);
            fputs ("\n",file);
    }

    fclose(file);
}

void statusBar(int i){
    if( (cols-2) < 100 )
        return;
    i += 1; //compensate for array index
    if( i - progress == onePercentage ){
        cout << "|" << flush;
        progress += onePercentage;
    }
    if( i == puzzle_size ) cout << " done!" << endl;
}

//after searching the board...
//traverse the trie and push all found words into vector 'found'
//Recursive!
void getFoundWords(Trie* p){
    int i;

    if( p ){
        if( p->found )
            found.push_back( p->word );

        for(i = 0; i < ALPHA_SIZE; i++){
            getFoundWords( p->children[i] );
        }
    }
}

void * buildBoard( char boggleFile[] ){
    char * buffer = readF( boggleFile );

    //calculate the dimensions
    int len = strlen( buffer );
    cols = sqrt( len ) + 2;
    children = new int[NUM_BRANCHES] {-1-cols, -cols, 1-cols, -1, 1, cols-1, cols, cols+1};
    board_size = cols * cols;
    puzzle_size = (cols-2) * (cols-2);
    onePercentage = double(1) / 100 * puzzle_size;
    board = new int[board_size];

    //add border
    int j = 0;
    for(int i = 0; i < board_size; i++){
        if( (i < cols) ||           //top
        ((i+1) % cols == 0) ||      //right
        (i > cols * (cols -1)) ||   //bot
        (i % cols == 0) ){             //left
            board[i] = BORDER;
        }else{
            board[i] = buffer[j] - 'a';
            j++;
        }
    }
}

//add word to Trie
void insertWord( char word[], const int len ){
    Trie* p = dict;
    int i;

    for(i = 0; word[i]; i++){
        int letter = word[i] - 'a'; //convert characters to ints: a=0 z=25

        //combine 'qu' into single cube represented by 'q'
        if( ('q' == word[i]) && ('u' == word[i+1]) )
            i++;

        p->count++; //track how many words use this prefix

        if( !p->children[letter] )
            p->children[letter] = new Trie;

        p->children[letter]->parent = p;

        p = p->children[letter];
    }

    p->word = word; //the last node completes the word, save it here
}

// load dictionary into trie
void buildTrie( char dictFile[] )
{
    char * buffer = readF( dictFile );
    char * word;
    int len, i;

    dict = new Trie;

    word = strtok(buffer,"\n\t");
    while (word != NULL) {
        len = strlen( word );
        if( len >= 3 ){ //per Boggle rules
            insertWord( word, len );
            wordsParsed++;
        }

        word = strtok (NULL, "\n\t");
    }
}

//returns a Trie* to the next prefix or NULL
//keeps track of which words have been found
inline Trie* lookup(const int i, Trie* p){
    p = p->children[i];
    if( p && p->word ){
        if( p->found ){
            duplicates++;
        }else{
            p->found = true;
            p->parent->count--; //decrement how many words are left that use this prefix
        }
    }

    return p;
}

//depth first search. recursive!
//returns the number of NEW words found in children + self
//Words that have already been found do not count
inline bool descend(int cubeIndex, Trie* p, vector<bool> searched){
    ++checkedNodes;

    p = lookup( board[cubeIndex], p);

    if( p && p->count ){ //is this a valid prefix? Are there any remaining words that use it?

        searched[cubeIndex] = true; //mark this cube as used

        for(int i = 0; i < NUM_BRANCHES; i++){ //descend to each neighboring cube
            int child = cubeIndex + children[i];
            if((board[child] != BORDER) && !searched[child]) //faster to check here
                descend(child, p, searched);
        }
    }
}



//for each cube on the board, perform a depth first search using descend()
void traverseBoard(){
    Trie* p = dict;
    vector<bool> searched; //cubes should be used only once per word
    int i, j = 0;

    //initialize searched to false for all cubes on the board
    for(int i = 0; i < board_size; i++) searched.push_back(false);

    for(int i = 0; i < board_size; i++){ //for each cube
        if(board[i] != BORDER){
            descend(i, p, searched); //DFS
            statusBar(j);
            ++j;
        }
    }
}



int main(int argc, char* argv[]){
    char boggleFile[100] = "boggle.txt";
    char dictFile[100] = "mydictionary.txt";
    char resultsFile[100] = "results.txt";

    cout.imbue(std::locale("")); //adds commas to large numbers

    if( argc > 1 )
        strcpy(boggleFile, argv[1]);
    if( argc > 2 )
        strcpy(dictFile, argv[2]);

    buildBoard( boggleFile );

    cout    << "================================================" << endl;

    buildTrie( dictFile );

    cout    << wordsParsed << " words parsed in " << dictFile << endl;
    cout    << "Word length limit of " << WSIZE << " characters" << endl;
    cout    << puzzle_size << " cubes on the board" << endl;

    for(int i = 0; i < 100; i++){
        cout << ".";
    }
    cout << " 100%" << endl;

    begin = clock();

    traverseBoard();
    getFoundWords( dict );

    double end = double(clock() - begin) / CLOCKS_PER_SEC;
    double speed = 0;
    if( end )
        speed = floor((checkedNodes/end)/1000);

    cout    << found.size() << " words found in "
            << end  << " seconds" << endl
            << (checkedNodes/puzzle_size) << " nodes checked per cube" << endl
            << "~" << speed << " nodes checked per millisecond" << endl
            << duplicates << " duplicate words found" << endl;

    saveResults( resultsFile);

    cout    << "Results saved to " << resultsFile << endl;
    cout    << "================================================" << endl << endl;
}
