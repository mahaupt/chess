#!/usr/bin/env python3
import argparse
import json
import re
import subprocess
import sys
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Iterable


DEFAULT_POSITIONS = [
    {
        "name": "startpos",
        "fen": "startpos",
        "expected": [],
    },
    {
        "name": "scholars_mate",
        "fen": "rnbqkb1r/pppp1ppp/5n2/4p2Q/2B1P3/8/PPPP1PPP/RNB1K1NR w KQkq - 0 1",
        "expected": ["H5F7"],
    },
    {
        "name": "mate_in_one",
        "fen": "7k/8/5KQ1/8/8/8/8/8 w - - 0 1",
        "expected": ["G6G7"],
    },
    {
        "name": "mate_in_three",
        "fen": "8/8/8/8/8/8/4k3/KQ6 w - - 0 1",
        "expected": ["B1B3", "B1D3", "B1E4"],
    },
    {
        "name": "win_queen",
        "fen": "4k3/8/8/8/8/8/4q3/4K2R w - - 0 1",
        "expected": ["E1E2"],
    },
    {
        "name": "win_rook",
        "fen": "4k3/8/8/8/8/8/4r3/4K3 w - - 0 1",
        "expected": ["E1E2"],
    },
    {
        "name": "bishop_takes_queen",
        "fen": "4k2q/6B1/8/8/8/8/8/4K3 w - - 0 1",
        "expected": ["G7H8"],
    },
    {
        "name": "knight_takes_queen",
        "fen": "4k3/8/8/3q4/5N2/8/8/4K3 w - - 0 1",
        "expected": ["F4D5"],
    },
    {
        "name": "black_bishop_takes_queen",
        "fen": "4k3/8/8/8/8/8/6b1/4K2Q b - - 0 1",
        "expected": ["G2H1"],
    },
    {
        "name": "black_rook_takes_queen",
        "fen": "4k3/8/8/8/8/8/4Q3/4r2K b - - 0 1",
        "expected": ["E1E2"],
    },
]


BESTMOVE_RE = re.compile(r"^bestmove\s+(?P<move>\S+)", re.MULTILINE)
NUMBER_PATTERN = r"[0-9.eE+-]+"

SEARCH_RE = re.compile(
    r"Computer searched (?P<depth>\d+) plies, evaluated (?P<nodes>\d+) nodes in "
    rf"(?P<seconds>{NUMBER_PATTERN}) seconds of a (?P<budget>{NUMBER_PATTERN}) second budget "
    rf"\((?P<nps>{NUMBER_PATTERN}) nodes/sec\)"
)
TT_RE = re.compile(
    r"Transposition table: (?P<probes>\d+) probes, (?P<hits>\d+) hits, "
    r"(?P<exact_hits>\d+) exact hits, (?P<bound_cutoffs>\d+) bound cutoffs, "
    r"(?P<stores>\d+) stores"
)


@dataclass
class BenchmarkResult:
    name: str
    fen: str
    bestmove: str
    expected: list[str]
    passed: bool | None
    depth: int
    nodes: int
    seconds: float
    budget: float
    nodes_per_second: float
    tt_probes: int
    tt_hits: int
    tt_exact_hits: int
    tt_bound_cutoffs: int
    tt_stores: int


def load_positions(path: Path | None) -> list[dict]:
    if path is None:
        return DEFAULT_POSITIONS

    with path.open("r", encoding="utf-8") as file:
        positions = json.load(file)

    if not isinstance(positions, list):
        raise ValueError("positions file must contain a JSON list")

    for position in positions:
        if "name" not in position or "fen" not in position:
            raise ValueError("each position needs at least 'name' and 'fen'")
        position.setdefault("expected", [])

    return positions


def parse_output(name: str, fen: str, expected: list[str], output: str) -> BenchmarkResult:
    bestmove_match = BESTMOVE_RE.search(output)
    search_match = SEARCH_RE.search(output)
    tt_match = TT_RE.search(output)

    if bestmove_match is None or search_match is None or tt_match is None:
        raise ValueError(f"could not parse engine output for {name}:\n{output}")

    bestmove = bestmove_match.group("move")
    normalized_expected = [move.upper() for move in expected]
    passed = None if not normalized_expected else bestmove.upper() in normalized_expected

    return BenchmarkResult(
        name=name,
        fen=fen,
        bestmove=bestmove,
        expected=expected,
        passed=passed,
        depth=int(search_match.group("depth")),
        nodes=int(search_match.group("nodes")),
        seconds=float(search_match.group("seconds")),
        budget=float(search_match.group("budget")),
        nodes_per_second=float(search_match.group("nps")),
        tt_probes=int(tt_match.group("probes")),
        tt_hits=int(tt_match.group("hits")),
        tt_exact_hits=int(tt_match.group("exact_hits")),
        tt_bound_cutoffs=int(tt_match.group("bound_cutoffs")),
        tt_stores=int(tt_match.group("stores")),
    )


def run_position(engine: Path, position: dict, time_budget: float) -> BenchmarkResult:
    command = [
        str(engine),
        "--bestmove",
        position["fen"],
        "--time",
        str(time_budget),
    ]
    completed = subprocess.run(
        command,
        check=False,
        capture_output=True,
        text=True,
    )

    output = completed.stdout + completed.stderr
    if completed.returncode != 0:
        raise RuntimeError(f"engine failed for {position['name']}:\n{output}")

    return parse_output(
        position["name"],
        position["fen"],
        list(position.get("expected", [])),
        output,
    )


def print_table(results: Iterable[BenchmarkResult]) -> None:
    rows = list(results)
    name_width = max(len("position"), *(len(result.name) for result in rows))
    move_width = max(len("bestmove"), *(len(result.bestmove) for result in rows))

    print(
        f"{'position':<{name_width}}  {'ok':<4}  {'bestmove':<{move_width}}  "
        f"{'depth':>5}  {'nodes':>10}  {'seconds':>8}  {'nodes/sec':>10}  {'tt hit%':>7}"
    )
    print("-" * (name_width + move_width + 66))

    for result in rows:
        if result.passed is None:
            ok = "-"
        else:
            ok = "yes" if result.passed else "no"
        hit_rate = result.tt_hits / result.tt_probes * 100 if result.tt_probes > 0 else 0.0
        print(
            f"{result.name:<{name_width}}  {ok:<4}  {result.bestmove:<{move_width}}  "
            f"{result.depth:>5}  {result.nodes:>10}  {result.seconds:>8.3f}  "
            f"{result.nodes_per_second:>10.0f}  {hit_rate:>6.1f}%"
        )

    total_nodes = sum(result.nodes for result in rows)
    total_seconds = sum(result.seconds for result in rows)
    average_nps = total_nodes / total_seconds if total_seconds > 0 else 0.0
    checked = [result for result in rows if result.passed is not None]
    passed = sum(1 for result in checked if result.passed)

    print()
    print(f"Total: {total_nodes} nodes in {total_seconds:.3f} seconds ({average_nps:.0f} nodes/sec)")
    if checked:
        print(f"Bestmove checks: {passed}/{len(checked)} passed")


def main() -> int:
    parser = argparse.ArgumentParser(description="Benchmark the chess engine with --bestmove positions.")
    parser.add_argument("--engine", default="./build/chess", help="path to the chess executable")
    parser.add_argument("--time", type=float, default=1.0, help="time budget per position in seconds")
    parser.add_argument("--positions", type=Path, help="optional JSON file with benchmark positions")
    parser.add_argument("--json", type=Path, help="write detailed results to a JSON file")
    args = parser.parse_args()

    if args.time <= 0:
        print("--time must be greater than zero", file=sys.stderr)
        return 1

    engine = Path(args.engine)
    if not engine.exists():
        print(f"engine not found: {engine}", file=sys.stderr)
        return 1

    try:
        positions = load_positions(args.positions)
        results = [run_position(engine, position, args.time) for position in positions]
    except (OSError, RuntimeError, ValueError) as error:
        print(error, file=sys.stderr)
        return 1

    print_table(results)

    if args.json is not None:
        with args.json.open("w", encoding="utf-8") as file:
            json.dump([asdict(result) for result in results], file, indent=2)
            file.write("\n")

    failed = [result for result in results if result.passed is False]
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
