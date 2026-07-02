// rubik.cs
using System;
using System.Collections.Generic;

class Rubik
{
    static string Colorize(string text, string color)
    {
        string col = color switch
        {
            "white" => "\x1b[97m",
            "red" => "\x1b[91m",
            "green" => "\x1b[92m",
            "yellow" => "\x1b[93m",
            "blue" => "\x1b[94m",
            "orange" => "\x1b[38;5;208m",
            _ => "\x1b[0m"
        };
        return col + text + "\x1b[0m";
    }

    static string[] COLOR_NAMES = {"white","red","green","yellow","blue","orange"};

    class Cube
    {
        public int[,,] faces = new int[6,2,2];
        public int moves;
        public DateTime startTime;

        public Cube()
        {
            Reset();
        }

        public void Reset()
        {
            for (int f=0; f<6; f++)
                for (int r=0; r<2; r++)
                    for (int c=0; c<2; c++)
                        faces[f,r,c] = f;
            moves = 0;
            startTime = DateTime.MinValue;
        }

        public bool IsSolved()
        {
            for (int f=0; f<6; f++)
            {
                int first = faces[f,0,0];
                for (int r=0; r<2; r++)
                    for (int c=0; c<2; c++)
                        if (faces[f,r,c] != first) return false;
            }
            return true;
        }

        public void RotateFace(int f, bool cw)
        {
            int[,] tmp = new int[2,2];
            for (int r=0; r<2; r++)
                for (int c=0; c<2; c++)
                    tmp[r,c] = faces[f,r,c];
            if (cw)
            {
                faces[f,0,0] = tmp[1,0];
                faces[f,0,1] = tmp[0,0];
                faces[f,1,0] = tmp[1,1];
                faces[f,1,1] = tmp[0,1];
            }
            else
            {
                faces[f,0,0] = tmp[0,1];
                faces[f,0,1] = tmp[1,1];
                faces[f,1,0] = tmp[0,0];
                faces[f,1,1] = tmp[1,0];
            }
        }

        public void ApplyMove(string move)
        {
            if (string.IsNullOrEmpty(move)) return;
            bool cw = true;
            if (move.EndsWith("'"))
            {
                cw = false;
                move = move.Substring(0, move.Length-1);
            }
            var faceMap = new Dictionary<string,int> { {"F",0},{"R",1},{"U",2},{"B",3},{"L",4},{"D",5} };
            if (!faceMap.ContainsKey(move)) return;
            int f = faceMap[move];
            RotateFace(f, cw);
            UpdateAdjacent(f, cw);
            moves++;
            if (startTime == DateTime.MinValue) startTime = DateTime.Now;
        }

        void UpdateAdjacent(int f, bool cw)
        {
            int[] Shift(int[] src)
            {
                int[] dst = new int[8];
                if (cw)
                {
                    for (int i=0; i<8; i++) dst[i] = src[(i+6)%8];
                }
                else
                {
                    for (int i=0; i<8; i++) dst[i] = src[(i+2)%8];
                }
                return dst;
            }
            int[] src;
            switch(f)
            {
                case 2: // U
                    src = new int[] {
                        faces[0,0,0], faces[0,0,1],
                        faces[1,0,0], faces[1,0,1],
                        faces[3,0,0], faces[3,0,1],
                        faces[4,0,0], faces[4,0,1]
                    };
                    break;
                case 5: // D
                    src = new int[] {
                        faces[0,1,0], faces[0,1,1],
                        faces[4,1,0], faces[4,1,1],
                        faces[3,1,0], faces[3,1,1],
                        faces[1,1,0], faces[1,1,1]
                    };
                    break;
                case 1: // R
                    src = new int[] {
                        faces[2,0,1], faces[2,1,1],
                        faces[3,0,1], faces[3,1,1],
                        faces[5,0,1], faces[5,1,1],
                        faces[0,0,1], faces[0,1,1]
                    };
                    break;
                case 4: // L
                    src = new int[] {
                        faces[2,0,0], faces[2,1,0],
                        faces[0,0,0], faces[0,1,0],
                        faces[5,0,0], faces[5,1,0],
                        faces[3,0,0], faces[3,1,0]
                    };
                    break;
                case 0: // F
                    src = new int[] {
                        faces[2,1,0], faces[2,1,1],
                        faces[1,0,0], faces[1,1,0],
                        faces[5,0,0], faces[5,0,1],
                        faces[4,0,1], faces[4,1,1]
                    };
                    break;
                case 3: // B
                    src = new int[] {
                        faces[2,0,0], faces[2,0,1],
                        faces[4,0,1], faces[4,1,1],
                        faces[5,1,0], faces[5,1,1],
                        faces[1,0,1], faces[1,1,1]
                    };
                    break;
                default: return;
            }
            int[] dst = Shift(src);
            var assignments = new Dictionary<int, int[][]> {
                {2, new int[][] { new int[]{0,0,0}, new int[]{0,0,1}, new int[]{1,0,0}, new int[]{1,0,1},
                                  new int[]{3,0,0}, new int[]{3,0,1}, new int[]{4,0,0}, new int[]{4,0,1} } },
                {5, new int[][] { new int[]{0,1,0}, new int[]{0,1,1}, new int[]{4,1,0}, new int[]{4,1,1},
                                  new int[]{3,1,0}, new int[]{3,1,1}, new int[]{1,1,0}, new int[]{1,1,1} } },
                {1, new int[][] { new int[]{2,0,1}, new int[]{2,1,1}, new int[]{3,0,1}, new int[]{3,1,1},
                                  new int[]{5,0,1}, new int[]{5,1,1}, new int[]{0,0,1}, new int[]{0,1,1} } },
                {4, new int[][] { new int[]{2,0,0}, new int[]{2,1,0}, new int[]{0,0,0}, new int[]{0,1,0},
                                  new int[]{5,0,0}, new int[]{5,1,0}, new int[]{3,0,0}, new int[]{3,1,0} } },
                {0, new int[][] { new int[]{2,1,0}, new int[]{2,1,1}, new int[]{1,0,0}, new int[]{1,1,0},
                                  new int[]{5,0,0}, new int[]{5,0,1}, new int[]{4,0,1}, new int[]{4,1,1} } },
                {3, new int[][] { new int[]{2,0,0}, new int[]{2,0,1}, new int[]{4,0,1}, new int[]{4,1,1},
                                  new int[]{5,1,0}, new int[]{5,1,1}, new int[]{1,0,1}, new int[]{1,1,1} } }
            };
            var assign = assignments[f];
            for (int i=0; i<8; i++)
            {
                int[] pos = assign[i];
                faces[pos[0], pos[1], pos[2]] = dst[i];
            }
        }

        public void Scramble(int n=20)
        {
            string[] moves = {"U","U'","D","D'","R","R'","L","L'","F","F'","B","B'"};
            Random rnd = new Random();
            for (int i=0; i<n; i++) ApplyMove(moves[rnd.Next(moves.Length)]);
            moves = 0;
            startTime = DateTime.MinValue;
        }

        public void Display()
        {
            string FaceStr(int f)
            {
                string s = "";
                for (int r=0; r<2; r++)
                {
                    for (int c=0; c<2; c++)
                        s += Colorize("■", COLOR_NAMES[faces[f,r,c]]) + " ";
                    s += "\n";
                }
                return s;
            }
            Console.WriteLine("   U");
            var uLines = FaceStr(2).Split('\n', StringSplitOptions.RemoveEmptyEntries);
            foreach (var line in uLines) Console.WriteLine("   " + line);
            Console.WriteLine("F  R  B  L");
            var fLines = FaceStr(0).Split('\n', StringSplitOptions.RemoveEmptyEntries);
            var rLines = FaceStr(1).Split('\n', StringSplitOptions.RemoveEmptyEntries);
            var bLines = FaceStr(3).Split('\n', StringSplitOptions.RemoveEmptyEntries);
            var lLines = FaceStr(4).Split('\n', StringSplitOptions.RemoveEmptyEntries);
            for (int i=0; i<2; i++)
                Console.WriteLine($"{fLines[i]}  {rLines[i]}  {bLines[i]}  {lLines[i]}");
            Console.WriteLine("   D");
            var dLines = FaceStr(5).Split('\n', StringSplitOptions.RemoveEmptyEntries);
            foreach (var line in dLines) Console.WriteLine("   " + line);
            Console.WriteLine($"Ходы: {moves}" + (startTime != DateTime.MinValue ? $", Время: {(int)(DateTime.Now-startTime).TotalSeconds} сек" : ""));
        }

        public void RunInteractive()
        {
            Console.WriteLine("Кубик Рубика 2x2. Команды: U, U', D, D', R, R', L, L', F, F', B, B', scramble, reset, display, solve, quit");
            while (true)
            {
                Console.Write("> ");
                string cmd = Console.ReadLine().Trim();
                if (cmd == "quit") break;
                else if (cmd == "scramble") { Scramble(); Display(); }
                else if (cmd == "reset") { Reset(); Display(); }
                else if (cmd == "display") Display();
                else if (cmd == "solve") Console.WriteLine(IsSolved() ? "Собран!" : "Не собран.");
                else { ApplyMove(cmd); Display(); }
            }
        }
    }

    static void Main(string[] args)
    {
        Cube cube = new Cube();
        if (args.Length > 0)
        {
            foreach (string m in args) cube.ApplyMove(m);
            cube.Display();
        }
        else
        {
            cube.Scramble(20);
            cube.Display();
            cube.RunInteractive();
        }
    }
}
