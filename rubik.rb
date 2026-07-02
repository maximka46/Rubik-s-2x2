#!/usr/bin/env ruby
# rubik.rb
# encoding: UTF-8

COLORS = {
  reset: "\e[0m",
  white: "\e[97m",
  red: "\e[91m",
  green: "\e[92m",
  yellow: "\e[93m",
  blue: "\e[94m",
  orange: "\e[38;5;208m"
}
COLOR_NAMES = ['white','red','green','yellow','blue','orange']

def colorize(text, color)
  "#{COLORS[color]}#{text}#{COLORS[:reset]}"
end

class Rubik2x2
  attr_reader :moves, :start_time

  def initialize
    reset
  end

  def reset
    @faces = (0...6).map { |f| Array.new(2) { Array.new(2, f) } }
    @moves = 0
    @start_time = nil
  end

  def solved?
    @faces.all? { |face| face.flatten.uniq.size == 1 }
  end

  def rotate_face(f, cw)
    tmp = @faces[f]
    if cw
      @faces[f] = [[tmp[1][0], tmp[0][0]], [tmp[1][1], tmp[0][1]]]
    else
      @faces[f] = [[tmp[0][1], tmp[1][1]], [tmp[0][0], tmp[1][0]]]
    end
  end

  def apply_move(move)
    return unless move && !move.empty?
    cw = true
    if move.end_with?("'")
      cw = false
      move = move.chop
    end
    face_map = {'F'=>0,'R'=>1,'U'=>2,'B'=>3,'L'=>4,'D'=>5}
    f = face_map[move]
    return unless f
    rotate_face(f, cw)
    update_adjacent(f, cw)
    @moves += 1
    @start_time ||= Time.now
  end

  def update_adjacent(f, cw)
    shift = ->(arr) {
      if cw
        [arr[6], arr[7], arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]]
      else
        [arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[0], arr[1]]
      end
    }
    src = case f
    when 2 # U
      [@faces[0][0][0], @faces[0][0][1],
       @faces[1][0][0], @faces[1][0][1],
       @faces[3][0][0], @faces[3][0][1],
       @faces[4][0][0], @faces[4][0][1]]
    when 5 # D
      [@faces[0][1][0], @faces[0][1][1],
       @faces[4][1][0], @faces[4][1][1],
       @faces[3][1][0], @faces[3][1][1],
       @faces[1][1][0], @faces[1][1][1]]
    when 1 # R
      [@faces[2][0][1], @faces[2][1][1],
       @faces[3][0][1], @faces[3][1][1],
       @faces[5][0][1], @faces[5][1][1],
       @faces[0][0][1], @faces[0][1][1]]
    when 4 # L
      [@faces[2][0][0], @faces[2][1][0],
       @faces[0][0][0], @faces[0][1][0],
       @faces[5][0][0], @faces[5][1][0],
       @faces[3][0][0], @faces[3][1][0]]
    when 0 # F
      [@faces[2][1][0], @faces[2][1][1],
       @faces[1][0][0], @faces[1][1][0],
       @faces[5][0][0], @faces[5][0][1],
       @faces[4][0][1], @faces[4][1][1]]
    when 3 # B
      [@faces[2][0][0], @faces[2][0][1],
       @faces[4][0][1], @faces[4][1][1],
       @faces[5][1][0], @faces[5][1][1],
       @faces[1][0][1], @faces[1][1][1]]
    end
    dst = shift.call(src)
    assignments = {
      2 => [[0,0,0],[0,0,1],[1,0,0],[1,0,1],[3,0,0],[3,0,1],[4,0,0],[4,0,1]],
      5 => [[0,1,0],[0,1,1],[4,1,0],[4,1,1],[3,1,0],[3,1,1],[1,1,0],[1,1,1]],
      1 => [[2,0,1],[2,1,1],[3,0,1],[3,1,1],[5,0,1],[5,1,1],[0,0,1],[0,1,1]],
      4 => [[2,0,0],[2,1,0],[0,0,0],[0,1,0],[5,0,0],[5,1,0],[3,0,0],[3,1,0]],
      0 => [[2,1,0],[2,1,1],[1,0,0],[1,1,0],[5,0,0],[5,0,1],[4,0,1],[4,1,1]],
      3 => [[2,0,0],[2,0,1],[4,0,1],[4,1,1],[5,1,0],[5,1,1],[1,0,1],[1,1,1]]
    }
    assign = assignments[f]
    8.times do |i|
      fr, fc, fcc = assign[i]
      @faces[fr][fc][fcc] = dst[i]
    end
  end

  def scramble(n=20)
    moves = ['U',"U'",'D',"D'",'R',"R'",'L',"L'",'F',"F'",'B',"B'"]
    n.times { apply_move(moves.sample) }
    @moves = 0
    @start_time = nil
  end

  def display
    face_str = ->(f) {
      s = ""
      2.times do |r|
        2.times do |c|
          s += colorize('■', COLOR_NAMES[@faces[f][r][c]]) + ' '
        end
        s += "\n"
      end
      s
    }
    puts "   U"
    face_str.call(2).each_line { |line| puts "   " + line.chomp }
    puts "F  R  B  L"
    f_lines = face_str.call(0).lines.map(&:chomp)
    r_lines = face_str.call(1).lines.map(&:chomp)
    b_lines = face_str.call(3).lines.map(&:chomp)
    l_lines = face_str.call(4).lines.map(&:chomp)
    2.times do |i|
      puts "#{f_lines[i]}  #{r_lines[i]}  #{b_lines[i]}  #{l_lines[i]}"
    end
    puts "   D"
    face_str.call(5).each_line { |line| puts "   " + line.chomp }
    print "Ходы: #{@moves}"
    print ", Время: #{(Time.now - @start_time).to_i} сек" if @start_time
    puts
  end

  def run_interactive
    puts "Кубик Рубика 2x2. Команды: U, U', D, D', R, R', L, L', F, F', B, B', scramble, reset, display, solve, quit"
    loop do
      print "> "
      cmd = gets.chomp.strip
      case cmd
      when 'quit' then break
      when 'scramble' then scramble; display
      when 'reset' then reset; display
      when 'display' then display
      when 'solve' then puts solved? ? 'Собран!' : 'Не собран.'
      else
        apply_move(cmd)
        display
      end
    end
  end
end

if __FILE__ == $0
  cube = Rubik2x2.new
  if ARGV.size > 0
    ARGV.each { |m| cube.apply_move(m) }
    cube.display
  else
    cube.scramble(20)
    cube.display
    cube.run_interactive
  end
end
