#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <map>
#include <vector>

using namespace std;

const int num_neighbors = 8;
const int MAX_INPUT = 50; //longest word in dictionary

int duplicates = 0;
clock_t begin; //used to time search duration


map <string,bool> dict;
map <string,int> prefixes;
map <string,bool> words; //found words


string buildBoard( string boggleFile ){
    ifstream file;
    string board;
    string tmp;
    file.open( boggleFile.c_str() );
    if( file ){
        getline( file, tmp );
        int cols = sqrt( tmp.length() ) + 2;
        int size = cols * cols;
        int j = 0;

        //add border
        for(int i = 0; i < size; i++){
            if( (i < cols) ||           //top
            ((i+1) % cols == 0) ||      //right
            (i > cols * (cols -1)) ||   //bot
            (i % cols == 0) ){             //left
                board += '*';
            }else{
                board += tmp[j];
                j++;
            }
        }
        return board;
    }else{
        cout << "ERROR: Board " << boggleFile << " could not be found. Exiting." << endl;
        exit(0);
    }
}   

void printboard(string board, ofstream &file){
    int len = board.length();
    int cols = sqrt( len );
    for(int i = 0; i < len; i++){
        char c = board[i];
        if( c == 'q' ){ 
            file << "Qu";
        }else{
            file << c << ' ';
        }
        if( (i+1) % cols == 0 ) file << endl;     
    }
    file << endl;
}

void printWords( ofstream &file ){
    map<string, bool>::iterator p;
    for(p = words.begin(); p != words.end(); p++) {
        file << p->first << endl;
    }
}

void incrementPrefixes( string word ){
    int len = word.length(), i;
    string prefix;
    map<string,int>::iterator j;

    for(i = 0; i < len; i++){
        prefix += word[i];
        j = prefixes.find(prefix);
        if( j != prefixes.end() ){
            j->second++;
        }else{
            prefixes[prefix] = 1;
        }
    }
}

//also erases prefixes who's count has reached 0
void decrementPrefixes( string word ){
    int len = word.length(), i;
    string prefix;
    map<string,int>::iterator j;

    for(i = 0; i < len; i++){
        prefix += word[i];
        j = prefixes.find(prefix);
        if( j != prefixes.end() ){
            j->second--;
            if( j->second <= 0 ) prefixes.erase( j );
        }
    }
}

// load dictionary into hash table
void buildDict( string dictFile )
{
    ifstream in_file;
    in_file.open( dictFile.c_str() ); //official scrabble players dictionary
    if( in_file ){
        string word;
        int len;
        int i;

        while( getline(in_file,word) )
        {
            len = word.length();
            if( len >= 3 ){ //per Boggle rules
                dict[word] = true;

                //track how many times each segment of a word appears in the dictionary
                incrementPrefixes( word );
            }
        }
    }else{
        cout << "ERROR: Dictionary " << dictFile << " could not be found. Exiting." << endl;
        exit(0);
    }
}

// searched = used cubes on the board
void find(int i, string str, vector<bool> searched, string &board, int neighbors[]){
    str += board[i];
    if(board[i] == 'q') str += 'u';

    if( searched[i] || (board[i] == '*') || (prefixes.find(str) == prefixes.end())   ) return; 
    if( dict.find(str) != dict.end() ){ 
        if( words.find(str) != words.end() ){ 
            duplicates++;
        }else{
            decrementPrefixes( str ); //only decrement if this is the first occurance
        }
        words[str] = true;
    }
    searched[i] = true;

    //recursion
    for(int j = 0; j < num_neighbors; j++){
        find(neighbors[j] + i, str, searched, board, neighbors);
    }
    
}

void findWords( string board ){
    string str;
    vector <bool> searched;
    int len = board.length();
    int cols = sqrt( len );
    int neighbors[num_neighbors] = {-1-cols,-cols,1-cols,-1,1,cols-1,cols,cols+1};

    for(int i = 0; i < len; i++) searched.push_back(false);
    begin = clock();

    for(int i = 0; i < len; i++){
        find(i, str, searched, board, neighbors);
    }
}

void saveResults( string board ){
    ofstream file;
    string fname = "results.txt";
    file.open( fname.c_str() );
    printboard( board, file );
    printWords( file );
    file.close();
    cout    << "Results saved to " << fname << endl
            << "================================================" << endl << endl;

}

int main(int argc, char* argv[]){
    string boggleFile = "boggle.txt";
    string dictFile = "ospd.txt";

    if( argc > 1 ) boggleFile = argv[1];
    if( argc > 2 ) dictFile = argv[2];

    string board = buildBoard( boggleFile );
    buildDict( dictFile );
    findWords( board );

    cout    << "================================================" << endl
            << dict.size() << " words parsed in " << dictFile << endl
            << words.size() << " words found in "
            << double(clock() - begin) / CLOCKS_PER_SEC << " seconds" << endl
            << duplicates << " duplicates found" << endl;

    saveResults( board );
}
