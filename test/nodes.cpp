#include <gtest/gtest.h>

#include <PiwcsPrwModel>

using namespace piwcs::prw;

TEST(ThruNode, Constructor) {
    ThruNode node("123");
    (void)node;
}

TEST(ThruNode, AllowedRoutes) {
    ThruNode node("123");
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
    SwitchNode node1(Node::Type::MOTORIZED, "123");
    SwitchNode node2(Node::Type::PASSIVE, "123");
    SwitchNode node3(Node::Type::FIXED, "123");
    (void)node1;
    (void)node2;
    (void)node3;
}

TEST(SwitchNode, MotorizedAllowedRoutes) {
    SwitchNode node(Node::Type::MOTORIZED, "123");
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

TEST(SwitchNode, PassiveAllowedRoutes) {
    SwitchNode node(Node::Type::PASSIVE, "123");
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

TEST(SwitchNode, FixedAllowedRoutes) {
    SwitchNode node(Node::Type::FIXED, "123");
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
    EXPECT_TRUE(node.couldTraverse(2, 0));

    // Invalid routes
    for (size_t i = 0; i < max; i++) {
        EXPECT_FALSE(node.couldTraverse(i, i));
    }
    EXPECT_FALSE(node.couldTraverse(0, 2));
    EXPECT_FALSE(node.couldTraverse(1, 2));
    EXPECT_FALSE(node.couldTraverse(2, 1));
}
