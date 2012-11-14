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

const int WSIZE = 20; //longest word read in dictionary

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
dense_hash_map <const char*, bool, MurmurHash, eqstr> bigrams;


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



//erase bigrams that exist in dictionary
void decrementbigrams( const char * word ){
    int len = strlen(word), i;
    char pre[len+1];
    dense_hash_map<const char*,bool, MurmurHash, eqstr>::iterator j;

    for(i = 0; i < len; i++){
        if(word[i+1] == '\0') break;

        pre[i] = word[i];
        pre[i+1] = word[i+1];
        pre[i+2] = '\0';

        j = bigrams.find(pre);
        if( j != bigrams.end() ){
            bigrams.erase( j );
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
            // dict[word] = true;

            //track how many times each segment of a word appears in the dictionary
            decrementbigrams( word );
        }
        
        word = strtok (NULL, "\n\t");
    }
}


void save( char fname[] ){    
    FILE * file;
    file = fopen(fname, "w");

    dense_hash_map<const char*, bool, MurmurHash, eqstr>::iterator p;
    for(p = bigrams.begin(); p != bigrams.end(); p++) {
        fputs (p->first,file);
        fputs ("\n",file);
    }
}


int main(int argc, char* argv[]){
    char dictionary[] = "mydictionary.txt";
    char output[] = "bigrams.txt";
    dict.set_empty_key(NULL);
    bigrams.set_empty_key(NULL);
    bigrams.set_deleted_key("!");
  

    char alpha[] = "abcdefghijklmnopqrstuvwxyz";

    for(int i = 0; i < 26; i++){
        for(int j = 0; j < 26; j++){
            char * str = new char[3];
            str[0] = alpha[i];
            str[1] = alpha[j];
            str[2] = '\0';
            // cout << str << endl;
            bigrams[str] = true;
        }
    }
    buildDict( dictionary );
    cout << bigrams.size() << " illegal bigrams" << endl;
    save( output );
}
