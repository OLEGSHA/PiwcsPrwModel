#include <gtest/gtest.h>

#include <piwcsprwmodel.h>

using namespace piwcs::prw;

TEST(ThruNode, Constructor) {
    Node node(THRU, "123");
    (void)node;
}

TEST(ThruNode, AllowedRoutes) {
    Node node(THRU, "123");
    size_t max = node.sectionCount();

    // Invalid inputs
    EXPECT_FALSE(node.couldTraverse(0, max));
    EXPECT_FALSE(node.couldTraverse(max, 0));
    EXPECT_FALSE(node.couldTraverse(max + 1, max));
    EXPECT_FALSE(node.couldTraverse(0, 127));
    EXPECT_FALSE(node.couldTraverse(127, 0));
    EXPECT_FALSE(node.couldTraverse(127, 126));

    // Valid routes
    EXPECT_TRUE(node.couldTraverse(0, 1));
    EXPECT_TRUE(node.couldTraverse(1, 0));

    // Invalid routes
    for (size_t i = 0; i < max; i++) {
        EXPECT_FALSE(node.couldTraverse(i, i));
    }
}

TEST(SwitchNode, Constructor) {
    Node node1(MOTORIZED, "123");
    Node node2(PASSIVE, "123");
    Node node3(FIXED, "123");
    (void)node1;
    (void)node2;
    (void)node3;
}

TEST(SwitchNode, MotorizedAllowedRoutes) {
    Node node(MOTORIZED, "123");
    size_t max = node.sectionCount();

    // Invalid inputs
    EXPECT_FALSE(node.couldTraverse(0, max));
    EXPECT_FALSE(node.couldTraverse(max, 0));
    EXPECT_FALSE(node.couldTraverse(max + 1, max));
    EXPECT_FALSE(node.couldTraverse(0, 127));
    EXPECT_FALSE(node.couldTraverse(127, 0));
    EXPECT_FALSE(node.couldTraverse(127, 126));

    // Valid routes
    EXPECT_TRUE(node.couldTraverse(0, 1));
    EXPECT_TRUE(node.couldTraverse(0, 2));

    // Invalid routes
    for (size_t i = 0; i < max; i++) {
        EXPECT_FALSE(node.couldTraverse(i, i));
    }
    EXPECT_FALSE(node.couldTraverse(1, 0));
    EXPECT_FALSE(node.couldTraverse(2, 0));
    EXPECT_FALSE(node.couldTraverse(1, 2));
    EXPECT_FALSE(node.couldTraverse(2, 1));
}

TEST(SwitchNode, PassiveAllowedRoutes) {
    Node node(PASSIVE, "123");
    size_t max = node.sectionCount();

    // Invalid inputs
    EXPECT_FALSE(node.couldTraverse(0, max));
    EXPECT_FALSE(node.couldTraverse(max, 0));
    EXPECT_FALSE(node.couldTraverse(max + 1, max));
    EXPECT_FALSE(node.couldTraverse(0, 127));
    EXPECT_FALSE(node.couldTraverse(127, 0));
    EXPECT_FALSE(node.couldTraverse(127, 126));

    // Valid routes
    EXPECT_TRUE(node.couldTraverse(1, 0));
    EXPECT_TRUE(node.couldTraverse(2, 0));

    // Invalid routes
    for (size_t i = 0; i < max; i++) {
        EXPECT_FALSE(node.couldTraverse(i, i));
    }
    EXPECT_FALSE(node.couldTraverse(0, 1));
    EXPECT_FALSE(node.couldTraverse(0, 2));
    EXPECT_FALSE(node.couldTraverse(1, 2));
    EXPECT_FALSE(node.couldTraverse(2, 1));
}

TEST(SwitchNode, FixedAllowedRoutes) {
    Node node(FIXED, "123");
    size_t max = node.sectionCount();

    // Invalid inputs
    EXPECT_FALSE(node.couldTraverse(0, max));
    EXPECT_FALSE(node.couldTraverse(max, 0));
    EXPECT_FALSE(node.couldTraverse(max + 1, max));
    EXPECT_FALSE(node.couldTraverse(0, 127));
    EXPECT_FALSE(node.couldTraverse(127, 0));
    EXPECT_FALSE(node.couldTraverse(127, 126));

    // Valid routes
    EXPECT_TRUE(node.couldTraverse(0, 1));
    EXPECT_TRUE(node.couldTraverse(2, 0));

    // Invalid routes
    for (size_t i = 0; i < max; i++) {
        EXPECT_FALSE(node.couldTraverse(i, i));
    }
    EXPECT_FALSE(node.couldTraverse(1, 0));
    EXPECT_FALSE(node.couldTraverse(0, 2));
    EXPECT_FALSE(node.couldTraverse(1, 2));
    EXPECT_FALSE(node.couldTraverse(2, 1));
}

TEST(CrossingNode, Constructor) {
    Node node(CROSSING, "123");
    (void)node;
}

TEST(CrossingNode, AllowedRoutes) {
    Node node(CROSSING, "123");
    size_t max = node.sectionCount();

    // Invalid inputs
    EXPECT_FALSE(node.couldTraverse(0, max));
    EXPECT_FALSE(node.couldTraverse(max, 0));
    EXPECT_FALSE(node.couldTraverse(max + 1, max));
    EXPECT_FALSE(node.couldTraverse(0, 127));
    EXPECT_FALSE(node.couldTraverse(127, 0));
    EXPECT_FALSE(node.couldTraverse(127, 126));

    // Valid routes
    EXPECT_TRUE(node.couldTraverse(0, 1));
    EXPECT_TRUE(node.couldTraverse(1, 0));
    EXPECT_TRUE(node.couldTraverse(2, 3));
    EXPECT_TRUE(node.couldTraverse(3, 2));

    // Invalid routes
    for (size_t i = 0; i < max; i++) {
        EXPECT_FALSE(node.couldTraverse(i, i));
    }
    EXPECT_FALSE(node.couldTraverse(0, 2));
    EXPECT_FALSE(node.couldTraverse(0, 3));
    EXPECT_FALSE(node.couldTraverse(1, 2));
    EXPECT_FALSE(node.couldTraverse(1, 3));
    EXPECT_FALSE(node.couldTraverse(2, 0));
    EXPECT_FALSE(node.couldTraverse(2, 1));
    EXPECT_FALSE(node.couldTraverse(3, 0));
    EXPECT_FALSE(node.couldTraverse(3, 1));
}

TEST(Nodes, SlotOf) {
    Model model;
    model.newNode(CROSSING, "n1");
    model.newNode(CROSSING, "n2");
    model.newSection("s1", false);
    model.newSection("s2", false);
    model.newSection("s3", false);
    model.link("s1", "n1", 0, "n2", 3);
    model.link("s2", "n1", 1, "n2", 2);

    EXPECT_EQ(model.node("n1")->slotOf("s1"), 0);
    EXPECT_EQ(model.node("n1")->slotOf("s2"), 1);
    EXPECT_EQ(model.node("n1")->slotOf("s3"), SLOT_INVALID);
    EXPECT_EQ(model.node("n1")->slotOf("s4"), SLOT_INVALID);

    EXPECT_EQ(model.node("n2")->slotOf("s1"), 3);
    EXPECT_EQ(model.node("n2")->slotOf("s2"), 2);
    EXPECT_EQ(model.node("n2")->slotOf("s3"), SLOT_INVALID);
    EXPECT_EQ(model.node("n2")->slotOf("s4"), SLOT_INVALID);
}
