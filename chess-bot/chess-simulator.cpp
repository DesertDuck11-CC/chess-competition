#include "chess-simulator.h"
// disservin's lib. drop a star on his hard work!
// https://github.com/Disservin/chess-library
#include "chess.hpp"
#include <random>
using namespace ChessSimulator;

static int PIECE_VALUES[6] = { 100, 320, 330, 500, 900, 20000 };

const int PAWN_TABLE[64]{
0,  0,  0,  0,  0,  0,  0,  0,
50, 50, 50, 50, 50, 50, 50, 50,
10, 10, 20, 30, 30, 20, 10, 10,
 5,  5, 10, 25, 25, 10,  5,  5,
 0,  0,  0, 20, 20,  0,  0,  0,
 5, -5,-10,  0,  0,-10, -5,  5,
 5, 10, 10,-20,-20, 10, 10,  5,
 0,  0,  0,  0,  0,  0,  0,  0 };

const int KNIGHT_TABLE[64]{
-50,-40,-30,-30,-30,-30,-40,-50,
-40,-20,  0,  0,  0,  0,-20,-40,
-30,  0, 10, 15, 15, 10,  0,-30,
-30,  5, 15, 20, 20, 15,  5,-30,
-30,  0, 15, 20, 20, 15,  0,-30,
-30,  5, 10, 15, 15, 10,  5,-30,
-40,-20,  0,  5,  5,  0,-20,-40,
-50,-40,-30,-30,-30,-30,-40,-50, };

const int BISHOP_TABLE[64]{
-20,-10,-10,-10,-10,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5, 10, 10,  5,  0,-10,
-10,  5,  5, 10, 10,  5,  5,-10,
-10,  0, 10, 10, 10, 10,  0,-10,
-10, 10, 10, 10, 10, 10, 10,-10,
-10,  5,  0,  0,  0,  0,  5,-10,
-20,-10,-10,-10,-10,-10,-10,-20, };

const int ROOK_TABLE[64]{
  0,  0,  0,  0,  0,  0,  0,  0,
  5, 10, 10, 10, 10, 10, 10,  5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
  0,  0,  0,  5,  5,  0,  0,  0 };

const int QUEEN_TABLE[64]{
-20,-10,-10, -5, -5,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5,  5,  5,  5,  0,-10,
 -5,  0,  5,  5,  5,  5,  0, -5,
  0,  0,  5,  5,  5,  5,  0, -5,
-10,  5,  5,  5,  5,  5,  0,-10,
-10,  0,  5,  0,  0,  0,  0,-10,
-20,-10,-10, -5, -5,-10,-10,-20 };

const int KING_MIDDLE_GAME[64]{
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-20,-30,-30,-40,-40,-30,-30,-20,
-10,-20,-20,-20,-20,-20,-20,-10,
 20, 20,  0,  0,  0,  0, 20, 20,
 20, 30, 10,  0,  0, 10, 30, 20, };

const int KING_END_GAME[64]{
-50,-40,-30,-20,-20,-30,-40,-50,
-30,-20,-10,  0,  0,-10,-20,-30,
-30,-10, 20, 30, 30, 20,-10,-30,
-30,-10, 30, 40, 40, 30,-10,-30,
-30,-10, 30, 40, 40, 30,-10,-30,
-30,-10, 20, 30, 30, 20,-10,-30,
-30,-30,  0,  0,  0,  0,-30,-30,
-50,-30,-30,-30,-30,-30,-30,-50 };

int evaluate(const chess::Board& board)
{
    int score = 0;

    for (int sq = 0; sq < 64; sq++)
    {
        auto piece = board.at(chess::Square(sq));

        if (piece == chess::Piece::NONE)
        {
            continue;
        }

        int value = 0;
        int type = (int)piece.type() - 1;
        value += PIECE_VALUES[type];

        switch (piece.type())
        {
        case (int)chess::PieceType::PAWN: 
            value += PAWN_TABLE[sq];
            break;
        case (int)chess::PieceType::KNIGHT:
            value += KNIGHT_TABLE[sq];
            break;
        case (int)chess::PieceType::BISHOP: 
            value += BISHOP_TABLE[sq];
            break;
        case (int)chess::PieceType::ROOK: 
            value += ROOK_TABLE[sq];
            break;
        case (int)chess::PieceType::QUEEN: 
            value += QUEEN_TABLE[sq];
            break;
        case (int)chess::PieceType::KING: 
            value += KING_MIDDLE_GAME[sq];
            break;
        }

        if (piece.color() == chess::Color::WHITE)
        {
            score += value;
        }
        else
        {
            score -= value;
        }
    }

    return score;
}

int minimax(chess::Board& board, int depth, int alpha, int beta, bool maximizing)
{
    if (depth == 0)
    {
        return evaluate(board);
    }

    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);

    if (moves.size() == 0)
    {
        return evaluate(board);
    }

    if (maximizing)
    {
        int best = -INFINITY;

        for (auto move : moves)
        {
            board.makeMove(move);
            int score = minimax(board, depth - 1, alpha, beta, false);
            board.unmakeMove(move);

            best = std::max(best, score);
            alpha = std::max(alpha, score);

            if (alpha >= beta)
            {
                break;
            }
        }

        return best;
    }
    else
    {
        int best = INFINITY;

        for (auto move : moves)
        {
            board.makeMove(move);
            int score = minimax(board, depth - 1, alpha, beta, true);
            board.unmakeMove(move);

            best = std::min(best, score);
            beta = std::min(beta, score);

            if (alpha >= beta)
            {
                break;
            }
        }

        return best;
    }
}

chess::Move findBestMove(chess::Board& board, int depth, chess::Movelist moves)
{
    int bestScore = -INFINITY;
    chess::Move bestMove = moves[0];

    for (auto move : moves)
    {
        board.makeMove(move);

        int score = minimax(board, depth - 1, -INFINITY, INFINITY, false);

        board.unmakeMove(move);

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

std::string ChessSimulator::Move(std::string fen, int timeLimitMs) {
    // create your board based on the board string following the FEN notation
    // search for the best move using minimax / monte carlo tree search /
    // alpha-beta pruning / ... try to use nice heuristics to speed up the search
    // and have better results return the best move in UCI notation you will gain
    // extra points if you create your own board/move representation instead of
    // using the one provided by the library    

    chess::Board board(fen);

    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);

    if (moves.size() == 0)
    {
        return "";
    }

    chess::Move move;

    if (board.sideToMove() == chess::Color::WHITE)
    {
        move = findBestMove(board, 3, moves);
    }
    else
    {
        // get random move
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, moves.size() - 1);
        move = moves[dist(gen)];
    }

    return chess::uci::moveToUci(move);
}