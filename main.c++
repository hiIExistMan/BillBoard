#include "chess.hpp"
#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;
using namespace chess;

//Piece Square Tables
int pawnMidTable[8][8] = {
    {   0,   0,   0,   0,   0,   0,   0,   0},
    { 150, 150, 150, 150, 150, 150, 150, 150},
    { 135, 115, 120, 130, 130, 120, 115, 135},
    { 105, 105, 100, 140, 140, 100, 105, 105},
    { 100, 100, 100, 130, 130, 100, 100, 100},
    { 120, 115, 100, 100, 100,  80, 115, 120},
    { 120, 120, 110,  80,  80, 110, 120, 120},
    {   0,   0,   0,   0,   0,   0,   0,   0}
};
int pawnEndTable[8][8] = {
    {   0,   0,   0,   0,   0,   0,   0,   0},
    {  50,  50,  50,  50,  50,  50,  50,  50},
    {  40,  40,  40,  40,  40,  40,  40,  40},
    {  30,  30,  30,  30,  30,  30,  30,  30},
    {  20,  20,  20,  20,  20,  20,  20,  20},
    {  10,  10,  10,  10,  10,  10,  10,  10},
    {  10,  10,  10,  10,  10,  10,  10,  10},
    {   0,   0,   0,   0,   0,   0,   0,   0}
};
int knightTable[8][8] = {
    { 250, 250, 260, 270, 270, 260, 250, 250},
    { 250, 300, 300, 310, 310, 300, 300, 250},
    { 260, 300, 310, 315, 315, 310, 300, 260},
    { 270, 300, 315, 320, 320, 315, 300, 270},
    { 270, 300, 315, 320, 320, 315, 300, 270},
    { 260, 300, 310, 315, 315, 310, 300, 260},
    { 250, 300, 300, 310, 310, 300, 300, 250},
    { 250, 250, 260, 270, 270, 260, 250, 250}
};
int bishopTable[8][8] = {
    { 305, 315, 315, 315, 315, 315, 315, 305},
    { 315, 325, 325, 325, 325, 325, 325, 315},
    { 315, 325, 320, 335, 335, 320, 325, 315},
    { 315, 350, 320, 335, 335, 320, 350, 315},
    { 315, 350, 335, 335, 335, 335, 350, 315},
    { 315, 335, 335, 335, 335, 335, 335, 315},
    { 315, 325, 325, 325, 325, 325, 325, 315},
    { 305, 315, 315, 315, 315, 315, 315, 305}
};
int rookTable[8][8] = {
    { 500, 500, 500, 500, 500, 500, 500, 500},
    { 555, 555, 555, 555, 555, 555, 555, 555},
    { 520, 520, 525, 560, 560, 525, 520, 520},
    { 500, 500, 500, 500, 500, 500, 500, 500},
    { 500, 500, 500, 500, 500, 500, 500, 500},
    { 500, 500, 500, 500, 500, 500, 500, 500},
    { 500, 500, 500, 500, 500, 500, 500, 500},
    { 500, 500, 520, 550, 550, 520, 500, 500}
};
int queenTable[8][8] = {
    { 880, 890, 900, 900, 900, 900, 890, 880},
    { 890, 900, 900, 900, 900, 900, 900, 890},
    { 890, 905, 905, 905, 905, 905, 905, 890},
    { 900, 900, 905, 905, 905, 905, 900, 900},
    { 900, 900, 905, 905, 905, 905, 900, 900},
    { 890, 905, 905, 905, 905, 905, 905, 890},
    { 890, 900, 900, 900, 900, 900, 900, 890},
    { 880, 890, 900, 900, 900, 900, 890, 880}
};
int kingMidTable[8][8] = {
    {  70,  60,  60,  50,  50,  60,  60,  70},
    {  70,  60,  60,  50,  50,  60,  60,  70},
    {  70,  60,  60,  50,  50,  60,  60,  70},
    {  70,  60,  60,  50,  50,  60,  60,  70},
    {  80,  70,  70,  65,  65,  70,  70,  80},
    {  90,  70,  70,  65,  65,  70,  70,  90},
    { 110,  85,  85,  90,  90,  85,  85, 110},
    { 100, 120, 100,  95,  95, 100, 120, 100}
};
int kingEndTable[8][8] = {
    {  50,  60,  70,  80,  80,  70,  60,  50},
    {  70,  80,  90, 100, 100,  90,  80,  70},
    {  70,  90, 120, 130, 130, 120,  90,  70},
    {  70,  90, 130, 140, 140, 130,  90,  70},
    {  70,  90, 130, 140, 140, 130,  90,  70},
    {  70,  90, 120, 130, 130, 120,  90,  70},
    {  70,  70, 100, 100, 100, 100,  70,  70},
    {  50,  70,  70,  70,  70,  70,  70,  50},
};

//Helps store transposition table
struct Transposition {
    U64 zobrist;
    int eval;
    int d;
};

struct bestMove {
    U64 zobrist;
    Move move;
    bool isWhite;
    int d;
};

//Transposition table
int transpositions = 32768;
Transposition transpositionTable[32768];
int transpositionInsertionLocation = 0;

//Best move table
int bestMoves = 32768;
bestMove moveTable[32768];
int moveInsertionLocation = 0;

//Some big numbers (INF has to be greater than mateScore for the search function to work properly)
int INF       = 10000000;
int mateScore =  1000000;

//Basic bitboard utility function, counts number of 1s in bitboard
int bitcount(unsigned long long n)
{
    int count = 0;
    while (n > 0)
    {
        count = count + 1;
        n = n & (n - 1);
    }
    return count;
}


//Basic static evaluation function using only PST
int staticEval(Board board) {
    // Calculating interpolated PST
    int pieceCount = bitcount(board.us(Color::WHITE)) + bitcount(board.them(Color::WHITE));
    double endgameWeight = max(0, (pieceCount - 10) / 22);

    int pawnTable[8][8] = {{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};
    

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            pawnTable[i][j] = ((pawnMidTable[i][j]*endgameWeight)+(pawnEndTable[i][j]*(1-endgameWeight)))/2;
        }
    }

    int kingTable[8][8] = {{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            kingTable[i][j] = ((kingMidTable[i][j]*endgameWeight)+(kingEndTable[i][j]*(1-endgameWeight)))/2;
        }
    }
    
        
    int pieceScore = 0;
    unsigned long long whitePawn = board.pieces(PieceType::PAWN,Color::WHITE);
    unsigned long long blackPawn = board.pieces(PieceType::PAWN,Color::BLACK);

    unsigned long long whiteKnight = board.pieces(PieceType::KNIGHT,Color::WHITE);
    unsigned long long blackKnight = board.pieces(PieceType::KNIGHT,Color::BLACK);

    unsigned long long whiteBishop = board.pieces(PieceType::BISHOP,Color::WHITE);
    unsigned long long blackBishop = board.pieces(PieceType::BISHOP,Color::BLACK);

    unsigned long long whiteRook = board.pieces(PieceType::ROOK,Color::WHITE);
    unsigned long long blackRook = board.pieces(PieceType::ROOK,Color::BLACK);

    unsigned long long whiteQueen = board.pieces(PieceType::QUEEN,Color::WHITE);
    unsigned long long blackQueen = board.pieces(PieceType::QUEEN,Color::BLACK);

    unsigned long long whiteKing = board.pieces(PieceType::KING,Color::WHITE);
    unsigned long long blackKing = board.pieces(PieceType::KING,Color::BLACK);

    for(int i = 0; i < 64; i++) {

        pieceScore += pawnTable  [7-(int)(i/8.0f)][i % 8]*(whitePawn   % 2);
        pieceScore += knightTable[7-(int)(i/8.0f)][i % 8]*(whiteKnight % 2);
        pieceScore += bishopTable[7-(int)(i/8.0f)][i % 8]*(whiteBishop % 2);
        pieceScore += rookTable  [7-(int)(i/8.0f)][i % 8]*(whiteRook   % 2);
        pieceScore += queenTable [7-(int)(i/8.0f)][i % 8]*(whiteQueen  % 2);
        pieceScore += kingTable  [7-(int)(i/8.0f)][i % 8]*(whiteKing   % 2);

        pieceScore -= pawnTable  [  (int)(i/8.0f)][i % 8]*(blackPawn   % 2);
        pieceScore -= knightTable[  (int)(i/8.0f)][i % 8]*(blackKnight % 2);
        pieceScore -= bishopTable[  (int)(i/8.0f)][i % 8]*(blackBishop % 2);
        pieceScore -= rookTable  [  (int)(i/8.0f)][i % 8]*(blackRook   % 2);
        pieceScore -= queenTable [  (int)(i/8.0f)][i % 8]*(blackQueen  % 2);
        pieceScore -= kingTable  [  (int)(i/8.0f)][i % 8]*(blackKing   % 2);



        whitePawn   = whitePawn   >> 1;
        whiteKnight = whiteKnight >> 1;
        whiteBishop = whiteBishop >> 1;
        whiteRook   = whiteRook   >> 1;
        whiteQueen  = whiteQueen  >> 1;
        whiteKing   = whiteKing   >> 1;

        blackPawn   = blackPawn   >> 1;
        blackKnight = blackKnight >> 1;
        blackBishop = blackBishop >> 1;
        blackRook   = blackRook   >> 1;
        blackQueen  = blackQueen  >> 1;
        blackKing   = blackKing   >> 1;
    }

    return pieceScore;
}

//Check transposition table for transposition
Transposition checkTransposition(Board board, int depth) {
    U64 hash = board.hash();
    for(int i = 0; i < transpositions; i++) {
        Transposition t = transpositionTable[i];
        if(t.zobrist == hash && t.d == depth) {
            return t;
        }
    }
    Transposition t;
    t.d = -1;
    return t;
}
//Add transposition
void insertTransposition(Board board, int eval, int depth) {
    Transposition t;
    t.zobrist = board.hash();
    t.eval = eval;
    t.d = depth;

    transpositionInsertionLocation++;
    transpositionInsertionLocation %= transpositions;
    transpositionTable[transpositionInsertionLocation] = t;
}
//Add move
void insertMove(Board board, Move move, int depth) {
    bestMove t;
    t.zobrist = board.hash();
    t.move = move;
    t.d = depth;
    t.isWhite = board.sideToMove() == Color::WHITE;

    moveInsertionLocation++;
    moveInsertionLocation %= bestMoves;
    moveTable[moveInsertionLocation] = t;
}
//Check transposition table for transposition
bestMove checkMove(Board board, int depth) {
    U64 hash = board.hash();
    for(int i = 0; i < bestMoves; i++) {
        bestMove t = moveTable[i];
        if(t.zobrist == hash && t.d == depth && t.isWhite == (board.sideToMove() == Color::WHITE)) {
            return t;
        }
    }
    bestMove t;
    t.d = -1;
    return t;
}
//node counter
int nodes = 0;
//Quiescence search
int quiesce(Board board, int alpha, int beta) {
    int stat = staticEval(board);

    nodes++;
    Movelist moves;
    movegen::legalmoves(moves,board);
    if(board.sideToMove() == Color::WHITE) {
        if(stat >= beta) return beta;
        if(stat > alpha) alpha = stat;
        for(auto move : moves) {
            if(!board.isCapture(move)) continue;
            board.makeMove(move);
            int eval = quiesce(board,alpha,beta);
            board.unmakeMove(move);
            if(eval >= beta) return beta;
            if(eval > alpha) alpha = eval;
        }
        return alpha;
    } else {
        if(stat <= alpha) return alpha;
        if(stat < beta) beta = stat;
        for(auto move : moves) {
            if(!board.isCapture(move)) continue;
            board.makeMove(move);
            int eval = quiesce(board,alpha,beta);
            board.unmakeMove(move);
            if(eval <= alpha) return alpha;
            if(eval < beta) beta = eval;
        }
        return beta;
    }
}

//Normal minimax search
int minimax(Board board, int depth,int alpha, int beta) {
    if(depth == 0) {
        return quiesce(board,alpha,beta);
    } 
    int result = (int)board.isGameOver().second;
    if (result != 3)
    {
        if (result == 0)
        {
            return -mateScore * ((board.sideToMove() == Color::BLACK)*2-1);
        }
        if (result == 2)
        {
            return 0;
        }
    }

        Transposition t = checkTransposition(board,depth);
        if (t.d != -1) {
            return t.eval * ((board.sideToMove() == Color::WHITE)*2-1);
        }
        nodes ++;

        Movelist moves;
        movegen::legalmoves(moves,board);

    

        if(board.sideToMove() == Color::WHITE) {
            bestMove pBest = checkMove(board,depth-1);
            Move best;
            if(pBest.d != -1 && moves.find(pBest.move) != -1) {
                best = pBest.move;
                board.makeMove(pBest.move);
                int eval = minimax(board,depth-1,alpha,beta);
                if(eval > mateScore/2) eval--; //forces engine to choose mate faster
                insertTransposition(board,eval,depth-1);
                board.unmakeMove(pBest.move);
                if(eval >= beta) {
                    return beta;
                }
                if(eval > alpha) {
                    alpha = eval;
                }
            }
            for(auto move : moves) {
                board.makeMove(move);
                int eval = minimax(board,depth-1,alpha,beta);
                if(eval < -mateScore/2) eval++; //forces engine to choose mate faster
                insertTransposition(board,eval,depth-1);
                board.unmakeMove(move);
                if(eval >= beta) {
                    insertMove(board,best,depth);
                    return beta;
                }
                if(eval > alpha) {
                    alpha = eval;
                    best = move;
                }
            }
            insertMove(board,best,depth);
            return alpha;

        } else {
            bestMove pBest = checkMove(board,depth-1);
            Move best;
            if(pBest.d != -1 && moves.find(pBest.move) != -1) {
                best = pBest.move;
                board.makeMove(pBest.move);
                int eval = minimax(board,depth-1,alpha,beta);
                insertTransposition(board,eval,depth-1);
                board.unmakeMove(pBest.move);
                if(eval <= alpha) {
                    return alpha;
                }
                if(eval < beta) {
                    beta = eval;
                }
            }
            for(auto move : moves) {
                board.makeMove(move);
                int eval = minimax(board,depth-1,alpha,beta);
                insertTransposition(board,eval,depth-1);
                board.unmakeMove(move);
                if(eval <= alpha) {
                    insertMove(board,best,depth);
                    return alpha;
                }
                if(eval < beta) {
                    beta = eval;
                    best = move;
                }
            }
            insertMove(board,best,depth);
            return beta;

        }
}
//Find best move
Move search(Board board,int d, bool v=false) 
{
    Movelist moves;
    movegen::legalmoves(moves,board);

    

    if(board.sideToMove() == Color::WHITE) {
        int  bestEval = -INF;
        Move bestMove;
        for(auto move : moves) {
            if(v) cout << "Evaluating " << uci::moveToSan(board,move) << ": ";
            board.makeMove(move);
            int eval = minimax(board,d,-mateScore,mateScore);
            if(v) cout << eval << " cp" << endl;
            board.unmakeMove(move);
            if(eval > bestEval) {
                bestEval = eval;
                bestMove = move;
            }
        }
        return bestMove;
    } else {
        int  bestEval = INF;
        Move bestMove;
        for(auto move : moves) {
            if(v) cout << "Evaluating " << uci::moveToSan(board,move) << ": ";
            board.makeMove(move);
            int eval = minimax(board,d,-mateScore,mateScore);
            if(v) cout << eval << " cp" << endl;
            board.unmakeMove(move);
            if(eval < bestEval) {
                bestEval = eval;
                bestMove = move;
            }
        }
        return bestMove;
    }
}
//Helper function, iterative search runs faster than normal search because of the transposition table
Move iterativeSearch(Board board) {
    Move bestMove;
    transpositionInsertionLocation = 0;
    Transposition t;
    t.d = -1;
    fill_n(transpositionTable, transpositions, t);
    for(int i = 1; i < 3; i++) {
        bestMove = search(board,i);
    }
    return bestMove;
}
//Board to text
string render(Board board, bool isWhite)
{
    char boardString[8][8];

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {

            Square square = Square(i + j * 8);
            Piece p = board.at(square);
            char s = ' ';
            switch (p)
            {
            case Piece::NONE:
                s = ' ';
                break;
            case Piece::BLACKPAWN:
                s = 'p';
                break;
            case Piece::BLACKKNIGHT:
                s = 'n';
                break;
            case Piece::BLACKBISHOP:
                s = 'b';
                break;
            case Piece::BLACKROOK:
                s = 'r';
                break;
            case Piece::BLACKQUEEN:
                s = 'q';
                break;
            case Piece::BLACKKING:
                s = 'k';
                break;
            case Piece::WHITEPAWN:
                s = 'P';
                break;
            case Piece::WHITEKNIGHT:
                s = 'N';
                break;
            case Piece::WHITEBISHOP:
                s = 'B';
                break;
            case Piece::WHITEROOK:
                s = 'R';
                break;
            case Piece::WHITEQUEEN:
                s = 'Q';
                break;
            case Piece::WHITEKING:
                s = 'K';
                break;
            }
            boardString[i][j] = s;
        }
    }
    if (!isWhite)
    {
        string result = 
        "  H   G   F   E   D   C   B   A   \n"
        "---------------------------------\n";
        for (int i = 0; i < 8; i++)
        {
                result += "|";

            for (int j = 0; j < 8; j++)
            {
                result += " ";
                result += boardString[7 - j][i];
                result += " ";

                result += "|";
            }
            result += " ";
            result += to_string(i+1);
            result += "\n";
            result += "---------------------------------";
            result += "\n";
        }

        result += 
        "  H   G   F   E   D   C   B   A   \n";
        return result;
    }
    else
    {
        string result = 
        "  A   B   C   D   E   F   G   H   \n"
        "---------------------------------\n";
        for (int i = 0; i < 8; i++)
        {
            result += "|";
            for (int j = 0; j < 8; j++)
            {
                result += " ";
                result += boardString[j][7 - i];
                result += " ";
                    result += "|";
            }
            result += " ";
            result += to_string(8-i);
            result += "\n";
            result += "---------------------------------";
            result += "\n";
        }
        result += 
        "  A   B   C   D   E   F   G   H   \n";
        return result;
    }
}

void solvePuzzle(string fen) {
    Board board = Board(fen);
    for(int i = 1; i < 5; i++) {
        Move move = search(board,i,true);
        cout << uci::moveToSan(board,move) << endl;
    }
}

//Engine vs. Engine match arranger
void eve() {
    Board board = Board();
    string pgn = "";
    int moveCount = 1;
    int ms = 1;
    while(true) {
        if((int)board.isGameOver().second != 3) break;
        cout << render(board,true) << "\n";
        

        cout << "Nodes: " << nodes << "\n";
        cout << "Mlliseconds: " << ms/1000.0f << "\n";
        cout << "Nodes / Second: " << nodes/(ms/1000000.0f) << "\n";
        cout << pgn << "\n\n\n";
        nodes = 0;
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        Move move = iterativeSearch(board);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        

        if(board.sideToMove() == Color::WHITE) {
            pgn += to_string(moveCount) + ". ";
            moveCount++;
        }
        pgn += uci::moveToSan(board,move);
        pgn += " ";
        board.makeMove(move);

    }
    cout << "\n\n\n\n" << pgn << "\n";
}
//Player vs. Engine match arranger
void pve() {
    Board board = Board();
    string pgn;
    int moveCount = 1;
    Color human = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() % 2 == 0 ? Color::WHITE : Color::BLACK;
    cout << render(board,human==Color::WHITE) << endl;
    while(true) {
        if((int)board.isGameOver().second != 3) break;
        
        if(board.sideToMove() == Color::WHITE) {
            pgn += to_string(moveCount) + ". ";
            moveCount++;
        }
        if(board.sideToMove() == human) {
            cout << "Give Move: ";
            
            while (true) {
                string move;
                cin >> move;
                pgn += uci::moveToSan(board,uci::parseSan(board,move)) + " ";

                try{
                    board.makeMove(uci::parseSan(board,move));
                    break;
                } catch (exception e) {
                    cout << "That Move Is Illegal, Give proper Move: ";
                }
            }
        } else {
            Move m = iterativeSearch(board);
            pgn += uci::moveToSan(board,m) + " ";
            board.makeMove(m);
        }
        cout << render(board,human==Color::WHITE) << endl;
        cout << pgn << "\n\n\n";
    }
}

int main() {
    cout << "1. Player v.s Engine" << endl;
    cout << "2. Engine v.s Engine" << endl;
    cout << "3. Solve Puzzle" << endl;
    string mode;
    getline(cin,mode);
    if(mode == "1") {
        pve();
    } else if (mode == "2") {
        eve();
    } else if (mode == "3") {
        string fen;
        cout << "Give fen: ";
        getline(cin,fen);
        cout << "\n" << "\"" << fen << "\"" << "\n";

        solvePuzzle(fen);
    }
    return 0;
}
