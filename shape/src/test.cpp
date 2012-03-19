// test.cpp

#include "shape/shape.h"

#include <cstdlib>
#include <iostream>

#include "logging/logging.h"

using namespace std;
using namespace shape;
using namespace logging;

void test_d2_point();
void test_d3_point();
void test_rectangle();
void test_line();
void test_polygon();

int main(int argc, char* argv[]) {
    test_d2_point();
    test_d3_point();
    test_rectangle();
    test_line();
    test_polygon();
    system("pause");
    return 0;
}

void test_d2_point() {
    Trace trace(INFO_, "test_d2_point()");
    bool pass = true;
    d2::Point<int> p1(5,6), p2(7,8);

    p1 += p2;
    pass = (12==p1.x && 14==p1.y);
    log(INFO_) << "test +=: " << (pass ? "pass" : "NOT PASS") << endl;

    p1 -= p2;
    pass = (5==p1.x && 6==p1.y);
    log(INFO_) << "test -=: " << (pass ? "pass" : "NOT PASS") << endl;

    p1 *= 3;
    pass = (15==p1.x && 18==p1.y);
    log(INFO_) << "test *=: " << (pass ? "pass" : "NOT PASS") << endl;

    p1 /= 3;
    pass = (5==p1.x && 6==p1.y);
    log(INFO_) << "test /=: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = (d2::Point<int>(5,6) == p1);
    log(INFO_) << "test ==: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = (d2::Point<int>(5,8) != p1);
    log(INFO_) << "test !=: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = (d2::Point<int>(12,14)==p1+p2) && (d2::Point<int>(5,6)==+p1);
    log(INFO_) << "test +: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = (d2::Point<int>(-2,-2)==p1-p2) && (d2::Point<int>(-5,-6)==-p1);
    log(INFO_) << "test -: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = (d2::Point<int>(15,18) == p1*3) && (d2::Point<int>(15,18) == 3*p1);
    log(INFO_) << "test *: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = (d2::Point<int>(1,2) == p1/3);
    log(INFO_) << "test /: " << (pass ? "pass" : "NOT PASS") << endl;

    int product = scalar_product(p1, p2);
    pass = (35+48 == product);
    log(INFO_) << "test scalar_product: " << (pass ? "pass" : "NOT PASS") << endl;

    product = cross_product(p1, p2);
    pass = (40-42 == product);
    log(INFO_) << "test cross_product: " << (pass ? "pass" : "NOT PASS") << endl;
}

void test_d3_point() {
    Trace trace(INFO_, "test_d3_point()");
    bool pass = true;
    d3::Point<int> p1(2,3,4), p2(5,6,7);

    p1 += p2;
    pass = (7==p1.x && 9==p1.y && 11==p1.z);
    log(INFO_) << "test +=: " << (pass ? "pass" : "NOT PASS") << endl;

    p1 -= p2;
    pass = (2==p1.x && 3==p1.y && 4==p1.z);
    log(INFO_) << "test -=: " << (pass ? "pass" : "NOT PASS") << endl;

    p1 *= 3;
    pass = (6==p1.x && 9==p1.y && 12==p1.z);
    log(INFO_) << "test *=: " << (pass ? "pass" : "NOT PASS") << endl;

    p1 /= 3;
    pass = (2==p1.x && 3==p1.y && 4==p1.z);
    log(INFO_) << "test /=: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = (d3::Point<int>(2,3,4) == p1);
    log(INFO_) << "test ==: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = (d3::Point<int>(2,3,5) != p1);
    log(INFO_) << "test !=: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = (d3::Point<int>(7,9,11)==p1+p2) && (d3::Point<int>(2,3,4)==+p1);
    log(INFO_) << "test +: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = (d3::Point<int>(-3,-3,-3)==p1-p2) && (d3::Point<int>(-2,-3,-4)==-p1);
    log(INFO_) << "test -: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = (d3::Point<int>(6,9,12) == p1*3) && (d3::Point<int>(6,9,12) == 3*p1);
    log(INFO_) << "test *: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = (d3::Point<int>(1,1,2) == p1/2);
    log(INFO_) << "test /: " << (pass ? "pass" : "NOT PASS") << endl;

    int product = scalar_product(p1, p2);
    pass = (10+18+28 == product);
    log(INFO_) << "test scalar_product: " << (pass ? "pass" : "NOT PASS") << endl;

    d3::Point<int> pp = cross_product(p1, p2);
    pass = (-3==pp.x && 6==pp.y && -3==pp.z) &&
        (0==scalar_product(p1,pp) && 0==scalar_product(p2,pp));
    log(INFO_) << "test cross_product: " << (pass ? "pass" : "NOT PASS") << endl;
}

void test_rectangle() {
    Trace trace(INFO_, "test_rectangle()");
    d2::Point<int> p1(1,2), p2(3,4), p3(2,3), p4(2,5);
    d2::Rectangle<int> rect(p1,p2);
    bool pass = true;

    pass = rect==d2::Rectangle<int>(1,2,2,2);
    log(INFO_) << "test ==: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = rect!=d2::Rectangle<int>(p1,p3);
    log(INFO_) << "test !=: " << (pass ? "pass" : "NOT PASS") << endl;
    
    pass = point_in_rectangle(rect,p1) && point_in_rectangle(rect,p2) &&
        point_in_rectangle(rect,p3) && !point_in_rectangle(rect,p4);
    log(INFO_) << "test point_in_rectangle: " << (pass ? "pass" : "NOT PASS") << endl;

    typedef d2::Rectangle<int> Rect;
    Rect rect0(10,8,2,2);
    Rect rects0[] = {
        Rect(7,11,2,2), Rect(8,11,2,2), Rect(9,11,2,2), Rect(10,11,2,2), Rect(11,11,2,2), Rect(12,11,2,2),
        Rect(13,11,2,2), Rect(13,10,2,2), Rect(13,9,2,2), Rect(13,8,2,2), Rect(13,7,2,2), Rect(13,6,2,2),
        Rect(13,5,2,2), Rect(12,5,2,2), Rect(11,5,2,2), Rect(10,5,2,2), Rect(9,5,2,2), Rect(8,5,2,2),
        Rect(7,5,2,2), Rect(7,6,2,2), Rect(7,7,2,2), Rect(7,8,2,2), Rect(7,9,2,2), Rect(7,10,2,2)
    };
    pass = true;
    for (size_t i=0; i<sizeof(rects0)/sizeof(Rect); ++i) {
        pass = pass && !rectangle_intersect(rect0, rects0[i]);
    }
    log(INFO_) << "test rectangle_intersect: " << (pass ? "pass" : "NOT PASS") << endl;

    Rect rect1(0,0,4,4);
    for (int x=-2; x<4; ++x) {
        for (int y=-2; y<4; ++y) {
            pass = pass && rectangle_intersect(rect1, Rect(x,y,2,2));
        }
    }
    log(INFO_) << "test rectangle_intersect: " << (pass ? "pass" : "NOT PASS") << endl;
}

void test_line() {
    Trace trace(INFO_, "test_line()");
    d2::Point<int> p1(1,2), p2(2,4), p3(3,6), p4(4,8), p5(0,2), p6(1,0), p7(3,4);
    d2::Line<double> line1(p1,p2), line2(p3,p4), line3(p5,p6);
    d2::Line<double> line4(p3,p3), line5(p5,p5), line7(p5,p7);
    d2::Line<int> line6(p1,p3), line8(p1,p2);
    bool pass = true;

    pass = collinear(p1, p2, p3) && !collinear(p1, p2, p5);
    log(INFO_) << "test collinear: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = collinear(line8, p3) && !collinear(line8, p5);
    log(INFO_) << "test collinear: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = collinear(line1, line2) && !collinear(line1, line3) &&
        collinear(line1, line4) && !collinear(line1, line5) &&
        collinear(line4, line5);
    log(INFO_) << "test collinear: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = intersection(line1,line3)==d2::Point<double>(0.5,1);
    try {
        intersection(line1, line2);
        pass = false;
    } catch (const runtime_error&) {
        // should go this path
    }
    log(INFO_) << "test intersection: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = point_on_line_segment(line6, p2) && !point_on_line_segment(line6, p5);
    log(INFO_) << "test point_on_line_segment: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = line_segment_intersect(line1,line7) &&
        !line_segment_intersect(line1, line2);
    log(INFO_) << "test line_segment_intersect: " << (pass ? "pass" : "NOT PASS") << endl;
}

void test_polygon() {
    Trace trace(INFO_, "test_polygon()");
    typedef d2::Point<double> Pt;
    Pt points[] = {Pt(-1,0), Pt(0,1), Pt(1,0), Pt(0,-1)};
    d2::Polygon<double> polygon;
    polygon.vertices.assign(points, points+sizeof(points)/sizeof(Pt));
    Pt p1(0,0), p2(1,1);
    bool pass = true;

    pass = polygon.envelop_rectangle() == d2::Rectangle<double>(-1,-1,2,2);
    log(INFO_) << "test envelop_rectangle: " << (pass ? "pass" : "NOT PASS") << endl;

    pass = point_in_polygon(polygon,p1) && !point_in_polygon(polygon,p2);
    log(INFO_) << "test point_in_polygon: " << (pass ? "pass" : "NOT PASS") << endl;
}
