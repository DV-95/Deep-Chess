#include <bits/stdc++.h>
#include "chess-library/include/chess.hpp"

using namespace std;

bool givesCheck(chess::Board& board, chess::Move move) {
    board.makeMove(move);
    bool ans = board.inCheck();
    board.unmakeMove(move);
    return ans;
}

bool isCapture(chess::Board& board, chess::Move move) {
    return board.at(move.to()) != chess::Piece::NONE ||
           move.typeOf() == chess::Move::ENPASSANT;
}

pair<int, vector<string>> utility (chess::Board& board, int depth, int alpha, int beta){
    chess::Movelist moves2;
    chess::movegen::legalmoves(moves2,board);
    vector<string> line{};
    if(moves2.empty()){
        if(board.inCheck()){
            if(board.sideToMove()==chess::Color::BLACK) return make_pair(-1,line);
            else return make_pair(1,line);
        }
        else return make_pair(0,line);
    }
    else {

        if(depth==0) return make_pair(0,line);

        chess::Movelist moves;
        for(auto& move : moves2) if(givesCheck(board,move)) moves.add(move);
        for(auto&move : moves2) if(isCapture(board,move) && (!givesCheck(board,move))) moves.add(move);
        for(auto& move :moves2) if((!isCapture(board,move))&&(!givesCheck(board,move))) moves.add(move);



        if(board.sideToMove()==chess::Color::BLACK){
            chess::Move MAX;
            string maxmove;
            pair<int,vector<string>> maxutil = make_pair(-100,line);
            

        for (const auto& move : moves){
            board.makeMove(move);
            pair<int,vector<string>> per = utility(board,depth-1,alpha,beta);
            int x = per.first;

            if(x>maxutil.first) {
                maxutil = per;
                MAX = move;
            }
            if (x>alpha){
                alpha = x;
            }
            
            board.unmakeMove(move);
            if(alpha>=beta){
                break;
            }
        }
        maxmove = chess::uci::moveToSan(board,MAX);
        maxutil.second.insert(maxutil.second.begin(),maxmove);
        return maxutil;

        }
        else {
            chess::Move MIN;
            string minmove;
            pair<int,vector<string>> minutil = make_pair(100,line);

        for (const auto& move : moves){
            board.makeMove(move);
            pair<int,vector<string>> per = utility(board,depth-1,alpha,beta);
            int x = per.first;
            if(x<minutil.first) {
                minutil = per;
                MIN = move;
            }
            if(x<beta){
                beta=x;
            }
            
            board.unmakeMove(move);
            if(alpha>=beta){
                break;
            }
        }
        
        minmove = chess::uci::moveToSan(board,MIN);
        minutil.second.insert(minutil.second.begin(),minmove);
        return minutil;

        }
    }

}



int main () {
    string fen;
    cout<<"Please enter the fen string of the puzzle"<<endl;
    getline(cin,fen);

    chess::Board board = chess::Board(fen);
    int matein;
    cout<<"Please enter the number of moves required to checkmate"<<endl;
    cin>>matein;

    auto x = utility(board,2*matein -1,-1000,1000);
    cout<<x.first<<endl;
    for(auto& move : x.second){
        cout<<move<<endl;
    }

    return 0;
}