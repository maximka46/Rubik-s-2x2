// rubik.go
package main

import (
	"bufio"
	"fmt"
	"math/rand"
	"os"
	"strconv"
	"strings"
	"time"
)

const (
	reset  = "\033[0m"
	white  = "\033[97m"
	red    = "\033[91m"
	green  = "\033[92m"
	yellow = "\033[93m"
	blue   = "\033[94m"
	orange = "\033[38;5;208m"
	bold   = "\033[1m"
)

var colorNames = []string{"white", "red", "green", "yellow", "blue", "orange"}

func colorize(text, color string) string {
	return color + text + reset
}

type Cube struct {
	faces     [6][2][2]int
	moves     int
	startTime int64
}

func NewCube() *Cube {
	c := &Cube{}
	c.reset()
	return c
}

func (c *Cube) reset() {
	for f := 0; f < 6; f++ {
		for r := 0; r < 2; r++ {
			for col := 0; col < 2; col++ {
				c.faces[f][r][col] = f
			}
		}
	}
	c.moves = 0
	c.startTime = 0
}

func (c *Cube) isSolved() bool {
	for f := 0; f < 6; f++ {
		first := c.faces[f][0][0]
		for r := 0; r < 2; r++ {
			for col := 0; col < 2; col++ {
				if c.faces[f][r][col] != first {
					return false
				}
			}
		}
	}
	return true
}

func (c *Cube) rotateFace(f int, cw bool) {
	tmp := c.faces[f]
	if cw {
		c.faces[f] = [2][2]int{{tmp[1][0], tmp[0][0]}, {tmp[1][1], tmp[0][1]}}
	} else {
		c.faces[f] = [2][2]int{{tmp[0][1], tmp[1][1]}, {tmp[0][0], tmp[1][0]}}
	}
}

func (c *Cube) applyMove(move string) {
	if move == "" {
		return
	}
	cw := true
	if move[len(move)-1] == '\'' {
		cw = false
		move = move[:len(move)-1]
	}
	faceMap := map[string]int{"F": 0, "R": 1, "U": 2, "B": 3, "L": 4, "D": 5}
	f, ok := faceMap[move]
	if !ok {
		return
	}
	c.rotateFace(f, cw)
	c.updateAdjacent(f, cw)
	c.moves++
	if c.startTime == 0 {
		c.startTime = time.Now().Unix()
	}
}

func shift(src [8]int, cw bool) [8]int {
	var dst [8]int
	if cw {
		for i := 0; i < 8; i++ {
			dst[i] = src[(i+6)%8]
		}
	} else {
		for i := 0; i < 8; i++ {
			dst[i] = src[(i+2)%8]
		}
	}
	return dst
}

func (c *Cube) updateAdjacent(f int, cw bool) {
	var src [8]int
	switch f {
	case 2: // U
		src = [8]int{
			c.faces[0][0][0], c.faces[0][0][1],
			c.faces[1][0][0], c.faces[1][0][1],
			c.faces[3][0][0], c.faces[3][0][1],
			c.faces[4][0][0], c.faces[4][0][1],
		}
		dst := shift(src, cw)
		c.faces[0][0][0], c.faces[0][0][1] = dst[0], dst[1]
		c.faces[1][0][0], c.faces[1][0][1] = dst[2], dst[3]
		c.faces[3][0][0], c.faces[3][0][1] = dst[4], dst[5]
		c.faces[4][0][0], c.faces[4][0][1] = dst[6], dst[7]
	case 5: // D
		src = [8]int{
			c.faces[0][1][0], c.faces[0][1][1],
			c.faces[4][1][0], c.faces[4][1][1],
			c.faces[3][1][0], c.faces[3][1][1],
			c.faces[1][1][0], c.faces[1][1][1],
		}
		dst := shift(src, cw)
		c.faces[0][1][0], c.faces[0][1][1] = dst[0], dst[1]
		c.faces[4][1][0], c.faces[4][1][1] = dst[2], dst[3]
		c.faces[3][1][0], c.faces[3][1][1] = dst[4], dst[5]
		c.faces[1][1][0], c.faces[1][1][1] = dst[6], dst[7]
	case 1: // R
		src = [8]int{
			c.faces[2][0][1], c.faces[2][1][1],
			c.faces[3][0][1], c.faces[3][1][1],
			c.faces[5][0][1], c.faces[5][1][1],
			c.faces[0][0][1], c.faces[0][1][1],
		}
		dst := shift(src, cw)
		c.faces[2][0][1], c.faces[2][1][1] = dst[0], dst[1]
		c.faces[3][0][1], c.faces[3][1][1] = dst[2], dst[3]
		c.faces[5][0][1], c.faces[5][1][1] = dst[4], dst[5]
		c.faces[0][0][1], c.faces[0][1][1] = dst[6], dst[7]
	case 4: // L
		src = [8]int{
			c.faces[2][0][0], c.faces[2][1][0],
			c.faces[0][0][0], c.faces[0][1][0],
			c.faces[5][0][0], c.faces[5][1][0],
			c.faces[3][0][0], c.faces[3][1][0],
		}
		dst := shift(src, cw)
		c.faces[2][0][0], c.faces[2][1][0] = dst[0], dst[1]
		c.faces[0][0][0], c.faces[0][1][0] = dst[2], dst[3]
		c.faces[5][0][0], c.faces[5][1][0] = dst[4], dst[5]
		c.faces[3][0][0], c.faces[3][1][0] = dst[6], dst[7]
	case 0: // F
		src = [8]int{
			c.faces[2][1][0], c.faces[2][1][1],
			c.faces[1][0][0], c.faces[1][1][0],
			c.faces[5][0][0], c.faces[5][0][1],
			c.faces[4][0][1], c.faces[4][1][1],
		}
		dst := shift(src, cw)
		c.faces[2][1][0], c.faces[2][1][1] = dst[0], dst[1]
		c.faces[1][0][0], c.faces[1][1][0] = dst[2], dst[3]
		c.faces[5][0][0], c.faces[5][0][1] = dst[4], dst[5]
		c.faces[4][0][1], c.faces[4][1][1] = dst[6], dst[7]
	case 3: // B
		src = [8]int{
			c.faces[2][0][0], c.faces[2][0][1],
			c.faces[4][0][1], c.faces[4][1][1],
			c.faces[5][1][0], c.faces[5][1][1],
			c.faces[1][0][1], c.faces[1][1][1],
		}
		dst := shift(src, cw)
		c.faces[2][0][0], c.faces[2][0][1] = dst[0], dst[1]
		c.faces[4][0][1], c.faces[4][1][1] = dst[2], dst[3]
		c.faces[5][1][0], c.faces[5][1][1] = dst[4], dst[5]
		c.faces[1][0][1], c.faces[1][1][1] = dst[6], dst[7]
	}
}

func (c *Cube) scramble(n int) {
	moves := []string{"U", "U'", "D", "D'", "R", "R'", "L", "L'", "F", "F'", "B", "B'"}
	rand.Seed(time.Now().UnixNano())
	for i := 0; i < n; i++ {
		c.applyMove(moves[rand.Intn(len(moves))])
	}
	c.moves = 0
	c.startTime = 0
}

func (c *Cube) display() {
	faceStr := func(f int) string {
		var s string
		for r := 0; r < 2; r++ {
			for col := 0; col < 2; col++ {
				s += colorize("■", colorNames[c.faces[f][r][col]]) + " "
			}
			s += "\n"
		}
		return s
	}
	fmt.Println("   U")
	uLines := strings.Split(faceStr(2), "\n")
	for _, line := range uLines[:2] {
		fmt.Println("   " + line)
	}
	fmt.Println("F  R  B  L")
	fLines := strings.Split(faceStr(0), "\n")
	rLines := strings.Split(faceStr(1), "\n")
	bLines := strings.Split(faceStr(3), "\n")
	lLines := strings.Split(faceStr(4), "\n")
	for i := 0; i < 2; i++ {
		fmt.Printf("%s  %s  %s  %s\n", fLines[i], rLines[i], bLines[i], lLines[i])
	}
	fmt.Println("   D")
	dLines := strings.Split(faceStr(5), "\n")
	for _, line := range dLines[:2] {
		fmt.Println("   " + line)
	}
	fmt.Printf("Ходы: %d", c.moves)
	if c.startTime != 0 {
		fmt.Printf(", Время: %d сек", time.Now().Unix()-c.startTime)
	}
	fmt.Println()
}

func (c *Cube) runInteractive() {
	scanner := bufio.NewScanner(os.Stdin)
	fmt.Println("Кубик Рубика 2x2. Команды: U, U', D, D', R, R', L, L', F, F', B, B', scramble, reset, display, solve, quit")
	for {
		fmt.Print("> ")
		if !scanner.Scan() {
			break
		}
		cmd := strings.TrimSpace(scanner.Text())
		if cmd == "quit" {
			break
		} else if cmd == "scramble" {
			c.scramble(20)
			c.display()
		} else if cmd == "reset" {
			c.reset()
			c.display()
		} else if cmd == "display" {
			c.display()
		} else if cmd == "solve" {
			if c.isSolved() {
				fmt.Println("Собран!")
			} else {
				fmt.Println("Не собран.")
			}
		} else {
			c.applyMove(cmd)
			c.display()
		}
	}
}

func main() {
	cube := NewCube()
	if len(os.Args) > 1 {
		for _, arg := range os.Args[1:] {
			cube.applyMove(arg)
		}
		cube.display()
	} else {
		cube.scramble(20)
		cube.display()
		cube.runInteractive()
	}
}
