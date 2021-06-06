#include "shape.h"

#include <cstdlib>
#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace shape {
namespace {

using ::testing::Pair;

TEST(D2PointTest, EndToEnd) {
  d2::Point<int> p1(5, 6), p2(7, 8);

  p1 += p2;
  EXPECT_EQ(p1.x, 12);
  EXPECT_EQ(p1.y, 14);

  p1 -= p2;
  EXPECT_EQ(p1.x, 5);
  EXPECT_EQ(p1.y, 6);

  p1 *= 3;
  EXPECT_EQ(p1.x, 15);
  EXPECT_EQ(p1.y, 18);

  p1 /= 3;
  EXPECT_EQ(p1.x, 5);
  EXPECT_EQ(p1.y, 6);

  EXPECT_EQ(p1, d2::Point<int>(5, 6));
  EXPECT_NE(p1, d2::Point<int>(5, 8));

  EXPECT_EQ(p1 + p2, d2::Point<int>(12, 14));
  EXPECT_EQ(+p1, d2::Point<int>(5, 6));
  EXPECT_EQ(p1 - p2, d2::Point<int>(-2, -2));
  EXPECT_EQ(-p1, d2::Point<int>(-5, -6));
  EXPECT_EQ(p1 * 3, d2::Point<int>(15, 18));
  EXPECT_EQ(3 * p1, d2::Point<int>(15, 18));
  EXPECT_EQ(p1 / 3, d2::Point<int>(1, 2));

  EXPECT_EQ(scalar_product(p1, p2), 35 + 48);
  EXPECT_EQ(cross_product(p1, p2), 40 - 42);
}

TEST(D3PointTest, EndToEnd) {
  d3::Point<int> p1(2, 3, 4), p2(5, 6, 7);

  p1 += p2;
  EXPECT_EQ(p1.x, 7);
  EXPECT_EQ(p1.y, 9);
  EXPECT_EQ(p1.z, 11);

  p1 -= p2;
  EXPECT_EQ(p1.x, 2);
  EXPECT_EQ(p1.y, 3);
  EXPECT_EQ(p1.z, 4);

  p1 *= 3;
  EXPECT_EQ(p1.x, 6);
  EXPECT_EQ(p1.y, 9);
  EXPECT_EQ(p1.z, 12);

  p1 /= 3;
  EXPECT_EQ(p1.x, 2);
  EXPECT_EQ(p1.y, 3);
  EXPECT_EQ(p1.z, 4);

  EXPECT_EQ(p1, d3::Point<int>(2, 3, 4));
  EXPECT_NE(p1, d3::Point<int>(2, 3, 5));

  EXPECT_EQ(p1 + p2, d3::Point<int>(7, 9, 11));
  EXPECT_EQ(+p1, d3::Point<int>(2, 3, 4));

  EXPECT_EQ(p1 - p2, d3::Point<int>(-3, -3, -3));
  EXPECT_EQ(-p1, d3::Point<int>(-2, -3, -4));

  EXPECT_EQ(p1 * 3, d3::Point<int>(6, 9, 12));
  EXPECT_EQ(3 * p1, d3::Point<int>(6, 9, 12));

  EXPECT_EQ(p1 / 2, d3::Point<int>(1, 1, 2));

  EXPECT_EQ(scalar_product(p1, p2), 10 + 18 + 28);

  d3::Point<int> pp = cross_product(p1, p2);
  EXPECT_EQ(pp, d3::Point<int>(-3, 6, -3));
  EXPECT_EQ(scalar_product(p1, pp), 0);
  EXPECT_EQ(scalar_product(p2, pp), 0);
}

TEST(RectangleTest, EndToEnd) {
  d2::Point<int> p1(1, 2), p2(3, 4), p3(2, 3), p4(2, 5);
  d2::Rectangle<int> rect(p1, p2);

  EXPECT_EQ(rect, d2::Rectangle<int>(1, 2, 2, 2));
  EXPECT_NE(rect, d2::Rectangle<int>(p1, p3));

  EXPECT_TRUE(point_in_rectangle(rect, p1));
  EXPECT_TRUE(point_in_rectangle(rect, p2));
  EXPECT_TRUE(point_in_rectangle(rect, p3));
  EXPECT_FALSE(point_in_rectangle(rect, p4));

  typedef d2::Rectangle<int> Rect;
  Rect rect0(10, 8, 2, 2);
  Rect rects0[] = {
      Rect(7, 11, 2, 2),  Rect(8, 11, 2, 2),  Rect(9, 11, 2, 2),
      Rect(10, 11, 2, 2), Rect(11, 11, 2, 2), Rect(12, 11, 2, 2),
      Rect(13, 11, 2, 2), Rect(13, 10, 2, 2), Rect(13, 9, 2, 2),
      Rect(13, 8, 2, 2),  Rect(13, 7, 2, 2),  Rect(13, 6, 2, 2),
      Rect(13, 5, 2, 2),  Rect(12, 5, 2, 2),  Rect(11, 5, 2, 2),
      Rect(10, 5, 2, 2),  Rect(9, 5, 2, 2),   Rect(8, 5, 2, 2),
      Rect(7, 5, 2, 2),   Rect(7, 6, 2, 2),   Rect(7, 7, 2, 2),
      Rect(7, 8, 2, 2),   Rect(7, 9, 2, 2),   Rect(7, 10, 2, 2),
  };
  for (size_t i = 0; i < sizeof(rects0) / sizeof(Rect); ++i) {
    EXPECT_FALSE(rectangle_intersect(rect0, rects0[i])) << i;
  }

  Rect rect1(0, 0, 4, 4);
  for (int x = -2; x < 4; ++x) {
    for (int y = -2; y < 4; ++y) {
      EXPECT_TRUE(rectangle_intersect(rect1, Rect(x, y, 2, 2)))
          << x << ", " << y;
    }
  }
}

TEST(LineTest, EndToEnd) {
  d2::Point<int> p1(1, 2), p2(2, 4), p3(3, 6), p4(4, 8), p5(0, 2), p6(1, 0),
      p7(3, 4);
  d2::Line<double> line1(p1, p2), line2(p3, p4), line3(p5, p6);
  d2::Line<double> line4(p3, p3), line5(p5, p5), line7(p5, p7);
  d2::Line<int> line6(p1, p3), line8(p1, p2);

  EXPECT_TRUE(collinear(p1, p2, p3));
  EXPECT_FALSE(collinear(p1, p2, p5));

  EXPECT_TRUE(collinear(line8, p3));
  EXPECT_FALSE(collinear(line8, p5));

  EXPECT_TRUE(collinear(line1, line2));
  EXPECT_FALSE(collinear(line1, line3));

  EXPECT_TRUE(collinear(line1, line4));
  EXPECT_FALSE(collinear(line1, line5));

  EXPECT_TRUE(collinear(line4, line5));

  EXPECT_EQ(intersection(line1, line3), d2::Point<double>(0.5, 1));
  try {
    intersection(line1, line2);
    FAIL();
  } catch (const std::runtime_error&) {
    SUCCEED();
  }

  EXPECT_TRUE(point_on_line_segment(line6, p2));
  EXPECT_FALSE(point_on_line_segment(line6, p5));

  EXPECT_TRUE(line_segment_intersect(line1, line7));
  EXPECT_FALSE(line_segment_intersect(line1, line2));
}

TEST(PolygonTest, EndToEnd) {
  typedef d2::Point<double> Pt;
  Pt points[] = {Pt(-1, 0), Pt(0, 1), Pt(1, 0), Pt(0, -1)};
  d2::Polygon<double> polygon;
  polygon.vertices.assign(points, points + sizeof(points) / sizeof(Pt));
  Pt p1(0, 0), p2(1, 1);
  EXPECT_EQ(polygon.envelop_rectangle(), d2::Rectangle<double>(-1, -1, 2, 2));

  EXPECT_TRUE(point_in_polygon(polygon, p1));
  EXPECT_FALSE(point_in_polygon(polygon, p2));
}

}  // namespace
}  // namespace shape
