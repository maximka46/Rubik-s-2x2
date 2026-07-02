// rubik.js
#!/usr/bin/env node
'use strict';

const readline = require('readline');

const COLORS = {
    reset: '\x1b[0m',
    white: '\x1b[97m',
    red: '\x1b[91m',
    green: '\x1b[92m',
    yellow: '\x1b[93m',
    blue: '\x1b[94m',
    orange: '\x1b[38;5;208m',
    bold: '\x1b[1m'
};
const COLOR_NAMES = ['white', 'red', 'green', 'yellow', 'blue', 'orange'];

function colorize(text, color) {
    return COLORS[color] + text + COLORS.reset;
}

class Rubik2x2 {
    constructor() {
        this.reset();
    }

    reset() {
        this.faces = Array.from({length:6}, (_,f) => Array.from({length:2}, () => Array(2).fill(f)));
        this.moves = 0;
        this.startTime = null;
    }

    isSolved() {
        for (let f=0; f<6; f++) {
            const first = this.faces[f][0][0];
            for (let r=0; r<2; r++)
                for (let c=0; c<2; c++)
                    if (this.faces[f][r][c] !== first) return false;
        }
        return true;
    }

    rotateFace(f, cw) {
        const face = this.faces[f];
        if (cw) {
            this.faces[f] = [[face[1][0], face[0][0]], [face[1][1], face[0][1]]];
        } else {
            this.faces[f] = [[face[0][1], face[1][1]], [face[0][0], face[1][0]]];
        }
    }

    applyMove(move) {
        if (!move) return;
        let cw = true;
        if (move.endsWith("'")) {
            cw = false;
            move = move.slice(0, -1);
        }
        const faceMap = {F:0, R:1, U:2, B:3, L:4, D:5};
        const f = faceMap[move];
        if (f === undefined) return;
        this.rotateFace(f, cw);
        this.updateAdjacent(f, cw);
        this.moves++;
        if (this.startTime === null) this.startTime = Date.now();
    }

    updateAdjacent(f, cw) {
        const shift = (arr) => {
            if (cw) {
                return [arr[6], arr[7], arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]];
            } else {
                return [arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[0], arr[1]];
            }
        };
        let src;
        switch (f) {
            case 2: // U
                src = [
                    this.faces[0][0][0], this.faces[0][0][1],
                    this.faces[1][0][0], this.faces[1][0][1],
                    this.faces[3][0][0], this.faces[3][0][1],
                    this.faces[4][0][0], this.faces[4][0][1]
                ];
                break;
            case 5: // D
                src = [
                    this.faces[0][1][0], this.faces[0][1][1],
                    this.faces[4][1][0], this.faces[4][1][1],
                    this.faces[3][1][0], this.faces[3][1][1],
                    this.faces[1][1][0], this.faces[1][1][1]
                ];
                break;
            case 1: // R
                src = [
                    this.faces[2][0][1], this.faces[2][1][1],
                    this.faces[3][0][1], this.faces[3][1][1],
                    this.faces[5][0][1], this.faces[5][1][1],
                    this.faces[0][0][1], this.faces[0][1][1]
                ];
                break;
            case 4: // L
                src = [
                    this.faces[2][0][0], this.faces[2][1][0],
                    this.faces[0][0][0], this.faces[0][1][0],
                    this.faces[5][0][0], this.faces[5][1][0],
                    this.faces[3][0][0], this.faces[3][1][0]
                ];
                break;
            case 0: // F
                src = [
                    this.faces[2][1][0], this.faces[2][1][1],
                    this.faces[1][0][0], this.faces[1][1][0],
                    this.faces[5][0][0], this.faces[5][0][1],
                    this.faces[4][0][1], this.faces[4][1][1]
                ];
                break;
            case 3: // B
                src = [
                    this.faces[2][0][0], this.faces[2][0][1],
                    this.faces[4][0][1], this.faces[4][1][1],
                    this.faces[5][1][0], this.faces[5][1][1],
                    this.faces[1][0][1], this.faces[1][1][1]
                ];
                break;
        }
        const dst = shift(src);
        const assignments = {
            2: [[0,0,0],[0,0,1],[1,0,0],[1,0,1],[3,0,0],[3,0,1],[4,0,0],[4,0,1]],
            5: [[0,1,0],[0,1,1],[4,1,0],[4,1,1],[3,1,0],[3,1,1],[1,1,0],[1,1,1]],
            1: [[2,0,1],[2,1,1],[3,0,1],[3,1,1],[5,0,1],[5,1,1],[0,0,1],[0,1,1]],
            4: [[2,0,0],[2,1,0],[0,0,0],[0,1,0],[5,0,0],[5,1,0],[3,0,0],[3,1,0]],
            0: [[2,1,0],[2,1,1],[1,0,0],[1,1,0],[5,0,0],[5,0,1],[4,0,1],[4,1,1]],
            3: [[2,0,0],[2,0,1],[4,0,1],[4,1,1],[5,1,0],[5,1,1],[1,0,1],[1,1,1]]
        };
        const assign = assignments[f];
        for (let i=0; i<8; i++) {
            const [fr,fc] = assign[i];
            this.faces[fr][fc[0]][fc[1]] = dst[i];
        }
    }

    scramble(n=20) {
        const moves = ['U',"U'",'D',"D'",'R',"R'",'L',"L'",'F',"F'",'B',"B'"];
        for (let i=0; i<n; i++) {
            this.applyMove(moves[Math.floor(Math.random()*moves.length)]);
        }
        this.moves = 0;
        this.startTime = null;
    }

    display() {
        const faceStr = (f) => {
            let s = '';
            for (let r=0; r<2; r++) {
                for (let c=0; c<2; c++) {
                    s += colorize('■', COLOR_NAMES[this.faces[f][r][c]]) + ' ';
                }
                s += '\n';
            }
            return s;
        };
        console.log('   U');
        const uLines = faceStr(2).split('\n');
        for (let i=0; i<2; i++) console.log('   ' + uLines[i]);
        console.log('F  R  B  L');
        const fLines = faceStr(0).split('\n');
        const rLines = faceStr(1).split('\n');
        const bLines = faceStr(3).split('\n');
        const lLines = faceStr(4).split('\n');
        for (let i=0; i<2; i++) {
            console.log(fLines[i] + '  ' + rLines[i] + '  ' + bLines[i] + '  ' + lLines[i]);
        }
        console.log('   D');
        const dLines = faceStr(5).split('\n');
        for (let i=0; i<2; i++) console.log('   ' + dLines[i]);
        console.log(`Ходы: ${this.moves}` + (this.startTime ? `, Время: ${Math.floor((Date.now()-this.startTime)/1000)} сек` : ''));
    }

    runInteractive() {
        const rl = readline.createInterface({
            input: process.stdin,
            output: process.stdout
        });
        console.log('Кубик Рубика 2x2. Команды: U, U\', D, D\', R, R\', L, L\', F, F\', B, B\', scramble, reset, display, solve, quit');
        const prompt = () => {
            rl.question('> ', (cmd) => {
                cmd = cmd.trim();
                if (cmd === 'quit') { rl.close(); return; }
                else if (cmd === 'scramble') { this.scramble(); this.display(); prompt(); }
                else if (cmd === 'reset') { this.reset(); this.display(); prompt(); }
                else if (cmd === 'display') { this.display(); prompt(); }
                else if (cmd === 'solve') { console.log(this.isSolved() ? 'Собран!' : 'Не собран.'); prompt(); }
                else {
                    this.applyMove(cmd);
                    this.display();
                    prompt();
                }
            });
        };
        prompt();
    }
}

function main() {
    const cube = new Rubik2x2();
    const args = process.argv.slice(2);
    if (args.length > 0) {
        for (const m of args) cube.applyMove(m);
        cube.display();
    } else {
        cube.scramble(20);
        cube.display();
        cube.runInteractive();
    }
}

main();
