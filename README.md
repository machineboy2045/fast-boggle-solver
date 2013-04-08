Fast Boggle Solver
==================

Benchmarks
-------------
The solver is written in C++. It uses Google's SparseHash hashtable implementation. Loading a dictionary of ~178,000 words is almost instant. Solving a 4x4 boggle is so trivial that there is no meaningful time measurement for it. So I tried it on various sizes. 100x100 boggle takes 6.79 seconds on my single core laptop.

How it Works
------------
I break the dictionary words into prefixes and store them in a hash table so look-up is O(1). "AND" becomes "A", "AN", "AND". I do a depth first search using the prefix dictionary to determine at each step if the string I'm building is part of a word.

The only optimization I'm using is that I record if a prefix has been used up. That is, I've found every word containing that prefix. For instance all words beginning with "a" (doesn't happen) or "dist" (might happen). This way I can abort searching when I come across a depleted prefix.

Dependencies
------------
You will need to download Google's SparseHash. Kudos to Google for an amazingly fast hashtable. Much faster than the C++ STL.

Also thanks to Austin Appleby for MurmurHash function.

Environment
-----------
This was written on Debian 64bit.

Compiling & Running
-------------------
When compiling with G++, use the -O3 optimization flag for maximum performance!

boggle_generator -> 
Generates the boggle puzzle. Optionally specify a puzzle width (4, 100, etc) when running from the command line.

boggle_solver -> 
Run this after generating a puzzle. The found words will be saved to results.txt.

mydictionary.txt -> 
~178,000 words.

Final thoughts
--------------
I found that the solver slows down as the puzzle size increases. I don't just mean it takes longer overall, I mean the
time required per node in the depth-first-search actually increases. I'm not sure if it's a bug in my code, a hardware limitation,
or the nature of this problem. If anyone has an answer please email me at willandmeling@gmail.com