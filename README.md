fast-boggle-solver
==================

Boggle Solver

I wrote a solver in C++. It uses Google's SparseHash hashtable implementation. Loading a dictionary of ~178,000 words is almost instant. Solving a 4x4 boggle is so trivial that there is no meaningful time measurement for it. So I tried it on various sizes. 100x100 boggle takes 6.79 seconds on my single core laptop.

One optimization I used was as I search the puzzle I record if a prefix has been used up. That is, I've found every word containing that prefix. For instance, all words beginning with "a" (doesn't happen) or "dis" (might happen). So I can abort searching when I come across a depleted prefix.

Kudos to Google for an amazingly fast hashtable. Much faster than the C++ STL.

I apologize for how messy this is. I plan to clean it up in the future.

The important files:
boggle_solver.cpp 
boggle_generator.cpp
mydictionary.txt 
