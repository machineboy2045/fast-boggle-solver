#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sparsehash/dense_hash_map>
#include <vector>
#include <map>
#include <locale>

using namespace std;
using google::dense_hash_map;      // namespace where class lives by default


const int NUM_BRANCHES = 8;

/*
The words & prefixes in the dictionary each have an int value.

-8 = A whole word, as opposed to a prefix
-16 = A whole word that has been found on the board
0+ = A prefix. Prefixes have positive values indicating how many times they occur in the whole
words of the dictionary. These values are decremented each time a whole word containing
that prefix is found. For instance if all the words begining with 'dist' are found,
the prefix 'dist' would have a value of 0 and would be removed from the dictionary.

*/
const int WHOLE_WORD = -8; 
const int FOUND_WORD = -16; 


const int WSIZE = 20; //longest word read in dictionary

int duplicates = 0;
int wordsFound = 0;
int wordsParsed = 0;
int checkedNodes = 0;
int progress = 0;
int onePercentage;
char longestWord[WSIZE];
clock_t begin; //used to time search duration
char * board;
int board_size;
int puzzle_size;
int cols;
int * SEARCHED;
int * children;


struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

map<const char*,bool,ltstr> sorted;

struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return (s1 == s2) || (s1 && s2 && strcmp(s1, s2) == 0);
  }
};

//MurmuHash by Austin Appleby
typedef struct{
  inline unsigned int operator() (const char * key) const {
    int seed = 95032;
    int len = strlen( key );

    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    // Initialize the hash to a 'random' value

    unsigned int h = seed ^ len;

    // Mix 4 bytes at a time into the hash

    const unsigned char * data = (const unsigned char *)key;

    while(len >= 4)
    {
        unsigned int k = *(unsigned int *)data;

        k *= m; 
        k ^= k >> r; 
        k *= m; 
        
        h *= m; 
        h ^= k;

        data += 4;
        len -= 4;
    }
    
    // Handle the last few bytes of the input array

    switch(len)
    {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0];
            h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;


  }
} MurmurHash;


dense_hash_map <const char*, int, MurmurHash, eqstr>  dict;
// dense_hash_map <const char*, int, MurmurHash, eqstr> prefixes;
// dense_hash_map <const char*, bool, MurmurHash, eqstr> words; //found words


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

void * buildBoard( char boggleFile[] ){
    char * buffer = readF( boggleFile );
    int len = strlen( buffer );
    cols = sqrt( len ) + 2;
    children = new int[NUM_BRANCHES] {-1-cols, -cols, 1-cols, -1, 1, cols-1, cols, cols+1};
    board_size = cols * cols;
    puzzle_size = (cols-2) * (cols-2);
    onePercentage = double(1) / 100 * puzzle_size;
    board = new char[board_size+1];
    // SEARCHED = new int[board_size * NUM_BRANCHES * WSIZE]; //word size = max depth, neighbors = branches

    //add border
    int j = 0;
    for(int i = 0; i < board_size; i++){
        if( (i < cols) ||           //top
        ((i+1) % cols == 0) ||      //right
        (i > cols * (cols -1)) ||   //bot
        (i % cols == 0) ){             //left
            board[i] = '*';
        }else{
            board[i] = buffer[j];
            j++;
        }
    }
    board[board_size+1] = '\0';
}   

void printboard( FILE * file ){
    char str[] = "  ";
    for(int i = 0; i < board_size; i++){
        str[0] = board[i];
        if( board[i] == 'q' ){ 
            fputs("Qu",file);
        }else{
            fputs(str,file);
        }
        if( (i+1) % cols == 0 ) fputs("\n",file);     
    }
    fputs("\n",file);
}

void printWords( FILE * file ){    
    dense_hash_map<const char*, int, MurmurHash, eqstr>::iterator p;
    map<const char*, bool, ltstr>::iterator m;
    for(p = dict.begin(); p != dict.end(); p++) {
        if( p->second == FOUND_WORD ){
            if( strlen(longestWord) < strlen(p->first) ) strcpy(longestWord, p->first);
            sorted[p->first] = true;
        }
    }

    //alphabetized
    for(m = sorted.begin(); m != sorted.end(); m++) {
        fputs (m->first,file);
        fputs ("\n",file);
    }

    
}

// Chech that this is a prefix not a WHOLE_WORD or FOUND_WORD which are < 0
bool isPrefix( int x ){
    return( x > 0 );
}

//splits word into all possible prefixes
//"AND" becomes "A", "AN", "AND"
//keeps count of how many times a prefix occurs
void incrementPrefixes( char word[] ){
    int len = WSIZE+1, i;
    char pre[len];
    dense_hash_map<const char*,int, MurmurHash, eqstr>::iterator j;
    
    for(i = 0; word[i]; i++){
        pre[i] = word[i];
        pre[i+1] = '\0';

        j = dict.find(pre);

        //if this prefix is already in the dictionary
        //increment the count
        if( j != dict.end() ){
            if( isPrefix(j->second) ) j->second++; 
        //otherwise, the count = 1
        }else{
            char * copy = new char[len];
            strcpy(copy,pre);
            dict[copy] = 1;
        }
    }
}

//also erases prefixes who's count has reached 0
inline void decrementPrefixes( const char * word ){
    int len = WSIZE+1, i;
    char pre[len];
    dense_hash_map<const char*,int, MurmurHash, eqstr>::iterator j;

    for(i = 0; word[i]; i++){
        pre[i] = word[i];
        pre[i+1] = '\0';

        j = dict.find(pre);
        if( j != dict.end() ){
            if( j->second > 0 ) j->second--;
            if( j->second == 0 ) dict.erase( j );
        }
    }
}

// load dictionary into hash table
void buildDict( char dictFile[] )
{
    char * buffer = readF( dictFile );
    char * word;
    int len, i;

    word = strtok(buffer,"\n\t");
    while (word != NULL) {
        len = strlen( word );
        if( len >= 3 ){ //per Boggle rules
            dict[word] = WHOLE_WORD;
            wordsParsed++;

            incrementPrefixes( word );
        }
        
        word = strtok (NULL, "\n\t");
    }
}

void statusBar(int i){
    if( (cols-2) < 100 ) 
        return;
    // int increment = double(1) / puzzle_size * 100; //number of bars for each 1%
    i += 1; //compensate for array index
    if( i - progress == onePercentage ){
        cout << "|" << flush;
        progress += onePercentage;
    }
    if( i == puzzle_size ) cout << " done!" << endl;
}

//returns true if found
//also keeps track of which words have been found
inline bool inDictionary(const char * str){
    dense_hash_map<const char*,int, MurmurHash, eqstr>::iterator p;

    p = dict.find( str );
    if( p == dict.end() ) return false; 

    if( !isPrefix(p->second) ){
        if( FOUND_WORD == p->second ){ 
            ++duplicates;
        }else{
            p->second = FOUND_WORD;
            ++wordsFound;
            decrementPrefixes( p->first ); //only decrement if this is the first occurance
        }
    }
    return true;
}

inline void appendCube(char * str, char c, int &len){
    str[len++] = c;
    if( 'q' == c){ 
        str[len++] = 'u';
    }
    str[len] = '\0';
}

inline void find(int node, char str[], vector<bool> searched, int len){
    
    ++checkedNodes;

    appendCube( str, board[node], len);

    if( inDictionary( str ) ){

        searched[node] = true;

        // if( len+1 == WSIZE ) //did not see any performance gain by checking this
        //     return;

        for(int i = 0; i < NUM_BRANCHES; i++){
            int child = node + children[i];
            if((board[child] != '*') && !searched[child]) //faster to check here            
                find(child, str, searched, len);
        }
    }
}

void findWords(){
    char str[WSIZE+1];
    vector<bool> searched;
    int j = 0;

    for(int i = 0; i < 100; i++){
        cout << ".";
    }
    cout << " 100%" << endl;


    for(int i = 0; i < board_size; i++) searched.push_back(false);
    for(int i = 0; i < board_size; i++){
        if(board[i] != '*'){
            find(i, str, searched, 0);
            statusBar(j);
            ++j;
        }
    }
}

void saveResults( char fname[] ){
    FILE * file;
    file = fopen (fname,"w");
    printboard( file );
    printWords( file );
    fclose(file);
}


int main(int argc, char* argv[]){
    cout.imbue(std::locale("")); //for comma seperated numbers


    dict.set_empty_key(NULL);
    dict.set_deleted_key("!");

    char boggleFile[100] = "boggle.txt";
    char dictFile[100] = "mydictionary.txt";
    char resultsFile[100] = "results.txt";

    if( argc > 1 ) 
        strcpy(boggleFile, argv[1]);
    if( argc > 2 ) 
        strcpy(dictFile, argv[2]);

    buildBoard( boggleFile );

    cout    << "================================================" << endl;

    buildDict( dictFile );

    cout    << wordsParsed << " words parsed in " << dictFile << endl;
    cout    << "Word length limit of " << WSIZE << " characters" << endl;
    // cout    << dict.size() << " word fragments in dictionary" << endl;
    cout    << puzzle_size << " cubes on the board" << endl;
    
    begin = clock();
    findWords();
    double end = double(clock() - begin) / CLOCKS_PER_SEC;
    double speed = 0;
    if( end )
        speed = floor((checkedNodes/end)/1000);

    cout    << wordsFound << " words found in "
            << end  << " seconds" << endl
            // << dict.size() << " word fragments remaining" << endl
            << (checkedNodes/puzzle_size) << " nodes checked per cube" << endl
            << "~" << speed << " nodes checked per millisecond" << endl
            << duplicates << " duplicate words found" << endl;

    saveResults( resultsFile);

    cout    << "\"" << longestWord << "\" was the longest word found (" << strlen(longestWord) << " characters)" << endl
            << "Results saved to " << resultsFile << endl
            << "================================================" << endl << endl;
}
