#include <gtest/gtest.h>

#include <piwcsprwmodel.h>
#include <sstream>

using namespace piwcs::prw;

namespace {

Model _read(const char *src) {
    std::istringstream in(src);
    return readModel(in);
}

#define MUST_FAIL(msg, src)                                                    \
    try {                                                                      \
        (void)_read(src);                                                      \
        FAIL() << "Error not detected by readModel: " << msg;                  \
    } catch (...) {                                                            \
        /* Do nothing */                                                       \
    }

#define MUST_PASS(msg, src)                                                    \
    try {                                                                      \
        (void)_read(src);                                                      \
    } catch (...) {                                                            \
        FAIL() << "Errors detected by readModel when testing " << msg;         \
    }

#define MUST_FAIL_TEST(testName, src)                                          \
    TEST(IoRead, testName) { MUST_FAIL(#testName, src) }

} // namespace

TEST(IoRead, Basic) {
    Model model = _read(R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": {}
        }
    ])json");

    auto *n1 = model.node("n1");
    ASSERT_NE(n1, nullptr);
    ASSERT_EQ(n1->type(), THRU);

    auto *n2 = model.node("n2");
    ASSERT_NE(n2, nullptr);
    ASSERT_EQ(n2->type(), THRU);

    auto *s1 = model.section("s1");
    ASSERT_NE(s1, nullptr);
    ASSERT_FALSE(s1->isBidir());
    ASSERT_EQ(s1->length(), 0);
    ASSERT_FALSE(s1->isDestination());

    ASSERT_EQ(n1->section(0), ID_NULL);
    ASSERT_EQ(n1->section(1), ID_NULL);
    ASSERT_EQ(n2->section(0), ID_NULL);
    ASSERT_EQ(n2->section(1), ID_NULL);
    ASSERT_EQ(s1->start(), ID_NULL);
    ASSERT_EQ(s1->end(), ID_NULL);
}

TEST(IoRead, BasicWithLink) {
    Model model = _read(R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": { "startNode":"n1", "startSlot":0,
                    "endNode":"n2", "endSlot":1 }
        }
    ])json");

    auto *n1 = model.node("n1");
    ASSERT_NE(n1, nullptr);
    ASSERT_EQ(n1->type(), THRU);

    auto *n2 = model.node("n2");
    ASSERT_NE(n2, nullptr);
    ASSERT_EQ(n2->type(), THRU);

    auto *s1 = model.section("s1");
    ASSERT_NE(s1, nullptr);
    ASSERT_FALSE(s1->isBidir());
    ASSERT_EQ(s1->length(), 0);
    ASSERT_FALSE(s1->isDestination());

    ASSERT_EQ(n1->section(0), "s1");
    ASSERT_EQ(n1->section(1), ID_NULL);
    ASSERT_EQ(n2->section(0), ID_NULL);
    ASSERT_EQ(n2->section(1), "s1");
    ASSERT_EQ(s1->start(), "n1");
    ASSERT_EQ(s1->end(), "n2");
}

TEST(IoRead, Empty) {
    Model model = _read("[{}, {}]");
    ASSERT_TRUE(model.nodes().empty());
    ASSERT_TRUE(model.sections().empty());
}

TEST(IoRead, Maximal) {
    Model model = _read(R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": {
                "startNode": "n1",
                "startSlot": 0,
                "endNode": "n2",
                "endSlot": 1,
                "length": 123,
                "bidir": true
            }
        }
    ])json");

    auto *n1 = model.node("n1");
    ASSERT_NE(n1, nullptr);
    ASSERT_EQ(n1->type(), THRU);

    auto *n2 = model.node("n2");
    ASSERT_NE(n2, nullptr);
    ASSERT_EQ(n2->type(), THRU);

    auto *s1 = model.section("s1");
    ASSERT_NE(s1, nullptr);
    ASSERT_TRUE(s1->isBidir());
    ASSERT_EQ(s1->length(), 123);
    ASSERT_FALSE(s1->isDestination());

    ASSERT_EQ(n1->section(0), "s1");
    ASSERT_EQ(n1->section(1), ID_NULL);
    ASSERT_EQ(n2->section(0), ID_NULL);
    ASSERT_EQ(n2->section(1), "s1");
    ASSERT_EQ(s1->start(), "n1");
    ASSERT_EQ(s1->end(), "n2");
}

MUST_FAIL_TEST(NotJson, R"json(
    Hello, world!
)json");

MUST_FAIL_TEST(NotArray, R"json({
    "n1": { "type":"THRU" }
})json");

MUST_FAIL_TEST(NoSectionsPart, R"json([
    {
        "n1": { "type":"THRU" }
    }
])json");

MUST_FAIL_TEST(TooManyParts, R"json([
    {
        "n1": { "type":"THRU" }
    },
    {
        "s1": {}
    },
    {
        "what": {}
    }
])json");

MUST_FAIL_TEST(InvalidParts, R"json([
    {
        "n1": { "type":"THRU" }
    },
    []
])json");

MUST_FAIL_TEST(NullNodeId, R"json([
    {
        "": { "type":"THRU" }
    },
    {}
])json");

MUST_FAIL_TEST(InvalidNodeId, R"json([
    {
        "#invalid": { "type":"THRU" }
    },
    {}
])json");

MUST_FAIL_TEST(NoNodeType, R"json([
    {
        "n1": {}
    },
    {}
])json");

MUST_FAIL_TEST(InvalidNodeType, R"json([
    {
        "n1": { "type":"FOOBAR" }
    },
    {}
])json");

MUST_FAIL_TEST(UnusedNodeData, R"json([
    {
        "n1": { "type":"THRU", "foobar":123 }
    },
    {}
])json");

MUST_FAIL_TEST(NullSectionId, R"json([
    {},
    {
        "": {}
    }
])json");

MUST_FAIL_TEST(InvalidSectionId, R"json([
    {},
    {
        "#invalid": {}
    }
])json");

MUST_FAIL_TEST(UnusedSectionData, R"json([
    {},
    {
        "s1": { "foobar":123 }
    }
])json");

TEST(IoRead, BadLinks) {
    // Base variant

    MUST_PASS("Basic self-link", R"json([
        {
            "n1": { "type":"THRU" }
        },
        {
            "s1": { "startNode":"n1", "startSlot":0,
                    "endNode":"n1", "endSlot":1 }
        }
    ])json");

    // One field missing

    MUST_FAIL("startNode missing", R"json([
        {
            "n1": { "type":"THRU" }
        },
        {
            "s1": {                   "startSlot":0,
                    "endNode":"n1", "endSlot":1 }
        }
    ])json");

    MUST_FAIL("startSlot missing", R"json([
        {
            "n1": { "type":"THRU" }
        },
        {
            "s1": { "startNode":"n1",
                    "endNode":"n1", "endSlot":1 }
        }
    ])json");

    MUST_FAIL("endNode missing", R"json([
        {
            "n1": { "type":"THRU" }
        },
        {
            "s1": { "startNode":"n1", "startSlot":0,
                                    "endSlot":1 }
        }
    ])json");

    MUST_FAIL("endSlot missing", R"json([
        {
            "n1": { "type":"THRU" }
        },
        {
            "s1": { "startNode":"n1", "startSlot":0,
                    "endNode":"n1"              }
        }
    ])json");

    // One stray field

    MUST_FAIL("stray startNode", R"json([
        {
            "n1": { "type":"THRU" }
        },
        {
            "s1": { "startNode":"n1" }
        }
    ])json");

    MUST_FAIL("stray startSlot", R"json([
        {
            "n1": { "type":"THRU" }
        },
        {
            "s1": { "startSlot":0 }
        }
    ])json");

    MUST_FAIL("stray endNode", R"json([
        {
            "n1": { "type":"THRU" }
        },
        {
            "s1": { "endNode":"n1" }
        }
    ])json");

    MUST_FAIL("stray endSlot", R"json([
        {
            "n1": { "type":"THRU" }
        },
        {
            "s1": { "endSlot":1 }
        }
    ])json");
}
