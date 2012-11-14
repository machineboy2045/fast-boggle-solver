#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sparsehash/dense_hash_map>
#include <vector>
#include <map>

using namespace std;
using google::dense_hash_map;      // namespace where class lives by default


const int num_neighbors = 8;
const int WSIZE = 20; //longest word read in dictionary

int duplicates = 0;
int checkedNodes = 0;
char longestWord[WSIZE];
clock_t begin; //used to time search duration
char * board;
int board_size;
int cols;
int *children;


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


dense_hash_map <const char*, bool, MurmurHash, eqstr>  dict;
dense_hash_map <const char*, int, MurmurHash, eqstr> prefixes;
dense_hash_map <const char*, bool, MurmurHash, eqstr> words; //found words


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

char* buildBoard( char boggleFile[] ){
    char * buffer = readF( boggleFile );
    int len = strlen( buffer );
    int cols = sqrt( len ) + 2;
    int size = cols * cols;
    char* board = new char[size+1];
    int j = 0;

    //add border
    for(int i = 0; i < size; i++){
        if( (i < cols) ||           //top
        ((i+1) % cols == 0) ||      //right
        (i > cols * (cols -1)) ||   //bot
        (i % cols == 0) ){             //left
            board[i] = '*';
        }else{
            board[i] = buffer[j];
            j++;
        }
        board[i+1] = '\0';
    }
    return board;
}   

void printboard( FILE * file ){
    int len = strlen(board);
    int cols = sqrt( len );
    char str[] = "  ";
    for(int i = 0; i < len; i++){
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
    dense_hash_map<const char*, bool, MurmurHash, eqstr>::iterator p;
    map<const char*, bool, ltstr>::iterator m;
    for(p = words.begin(); p != words.end(); p++) {
        if( strlen(longestWord) < strlen(p->first) ) strcpy(longestWord, p->first);
        sorted[p->first] = true;
    }

    //alphabetized
    for(m = sorted.begin(); m != sorted.end(); m++) {
        fputs (m->first,file);
        fputs ("\n",file);
    }

    
}

void incrementPrefixes( char word[] ){
    int len = strlen(word), i;
    char pre[len+1];
    dense_hash_map<const char*,int, MurmurHash, eqstr>::iterator j;
    
    for(i = 0; i < len; i++){
        pre[i] = word[i];
        pre[i+1] = '\0';

        j = prefixes.find(pre);
        if( j != prefixes.end() ){
            j->second++;
        }else{
            char * copy = new char[len+1];
            strcpy(copy,pre);
            prefixes[copy] = 1;
        }
    }
}

//also erases prefixes who's count has reached 0
void decrementPrefixes( const char * word ){
    int len = strlen(word), i;
    char pre[len+1];
    dense_hash_map<const char*,int, MurmurHash, eqstr>::iterator j;

    for(i = 0; i < len; i++){
        pre[i] = word[i];
        pre[i+1] = '\0';

        j = prefixes.find(pre);
        if( j != prefixes.end() ){
            j->second--;
            if( j->second <= 0 ) prefixes.erase( j );
        }
    }
}

// load dictionary into hash table
void buildDict( char dictFile[] )
{
    char * buffer = readF( dictFile );
    char * word;
    int len, i;
    // printf("boggle_easy: %s\n", buffer);

    word = strtok(buffer,"\n\t");
    while (word != NULL) {
        len = strlen( word );
        if( len >= 3 ){ //per Boggle rules
            dict[word] = true;

            //track how many times each segment of a word appears in the dictionary
            incrementPrefixes( word );
        }
        
        word = strtok (NULL, "\n\t");
    }
}
void find(int node, char str[], vector<bool> searched, int depth){
    dense_hash_map<const char*,bool, MurmurHash, eqstr>::iterator p;
    dense_hash_map<const char*,int, MurmurHash, eqstr>::iterator q;
    
    checkedNodes++;
    searched[node] = true;
    if( depth == WSIZE -2 ) return;

    str[depth] = board[node];
    if(board[node] == 'q'){ 
        str[depth+1] = 'u';
        depth++;
    }
    str[depth+1] = '\0';

    q = prefixes.find(str);
    if( q == prefixes.end() ) return; 
    p = dict.find( str );
    if( p != dict.end() ){ 
        if( words.find(str) != words.end() ){ 
            duplicates++;
        }else{
            decrementPrefixes( q->first ); //only decrement if this is the first occurance
            words[p->first] = true;
        }
    }

    int j = 0;
    while( j < 8 ){
        int child = node + children[j++];
        if((board[child] != '*') && !searched[child]){ //faster to check here
            find(child, str, searched, depth+1); //tail recursion transformed to loop by compiler
        }
    }
}

void findWords(){
    char str[WSIZE];
    vector <bool> searched;
    for(int i = 0; i < board_size; i++) searched.push_back(false);
    for(int i = 0; i < board_size; i++){
        if(board[i] != '*'){
            find(i, str, searched, 0);
        }
    }
}

void saveResults(){
    FILE * file;

    char fname[] = "results.txt";
    file = fopen ("mylog.txt","a");
    printboard( file );
    printWords( file );
    cout    << "\"" << longestWord << "\" was the longest word found (" << strlen(longestWord) << " characters)" << endl
            << "Results saved to " << fname << endl
            << "================================================" << endl << endl;

    fclose(file);
}



int main(int argc, char* argv[]){
    dict.set_empty_key(NULL);
    prefixes.set_empty_key(NULL);
    prefixes.set_deleted_key("!");
    words.set_empty_key(NULL);

    char boggleFile[] = "boggle_easy.txt";
    char dictFile[] = "mydictionary.txt";

    // if( argc > 1 ) boggleFile = argv[1];
    // if( argc > 2 ) dictFile = argv[2];

    board = buildBoard( boggleFile );
    board_size = strlen( board );
    cols = sqrt( board_size );
    int arr[] = {-1-cols, -cols, 1-cols, -1, 1, cols-1, cols, cols+1};
    children = arr;

    buildDict( dictFile );

    cout    << "================================================" << endl
            << dict.size() << " words parsed in " << dictFile << endl
            << "Word length limit of " << WSIZE << " characters" << endl;
    
    begin = clock();
    findWords();

    cout    << words.size() << " words found in "
            << double(clock() - begin) / CLOCKS_PER_SEC << " seconds" << endl
            << checkedNodes << " nodes checked " << endl
            << duplicates << " duplicate words found" << endl;

    saveResults();
}
