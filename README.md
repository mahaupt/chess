# chess

Basic terminal chess program with a built-in AI.

## Build

```sh
mkdir -p build
cd build
cmake ..
cmake --build .
```

The executable is created at `build/chess`.

The project builds as C++23.

## Usage

Start a normal interactive game:

```sh
./build/chess
```

Set the AI thinking time for an interactive game:

```sh
./build/chess --time 3
```

Start an interactive game from a FEN position:

```sh
./build/chess --fen "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
```

You can combine both options:

```sh
./build/chess --fen "rnbqkb1r/pppp1ppp/5n2/4p2Q/2B1P3/8/PPPP1PPP/RNB1K1NR w KQkq - 0 1" --time 5
```

The special position name `startpos` is accepted by the benchmark commands.

## AI benchmarking

Search the best move for a position:

```sh
./build/chess --bestmove "startpos" --time 3
```

The optional `--time` argument sets the search budget in seconds. The output
prints the selected move, searched depth, evaluated nodes, nodes per second and
transposition table statistics.

Example:

```text
bestmove E2E4
Computer searched 6 plies, evaluated 1094175 nodes in 3.00029 seconds of a 3 second budget (364690 nodes/sec)
Transposition table: 1094167 probes, 188189 hits, 160434 exact hits, 24435 bound cutoffs, 909293 stores
```

Run the Python benchmark suite:

```sh
python3 scripts/benchmark.py --time 1
```

The script runs several `--bestmove` positions, prints depth, nodes, speed and
transposition table hit rate, and checks known best moves where available.

You can also write the detailed results to JSON:

```sh
python3 scripts/benchmark.py --time 1 --json benchmark-results.json
```

## Perft

Run a perft node count from a position:

```sh
./build/chess --perft "startpos" 4
```

Run perft divide to print the node count for each root move:

```sh
./build/chess --perft-divide "startpos" 3
```

Perft is useful for checking that move generation is correct. The current engine
supports normal legal move filtering and check detection, but FEN en passant
state and promotion are not complete yet.

## Classes
### CBoard
Chess board. Owns the pieces, stores captured pieces, tracks side to move and
provides FEN loading, legal move generation and check detection.

### ChessAI
Calculates the best move for the computer opponent. Uses iterative deepening,
alpha-beta pruning, move ordering, root parallelism and a transposition table.

### CFigure
Base abstract class for all figures.

### Game
Main game class. Organizes interactive moves and turns.

### Move
Contains and applies a move, and can reverse it during search.

### movingPrefab
Settings class for the possible movement patterns of a figure.

### Point
Saves a board point, parses coordinate strings and turns coordinates into
strings, for example `G4`.

### _figures/*_
All concrete figure classes.
