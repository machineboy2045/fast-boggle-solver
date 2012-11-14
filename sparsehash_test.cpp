#include <iostream>
#include <sparsehash/dense_hash_map>
#include <string.h>

using namespace std;

using google::dense_hash_map;      // namespace where class lives by default
using std::cout;
using std::endl;
using tr1::hash;  // or __gnu_cxx::hash, or maybe tr1::hash, depending on your OS

template <class hash2>
 ( const void * key )
{
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.
cout << "murmur" << endl;
exit(1);
int len = 4;
int key = 5;
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


int main()
{
  dense_hash_map<const char*, int, hash<const char*> > months;
  
  months.set_empty_key(NULL);
  months["january"] = 31;
  months["february"] = 28;
  months["march"] = 31;
  months["april"] = 30;
  months["may"] = 31;
  months["june"] = 30;
  months["july"] = 31;
  months["august"] = 31;
  months["september"] = 30;
  months["october"] = 31;
  months["november"] = 30;
  months["december"] = 31;
  
  cout << "september -> " << months["september"] << endl;
  cout << "april     -> " << months["april"] << endl;
  cout << "june      -> " << months["june"] << endl;
  cout << "november  -> " << months["november"] << endl;
}