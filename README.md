C++ Fast Boggle Solver
==================

Benchmarks
-------------
CPU: Pentium SU2700 1.3GHz  
RAM: 3gb  

Loads dictionary of 178,590 words from file into a tree in < 1 second.  
Solves 100x100 Boggle (boggle.txt) in 4 seconds. A board that size usually contains ~44,000 words.  
Solving a 4x4 Boggle is too fast to provide a meaningful benchmark. :)


How it Works
------------
The solver uses a custom tree structure. I'm not sure it can be considered a trie but it's similar. 

Each node has 26 branches, 1 for each letter of the alphabet.   
I traverse the branches of the boggle board in parallel with the branches of my dictionary.  
If the branch does not exist in the dictionary, I stop searching it on the Boggle board.  
I convert all the letters on the board to ints. So 'A' = 0.  
Since it's just arrays, lookup is always O(1).  
Each node stores if it completes a word and how many words exist in its children.  
The tree is pruned as words are found to reduce repeatedly searching for the same words. I believe pruning is also O(1).

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
or the nature of this problem. If you have an idea why I'd love to hear it! -> machineboy2045@gmail.com
