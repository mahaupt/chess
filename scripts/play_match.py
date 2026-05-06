#!/usr/bin/env python3
import argparse
import sys
from pathlib import Path


def import_chess():
    try:
        import chess
        import chess.engine
        import chess.pgn
    except ModuleNotFoundError:
        print("Missing Python dependency: python-chess", file=sys.stderr)
        print("Install it with: python3 -m pip install chess", file=sys.stderr)
        raise SystemExit(1)

    return chess, chess.engine, chess.pgn


def open_engine(engine_module, command: list[str]):
    try:
        return engine_module.SimpleEngine.popen_uci(command)
    except FileNotFoundError:
        print(f"Engine not found: {command[0]}", file=sys.stderr)
        raise SystemExit(1)


def configure_stockfish(engine, skill_level: int | None) -> None:
    if skill_level is None:
        return

    try:
        engine.configure({"Skill Level": skill_level})
    except Exception as error:
        print(f"Warning: could not set Stockfish skill level: {error}", file=sys.stderr)


def game_result_for_engine_error(board, error: Exception) -> str:
    side_to_move = "White" if board.turn else "Black"
    print(f"{side_to_move} engine failed: {error}", file=sys.stderr)
    return "0-1" if board.turn else "1-0"


def play_game(chess, engine_module, pgn_module, white, black, white_name: str, black_name: str,
              round_number: int, time_seconds: float):
    board = chess.Board()
    game = pgn_module.Game()
    game.headers["Event"] = "Local engine match"
    game.headers["Site"] = "local"
    game.headers["Round"] = str(round_number)
    game.headers["White"] = white_name
    game.headers["Black"] = black_name
    game.headers["TimeControl"] = f"{time_seconds:.3f}s/move"

    node = game
    limit = engine_module.Limit(time=time_seconds)

    while not board.is_game_over(claim_draw=True):
        engine = white if board.turn else black
        try:
            result = engine.play(board, limit)
        except Exception as error:
            game.headers["Result"] = game_result_for_engine_error(board, error)
            return game

        if result.move is None:
            game.headers["Result"] = "0-1" if board.turn else "1-0"
            return game

        node = node.add_variation(result.move)
        board.push(result.move)

    game.headers["Result"] = board.result(claim_draw=True)
    return game


def print_score(results: list[str]) -> None:
    white_wins = results.count("1-0")
    black_wins = results.count("0-1")
    draws = results.count("1/2-1/2")
    other = len(results) - white_wins - black_wins - draws
    print()
    print(f"Results: {white_wins} white wins, {black_wins} black wins, {draws} draws", end="")
    if other:
        print(f", {other} other", end="")
    print()


def main() -> int:
    parser = argparse.ArgumentParser(description="Run a local UCI match between this engine and Stockfish.")
    parser.add_argument("--engine", default="./build/chess", help="path to this chess engine")
    parser.add_argument("--stockfish", default="stockfish", help="path to Stockfish")
    parser.add_argument("--games", type=int, default=2, help="number of games to play")
    parser.add_argument("--time", type=float, default=1.0, help="seconds per move")
    parser.add_argument("--stockfish-skill", type=int, default=1, help="Stockfish Skill Level option")
    parser.add_argument("--pgn", type=Path, default=Path("match.pgn"), help="PGN output file")
    args = parser.parse_args()

    if args.games <= 0:
        print("--games must be greater than zero", file=sys.stderr)
        return 1
    if args.time <= 0:
        print("--time must be greater than zero", file=sys.stderr)
        return 1
    if args.stockfish_skill < 0 or args.stockfish_skill > 20:
        print("--stockfish-skill must be between 0 and 20", file=sys.stderr)
        return 1

    chess, engine_module, pgn_module = import_chess()

    our_command = [args.engine, "--uci"]
    stockfish_command = [args.stockfish]
    our_name = "MyChess"
    stockfish_name = f"Stockfish skill {args.stockfish_skill}"

    our_engine = open_engine(engine_module, our_command)
    stockfish_engine = open_engine(engine_module, stockfish_command)
    configure_stockfish(stockfish_engine, args.stockfish_skill)

    results = []
    try:
        with args.pgn.open("w", encoding="utf-8") as pgn_file:
            for index in range(args.games):
                our_engine_plays_white = index % 2 == 0
                if our_engine_plays_white:
                    white = our_engine
                    black = stockfish_engine
                    white_name = our_name
                    black_name = stockfish_name
                else:
                    white = stockfish_engine
                    black = our_engine
                    white_name = stockfish_name
                    black_name = our_name

                game = play_game(
                    chess,
                    engine_module,
                    pgn_module,
                    white,
                    black,
                    white_name,
                    black_name,
                    index + 1,
                    args.time,
                )
                print(game, file=pgn_file, end="\n\n")

                result = game.headers["Result"]
                results.append(result)
                print(f"Game {index + 1}: {white_name} vs {black_name} -> {result}")
    finally:
        our_engine.quit()
        stockfish_engine.quit()

    print_score(results)
    print(f"PGN written to {args.pgn}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
