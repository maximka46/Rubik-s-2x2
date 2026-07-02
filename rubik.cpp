// rubik.cpp
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <map>

using namespace std;

const string RESET = "\033[0m";
const string WHITE = "\033[97m";
const string RED = "\033[91m";
const string GREEN = "\033[92m";
const string YELLOW = "\033[93m";
const string BLUE = "\033[94m";
const string ORANGE = "\033[38;5;208m";
const string BOLD = "\033[1m";

string colorize(const string& text, const string& color) {
    return color + text + RESET;
}

vector<string> COLOR_NAMES = {"white", "red", "green", "yellow", "blue", "orange"};

class Rubik2x2 {
public:
    int faces[6][2][2];
    int moves;
    time_t startTime;

    Rubik2x2() {
        reset();
    }

    void reset() {
        for (int f=0; f<6; ++f)
            for (int r=0; r<2; ++r)
                for (int c=0; c<2; ++c)
                    faces[f][r][c] = f;
        moves = 0;
        startTime = 0;
    }

    bool isSolved() {
        for (int f=0; f<6; ++f) {
            int first = faces[f][0][0];
            for (int r=0; r<2; ++r)
                for (int c=0; c<2; ++c)
                    if (faces[f][r][c] != first) return false;
        }
        return true;
    }

    void rotateFace(int f, bool cw) {
        int tmp[2][2];
        for (int r=0; r<2; ++r)
            for (int c=0; c<2; ++c)
                tmp[r][c] = faces[f][r][c];
        if (cw) {
            faces[f][0][0] = tmp[1][0];
            faces[f][0][1] = tmp[0][0];
            faces[f][1][0] = tmp[1][1];
            faces[f][1][1] = tmp[0][1];
        } else {
            faces[f][0][0] = tmp[0][1];
            faces[f][0][1] = tmp[1][1];
            faces[f][1][0] = tmp[0][0];
            faces[f][1][1] = tmp[1][0];
        }
    }

    void applyMove(const string& move) {
        bool cw = true;
        string m = move;
        if (m.back() == '\'') { cw = false; m.pop_back(); }
        map<string,int> faceMap = {{"F",0},{"R",1},{"U",2},{"B",3},{"L",4},{"D",5}};
        if (faceMap.find(m) == faceMap.end()) return;
        int f = faceMap[m];
        rotateFace(f, cw);
        updateAdjacent(f, cw);
        moves++;
        if (startTime == 0) startTime = time(nullptr);
    }

    void updateAdjacent(int f, bool cw) {
        // Аналогично Python, но вручную для каждой грани
        int vals[8];
        // Функция, которая забирает 8 значений и записывает обратно со сдвигом
        auto shift = [&](int src[8], int dst[8], bool cw) {
            if (cw) {
                for (int i=0; i<8; ++i) dst[i] = src[(i+6)%8];
            } else {
                for (int i=0; i<8; ++i) dst[i] = src[(i+2)%8];
            }
        };
        // Для U
        if (f == 2) {
            int src[8] = {
                faces[0][0][0], faces[0][0][1],
                faces[1][0][0], faces[1][0][1],
                faces[3][0][0], faces[3][0][1],
                faces[4][0][0], faces[4][0][1]
            };
            int dst[8];
            shift(src, dst, cw);
            faces[0][0][0]=dst[0]; faces[0][0][1]=dst[1];
            faces[1][0][0]=dst[2]; faces[1][0][1]=dst[3];
            faces[3][0][0]=dst[4]; faces[3][0][1]=dst[5];
            faces[4][0][0]=dst[6]; faces[4][0][1]=dst[7];
        } else if (f == 5) { // D
            int src[8] = {
                faces[0][1][0], faces[0][1][1],
                faces[4][1][0], faces[4][1][1],
                faces[3][1][0], faces[3][1][1],
                faces[1][1][0], faces[1][1][1]
            };
            int dst[8];
            shift(src, dst, cw);
            faces[0][1][0]=dst[0]; faces[0][1][1]=dst[1];
            faces[4][1][0]=dst[2]; faces[4][1][1]=dst[3];
            faces[3][1][0]=dst[4]; faces[3][1][1]=dst[5];
            faces[1][1][0]=dst[6]; faces[1][1][1]=dst[7];
        } else if (f == 1) { // R
            int src[8] = {
                faces[2][0][1], faces[2][1][1],
                faces[3][0][1], faces[3][1][1],
                faces[5][0][1], faces[5][1][1],
                faces[0][0][1], faces[0][1][1]
            };
            int dst[8];
            shift(src, dst, cw);
            faces[2][0][1]=dst[0]; faces[2][1][1]=dst[1];
            faces[3][0][1]=dst[2]; faces[3][1][1]=dst[3];
            faces[5][0][1]=dst[4]; faces[5][1][1]=dst[5];
            faces[0][0][1]=dst[6]; faces[0][1][1]=dst[7];
        } else if (f == 4) { // L
            int src[8] = {
                faces[2][0][0], faces[2][1][0],
                faces[0][0][0], faces[0][1][0],
                faces[5][0][0], faces[5][1][0],
                faces[3][0][0], faces[3][1][0]
            };
            int dst[8];
            shift(src, dst, cw);
            faces[2][0][0]=dst[0]; faces[2][1][0]=dst[1];
            faces[0][0][0]=dst[2]; faces[0][1][0]=dst[3];
            faces[5][0][0]=dst[4]; faces[5][1][0]=dst[5];
            faces[3][0][0]=dst[6]; faces[3][1][0]=dst[7];
        } else if (f == 0) { // F
            int src[8] = {
                faces[2][1][0], faces[2][1][1],
                faces[1][0][0], faces[1][1][0],
                faces[5][0][0], faces[5][0][1],
                faces[4][0][1], faces[4][1][1]
            };
            int dst[8];
            shift(src, dst, cw);
            faces[2][1][0]=dst[0]; faces[2][1][1]=dst[1];
            faces[1][0][0]=dst[2]; faces[1][1][0]=dst[3];
            faces[5][0][0]=dst[4]; faces[5][0][1]=dst[5];
            faces[4][0][1]=dst[6]; faces[4][1][1]=dst[7];
        } else if (f == 3) { // B
            int src[8] = {
                faces[2][0][0], faces[2][0][1],
                faces[4][0][1], faces[4][1][1],
                faces[5][1][0], faces[5][1][1],
                faces[1][0][1], faces[1][1][1]
            };
            int dst[8];
            shift(src, dst, cw);
            faces[2][0][0]=dst[0]; faces[2][0][1]=dst[1];
            faces[4][0][1]=dst[2]; faces[4][1][1]=dst[3];
            faces[5][1][0]=dst[4]; faces[5][1][1]=dst[5];
            faces[1][0][1]=dst[6]; faces[1][1][1]=dst[7];
        }
    }

    void scramble(int n=20) {
        string moves[] = {"U","U'","D","D'","R","R'","L","L'","F","F'","B","B'"};
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, 11);
        for (int i=0; i<n; ++i) {
            applyMove(moves[dis(gen)]);
        }
        moves = 0;
        startTime = 0;
    }

    void display() {
        auto faceStr = [&](int f) {
            string s;
            for (int r=0; r<2; ++r) {
                for (int c=0; c<2; ++c) {
                    int col = faces[f][r][c];
                    string color = COLOR_NAMES[col];
                    s += colorize("■", color) + " ";
                }
                s += "\n";
            }
            return s;
        };
        cout << "   U\n";
        string u = faceStr(2);
        for (string line; getline(stringstream(u), line); ) cout << "   " << line << endl;
        cout << "F  R  B  L\n";
        string fStr = faceStr(0), rStr = faceStr(1), bStr = faceStr(3), lStr = faceStr(4);
        vector<string> fLines, rLines, bLines, lLines;
        stringstream ssF(fStr), ssR(rStr), ssB(bStr), ssL(lStr);
        string line;
        while (getline(ssF, line)) fLines.push_back(line);
        while (getline(ssR, line)) rLines.push_back(line);
        while (getline(ssB, line)) bLines.push_back(line);
        while (getline(ssL, line)) lLines.push_back(line);
        for (int i=0; i<2; ++i) {
            cout << fLines[i] << "  " << rLines[i] << "  " << bLines[i] << "  " << lLines[i] << endl;
        }
        cout << "   D\n";
        string d = faceStr(5);
        for (string line; getline(stringstream(d), line); ) cout << "   " << line << endl;
        cout << "Ходы: " << moves;
        if (startTime) cout << ", Время: " << (int)(time(nullptr)-startTime) << " сек";
        cout << endl;
    }

    void runInteractive() {
        string cmd;
        cout << "Кубик Рубика 2x2. Команды: U, U', D, D', R, R', L, L', F, F', B, B', scramble, reset, display, solve, quit" << endl;
        while (true) {
            cout << "> ";
            getline(cin, cmd);
            if (cmd == "quit") break;
            else if (cmd == "scramble") { scramble(); display(); }
            else if (cmd == "reset") { reset(); display(); }
            else if (cmd == "display") display();
            else if (cmd == "solve") cout << (isSolved() ? "Собран!" : "Не собран.") << endl;
            else {
                applyMove(cmd);
                display();
            }
        }
    }
};

int main(int argc, char* argv[]) {
    Rubik2x2 cube;
    if (argc > 1) {
        for (int i=1; i<argc; ++i) {
            cube.applyMove(argv[i]);
        }
        cube.display();
    } else {
        cube.scramble();
        cube.display();
        cube.runInteractive();
    }
    return 0;
}
