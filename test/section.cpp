#include <gtest/gtest.h>

#include <PiwcsPrwModel>

using namespace piwcs::prw;

TEST(Section, Constructor) {
    Section section1("123", false);
    Section section2("123", true);
    (void)section1;
    (void)section2;
}

TEST(Section, AllowedRoutesDirected) {
    Section section("123", false);

    // Invalid inputs
    EXPECT_FALSE(section.canTraverse(0, 2));
    EXPECT_FALSE(section.canTraverse(2, 0));
    EXPECT_FALSE(section.canTraverse(3, 2));
    EXPECT_FALSE(section.canTraverse(0, 127));
    EXPECT_FALSE(section.canTraverse(127, 0));
    EXPECT_FALSE(section.canTraverse(127, 126));

    // Valid routes
    EXPECT_TRUE(section.canTraverse(0, 1));

    // Invalid routes
    EXPECT_FALSE(section.canTraverse(1, 0));
    EXPECT_FALSE(section.canTraverse(0, 0));
    EXPECT_FALSE(section.canTraverse(1, 1));
}

TEST(Section, AllowedRoutesUndirected) {
    Section section("123", true);

    // Invalid inputs
    EXPECT_FALSE(section.canTraverse(0, 2));
    EXPECT_FALSE(section.canTraverse(2, 0));
    EXPECT_FALSE(section.canTraverse(3, 2));
    EXPECT_FALSE(section.canTraverse(0, 127));
    EXPECT_FALSE(section.canTraverse(127, 0));
    EXPECT_FALSE(section.canTraverse(127, 126));

    // Valid routes
    EXPECT_TRUE(section.canTraverse(0, 1));
    EXPECT_TRUE(section.canTraverse(1, 0));

    // Invalid routes
    EXPECT_FALSE(section.canTraverse(0, 0));
    EXPECT_FALSE(section.canTraverse(1, 1));
}
