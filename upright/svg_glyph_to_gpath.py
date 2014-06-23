import re
import math

## DIN Condensed in SVG path form
DIGIT_X = 372

DIGIT_0 = """
M34 565q0 36 12.5 64t33.5 48t48.5 30.5t57.5 10.5t57.5 -10.5t48.5 -30.5t33.5 -48
t12.5 -64v-418q0 -36 -12.5 -64t-33.5 -48t-48.5 -30.5t-57.5 -10.5t-57.5 10.5
t-48.5 30.5t-33.5 48t-12.5 64v418zM136 147q0 -22 14 -36.5t36 -14.5t36 14.5
t14 36.5v418q0 22 -14 36.5t-36 14.5t-36 -14.5t-14 -36.5v-418z"""
DIGIT_1 = """M155 604l-102 -75v108l102 75h102v-712h-102v604z"""
DIGIT_2 = """
M34 96l184 347q14 26 16 45.5t2 48.5q0 13 -1 27.5t-5.5 25.5t-14.5 18.5
t-29 7.5q-23 0 -36.5 -13t-13.5 -38v-58h-102v56q0 32 12 60t32.5 49
t48.5 33.5t60 12.5q40 0 68.5 -14.5t47 -39.5t27 -57t8.5 -68q0 -26 -1 -43.5
t-4 -33.5t-10 -32t-19 -39l-150 -289h184v-102h-304v96z"""
DIGIT_3 = """
M155 412q46 0 63.5 11t17.5 51v92q0 22 -13.5 36t-36.5 14
q-27 0 -38.5 -17t-11.5 -33v-58h-102v59q0 31 12 59t33 48t49 32t60 12
q42 0 69.5 -16.5t41.5 -33.5q10 -12 17.5 -24.5t12 -29t7 -40t2.5 -57.5
q0 -37 -1.5 -60t-8 -38.5t-19 -26.5t-34.5 -24q24 -15 36.5 -28
t18.5 -30.5t7 -42t1 -62.5q0 -35 -1.5 -58t-4.5 -38.5t-8 -26.5
t-13 -23q-19 -28 -49 -46.5t-77 -18.5q-24 0 -51 8t-49 26t-36.5 47
t-14.5 71v58h102v-53q0 -24 13.5 -39.5t36.5 -15.5t36.5 15.5t13.5 41.5
v102q0 21 -3.5 34t-12.5 20.5t-24.5 10t-40.5 2.5v90z"""
DIGIT_4 = """
M212 106h-203v96l147 510h108l-153 -510h101v203h102v-203h49v-96h-49v-106h-102v106z"""
DIGIT_5 = """
M338 616h-202v-192q14 14 36 23.5t49 9.5q52 0 84.5 -32t32.5 -94
v-184q0 -36 -12.5 -64t-33.5 -48t-48.5 -30.5t-57.5 -10.5t-57.5 10.5
t-48.5 30.5t-33.5 48t-12.5 64v30h102v-26q0 -26 14.5 -40.5t37.5 -14.5
t35.5 14t12.5 39v167q0 21 -13.5 36t-34.5 15q-13 0 -22 -4.5t-15 -10.5
t-9.5 -13l-5.5 -11h-90v384h292v-96z"""
DIGIT_6 = """
M185 413l2 -2q5 4 15.5 6.5t27.5 2.5q27 0 50 -13t36 -33q7 -11 11 -22
t6.5 -29t3.5 -45.5t1 -69.5q0 -35 -1 -57.5t-3.5 -38t-7 -26.5t-11.5 -23
q-20 -33 -53.5 -51t-75.5 -18t-75 18.5t-53 50.5q-8 12 -12.5 23t-7 26.5
t-3.5 38t-1 57.5q0 33 1 54.5t3 37t6 28t9 26.5l134 358h114zM236 279
q0 23 -15 37t-35 14t-35 -14t-15 -37v-132q0 -23 15 -37t35 -14t35 14
t15 37v132z"""
DIGIT_7 = """
M236 616h-112v-82h-90v178h304v-102l-173 -610h-108z"""
DIGIT_8 = """
M236 566q0 21 -15 35.5t-35 14.5t-35 -14.5t-15 -35.5v-104q0 -21 15 -35.5
t35 -14.5t35 14.5t15 35.5v104zM34 526q0 29 2 49.5t6.5 35.5t11.5 27t17 25
q20 26 50 40.5t65 14.5t65 -14.5t50 -40.5q10 -13 17 -25t11.5 -27t6.5 -35.5
t2 -49.5q0 -32 -1 -54t-6 -39t-15.5 -30.5t-28.5 -28.5q18 -14 28.5 -28
t15.5 -33t6 -46.5t1 -67.5q0 -33 -1.5 -54.5t-4.5 -37.5t-8.5 -27t-13.5 -23
q-17 -26 -48 -44.5t-76 -18.5t-76 18.5t-48 44.5q-8 12 -13.5 23t-8.5 27
t-4.5 37.5t-1.5 54.5q0 40 1 67.5t6 46.5t15.5 33t28.5 28q-18 15 -28.5 28.5
t-15.5 30.5t-6 39t-1 54zM236 286q0 21 -15 35.5t-35 14.5t-35 -14.5t-15 -35.5
v-140q0 -21 15 -35.5t35 -14.5t35 14.5t15 35.5v140z"""
DIGIT_9 = """
M187 299l-2 2q-5 -4 -15.5 -6.5t-27.5 -2.5q-27 0 -50 13t-36 33
q-7 10 -11 21.5t-6.5 29.5t-3.5 45.5t-1 69.5q0 35 1 57.5t3.5 38t7 26.5
t11.5 23q20 33 53.5 51t75.5 18t75 -18.5t53 -50.5q7 -12 12 -23t7.5 -26.5
t3.5 -38t1 -57.5q0 -33 -1 -54.5t-3.5 -37t-6 -28t-8.5 -26.5l-134 -358h-114
zM136 433q0 -23 15 -37t35 -14t35 14t15 37v132q0 23 -15 37t-35 14t-35 -14
t-15 -37v-132z"""

RE_PATH_COMMAND = re.compile(r'^([A-Za-z])([\d\.\-\s]+)')
RE_PATH_END = re.compile(r'^z')

class Point:
  def __init__(self, x, y):
    self.x = float(x)
    self.y = float(y)

  def floor(self):
    return Point(math.floor(self.x), math.floor(self.y))

  def is_integral_equal(self, other):
    return (int(self.x) == int(other.x) and int(self.y) == int(other.y))

  def __str__(self):
    return '{%f.1, %f.1}' % (self.x, self.y)

  def __eq__(self, other):
    return self.x - other.x < 0.1 and self.y - other.y < 0.1

  def __ne__(self, other):
    return self.x - other.x > 0.1 and self.y - other.y > 0.1


def get_next_path_command(path):
  has_command = RE_PATH_COMMAND.search(path)
  if has_command:
    command = has_command.group(1)
    params = has_command.group(2).split()
    return path[len(has_command.group(0)):], command, params

  has_path_end = RE_PATH_END.search(path)
  if has_path_end:
    return path[1:], 'z', []

  return '', None, None

def get_path_commands(path):
  commands = []
  remaining_path = path.strip()
  while True:
    remaining_path, command, params = get_next_path_command(remaining_path)
    commands.append((command, params))
    if len(remaining_path) < 1:
      break
  return commands

#
# Drawing Quad Beziers from:
# http://members.chello.at/easyfilter/bresenham.pdf
# http://rosettacode.org/wiki/Bitmap/B%C3%A9zier_curves/Quadratic

def points_for_quad_bezier_segment(x0, y0, x1, y1, x2, y2):
  points = []

  sx = int(x2-x1)
  sy = int(y2-y1)

  xx = int(x0-x1)
  yy = int(y0-y1)
  cur = float(xx * sy - yy * sx)  # curvature

  if sx * int(sx) + sy * int(sy) > xx * xx + yy * yy:
    x2 = x0
    x0 = sx+x1
    y2 = y0
    y0 = sy+y1
    cur = -cur;

  if cur != 0: # no straight line
    xx += sx
    if x0 < x2:
      sx = 1
    else:
      sx = -1
      xx *= -1

    yy += sy
    if y0 < y2:
      sy = 1
    else:
      sy = -1
      yy *= -1

    xy = 2*xx*yy
    xx *= xx
    yy *= yy;

    if cur * sx * sy < 0:
      xx = -xx
      yy = -yy
      xy = -xy
      cur = -cur

    dx = 4.0 * sy * cur * (x1-x0) + xx - xy
    dy = 4.0 * sx * cur * (y0-y1) + yy - xy
    xx += xx
    yy += yy
    err = dx + dy + xy

    while True:
      points.append(Point(x0, y0))
      if x0 == x2 and y0 == y2:
        return points # end
      y1 = 2 * err < dx
      if 2 * err > dy:
        x0 += sx
        dx -= xy
        dy += yy
        err += dy

      if y1:
        y0 += sy
        dy -= xy
        dx += xx
        err += dx

      if dy < dx:
        break

    points.append(Point(x0, y0))
    points.append(Point(x2, y2))
    return points


def points_for_quad_bezier(start, control, end):
  points = []
  x0, y0 = start.x, start.y
  x1, y1 = control.x, control.y
  x2, y2 = end.x, end.y

  x = x0 - x1
  y = y0 - y1
  t = x0 - 2 * x1 + x2
  r = 0

  if x * (x2 - x1) > 0:
    if y * (y2 - y1) > 0 and (math.abs(y0 - 2 * y1 + y2) / t * x) > math.abs(y):
        x0 = x2; x2 = x + x1; y0 = y2; y2 = y + y1
    t = (x0-x1)/t
    r = (1-t)*((1-t)*y0+2.0*t*y1)+t*t*y2;
    t = (x0*x2-x1*x1)*t/(x0-x1);

    x = math.floor(t+0.5)
    y = math.floor(r+0.5);
    r = (y1-y0)*(t-x0)/(x1-x0)+y0;

    more_points = points_for_quad_bezier_segment(x0, y0, x, math.floor(r + 0.5), x, y);
    points += more_points

    r = (y1-y2)*(t-x2)/(x1-x2)+y2;
    x0 = x1 = x; y0 = y; y1 = math.floor(r+0.5);

  if (y0 - y1) * ( y2 - y1) > 0:
    t = y0-2*y1+y2
    t = (y0-y1)/t
    r = (1-t)*((1-t)*x0+2.0*t*x1)+t*t*x2
    t = (y0*y2-y1*y1)*t/(y0-y1)

    x = math.floor(r+0.5)
    y = math.floor(t+0.5);

    r = (x1-x0)*(t-y0)/(y1-y0)+x0;
    more_points = points_for_quad_bezier_segment(x0, y0, floor(r+0.5), y, x, y);
    points += more_points

    r = (x1 - x2) * (t - y2) / (y1 - y2) + x2
    x0 = x
    x1 = math.floor(r+0.5)
    y0 = y1 = y

  more_points = points_for_quad_bezier_segment(x0, y0, x1, y1, x2, y2)
  points += more_points
  return points

def alt_quad_bezier(start, control, end, n = 10):
  points = []
  x0, y0 = start.x, start.y
  x1, y1 = control.x, control.y
  x2, y2 = end.x, end.y
  for i in range(n+1):
    t = i / float(n)
    a = (1. - t)**2
    b = 2. * t * (1. - t)
    c = t**2

    x = int(a * x0 + b * x1 + c * x2)
    y = int(a * y0 + b * y1 + c * y2)
    points.append(Point(x, y))

  return points


def convert_path_to_gpath(path, name):
  commands = get_path_commands(path)
  paths = []
  points = []

  last_p = None
  last_q = None

  for command, params in commands:
    if command == 'M':
      last_p = Point(params[0], params[1])
      points.append(last_p)
    elif command == 'h':
      v = float(params[0])
      last_p = Point(last_p.x + v, last_p.y)
      points.append(last_p)
    elif command == 'v':
      v = float(params[0])
      last_p = Point(last_p.x, last_p.y + v)
      points.append(last_p)
    elif command == 'l':
      v = Point(params[0], params[1])
      last_p = Point(last_p.x + v.x, last_p.y + v.y)
      points.append(last_p)
    elif command == 'z':
      paths.append(points)
      points = []
    elif command == 'q':
      v_control = Point(params[0], params[1])
      v_end = Point(params[2], params[3])
      q_start = last_p
      q_control = Point(last_p.x + v_control.x, last_p.y + v_control.y)
      q_end = Point(last_p.x + v_end.x, last_p.y + v_end.y)
      #points.append(q_start)
      #points.append(q_end)
      points += alt_quad_bezier(q_start, q_control, q_end)
      # TODO
      #points += points_for_quad_bezier(q_start, q_control, q_end)
      last_q = (q_start, q_control, q_end)
      last_p = q_end
    elif command == 't':
      v_end = Point(params[0], params[1])
      last_q_control = last_q[1]
      q_start = last_p
      q_control = Point(last_p.x + (last_p.x - last_q_control.x),
                        last_p.y + (last_p.y - last_q_control.y))
      q_end = Point(last_p.x + v_end.x, last_p.y + v_end.y)
      #points.append(q_start)
      #points.append(q_end)
      points += alt_quad_bezier(q_start, q_control, q_end)

      #points += points_for_quad_bezier(q_start, q_control, q_end)
      last_q = (q_start, q_control, q_end)
      last_p = q_end

    else:
      print 'unknown command: ', command, params

  SCALE_REDUCTION = int(math.ceil(372 / 24))
  GLYPH_HEIGHT = 720

  for path in paths:
    # scale all points in the paths.
    scaled_path = [Point(p.x / SCALE_REDUCTION, (GLYPH_HEIGHT - p.y) / SCALE_REDUCTION) for p in path]
    uniq_scaled_path = [scaled_path[0]]
    for i in range(1, len(scaled_path) - 1):
      if not uniq_scaled_path[-1].is_integral_equal(scaled_path[i]):
        uniq_scaled_path.append(scaled_path[i])

    print 'static const GPathInfo %s = {\n ' % name
    print '  .num_points = %d,' % len(uniq_scaled_path)
    print '  .points = (GPoint[]) {'
    for p in uniq_scaled_path:
      print '    {%d, %d}, ' % (int(math.floor(p.x)), int(math.floor(p.y)))
    print '  }'
    print '};'
def main():
  #convert_path_to_gpath(DIGIT_0, 'DIGIT_GLYPH_0')
  convert_path_to_gpath(DIGIT_1, 'DIGIT_GLYPH_1')
  convert_path_to_gpath(DIGIT_3, 'DIGIT_GLYPH_3')
  convert_path_to_gpath(DIGIT_4, 'DIGIT_GLYPH_4')
  convert_path_to_gpath(DIGIT_7, 'DIGIT_GLYPH_7')


if __name__ == '__main__':
  main()





