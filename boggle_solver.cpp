#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <map>
#include <vector>

using namespace std;

const int num_neighbors = 8;
const int MAX_WORD_LENGTH = 15; //longest word read in dictionary

int duplicates = 0;
int checkedNodes = 0;
string longestWord;
clock_t begin; //used to time search duration


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

// load dictionary into hash table
void buildDict( string dictFile )
{
    ifstream in_file;
    in_file.open( dictFile.c_str() ); //official scrabble players dictionary
    if( in_file ){
        string word;
        int len, i;

        while( getline(in_file,word) )
        {
            len = word.length();
            if( (len >= 3) && (len <= MAX_WORD_LENGTH) ){ //per Boggle rules
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
        find(node + children[j++], str, searched, board, children); //tail recursion transformed to loop by compiler
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

    cout    << "================================================" << endl
            << dict.size() << " words parsed in " << dictFile << endl
            << "Word length limit of " << MAX_WORD_LENGTH << " characters" << endl;
    
    begin = clock();
    findWords( board );

    cout    << words.size() << " words found in "
            << double(clock() - begin) / CLOCKS_PER_SEC << " seconds" << endl
            << checkedNodes << " nodes checked " << endl
            << duplicates << " duplicate words found" << endl;

    saveResults( board );
}
