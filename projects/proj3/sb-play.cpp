#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <queue>
#include <algorithm>

#include "disjoint.h"

using namespace std;

// Structure to track possible moves and their scores
struct Move {
    enum Type { SCORE, SWAP } type;
    int r1, c1, r2, c2;  // For SWAP: coordinates of both pieces. For SCORE: r1,c1 are goal coordinates
    int value;           // Evaluation score for this move
    char color;          // Color involved in the move
    int setSize;         // Size of the set (for scoring moves)
    
    Move(Type t, int row1, int col1, int row2, int col2, int val, char clr, int size = 0) :
        type(t), r1(row1), c1(col1), r2(row2), c2(col2), value(val), color(clr), setSize(size) {}
    
    // Comparison operator for sorting moves by value
    bool operator<(const Move& other) const {
        return value < other.value;
    }
};

class Superball {
    public:
        Superball(int argc, char **argv);
        int r;
        int c;
        int mss;
        int empty;
        vector<int> board;
        vector<int> goals;
        vector<int> colors;
        int total_cells;
        
        // New helper methods
        bool isGoal(int i, int j) const;
        bool isEmpty(int i, int j) const;
        char getPiece(int i, int j) const;
        void findBestMove();
        int evaluateBoard();
        int evaluateSwap(int r1, int c1, int r2, int c2);
        vector<Move> findScoringMoves(DisjointSetByRankWPC& ds);
        vector<Move> findStrategicSwaps();
        bool willCreateScoringOpportunity(int r1, int c1, int r2, int c2);
        int colorFrequency(char color) const;
};

void usage(const char *s)
{
    fprintf(stderr, "usage: sb-analyze rows cols min-score-size colors\n");
    if (s != NULL) fprintf(stderr, "%s\n", s);
    exit(1);
}

Superball::Superball(int argc, char **argv)
{
    int i, j;
    string s;

    if (argc != 5) usage(NULL);

    if (sscanf(argv[1], "%d", &r) == 0 || r <= 0) usage("Bad rows");
    if (sscanf(argv[2], "%d", &c) == 0 || c <= 0) usage("Bad cols");
    if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0) usage("Bad min-score-size");

    colors.resize(256, 0);

    for (i = 0; i < (int) strlen(argv[4]); i++) {
        if (!isalpha(argv[4][i])) usage("Colors must be distinct letters");
        if (!islower(argv[4][i])) usage("Colors must be lowercase letters");
        if (colors[argv[4][i]] != 0) usage("Duplicate color");
        colors[argv[4][i]] = 2+i;
        colors[toupper(argv[4][i])] = 2+i;
    }

    board.resize(r*c);
    goals.resize(r*c, 0);

    empty = 0;

    for (i = 0; i < r; i++) {
        if (!(cin >> s)) {
            fprintf(stderr, "Bad board: not enough rows on standard input\n");
            exit(1);
        }
        if ((int) s.size() != c) {
            fprintf(stderr, "Bad board on row %d - wrong number of characters.\n", i);
            exit(1);
        }
        for (j = 0; j < c; j++) {
            if (s[j] != '*' && s[j] != '.' && colors[s[j]] == 0) {
                fprintf(stderr, "Bad board row %d - bad character %c.\n", i, s[j]);
                exit(1);
            }
            board[i*c+j] = s[j];
            if (board[i*c+j] == '.') empty++;
            if (board[i*c+j] == '*') empty++;
            if (isupper(board[i*c+j]) || board[i*c+j] == '*') {
                goals[i*c+j] = 1;
                board[i*c+j] = tolower(board[i*c+j]);
            }
        }
    }
    
    total_cells = r * c;
}

// Helper method to check if a position is a goal
bool Superball::isGoal(int i, int j) const {
    if (i < 0 || i >= r || j < 0 || j >= c) return false;
    return goals[i*c+j] == 1;
}

// Helper method to check if a position is empty
bool Superball::isEmpty(int i, int j) const {
    if (i < 0 || i >= r || j < 0 || j >= c) return true;
    return board[i*c+j] == '.' || board[i*c+j] == '*';
}

// Helper method to get the piece at a position
char Superball::getPiece(int i, int j) const {
    if (i < 0 || i >= r || j < 0 || j >= c) return '.';
    return board[i*c+j];
}

// Count frequency of a color on the board
int Superball::colorFrequency(char color) const {
    int count = 0;
    for (int i = 0; i < total_cells; i++) {
        if (board[i] == color) count++;
    }
    return count;
}

// Find all possible scoring moves
vector<Move> Superball::findScoringMoves(DisjointSetByRankWPC& ds) {
    vector<Move> scoringMoves;
    unordered_map<int, int> setSizes;
    unordered_map<int, char> setColors;
    unordered_map<int, bool> setHasGoal;
    set<int> processed;
    
    // Calculate set sizes and track if they contain goal cells
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            int index = i * c + j;
            if (isEmpty(i, j)) continue;
            
            int leader = ds.Find(index);
            setSizes[leader]++;
            setColors[leader] = board[index];
            if (isGoal(i, j)) {
                setHasGoal[leader] = true;
            }
        }
    }
    
    // Find scoring moves
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            int index = i * c + j;
            if (isEmpty(i, j)) continue;
            
            int leader = ds.Find(index);
            if (processed.find(leader) != processed.end()) continue;
            processed.insert(leader);
            
            if (setSizes[leader] >= mss && setHasGoal[leader]) {
                // Find the goal cell in this set
                for (int gi = 0; gi < r; gi++) {
                    for (int gj = 0; gj < c; gj++) {
                        int goalIndex = gi * c + gj;
                        if (isGoal(gi, gj) && !isEmpty(gi, gj) && ds.Find(goalIndex) == leader) {
                            // Calculate value: base on set size and rarity of color
                            int colorValue = 10 - min(9, colorFrequency(board[goalIndex]));
                            int value = setSizes[leader] * 100 + colorValue;
                            
                            // Add bonus for large percentage of remaining pieces of this color
                            double percentOfColor = (double)setSizes[leader] / colorFrequency(board[goalIndex]);
                            if (percentOfColor > 0.5) value += 50;
                            
                            // Add bonus for emptier board (end-game strategy)
                            if ((double)empty / total_cells > 0.6) value += 30;
                            
                            scoringMoves.push_back(Move(Move::SCORE, gi, gj, -1, -1, 
                                                  value, board[goalIndex], setSizes[leader]));
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return scoringMoves;
}

// Check if a swap will create a scoring opportunity
bool Superball::willCreateScoringOpportunity(int r1, int c1, int r2, int c2) {
    // Temporarily swap pieces
    char temp = board[r1*c+c1];
    board[r1*c+c1] = board[r2*c+c2];
    board[r2*c+c2] = temp;
    
    // Check if this creates a scoring opportunity
    DisjointSetByRankWPC tempDs(r * c);
    
    // Create disjoint sets with the temporary board
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            int index = i * c + j;
            if (isEmpty(i, j)) continue;
            
            // Check right
            if (j + 1 < c && board[index] == board[i*c+(j+1)]) {
                tempDs.Union(tempDs.Find(index), tempDs.Find(i*c+(j+1)));
            }
            
            // Check down
            if (i + 1 < r && board[index] == board[(i+1)*c+j]) {
                tempDs.Union(tempDs.Find(index), tempDs.Find((i+1)*c+j));
            }
        }
    }
    
    // Check for scorable sets
    bool foundScoring = false;
    unordered_map<int, int> setSizes;
    unordered_map<int, bool> setHasGoal;
    
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            int index = i * c + j;
            if (isEmpty(i, j)) continue;
            
            int leader = tempDs.Find(index);
            setSizes[leader]++;
            if (isGoal(i, j)) {
                setHasGoal[leader] = true;
            }
        }
    }
    
    for (const auto& entry : setSizes) {
        if (entry.second >= mss && setHasGoal[entry.first]) {
            foundScoring = true;
            break;
        }
    }
    
    // Restore original board
    board[r2*c+c2] = board[r1*c+c1];
    board[r1*c+c1] = temp;
    
    return foundScoring;
}

// Calculate a strategic value for a swap
int Superball::evaluateSwap(int r1, int c1, int r2, int c2) {
    int value = 0;
    
    // Skip invalid swaps
    if (isEmpty(r1, c1) || isEmpty(r2, c2)) return -1000;
    if (board[r1*c+c1] == board[r2*c+c2]) return -900;  // Same color, less useful
    
    // Base value: will it create a scoring opportunity?
    if (willCreateScoringOpportunity(r1, c1, r2, c2)) {
        value += 500;
    }
    
    // Bonus for swaps near goals
    if (isGoal(r1, c1) || isGoal(r2, c2)) {
        value += 50;
    }
    
    // Check for potential connections after swap
    int potentialConnections = 0;
    
    // Check r1,c1 with new piece (from r2,c2)
    char piece2 = board[r2*c+c2];
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            if (dr != 0 && dc != 0) continue;  // Only check orthogonal neighbors
            
            int nr = r1 + dr;
            int nc = c1 + dc;
            if (nr < 0 || nr >= r || nc < 0 || nc >= c) continue;
            if (board[nr*c+nc] == piece2) {
                potentialConnections++;
            }
        }
    }
    
    // Check r2,c2 with new piece (from r1,c1)
    char piece1 = board[r1*c+c1];
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            if (dr != 0 && dc != 0) continue;  // Only check orthogonal neighbors
            
            int nr = r2 + dr;
            int nc = c2 + dc;
            if (nr < 0 || nr >= r || nc < 0 || nc >= c) continue;
            if (board[nr*c+nc] == piece1) {
                potentialConnections++;
            }
        }
    }
    
    value += potentialConnections * 30;
    
    // Pieces near center might be more valuable
    int centerR = r / 2;
    int centerC = c / 2;
    int r1Distance = abs(r1 - centerR) + abs(c1 - centerC);
    int r2Distance = abs(r2 - centerR) + abs(c2 - centerC);
    value -= (r1Distance + r2Distance);  // Small penalty for distance from center
    
    return value;
}

// Find strategic swaps
vector<Move> Superball::findStrategicSwaps() {
    vector<Move> swapMoves;
    
    // Consider all possible swaps
    for (int r1 = 0; r1 < r; r1++) {
        for (int c1 = 0; c1 < c; c1++) {
            if (isEmpty(r1, c1)) continue;
            
            // Try adjacent swaps first (more natural)
            const int dr[] = {0, 1, 0, -1};  // right, down, left, up
            const int dc[] = {1, 0, -1, 0};
            
            for (int d = 0; d < 4; d++) {
                int r2 = r1 + dr[d];
                int c2 = c1 + dc[d];
                
                if (r2 < 0 || r2 >= r || c2 < 0 || c2 >= c) continue;
                if (isEmpty(r2, c2)) continue;
                
                int value = evaluateSwap(r1, c1, r2, c2);
                if (value > 0) {
                    swapMoves.push_back(Move(Move::SWAP, r1, c1, r2, c2, value, board[r1*c+c1]));
                }
            }
        }
    }
    
    // If no good adjacent swaps, try all combinations
    if (swapMoves.empty()) {
        for (int r1 = 0; r1 < r; r1++) {
            for (int c1 = 0; c1 < c; c1++) {
                if (isEmpty(r1, c1)) continue;
                
                for (int r2 = 0; r2 < r; r2++) {
                    for (int c2 = 0; c2 < c; c2++) {
                        if (r1 == r2 && c1 == c2) continue;
                        if (isEmpty(r2, c2)) continue;
                        
                        int value = evaluateSwap(r1, c1, r2, c2);
                        if (value > -500) {  // Lower threshold for non-adjacent swaps
                            swapMoves.push_back(Move(Move::SWAP, r1, c1, r2, c2, value, board[r1*c+c1]));
                        }
                    }
                }
            }
        }
    }
    
    // If still no moves, just use the first valid swap we can find
    if (swapMoves.empty()) {
        for (int r1 = 0; r1 < r; r1++) {
            for (int c1 = 0; c1 < c; c1++) {
                if (isEmpty(r1, c1)) continue;
                
                for (int r2 = r1; r2 < r; r2++) {
                    int startC = (r1 == r2) ? c1 + 1 : 0;
                    for (int c2 = startC; c2 < c; c2++) {
                        if (isEmpty(r2, c2)) continue;
                        
                        swapMoves.push_back(Move(Move::SWAP, r1, c1, r2, c2, 0, board[r1*c+c1]));
                        return swapMoves;  // Just return the first one
                    }
                }
            }
        }
    }
    
    return swapMoves;
}

// Find the best move to make
void Superball::findBestMove() {
    DisjointSetByRankWPC ds(r * c);
    
    // Create disjoint sets
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            int index = i * c + j;
            if (isEmpty(i, j)) continue;
            
            // Check right
            if (j + 1 < c && board[index] == board[i*c+(j+1)]) {
                ds.Union(ds.Find(index), ds.Find(i*c+(j+1)));
            }
            
            // Check down
            if (i + 1 < r && board[index] == board[(i+1)*c+j]) {
                ds.Union(ds.Find(index), ds.Find((i+1)*c+j));
            }
        }
    }
    
    // Find all possible scoring moves
    vector<Move> scoringMoves = findScoringMoves(ds);
    
    // If we have scorable moves, take the best one
    if (!scoringMoves.empty()) {
        // Sort by value (highest first)
        sort(scoringMoves.begin(), scoringMoves.end(), 
             [](const Move& a, const Move& b) { return a.value > b.value; });
        
        Move bestScore = scoringMoves[0];
        cout << "SCORE " << bestScore.r1 << " " << bestScore.c1 << endl;
        return;
    }
    
    // No scoring moves, look for strategic swaps
    vector<Move> swapMoves = findStrategicSwaps();
    
    if (!swapMoves.empty()) {
        // Sort by value (highest first)
        sort(swapMoves.begin(), swapMoves.end(), 
             [](const Move& a, const Move& b) { return a.value > b.value; });
        
        Move bestSwap = swapMoves[0];
        cout << "SWAP " << bestSwap.r1 << " " << bestSwap.c1 << " " 
             << bestSwap.r2 << " " << bestSwap.c2 << endl;
        return;
    }
    
    // If we somehow get here with no moves, just swap the first two valid pieces we find
    for (int r1 = 0; r1 < r; r1++) {
        for (int c1 = 0; c1 < c; c1++) {
            if (isEmpty(r1, c1)) continue;
            
            for (int r2 = 0; r2 < r; r2++) {
                for (int c2 = 0; c2 < c; c2++) {
                    if (r1 == r2 && c1 == c2) continue;
                    if (isEmpty(r2, c2)) continue;
                    
                    cout << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2 << endl;
                    return;
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    Superball *s = new Superball(argc, argv);
    s->findBestMove();
    delete s;
    return 0;
}
