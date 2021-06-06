#ifndef SHAPE_H_
#define SHAPE_H_

#include <vector>

namespace shape {

namespace d2 {  // 2-D namespace

// point type, also serve as vector type
// TBD: do not use std::complex<T> currently, for it has so many features not
// needed.
template <typename ScalarT>
struct Point {
  typedef ScalarT scalar_type;
  scalar_type x;
  scalar_type y;
  Point(scalar_type _x = scalar_type(), scalar_type _y = scalar_type())
      : x(_x), y(_y) {}
  template <typename ScalarType>
  operator Point<ScalarType>() const {
    return Point<ScalarType>(x, y);
  }
  operator std::pair<scalar_type, scalar_type>() const {
    return std::make_pair(x, y);
  }

  Point normal() const { return Point(-y, x); }

  Point& operator+=(const Point& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }
  Point& operator-=(const Point& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }
  template <typename ScalarType>
  Point& operator*=(const ScalarType& rhs) {
    x *= rhs;
    y *= rhs;
    return *this;
  }
  template <typename ScalarType>
  Point& operator/=(const ScalarType& rhs) {
    x /= rhs;
    y /= rhs;
    return *this;
  }

  Point operator+(const Point& rhs) const {
    Point p = *this;
    return p += rhs;
  }
  Point operator-(const Point& rhs) const {
    Point p = *this;
    return p -= rhs;
  }
  template <typename ScalarType>
  Point operator*(const ScalarType& rhs) const {
    Point p = *this;
    return p *= rhs;
  }
  template <typename ScalarType>
  friend Point operator*(const ScalarType& lhs, const Point& rhs) {
    return rhs * lhs;
  }
  template <typename ScalarType>
  Point operator/(const ScalarType& rhs) const {
    Point p = *this;
    return p /= rhs;
  }
  Point operator+() const { return *this; }
  Point operator-() const { return Point(-x, -y); }

  bool operator==(const Point& rhs) const { return x == rhs.x && y == rhs.y; }
  bool operator!=(const Point& rhs) const { return !(*this == rhs); }
};

template <typename ScalarT>
ScalarT scalar_product(const Point<ScalarT>& lhs, const Point<ScalarT>& rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

template <typename ScalarT>
ScalarT cross_product(const Point<ScalarT>& lhs, const Point<ScalarT>& rhs) {
  return lhs.x * rhs.y - lhs.y * rhs.x;
}

// struct Rectangle
// should ensure: width>=0 && height>=0
template <typename ScalarT>
struct Rectangle {
  typedef ScalarT scalar_type;
  scalar_type x;
  scalar_type y;
  scalar_type width;
  scalar_type height;

  Rectangle(const Point<scalar_type>& p1, const Point<scalar_type>& p2) {
    x = std::min(p1.x, p2.x);
    y = std::min(p1.y, p2.y);
    width = std::max(p1.x, p2.x) - x;
    height = std::max(p1.y, p2.y) - y;
  }
  Rectangle(const Point<scalar_type>& p, const scalar_type& _width,
            const scalar_type& _height)
      : x(p.x), y(p.y), width(_width), height(_height) {}
  Rectangle(const scalar_type& _x, const scalar_type& _y,
            const scalar_type& _width, const scalar_type& _height)
      : x(_x), y(_y), width(_width), height(_height) {}
  Rectangle()
      : x(scalar_type()),
        y(scalar_type()),
        width(scalar_type()),
        height(scalar_type()) {}

  bool operator==(const Rectangle& rhs) const {
    return x == rhs.x && y == rhs.y && width == rhs.width &&
           height == rhs.height;
  }
  bool operator!=(const Rectangle& rhs) const { return !(*this == rhs); }
};

template <typename ScalarT>
bool point_in_rectangle(const Rectangle<ScalarT>& rect,
                        const Point<ScalarT>& p) {
  return rect.x <= p.x && p.x <= rect.x + rect.width && rect.y <= p.y &&
         p.y <= rect.y + rect.height;
}

template <typename ScalarT>
bool rectangle_intersect(const Rectangle<ScalarT>& rect1,
                         const Rectangle<ScalarT>& rect2) {
  return rect1.x <= rect2.x + rect2.width && rect2.x <= rect1.x + rect1.width &&
         rect1.y <= rect2.y + rect2.height && rect2.y <= rect1.y + rect1.height;
}

// struct Line
template <typename ScalarT>
struct Line {
  typedef ScalarT scalar_type;
  Point<scalar_type> from;
  Point<scalar_type> to;

  Line(const Point<scalar_type>& p_from, const Point<scalar_type>& p_to)
      : from(p_from), to(p_to) {}
  Point<scalar_type> direction() const { return to - from; }
  Point<scalar_type> normal() const { return direction().normal(); }
};

template <typename ScalarT>
bool collinear(const Point<ScalarT>& p1, const Point<ScalarT>& p2,
               const Point<ScalarT>& p3) {
  return cross_product(p2 - p1, p3 - p1) == 0;
}

template <typename ScalarT>
bool collinear(const Line<ScalarT>& line, const Point<ScalarT>& p) {
  return collinear(line.from, line.to, p);
}

template <typename ScalarT>
bool collinear(const Line<ScalarT>& line1, const Line<ScalarT>& line2) {
  return collinear(line1, line2.from) && collinear(line1, line2.to) &&
         collinear(line2, line1.from) && collinear(line2, line1.to);
}

template <typename ScalarT>
Point<ScalarT> intersection(const Line<ScalarT>& line1,
                            const Line<ScalarT>& line2) {
  ScalarT direction_cross = cross_product(line1.direction(), line2.direction());
  if (0 == direction_cross) {
    throw std::runtime_error("intersection(line1, line2): parallel lines");
  }

  // p_inter = p1 + lambda * (p2-p1);
  // lambda = cross_product(p3-p1,p4-p3) / cross_product(p2-p1,p4-p3);
  long double lambda =
      cross_product(line2.from - line1.from, line2.direction()) /
      (long double)direction_cross;
  return line1.from + lambda * line1.direction();
}

template <typename ScalarT>
bool point_on_line_segment(const Line<ScalarT>& line, const Point<ScalarT>& p) {
  return collinear(line, p) &&
         point_in_rectangle(Rectangle<ScalarT>(line.from, line.to), p);
}

template <typename ScalarT>
bool line_segment_intersect(const Line<ScalarT>& line1,
                            const Line<ScalarT>& line2) {
  Rectangle<ScalarT> rect1(line1.from, line1.to);
  Rectangle<ScalarT> rect2(line2.from, line2.to);
  if (!rectangle_intersect(rect1, rect2)) return false;

  ScalarT p1xq = cross_product(line1.from - line2.from, line2.direction());
  ScalarT p2xq = cross_product(line1.to - line2.from, line2.direction());
  if (p1xq * p2xq > 0) return false;

  ScalarT q1xp = cross_product(line2.from - line1.from, line1.direction());
  ScalarT q2xp = cross_product(line2.to - line1.from, line1.direction());
  if (q1xp * q2xp > 0) return false;

  return true;
}

// struct Polygon
template <typename ScalarT>
struct Polygon {
  typedef ScalarT scalar_type;
  typedef Point<scalar_type> point;
  std::vector<point> vertices;

  // TBD: consider more conditions
  bool valid() const { return vertices.size() >= 3; }
  Rectangle<scalar_type> envelop_rectangle() const {
    if (!valid()) {
      throw std::runtime_error(
          "Polygon<T>::envelop_rectangle(): invalid polygon");
    }

    point p_min(vertices[0]), p_max(vertices[0]);
    for (std::size_t i = 1; i < vertices.size(); ++i) {
      if (vertices[i].x < p_min.x) p_min.x = vertices[i].x;
      if (vertices[i].y < p_min.y) p_min.y = vertices[i].y;
      if (vertices[i].x > p_max.x) p_max.x = vertices[i].x;
      if (vertices[i].y > p_max.y) p_max.y = vertices[i].y;
    }
    return Rectangle<ScalarT>(p_min, p_max);
  }
};

template <typename ScalarT>
bool point_in_polygon(const Polygon<ScalarT>& polygon,
                      const Point<ScalarT>& p) {
  Rectangle<ScalarT> envelop_rect = polygon.envelop_rectangle();
  if (!point_in_rectangle(envelop_rect, p)) {
    return false;
  }

  Line<ScalarT> secant(p, Point<ScalarT>(envelop_rect.x, p.y));
  std::size_t cross_count = 0;
  for (std::size_t i = 0; i < polygon.vertices.size(); ++i) {
    Line<ScalarT> side(polygon.vertices[i],
                       polygon.vertices[(i + 1) % polygon.vertices.size()]);
    if (point_on_line_segment(side, p)) {
      return true;  // on side
    }
    if (0 == side.direction().x) {
      continue;  // no consider parallel side
    }

    if (p.y == side.from.y && side.from.y > side.to.y) {
      ++cross_count;
    } else if (p.y == side.to.y && side.to.y > side.from.y) {
      ++cross_count;
    } else if (line_segment_intersect(side, secant)) {
      ++cross_count;
    }
  }
  return (cross_count & 0x01) != 0;  // cross_count should be odd
}

}  // namespace d2

namespace d3 {  // 3-D namespace

template <typename ScalarT>
struct Point {
  typedef ScalarT scalar_type;
  scalar_type x;
  scalar_type y;
  scalar_type z;
  Point(scalar_type _x = scalar_type(), scalar_type _y = scalar_type(),
        scalar_type _z = scalar_type())
      : x(_x), y(_y), z(_z) {}
  template <typename ScalarType>
  operator Point<ScalarType>() const {
    return Point<ScalarType>(x, y, z);
  }

  Point& operator+=(const Point& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }
  Point& operator-=(const Point& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }
  template <typename ScalarType>
  Point& operator*=(const ScalarType& rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
  }
  template <typename ScalarType>
  Point& operator/=(const ScalarType& rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
  }

  Point operator+(const Point& rhs) const {
    Point p = *this;
    return p += rhs;
  }
  Point operator-(const Point& rhs) const {
    Point p = *this;
    return p -= rhs;
  }
  template <typename ScalarType>
  Point operator*(const ScalarType& rhs) const {
    Point p = *this;
    return p *= rhs;
  }
  template <typename ScalarType>
  friend Point operator*(const ScalarType& lhs, const Point& rhs) {
    return rhs * lhs;
  }
  template <typename ScalarType>
  Point operator/(const ScalarType& rhs) const {
    Point p = *this;
    return p /= rhs;
  }
  Point operator+() const { return *this; }
  Point operator-() const { return Point(-x, -y, -z); }

  bool operator==(const Point& rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z;
  }
  bool operator!=(const Point& rhs) const { return !(*this == rhs); }
};

template <typename ScalarT>
ScalarT scalar_product(const Point<ScalarT>& lhs, const Point<ScalarT>& rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template <typename ScalarT>
Point<ScalarT> cross_product(const Point<ScalarT>& lhs,
                             const Point<ScalarT>& rhs) {
  return Point<ScalarT>(lhs.x * rhs.y - lhs.y * rhs.x,
                        lhs.z * rhs.x - lhs.x * rhs.z,
                        lhs.y * rhs.z - lhs.z * rhs.y);
}

// TBD: define struct Cube, as well as d2::Rectangle.

}  // namespace d3

}  // namespace shape

#endif  // SHAPE_H_
