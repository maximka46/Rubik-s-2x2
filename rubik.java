// rubik.java
import java.io.*;
import java.util.*;

public class rubik {
    private static final String RESET = "\u001B[0m";
    private static final String WHITE = "\u001B[97m";
    private static final String RED = "\u001B[91m";
    private static final String GREEN = "\u001B[92m";
    private static final String YELLOW = "\u001B[93m";
    private static final String BLUE = "\u001B[94m";
    private static final String ORANGE = "\u001B[38;5;208m";

    private static String colorize(String text, String color) {
        return color + text + RESET;
    }

    private static String[] COLOR_NAMES = {"white","red","green","yellow","blue","orange"};

    static class Cube {
        int[][][] faces = new int[6][2][2];
        int moves;
        long startTime;

        Cube() { reset(); }

        void reset() {
            for (int f=0; f<6; f++)
                for (int r=0; r<2; r++)
                    for (int c=0; c<2; c++)
                        faces[f][r][c] = f;
            moves = 0;
            startTime = 0;
        }

        boolean isSolved() {
            for (int f=0; f<6; f++) {
                int first = faces[f][0][0];
                for (int r=0; r<2; r++)
                    for (int c=0; c<2; c++)
                        if (faces[f][r][c] != first) return false;
            }
            return true;
        }

        void rotateFace(int f, boolean cw) {
            int[][] tmp = new int[2][2];
            for (int r=0; r<2; r++)
                for (int c=0; c<2; c++)
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

        void applyMove(String move) {
            if (move.isEmpty()) return;
            boolean cw = true;
            if (move.endsWith("'")) {
                cw = false;
                move = move.substring(0, move.length()-1);
            }
            Map<String,Integer> faceMap = new HashMap<>();
            faceMap.put("F",0); faceMap.put("R",1); faceMap.put("U",2);
            faceMap.put("B",3); faceMap.put("L",4); faceMap.put("D",5);
            if (!faceMap.containsKey(move)) return;
            int f = faceMap.get(move);
            rotateFace(f, cw);
            updateAdjacent(f, cw);
            moves++;
            if (startTime == 0) startTime = System.currentTimeMillis();
        }

        private int[] shift(int[] src, boolean cw) {
            int[] dst = new int[8];
            if (cw) {
                for (int i=0; i<8; i++) dst[i] = src[(i+6)%8];
            } else {
                for (int i=0; i<8; i++) dst[i] = src[(i+2)%8];
            }
            return dst;
        }

        void updateAdjacent(int f, boolean cw) {
            int[] src;
            switch (f) {
                case 2: // U
                    src = new int[] {
                        faces[0][0][0], faces[0][0][1],
                        faces[1][0][0], faces[1][0][1],
                        faces[3][0][0], faces[3][0][1],
                        faces[4][0][0], faces[4][0][1]
                    };
                    break;
                case 5: // D
                    src = new int[] {
                        faces[0][1][0], faces[0][1][1],
                        faces[4][1][0], faces[4][1][1],
                        faces[3][1][0], faces[3][1][1],
                        faces[1][1][0], faces[1][1][1]
                    };
                    break;
                case 1: // R
                    src = new int[] {
                        faces[2][0][1], faces[2][1][1],
                        faces[3][0][1], faces[3][1][1],
                        faces[5][0][1], faces[5][1][1],
                        faces[0][0][1], faces[0][1][1]
                    };
                    break;
                case 4: // L
                    src = new int[] {
                        faces[2][0][0], faces[2][1][0],
                        faces[0][0][0], faces[0][1][0],
                        faces[5][0][0], faces[5][1][0],
                        faces[3][0][0], faces[3][1][0]
                    };
                    break;
                case 0: // F
                    src = new int[] {
                        faces[2][1][0], faces[2][1][1],
                        faces[1][0][0], faces[1][1][0],
                        faces[5][0][0], faces[5][0][1],
                        faces[4][0][1], faces[4][1][1]
                    };
                    break;
                case 3: // B
                    src = new int[] {
                        faces[2][0][0], faces[2][0][1],
                        faces[4][0][1], faces[4][1][1],
                        faces[5][1][0], faces[5][1][1],
                        faces[1][0][1], faces[1][1][1]
                    };
                    break;
                default: return;
            }
            int[] dst = shift(src, cw);
            int[][][] assignments = {
                { // 0: F
                    {2,1,0},{2,1,1},{1,0,0},{1,1,0},
                    {5,0,0},{5,0,1},{4,0,1},{4,1,1}
                },
                { // 1: R
                    {2,0,1},{2,1,1},{3,0,1},{3,1,1},
                    {5,0,1},{5,1,1},{0,0,1},{0,1,1}
                },
                { // 2: U
                    {0,0,0},{0,0,1},{1,0,0},{1,0,1},
                    {3,0,0},{3,0,1},{4,0,0},{4,0,1}
                },
                { // 3: B
                    {2,0,0},{2,0,1},{4,0,1},{4,1,1},
                    {5,1,0},{5,1,1},{1,0,1},{1,1,1}
                },
                { // 4: L
                    {2,0,0},{2,1,0},{0,0,0},{0,1,0},
                    {5,0,0},{5,1,0},{3,0,0},{3,1,0}
                },
                { // 5: D
                    {0,1,0},{0,1,1},{4,1,0},{4,1,1},
                    {3,1,0},{3,1,1},{1,1,0},{1,1,1}
                }
            };
            int[][] assign = assignments[f];
            for (int i=0; i<8; i++) {
                int[] pos = assign[i];
                faces[pos[0]][pos[1]][pos[2]] = dst[i];
            }
        }

        void scramble(int n) {
            String[] moves = {"U","U'","D","D'","R","R'","L","L'","F","F'","B","B'"};
            Random rnd = new Random();
            for (int i=0; i<n; i++) applyMove(moves[rnd.nextInt(moves.length)]);
            moves = 0;
            startTime = 0;
        }

        void display() {
            String faceStr(int f) {
                StringBuilder sb = new StringBuilder();
                for (int r=0; r<2; r++) {
                    for (int c=0; c<2; c++) {
                        sb.append(colorize("■", COLOR_NAMES[faces[f][r][c]])).append(" ");
                    }
                    sb.append("\n");
                }
                return sb.toString();
            }
            System.out.println("   U");
            String u = faceStr(2);
            for (String line : u.split("\n")) System.out.println("   " + line);
            System.out.println("F  R  B  L");
            String fStr = faceStr(0), rStr = faceStr(1), bStr = faceStr(3), lStr = faceStr(4);
            String[] fLines = fStr.split("\n"), rLines = rStr.split("\n"), bLines = bStr.split("\n"), lLines = lStr.split("\n");
            for (int i=0; i<2; i++) {
                System.out.println(fLines[i] + "  " + rLines[i] + "  " + bLines[i] + "  " + lLines[i]);
            }
            System.out.println("   D");
            String d = faceStr(5);
            for (String line : d.split("\n")) System.out.println("   " + line);
            System.out.print("Ходы: " + moves);
            if (startTime != 0) System.out.print(", Время: " + (System.currentTimeMillis()-startTime)/1000 + " сек");
            System.out.println();
        }

        void runInteractive() throws IOException {
            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
            System.out.println("Кубик Рубика 2x2. Команды: U, U', D, D', R, R', L, L', F, F', B, B', scramble, reset, display, solve, quit");
            while (true) {
                System.out.print("> ");
                String cmd = reader.readLine().trim();
                if (cmd.equals("quit")) break;
                else if (cmd.equals("scramble")) { scramble(20); display(); }
                else if (cmd.equals("reset")) { reset(); display(); }
                else if (cmd.equals("display")) display();
                else if (cmd.equals("solve")) System.out.println(isSolved() ? "Собран!" : "Не собран.");
                else { applyMove(cmd); display(); }
            }
        }
    }

    public static void main(String[] args) throws IOException {
        Cube cube = new Cube();
        if (args.length > 0) {
            for (String m : args) cube.applyMove(m);
            cube.display();
        } else {
            cube.scramble(20);
            cube.display();
            cube.runInteractive();
        }
    }
}
