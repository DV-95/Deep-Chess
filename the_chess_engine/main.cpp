#include <bits/stdc++.h>
#include "chess-library/include/chess.hpp"
#include <sstream>
#include <chrono>

using namespace std;

// Piece Square Tables
const int pawnPST[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5, 5, 10, 25, 25, 10, 5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, -5, -10, 0, 0, -10, -5, 5,
    5, 10, 10, -20, -20, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0};

const int knightPST[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50};

const int bishopPST[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 5, 5, 10, 10, 5, 5, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20};

const int rookPST[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 10, 10, 10, 10, 10, 10, 5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 5, 5, 0, 0, 0};

const int queenPST[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 5, 5, 5, 5, 5, 0, -10,
    -10, 0, 5, 0, 0, 0, 0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20};

const int kingPST[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, 20, 0, 0, 0, 0, 20, 20,
    20, 30, 10, 0, 0, 10, 30, 20};

bool givesCheck(chess::Board &board, chess::Move move)
{
    board.makeMove(move);
    bool ans = board.inCheck();
    board.unmakeMove(move);
    return ans;
}

bool isCapture(chess::Board &board, chess::Move move)
{
    return board.at(move.to()) != chess::Piece::NONE ||
           move.typeOf() == chess::Move::ENPASSANT;
}

inline int flipSquare(int sq)
{
    return sq ^ 56;
}

struct TTEntry
{
    uint64_t key;
    int score;
    int depth;
    chess::Move bestmove;
    int flag;
    // 0 exact, 1 lowerbound, 2 upperbound
};

// pawn 100, knight 320, bishop 330, rook 500, queen 900, king 10000
int mvvLva(chess::Board &board, chess::Move move)
{
    chess::Piece victim = board.at(move.to());

    if (move.typeOf() == chess::Move::ENPASSANT)
        victim = chess::Piece(chess::PieceType::PAWN,
                              board.sideToMove() == chess::Color::WHITE ? chess::Color::BLACK : chess::Color::WHITE);

    chess::Piece attacker = board.at(move.from());

    int victimValue = 0;
    switch (victim.type().internal())
    {
    case chess::PieceType::PAWN:
        victimValue = 100;
        break;
    case chess::PieceType::KNIGHT:
        victimValue = 320;
        break;
    case chess::PieceType::BISHOP:
        victimValue = 330;
        break;
    case chess::PieceType::ROOK:
        victimValue = 500;
        break;
    case chess::PieceType::QUEEN:
        victimValue = 900;
        break;
    case chess::PieceType::KING:
        victimValue = 10000;
        break;
    default:
        break;
    }

    int attackerValue = 0;
    switch (attacker.type().internal())
    {
    case chess::PieceType::PAWN:
        attackerValue = 100;
        break;
    case chess::PieceType::KNIGHT:
        attackerValue = 320;
        break;
    case chess::PieceType::BISHOP:
        attackerValue = 330;
        break;
    case chess::PieceType::ROOK:
        attackerValue = 500;
        break;
    case chess::PieceType::QUEEN:
        attackerValue = 900;
        break;
    case chess::PieceType::KING:
        attackerValue = 10000;
        break;
    default:
        break;
    }

    return victimValue * 10 - attackerValue;
}

int mobility(chess::Board &board)
{
    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);

    const int MOBILITY_WEIGHT = 5;

    if (board.sideToMove() == chess::Color::WHITE)
        return MOBILITY_WEIGHT * moves.size();
    else
        return -MOBILITY_WEIGHT * moves.size();
}

int eval(chess::Board &board)
{
    int score = 0;

    for (int sq = 0; sq < 64; sq++)
    {
        auto piece = board.at(chess::Square(sq));
        if (piece == chess::Piece::NONE)
            continue;
        {

            switch (piece.internal())
            {
            case chess::Piece::WHITEPAWN:
                score += 100 + pawnPST[sq];
                break;
            case chess::Piece::BLACKPAWN:
                score -= 100 + pawnPST[flipSquare(sq)];
                break;

            case chess::Piece::WHITEKNIGHT:
                score += 320 + knightPST[sq];
                break;
            case chess::Piece::BLACKKNIGHT:
                score -= 320 + knightPST[flipSquare(sq)];
                break;

            case chess::Piece::WHITEBISHOP:
                score += 330 + bishopPST[sq];
                break;
            case chess::Piece::BLACKBISHOP:
                score -= 330 + bishopPST[flipSquare(sq)];
                break;

            case chess::Piece::WHITEROOK:
                score += 500 + rookPST[sq];
                break;
            case chess::Piece::BLACKROOK:
                score -= 500 + rookPST[flipSquare(sq)];
                break;

            case chess::Piece::WHITEQUEEN:
                score += 900 + queenPST[sq];
                break;
            case chess::Piece::BLACKQUEEN:
                score -= 900 + queenPST[flipSquare(sq)];
                break;

            case chess::Piece::WHITEKING:
                score += 10000 + kingPST[sq];
                break;
            case chess::Piece::BLACKKING:
                score -= 10000 + kingPST[flipSquare(sq)];
                break;
            }
        }
    }
    score += mobility(board);
    return score;
}

int quiescence(chess::Board &board, int alpha, int beta)
{
    int stand_pat = eval(board);

    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);

    if (moves.empty())
    {
        if (board.inCheck())
        {
            if (board.sideToMove() == chess::Color::WHITE)
                return -100000;
            else
                return 100000;
        }
        return 0;
    }

    if (board.sideToMove() == chess::Color::WHITE)
    {
        if (stand_pat >= beta)
            return beta;

        if (stand_pat > alpha)
            alpha = stand_pat;

        for (auto move : moves)
        {
            if (!isCapture(board, move))
                continue;

            board.makeMove(move);

            int score = quiescence(board, alpha, beta);

            board.unmakeMove(move);

            if (score >= beta)
                return beta;

            if (score > alpha)
                alpha = score;
        }

        return alpha;
    }
    else
    {
        if (stand_pat <= alpha)
            return alpha;

        if (stand_pat < beta)
            beta = stand_pat;

        for (auto move : moves)
        {
            if (!isCapture(board, move))
                continue;

            board.makeMove(move);

            int score = quiescence(board, alpha, beta);

            board.unmakeMove(move);

            if (score <= alpha)
                return alpha;

            if (score < beta)
                beta = score;
        }

        return beta;
    }
}

unordered_map<uint64_t, TTEntry> TT;

int utility(chess::Board &board, int depth, int alpha, int beta, int searchDepth)
{
    if (board.isHalfMoveDraw())
    {
        if (board.getHalfMoveDrawType().first == chess::GameResultReason::CHECKMATE)
        {
            return (board.sideToMove() == chess::Color::WHITE) ? (-100000 - depth) : (100000 + depth);
        }
        return 0; 
    }

    if (board.isRepetition())
    {
        return 0; 
    }

    uint64_t hash = board.hash();

    auto it = TT.find(hash);
    if (it != TT.end() && it->second.key == hash && (*it).second.depth >= depth)
    {

        if (it->second.flag == 0)
        {
            return (*it).second.score;
        }

        else if (it->second.flag == 1 && it->second.score >= beta)
        {
            return it->second.score;
        }

        else if (it->second.flag == 2 && it->second.score <= alpha)
        {
            return it->second.score;
        }
    }

    chess::Movelist moves2;
    chess::movegen::legalmoves(moves2, board);

    if (moves2.empty())
    {
        if (board.inCheck())
        {
            if (board.sideToMove() == chess::Color::WHITE)
                return -1e5 - depth;
            else
                return 1e5 + depth;
        }
        else
            return 0;
    }
    else
    {

        if (depth == 0)
            return quiescence(board, alpha, beta);

        chess::Movelist moves;

        if (it != TT.end())
        {
            chess::Move goodmove = (*it).second.bestmove;
            moves.add(goodmove);
        }
        for (auto &move : moves2)
        {
            if (it != TT.end() && it->second.key == hash && move == it->second.bestmove)
                continue;
            if (givesCheck(board, move))
                moves.add(move);
        }

        vector<pair<int, chess::Move>> captures;

        for (auto &move : moves2)
        {
            if (it != TT.end() && it->second.key == hash && move == it->second.bestmove)
                continue;
            // if (isCapture(board, move) && (!givesCheck(board, move)))
            //     moves.add(move);
            if (isCapture(board, move) && !givesCheck(board, move))
            {
                captures.push_back({mvvLva(board, move), move});
            }
        }

        sort(captures.begin(), captures.end(),
             [](const auto &a, const auto &b)
             {
                 return a.first > b.first;
             });

        for (auto &x : captures)
        {
            moves.add(x.second);
        }

        for (auto &move : moves2)
        {
            if (it != TT.end() && it->second.key == hash && move == it->second.bestmove)
                continue;
            if ((!isCapture(board, move)) && (!givesCheck(board, move)))
                moves.add(move);
        }

        int original_alpha = alpha;
        int original_beta = beta;

        if (board.sideToMove() == chess::Color::WHITE)
        {
            chess::Move MAX;
            string maxmove;
            int maxutil = -1e6;

            for (const auto &move : moves)
            {
                board.makeMove(move);
                int x = utility(board, depth - 1, alpha, beta, searchDepth);

                if (x > maxutil)
                {
                    maxutil = x;
                    MAX = move;
                }
                if (x > alpha)
                {
                    alpha = x;
                }

                board.unmakeMove(move);
                if (alpha >= beta)
                {
                    break;
                }
            }
            maxmove = chess::uci::moveToUci(MAX);

            TT[hash].score = maxutil;
            TT[hash].depth = depth;
            TT[hash].bestmove = MAX;
            TT[hash].key = hash;

            if (maxutil <= original_alpha)
            {
                TT[hash].flag = 2;
            }
            else if (maxutil >= beta)
            {
                TT[hash].flag = 1;
            }
            else
            {
                TT[hash].flag = 0;
            }

            return maxutil;
        }
        else
        {
            chess::Move MIN;
            string minmove;
            int minutil = 1e6;
            for (const auto &move : moves)
            {
                board.makeMove(move);
                int x = utility(board, depth - 1, alpha, beta, searchDepth);
                if (x < minutil)
                {
                    minutil = x;
                    MIN = move;
                }
                if (x < beta)
                {
                    beta = x;
                }

                board.unmakeMove(move);
                if (alpha >= beta)
                {
                    break;
                }
            }

            minmove = chess::uci::moveToUci(MIN);

            TT[hash].score = minutil;
            TT[hash].depth = depth;
            TT[hash].bestmove = MIN;
            TT[hash].key = hash;

            if (minutil <= alpha)
            {
                TT[hash].flag = 2;
            }
            else if (minutil >= original_beta)
            {
                TT[hash].flag = 1;
            }
            else
            {
                TT[hash].flag = 0;
            }

            return minutil;
        }
    }
}

int main()
{

    cout.setf(ios::unitbuf);

    string line;
    chess::Board board;

    while (getline(cin, line))
    {
        stringstream ss(line);
        string command;
        ss >> command;

        if (command == "uci")
        {

            cout << "id name MyChessEngine v1.0" << endl;
            cout << "id author Divesh" << endl;
            cout << "uciok" << endl;
        }
        else if (command == "isready")
        {
            cout << "readyok" << endl;
        }
        else if (command == "position")
        {
            string type;
            ss >> type;
            if (type == "startpos")
            {
                board = chess::Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

                string moves_keyword;
                if (ss >> moves_keyword && moves_keyword == "moves")
                {
                    string move_str;
                    while (ss >> move_str)
                    {
                        chess::Move move = chess::uci::uciToMove(board, move_str);
                        board.makeMove(move);
                    }
                }
            }
            else if (type == "fen")
            {
                string fen = "";
                string part;
                while (ss >> part && part != "moves")
                {
                    if (!fen.empty())
                        fen += " ";
                    fen += part;
                }
                board = chess::Board(fen);

                if (part == "moves")
                {
                    string move_str;
                    while (ss >> move_str)
                    {
                        chess::Move move = chess::uci::uciToMove(board, move_str);
                        board.makeMove(move);
                    }
                }
            }
        }
        else if (command == "go")
        {

            int search_depth = 10;

            int wtime = -1;
            int btime = -1;
            int winc = 0;
            int binc = 0;
            int movetime = -1;
            int movestogo = -1;

            int budget = -1;

            string sub_cmd;
            while (ss >> sub_cmd)
            {
                if (sub_cmd == "depth")
                    ss >> search_depth;

                else if (sub_cmd == "wtime")
                    ss >> wtime;

                else if (sub_cmd == "btime")
                    ss >> btime;

                else if (sub_cmd == "winc")
                    ss >> winc;

                else if (sub_cmd == "binc")
                    ss >> binc;

                else if (sub_cmd == "movetime")
                    ss >> movetime;

                else if (sub_cmd == "movestogo")
                    ss >> movestogo;
            }

            string bestmove = "";
            chess::Movelist moves;
            chess::movegen::legalmoves(moves, board);

            if (!moves.empty())
            {
                // Initialize bestmove with the first available legal move
                bestmove = chess::uci::moveToUci(moves[0]);
            }
            else
            {
                // The game is already over (checkmate or stalemate), but we assign a safety tag
                bestmove = "0000";
            }
            if (movetime != -1)
                budget = movetime;
            else if ((board.sideToMove() == chess::Color::WHITE && wtime != -1) ||
                     (board.sideToMove() == chess::Color::BLACK && btime != -1))
            {
                int myTime;
                int myInc;

                if (board.sideToMove() == chess::Color::WHITE)
                {
                    myTime = wtime;
                    myInc = winc;
                }
                else
                {
                    myTime = btime;
                    myInc = binc;
                }

                if (movestogo == -1)
                    movestogo = 30;

                budget = (myTime / movestogo) + (myInc / 2) - 25;

                if (budget < 5)
                    budget = 5;
            }
            auto start = chrono::steady_clock::now();

            for (int d = 1; d <= search_depth; d++)
            {

                utility(board, d, -1000000, 1000000, d);
                uint64_t hash = board.hash();

                if (TT.count(hash))
                    bestmove = chess::uci::moveToUci(TT[hash].bestmove);

                auto now = chrono::steady_clock::now();
                auto duration = now - start;
                auto elapsed = chrono::duration_cast<chrono::milliseconds>(duration).count();
                // cout << "info depth " << d
                //      << " elapsed " << elapsed
                //      << " ms\n";
                if (budget != -1 && elapsed > budget * 0.5)
                    break;
            }

            cout << "bestmove " << bestmove << endl;
        }

        else if (command == "ucinewgame")
        {
            TT.clear();
        }
        else if (command == "quit")
        {
            break;
        }
    }

    return 0;
}