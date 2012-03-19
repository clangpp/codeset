// shape.h
#ifndef SHAPE_H_
#define SHAPE_H_

#include <vector>

namespace shape {

namespace d2 {  // 2-D namespace

// point type, also serve as vector type
// TBD: do not use std::complex<T> currently, for it has so many features not needed.
template <typename ScalarT>
struct Point {
    typedef ScalarT scalar_type;
    scalar_type x;
    scalar_type y;
    Point(scalar_type _x=scalar_type(),
            scalar_type _y=scalar_type()): x(_x), y(_y) {}
    template <typename ScalarType>
    operator Point<ScalarType>() const { return Point<ScalarType>(x,y); }
    Point normal() const { return Point(-y,x); }

    Point& operator += (const Point& rhs) { x += rhs.x; y += rhs.y; return *this; }
    Point& operator -= (const Point& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    template <typename ScalarType>
    Point& operator *= (const ScalarType& rhs) { x *= rhs; y *= rhs; return *this; }
    template <typename ScalarType>
    Point& operator /= (const ScalarType& rhs) { x /= rhs; y /= rhs; return *this; }

    Point operator + (const Point& rhs) const { Point p=*this; return p+=rhs; }
    Point operator - (const Point& rhs) const { Point p=*this; return p-=rhs; }
    template <typename ScalarType>
    Point operator * (const ScalarType& rhs) const { Point p=*this; return p*=rhs; }
    template <typename ScalarType>
    friend Point operator * (const ScalarType& lhs, const Point& rhs) { return rhs*lhs; }
    template <typename ScalarType>
    Point operator / (const ScalarType& rhs) const { Point p=*this; return p/=rhs; }
    Point operator + () const { return *this; }
    Point operator - () const { return Point(-x, -y); }

    bool operator == (const Point& rhs) const { return x==rhs.x && y==rhs.y; }
    bool operator != (const Point& rhs) const { return !(*this==rhs); }
};

template <typename ScalarT>
inline ScalarT scalar_product(
        const Point<ScalarT>& lhs, const Point<ScalarT>& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

template <typename ScalarT>
inline ScalarT cross_product(
        const Point<ScalarT>& lhs, const Point<ScalarT>& rhs) {
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

    Rectangle(const Point<scalar_type>& p1, const Point<scalar_type>& p2);
    Rectangle(const Point<scalar_type>& p,
            const scalar_type& _width, const scalar_type& _height)
        : x(p.x), y(p.y), width(_width), height(_height) {}
    Rectangle(const scalar_type& _x, const scalar_type& _y,
            const scalar_type& _width, const scalar_type& _height)
        : x(_x), y(_y), width(_width), height(_height) {}
    Rectangle(): x(scalar_type()), y(scalar_type()),
        width(scalar_type()), height(scalar_type()) {}

    bool operator == (const Rectangle& rhs) const {
        return x==rhs.x && y==rhs.y && width==rhs.width && height==rhs.height;
    }
    bool operator != (const Rectangle& rhs) const { return !(*this==rhs); }
};

template <typename ScalarT>
inline bool point_in_rectangle(
        const Rectangle<ScalarT>& rect, const Point<ScalarT>& p);

template <typename ScalarT>
inline bool rectangle_intersect(
        const Rectangle<ScalarT>& rect1, const Rectangle<ScalarT>& rect2);

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
inline bool collinear(const Point<ScalarT>& p1,
        const Point<ScalarT>& p2, const Point<ScalarT>& p3) {
    return cross_product(p2-p1, p3-p1) == 0;
}

template <typename ScalarT>
inline bool collinear(const Line<ScalarT>& line, const Point<ScalarT>& p) {
    return collinear(line.from, line.to, p);
}

template <typename ScalarT>
inline bool collinear(const Line<ScalarT>& line1, const Line<ScalarT>& line2);

template <typename ScalarT>
Point<ScalarT> intersection(
        const Line<ScalarT>& line1, const Line<ScalarT>& line2);

template <typename ScalarT>
inline bool point_on_line_segment(
        const Line<ScalarT>& line, const Point<ScalarT>& p);

template <typename ScalarT>
bool line_segment_intersect(
        const Line<ScalarT>& line1, const Line<ScalarT>& line2);

// struct Polygon
template <typename ScalarT>
struct Polygon {
    typedef ScalarT scalar_type;
    typedef Point<scalar_type> point;
    std::vector<point> vertices;

    bool valid() const;
    Rectangle<scalar_type> envelop_rectangle() const;
};

template <typename ScalarT>
bool point_in_polygon(const Polygon<ScalarT>& polygon, const Point<ScalarT>& p);

}  // namespace d2

namespace d3 {  // 3-D namespace

template <typename ScalarT>
struct Point {
    typedef ScalarT scalar_type;
    scalar_type x;
    scalar_type y;
    scalar_type z;
    Point(scalar_type _x=scalar_type(), scalar_type _y=scalar_type(),
            scalar_type _z=scalar_type()): x(_x), y(_y), z(_z) {}
    template <typename ScalarType>
    operator Point<ScalarType>() const { return Point<ScalarType>(x,y,z); }

    Point& operator += (const Point& rhs) {
        x += rhs.x; y += rhs.y; z += rhs.z; return *this;
    }
    Point& operator -= (const Point& rhs) {
        x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this;
    }
    template <typename ScalarType>
    Point& operator *= (const ScalarType& rhs) {
        x *= rhs; y *= rhs; z *= rhs; return *this;
    }
    template <typename ScalarType>
    Point& operator /= (const ScalarType& rhs) {
        x /= rhs; y /= rhs; z /= rhs; return *this;
    }

    Point operator + (const Point& rhs) const { Point p=*this; return p+=rhs; }
    Point operator - (const Point& rhs) const { Point p=*this; return p-=rhs; }
    template <typename ScalarType>
    Point operator * (const ScalarType& rhs) const { Point p=*this; return p*=rhs; }
    template <typename ScalarType>
    friend Point operator * (const ScalarType& lhs, const Point& rhs) { return rhs*lhs; }
    template <typename ScalarType>
    Point operator / (const ScalarType& rhs) const { Point p=*this; return p/=rhs; }
    Point operator + () const { return *this; }
    Point operator - () const { return Point(-x, -y, -z); }

    bool operator == (const Point& rhs) const {
        return x==rhs.x && y==rhs.y && z==rhs.z;
    }
    bool operator != (const Point& rhs) const { return !(*this==rhs); }
};

template <typename ScalarT>
inline ScalarT scalar_product(
        const Point<ScalarT>& lhs, const Point<ScalarT>& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template <typename ScalarT>
inline Point<ScalarT> cross_product(
        const Point<ScalarT>& lhs, const Point<ScalarT>& rhs);

// TBD: define struct Cube, as well as d2::Rectangle.

}  // namespace d3

}  // namespace shape

#include "shape-inl.h"

#endif  // SHAPE_H_
