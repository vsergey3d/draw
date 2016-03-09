#include "common.h"

using namespace details;

go_bandit([]{

    describe("draw::Point", []{

        draw::Point point;
        draw::Point point1 {2, 3};
        draw::Point point2 {2, 3};

        it("should be zero by default", [&]{

            AssertThat(point.x, Equals(0));
            AssertThat(point.y, Equals(0));
        });

        it("should be constructed from params", [&]{

            AssertThat(point1.x, Equals(2));
            AssertThat(point1.y, Equals(3));
        });

        describe("equal operators", [&]{

            describe("equal to", [&]{

                it("should return true if points are equal", [&]{

                    AssertThat(point1 == point2, Is().True());
                });

                it("should return false if points are not equal", [&]{

                    AssertThat(point == point2, Is().False());
                });
            });

            describe("not equal to", [&]{

                it("should return true if points are not equal", [&]{

                    AssertThat(point != point1, Is().True());
                });

                it("should return false if points are equal", [&]{

                    AssertThat(point1 != point2, Is().False());
                });
            });
        });
    });

    describe("draw::Size", []{

        draw::Size size;
        draw::Size size1 {12, 88};
        draw::Size size2 {12, 88};

        it("should be constucted by default", [&]{

            AssertThat(size.width, Equals(1));
            AssertThat(size.height, Equals(1));
        });

        it("should be constructed from params", [&]{

            AssertThat(size1.width,  Equals(12));
            AssertThat(size1.height, Equals(88));
        });

        describe("equal operators", [&]{

            describe("equal to", [&]{

                it("should return true if sizes are equal", [&]{

                    AssertThat(size1 == size2, Is().True());
                });

                it("should return false if sizes are not equal", [&]{

                    AssertThat(size == size2, Is().False());
                });
            });

            describe("not equal to", [&]{

                it("should return true if sizes are not equal", [&]{

                    AssertThat(size != size1, Is().True());
                });

                it("should return false if sizes are equal", [&]{

                    AssertThat(size1 != size2, Is().False());
                });
            });
        });
    });

    describe("draw::Rect", []{

        draw::Rect rect;
        draw::Rect rect1 {9, -10, 33, 77};
        draw::Rect rect2 {9, -10, 33, 77};

        it("should be zero by default", [&]{

            AssertThat(rect.left, Equals(0));
            AssertThat(rect.bottom, Equals(0));
            AssertThat(rect.right, Equals(0));
            AssertThat(rect.top, Equals(0));
        });

        it("should be constructed from params", [&]{

            AssertThat(rect1.left, Equals(9));
            AssertThat(rect1.bottom, Equals(-10));
            AssertThat(rect1.right, Equals(33));
            AssertThat(rect1.top, Equals(77));
        });

        describe("equal operators", [&]{

            describe("equal to", [&]{

                it("should return true if rects are equal", [&]{

                    AssertThat(rect1 == rect2, Is().True());
                });

                it("should return false if rects are not equal", [&]{

                    AssertThat(rect == rect2, Is().False());
                });
            });

            describe("not equal to", [&]{

                it("should return true if rects are not equal", [&]{

                    AssertThat(rect != rect1, Is().True());
                });

                it("should return false if rects are equal", [&]{

                    AssertThat(rect1 != rect2, Is().False());
                });
            });
        });
    });

    describe("draw::Vector2", []{

        draw::Vector2 vector;
        draw::Vector2 vector1 {5.2f, 6.3f};

        it("should be zero by default", [&]{

            AssertThat(vector.x, Is().EqualToWithDelta(0.0f, FLT_EPSILON));
            AssertThat(vector.y, Is().EqualToWithDelta(0.0f, FLT_EPSILON));
        });

        it("should be constructed from params", [&]{

            AssertThat(vector1.x, Is().EqualToWithDelta(5.2f, FLT_EPSILON));
            AssertThat(vector1.y, Is().EqualToWithDelta(6.3f, FLT_EPSILON));
        });
    });
});

int main(int argc, char* argv[]) {

    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);

    return bandit::run(argc, argv);
}