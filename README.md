## Sindacco Overview

Sindacco is a decently strong chess engine written in C++. It has it's own GUI (not the prettiest)
made with SFML. It uses a matrix and some piece-lists to reprezent the board. It features a pretty 
fast legal move generator.  For the moment it only works on windows.

## Search features:

Sindacco's search currently has the following features:

* #### Alpha-beta pruning
* #### Iterative Deepening
* #### Quiescense search
* #### Transposition table
* #### MVV/LVA
* #### Killer Heuristic
* #### History Heuristic
* #### Internal Iterative Deepening
* #### Null Move Pruning
* #### Late Move Reductions
* #### Futility Pruning
* #### Razoring

## Evaluation:

* #### Piece-Square Tables
* #### Bishop, knight, rook pair penalties/bonuses
* #### Passed, double, isolated pawns
* #### Mobility
* #### Enemy king zone attacks
