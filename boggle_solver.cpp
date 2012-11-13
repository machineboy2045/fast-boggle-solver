#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <map>
#include <vector>

using namespace std;

const int num_neighbors = 8;
const int MAX_WORD_LENGTH = 15; //longest word in dictionary
const int MAX_DEPTH = 5; //used by map reduce

int duplicates = 0;
string longestWord;
clock_t begin; //used to time search duration
int checkedNodes = 0;
int invalidParts = 0;


map <string,bool> dict;
map <string,int> prefixes;
map <string,bool> parts; //all parts valid parts of words
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
        if( longestWord.length() < p->first.length() ) longestWord = p->first;
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

void hashParts( string word ){
    int i, j, len = word.length();
    string str;
    for(i = 0; i < len; i++){
        str = "";
        for(j = i; j < len; j++){
            str += word[j];
            parts[str] = true;
        }
    }

    for(i = len-1; i >= 0; i--){
        str = "";
        for(j = i; j >= 0; j--){
            str += word[j];
            parts[str] = true;
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
            if( (len >= 3) && (len <= MAX_WORD_LENGTH) ){ //per Boggle rules
                dict[word] = true;

                //track how many times each segment of a word appears in the dictionary
                incrementPrefixes( word );
                // hashParts( word );
            }
        }
    }else{
        cout << "ERROR: Dictionary " << dictFile << " could not be found. Exiting." << endl;
        exit(0);
    }
}


// searched = used cubes on the board
bool usableNode(int i, string str, vector<bool> searched, string &board, int children[], int depth){
    if( board[i] == '*' ) return false;
    if( searched[i] ) return true;
    // string ngram;
    // if( str.length() ) ngram += str[str.length()-1];
    // ngram += board[i];
    str += board[i];
    if(board[i] == 'q') str += 'u';
    if(  parts.find(str) == parts.end() ) return false; //if this is not a part of any word


    // if(prefixes.find(str) == prefixes.end())   ) return true; 
    searched[i] = true;
    if( depth == MAX_DEPTH ) return true; //all the characters in the string are still valid parts
    //recursion
    bool usable = false;
    depth++;
    // cout << "searching children at depth " << depth << endl;
    for(int j = 0; j < num_neighbors; j++){
        if( usableNode(children[j] + i, str, searched, board, children, depth) ) usable = true;
    }
    return usable;
}

void mapReduce( string &board ){
    string str;
    vector <bool> searched;
    int len = board.length();
    int cols = sqrt( len );
    int children[num_neighbors] = {-1-cols,-cols,1-cols,-1,1,cols-1,cols,cols+1};

    for(int i = 0; i < len; i++) searched.push_back(false);

    for(int i = 0; i < len; i++){
        if( board[i] != '*' ){
            if( !usableNode(i, str, searched, board, children, 1) ){ 
                invalidParts++;
                board[i] = '*';
            }
        }
    }
}
void find(int node, string str, vector<bool> searched, string &board, int children[]){
    if((board[node] == '*') || searched[node]) return;
    
    checkedNodes++;
    searched[node] = true;

    str += board[node];
    if(board[node] == 'q') str += 'u';

    if( prefixes.find(str) == prefixes.end() ) return; 
    if( dict.find(str) != dict.end() ){ 
        if( words.find(str) != words.end() ){ 
            duplicates++;
        }else{
            decrementPrefixes( str ); //only decrement if this is the first occurance
        }
        words[str] = true;
    }

    int j = 0;
    while( j < 8 ){
        find(node + children[j++], str, searched, board, children); 
    }
}

void findWords( string &board ){
    string str;
    vector <bool> searched;
    int len = board.length();
    int cols = sqrt( len );
    int children[num_neighbors] = {-1-cols,-cols,1-cols,-1,1,cols-1,cols,cols+1};

    for(int i = 0; i < len; i++) searched.push_back(false);
    for(int i = 0; i < len; i++){
        find(i, str, searched, board, children);
    }
}

void saveResults( string board ){
    ofstream file;
    string fname = "results.txt";
    file.open( fname.c_str() );
    printboard( board, file );
    printWords( file );
    file.close();
    cout    << "\"" << longestWord << "\" was the longest word found (" << longestWord.length() << " characters)" << endl
            << "Results saved to " << fname << endl
            << "================================================" << endl << endl;

}

int main(int argc, char* argv[]){
    string boggleFile = "boggle.txt";
    string dictFile = "mydictionary.txt";

    if( argc > 1 ) boggleFile = argv[1];
    if( argc > 2 ) dictFile = argv[2];

    string board = buildBoard( boggleFile );
    buildDict( dictFile );

    begin = clock();
    // for(int i = 0; i < 7; i++){
    //     mapReduce( board );
    // }
    cout    << "================================================" << endl
            << dict.size() << " words parsed in " << dictFile << endl
            << "Word length limit of " << MAX_WORD_LENGTH << " characters" << endl;
            // << double(clock() - begin) / CLOCKS_PER_SEC << " seconds spent reducing map" << endl
            // << invalidParts << " nodes that cannot form a valid word fragment with their children" << endl << endl;
    
    begin = clock();
    findWords( board );
    cout
            << words.size() << " words found in "
            << double(clock() - begin) / CLOCKS_PER_SEC << " seconds" << endl
            << checkedNodes << " nodes checked " << endl
            << duplicates << " duplicate words found" << endl;

    saveResults( board );
}
