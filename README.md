C++ Fast Boggle Solver
==================

Benchmarks
-------------
CPU: Pentium SU2700 1.3GHz
RAM: 3gb

Loads dictionary of 178,590 words in < 1 second.
Solves 100x100 Boggle (boggle.txt) in 4.87 seconds.
Solving a 4x4 Boggle is too fast to provide a meaningful benchmark. :)


How it Works
------------
The dictionary is loaded into a trie (prefix tree). 

Everything is stored as INTS: 'a'=0, 'z'=25. 

'q' cubes are treated as 'qu' per Boggle rules.

The board is stored in a 1 dimensional array of INTS. 

The puzzle is traversed using a depth first search.

As we traverse we check if the prefix we are building is valid. Lookup time in our trie is ~O(1).

The trie keeps count of how many words are children of each prefix. If we find all the words starting with "begi", that prefix will have a count of 0. The next time we come across "begi" we will know there's no point in searching further since we've found all possible words in that branch.

Running
-------
The binaries are compiled in Debian 64bit. 

boggle_generator -> 
Generates the boggle puzzle. Optionally specify a puzzle width (4, 100, etc) when running from the command line.

boggle_solver -> 
Run this after generating a puzzle. The found words will be saved to results.txt.

Compiling
---------
When compiling with G++ use the -O3 optimization flag for maximum performance!

Final thoughts
--------------
Earlier versions used Google's Sparse Hash. The trie implementation is 1.3x faster. The dictionary loading is also faster.

I found that the solver slows down as the puzzle size increases. It doesn't just take more time overall, the time required per node in the depth-first-search actually increases. I'm not sure if it's a bug in my code, a hardware limitation,
or the nature of this problem. If you have an idea why I'd love to hear it! -> willandmeling@gmail.com