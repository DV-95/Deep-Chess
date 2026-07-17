# Chess Engine ♟️

A UCI-compatible chess engine written in **C++**, built from scratch as part of my exploration into classical chess engine design.

The engine implements a modern alpha-beta search with several important search optimizations and evaluation heuristics, achieving a playing strength of approximately **1750 Elo** in testing against Stockfish.

---

## Features

### Search

- Alpha-Beta Pruning
- Iterative Deepening
- Quiescence Search
- Transposition Table (Zobrist Hashing)
- Checkmate distance scoring
- Draw detection
  - Threefold repetition
  - 50-move rule
  - Stalemate

---

### Move Ordering

Moves are searched in the following order:

1. Transposition Table best move
2. Checking moves
3. Captures ordered using MVV-LVA
4. Quiet moves

Good move ordering dramatically increases alpha-beta pruning efficiency.

---

### Evaluation Function

The evaluation combines several positional features:

- Material balance
- Piece-Square Tables
  - Pawn
  - Knight
  - Bishop
  - Rook
  - Queen
  - King
- Mobility evaluation
- Checkmate detection

---

### Time Management

Supports standard UCI time controls:

- Fixed depth search
- movetime
- wtime / btime
- winc / binc
- movestogo

The engine dynamically allocates thinking time and uses iterative deepening to ensure it always returns a legal move before the allotted time expires.

---

### UCI Support

Implemented commands include:

- `uci`
- `isready`
- `ucinewgame`
- `position startpos`
- `position fen`
- `go depth`
- `go movetime`
- `go wtime/btime`
- `quit`

Compatible with:

- CuteChess
- Arena
- BanksiaGUI
- Any UCI-compatible GUI

---

## Technologies Used

- C++
- STL
- Disservin Chess Library
- CuteChess CLI

---

# Testing

All testing results below are **as of 18 June 2026**.

Time control used:

- **180 + 2 seconds**
- 30 games
- Stockfish with fixed Elo limit

---

## Against Stockfish 1700 Elo

```
Score of Divesh vs Stockfish: 17 - 12 - 1  [0.583]


Elo Difference:
+58.5 ±130


```

Result:

- Positive overall score
- Estimated playing strength around **1758 Elo**

---

## Against Stockfish 1800 Elo

```
Score of Divesh vs Stockfish: 11 - 18 - 1  [0.383]

Elo Difference:
-82.6 ±132

```

Result:

- Competitive against 1800-strength Stockfish
- Estimated playing strength around **1717 Elo**

---

## Estimated Playing Strength

Based on the match results,

**Estimated Engine Rating: ~1740–1760 Elo**

---


## Building

Compile using any C++17 compiler.

Example:

```bash
g++ main.cpp -O3 -std=c++17 -o engine
```

---

## Example Usage

```bash
uci
isready
position startpos
go depth 8
```

or run the engine through any UCI GUI.

---

## Project Structure

```
main.cpp
chess-library/
README.md
```

---

## Acknowledgements

- Disservin Chess Library
- Stockfish
- CuteChess

---

## Author

**Divesh Khatwani**

Centre For MultiDisciplinary Education '29
IIT Bombay

Built as my Seasons of Code 2026 Project and part of my journey into chess programming and classical game-search algorithms.