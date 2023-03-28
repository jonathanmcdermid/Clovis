<div align="center" style="padding-top: 60px">
    <img src="/img/ClovisBanner.png" alt="Clovis banner" width="945px" height="auto">
    <br>
    <img src="https://img.shields.io/github/downloads/jonathanmcdermid/Clovis/total?color=red&style=for-the-badge">
    <img src="https://shields.io/github/v/release/jonathanmcdermid/Clovis?label=Latest%20release&color=blueviolet&style=for-the-badge">
    <br>
</div>

## Overview

Named after the first king of the Franks and the paleoamerican mammoth slayers, Clovis is a powerful UCI chess engine written in C++. It is a successor to [Hopper Chess](https://github.com/jonathanmcdermid/Hopper-Chess-Engine) in that it follows standard convention more closely. 

## Installation

### Binaries

[Releases](https://github.com/jonathanmcdermid/Clovis/releases)

### Build from source

Clovis requires `make` and a compiler with C++17 support.

```
git clone https://github.com/jonathanmcdermid/Clovis
cd Clovis
make
./build/Clovis
```

Commands for the engine can be issued using the standard [UCI format](http://page.mi.fu-berlin.de/block/uci.htm)

## Strength

### Rankings

Clovis is currently estimated to have an Elo rating of ~2880

- [CCRL 40/15](https://ccrl.chessdom.com/ccrl/4040/) **Clovis III 2860 Elo**
- [CCRL Blitz](https://ccrl.chessdom.com/ccrl/404/) **Clovis III 2852 Elo**

## Technical Details

### Board Representation and Move Generation

- [Bitboards](https://www.chessprogramming.org/Bitboards)
- [Magic Bitboards](https://www.chessprogramming.org/Magic_Bitboards)
- [Move Generation](https://www.chessprogramming.org/Move_Generation)

### Search

- [Negamax](https://www.chessprogramming.org/Negamax)
- [Principal Variation Search](https://www.chessprogramming.org/Principal_Variation_Search)
- [Quiescence Search](https://www.chessprogramming.org/Quiescence_Search)
- [Iterative Deepening](https://www.chessprogramming.org/Iterative_Deepening)
- [Transposition Table](https://www.chessprogramming.org/Transposition_Table)
- [Pawn Hash Table](https://www.chessprogramming.org/Pawn_Hash_Table)
- [Aspiration Windows](https://www.chessprogramming.org/Aspiration_Windows)
- [Internal Iterative Deepening](https://www.chessprogramming.org/Internal_Iterative_Deepening)
- [Reverse Futility Pruning](https://www.chessprogramming.org/Reverse_Futility_Pruning)
- [Null Move Pruning](https://www.chessprogramming.org/Null_Move_Pruning)
- [Static Exchange Evaluation](https://www.chessprogramming.org/Static_Exchange_Evaluation)
- [Late Move Reductions](https://www.chessprogramming.org/Late_Move_Reductions)
- [Killer Heuristic](https://www.chessprogramming.org/Killer_Heuristic)
- [Countermove Heuristic](https://www.chessprogramming.org/Countermove_Heuristic)

### Evaluation

- [Piece Square Tables](https://www.chessprogramming.org/Piece-Square_Tables)
- [Tapered Evaluation](https://www.chessprogramming.org/Tapered_Eval)
- [Texel's Tuning Method](https://www.chessprogramming.org/Texel%27s_Tuning_Method)
- [King Safety](https://www.chessprogramming.org/King_Safety)
- [Outposts](https://www.chessprogramming.org/Outposts)
- [Pawn Structure](https://www.chessprogramming.org/Pawn_Structure)
- [Mobility](https://www.chessprogramming.org/Mobility)

## Credits

This engine uses ideas from various sources

### Engine Inspirations

- [Stockfish](https://github.com/official-stockfish/Stockfish)
- [Ethereal](https://github.com/AndyGrant/Ethereal)
- [BBC](https://github.com/maksimKorzh/bbc)
- [Blunder](https://github.com/algerbrex/blunder)

### Resources

- [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page)
- [TalkChess Forum](http://talkchess.com/forum3/viewforum.php?f=7)
- [CCRL](https://kirill-kryukov.com/chess/discussion-board/viewforum.php?f=7)
- [Cute Chess](https://cutechess.com/)

Special thanks to the engine testers!

