#include <gtest/gtest.h>

#include <piwcsprwmodel.h>

using namespace piwcs::prw;

TEST(Completeness, Basic) {
    Model model;
    model.newNode(THRU, "n1");
    model.newNode(THRU, "n2");
    model.newSection("s1");
    model.newSection("s2");
    model.link("s1", "n1", 0, "n2", 0);
    model.link("s2", "n1", 1, "n2", 1);

    ASSERT_TRUE(isComplete(model));
}

TEST(Completeness, EmptyIsComplete) { ASSERT_TRUE(isComplete(Model())); }

TEST(Completeness, AlmostComplete) {
    // n2.2 is not connected

    Model model;
    model.newNode(THRU, "n1");
    model.newNode(MOTORIZED, "n2");
    model.newSection("s1");
    model.newSection("s2");
    model.link("s1", "n1", 0, "n2", 0);
    model.link("s2", "n1", 1, "n2", 1);

    ASSERT_FALSE(isComplete(model));
}

TEST(Completeness, VeryIncomplete) {
    Model model;
    model.newNode(THRU, "n1");
    model.newNode(THRU, "n2");
    model.newSection("s1");
    model.newSection("s2");

    ASSERT_FALSE(isComplete(model));
}
