# chess
Basic Chess program with AI

## Build

```sh
mkdir -p build
cd build
cmake ..
cmake --build .
```

The executable is created at `build/chess`.

The project builds as C++23.

## Classes
### CBoard
Chess board, contains all figures and the map of the board.

### ChessAI
Calculates the best move for the computer enemy.

### CFigure
Base abstract class for all figures.

### Game
Main game class, organize moves and turns

### Move
Saves, contains, do and reverse moves.

### movingPrefab
This is a settings class, for all possible moves of a figure

### Point
This the class to save a board point, parse coordinate strings and turn coordinates into strings (eg. "G4")

### _figures/*_
All figure classes
