#include <gtest/gtest.h>

#include <piwcsprwmodel.h>

using namespace piwcs::prw;

TEST(Metadata, InitialState) {
    Node n(THRU, "n1");
    const Node &cn = n;

    EXPECT_FALSE(n.hasMetadata());
    EXPECT_FALSE(n.hasMetadata("k1"));
    EXPECT_TRUE(cn.metadata().empty());
    EXPECT_TRUE(cn.metadata("k1").empty());
}

TEST(Metadata, SetGet) {
    Node n(THRU, "n1");
    const Node &cn = n;

    n.metadata("k1") = "value";
    EXPECT_TRUE(n.hasMetadata("k1"));
    EXPECT_EQ(n.metadata("k1"), "value");
    EXPECT_EQ(cn.metadata("k1"), "value");

    EXPECT_TRUE(n.hasMetadata());

    n.metadata("k2") = "banana";
    EXPECT_TRUE(n.hasMetadata("k2"));
    EXPECT_EQ(n.metadata("k2"), "banana");
    EXPECT_EQ(cn.metadata("k2"), "banana");

    n.metadata("k1") = "synchrophasotron";
    EXPECT_TRUE(n.hasMetadata("k1"));
    EXPECT_EQ(n.metadata("k1"), "synchrophasotron");
    EXPECT_EQ(cn.metadata("k1"), "synchrophasotron");
}

TEST(Metadata, ViewUpdates) {
    Node n(THRU, "n1");
    auto &view = n.metadata();

    n.metadata("k1") = "value";
    EXPECT_EQ(view["k1"], "value");
    n.metadata("k2") = "banana";
    EXPECT_EQ(view["k2"], "banana");
    n.metadata("k1") = "synchrophasotron";
    EXPECT_EQ(view["k1"], "synchrophasotron");

    view["k3"] = "stallman";
    EXPECT_EQ(n.metadata("k3"), "stallman");
    view["k4"] = "kraken";
    EXPECT_EQ(n.metadata("k4"), "kraken");
    view["k3"] = "jij";
    EXPECT_EQ(n.metadata("k3"), "jij");
}

TEST(Metadata, EmptyValVsNoVal) {
    Node n(THRU, "n1");
    const Node &cn = n;

    EXPECT_FALSE(n.hasMetadata("k1"));
    EXPECT_EQ(cn.metadata("k1"), "");

    n.metadata("k1") = "";

    EXPECT_TRUE(n.hasMetadata("k1"));
    EXPECT_EQ(cn.metadata("k1"), "");
}
