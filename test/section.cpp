#include <gtest/gtest.h>

#include <piwcsprwmodel.h>

using namespace piwcs::prw;

TEST(Section, Constructor) {
    Section section1("123");
    Section section2("123", Section::AllowedTravel::BIDIR,
                     std::make_unique<Destination>("1.0.1", "My Name"));
    (void)section1;
    (void)section2;
}

TEST(Section, AllowedRoutesUnidir) {
    Section section("123", Section::AllowedTravel::UNIDIR, nullptr);

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
    Section section("123", Section::AllowedTravel::BIDIR, nullptr);

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

TEST(Section, AllowedRoutesForbidden) {
    Section section("123", Section::AllowedTravel::NONE, nullptr);

    // Invalid inputs
    EXPECT_FALSE(section.canTraverse(0, 2));
    EXPECT_FALSE(section.canTraverse(2, 0));
    EXPECT_FALSE(section.canTraverse(3, 2));
    EXPECT_FALSE(section.canTraverse(0, 127));
    EXPECT_FALSE(section.canTraverse(127, 0));
    EXPECT_FALSE(section.canTraverse(127, 126));

    // Invalid routes
    EXPECT_FALSE(section.canTraverse(0, 1));
    EXPECT_FALSE(section.canTraverse(1, 0));
    EXPECT_FALSE(section.canTraverse(0, 0));
    EXPECT_FALSE(section.canTraverse(1, 1));
}
