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
    ASSERT_FALSE(n1->hasMetadata());

    auto *n2 = model.node("n2");
    ASSERT_NE(n2, nullptr);
    ASSERT_EQ(n2->type(), THRU);
    ASSERT_FALSE(n2->hasMetadata());

    auto *s1 = model.section("s1");
    ASSERT_NE(s1, nullptr);
    ASSERT_FALSE(s1->isBidir());
    ASSERT_FALSE(s1->isDestination());

    ASSERT_EQ(n1->section(0), ID_NULL);
    ASSERT_EQ(n1->section(1), ID_NULL);
    ASSERT_EQ(n2->section(0), ID_NULL);
    ASSERT_EQ(n2->section(1), ID_NULL);
    ASSERT_EQ(s1->start(), ID_NULL);
    ASSERT_EQ(s1->end(), ID_NULL);
    ASSERT_FALSE(s1->hasMetadata());
}

TEST(IoRead, BasicWithLink) {
    Model model = _read(R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": {
                "link": { "startNode":"n1", "startSlot":0,
                          "endNode":"n2", "endSlot":1 }
            }
        }
    ])json");

    auto *n1 = model.node("n1");
    ASSERT_NE(n1, nullptr);
    ASSERT_EQ(n1->type(), THRU);
    ASSERT_FALSE(n1->hasMetadata());

    auto *n2 = model.node("n2");
    ASSERT_NE(n2, nullptr);
    ASSERT_EQ(n2->type(), THRU);
    ASSERT_FALSE(n2->hasMetadata());

    auto *s1 = model.section("s1");
    ASSERT_NE(s1, nullptr);
    ASSERT_FALSE(s1->isBidir());
    ASSERT_FALSE(s1->isDestination());

    ASSERT_EQ(n1->section(0), "s1");
    ASSERT_EQ(n1->section(1), ID_NULL);
    ASSERT_EQ(n2->section(0), ID_NULL);
    ASSERT_EQ(n2->section(1), "s1");
    ASSERT_EQ(s1->start(), "n1");
    ASSERT_EQ(s1->end(), "n2");
    ASSERT_FALSE(s1->hasMetadata());
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
            "n2": {
                "type":"THRU",
                "metadata": {
                    "n2-key1": "apple",
                    "n2-key2": "orange"
                }
            }
        },
        {
            "s1": {
                "link": {
                    "startNode": "n1",
                    "startSlot": 0,
                    "endNode": "n2",
                    "endSlot": 1
                },
                "dir": "BIDIR",
                "dest": {
                    "address": "1.0.1",
                    "name": "My Destination",
                    "metadata": {
                        "d-key1": "grape",
                        "d-key2": "banana"
                    }
                },
                "metadata": {
                    "s1-key1": "tomato",
                    "s1-key2": "papaya"
                }
            }
        }
    ])json");

    auto *n1 = model.node("n1");
    ASSERT_NE(n1, nullptr);
    ASSERT_EQ(n1->type(), THRU);
    ASSERT_FALSE(n1->hasMetadata());

    auto *n2 = model.node("n2");
    ASSERT_NE(n2, nullptr);
    ASSERT_EQ(n2->type(), THRU);
    ASSERT_TRUE(n2->hasMetadata());
    ASSERT_EQ(n2->metadata().size(), 2);
    ASSERT_EQ(n2->metadata("n2-key1"), "apple");
    ASSERT_EQ(n2->metadata("n2-key2"), "orange");

    auto *s1 = model.section("s1");
    ASSERT_NE(s1, nullptr);
    ASSERT_TRUE(s1->isBidir());
    ASSERT_TRUE(s1->isDestination());
    ASSERT_EQ(s1->destination()->address(), "1.0.1");
    ASSERT_EQ(s1->destination()->name(), "My Destination");
    ASSERT_TRUE(s1->destination()->hasMetadata());
    ASSERT_EQ(s1->destination()->metadata().size(), 2);
    ASSERT_EQ(s1->destination()->metadata("d-key1"), "grape");
    ASSERT_EQ(s1->destination()->metadata("d-key2"), "banana");

    ASSERT_EQ(n1->section(0), "s1");
    ASSERT_EQ(n1->section(1), ID_NULL);
    ASSERT_EQ(n2->section(0), ID_NULL);
    ASSERT_EQ(n2->section(1), "s1");
    ASSERT_EQ(s1->start(), "n1");
    ASSERT_EQ(s1->end(), "n2");
    ASSERT_TRUE(s1->hasMetadata());
    ASSERT_EQ(s1->metadata().size(), 2);
    ASSERT_EQ(s1->metadata("s1-key1"), "tomato");
    ASSERT_EQ(s1->metadata("s1-key2"), "papaya");
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

    MUST_PASS("Basic link", R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": {
                "link": { "startNode":"n1", "startSlot":0,
                          "endNode":"n2", "endSlot":1 }
            }
        }
    ])json");

    // One field missing

    MUST_FAIL("startNode missing", R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": {
                "link": {                   "startSlot":0,
                          "endNode":"n2", "endSlot":1 }
            }
        }
    ])json");

    MUST_FAIL("startSlot missing", R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": {
                "link": { "startNode":"n1",
                          "endNode":"n2", "endSlot":1 }
            }
        }
    ])json");

    MUST_FAIL("endNode missing", R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": {
                "link": { "startNode":"n1", "startSlot":0,
                                          "endSlot":1 }
            }
        }
    ])json");

    MUST_FAIL("endSlot missing", R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": {
                "link": { "startNode":"n1", "startSlot":0,
                          "endNode":"n2",             }
            }
        }
    ])json");

    // Empty link section

    MUST_FAIL("empty link", R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": { "link": {} }
        }
    ])json");

    // One stray field

    MUST_FAIL("stray startNode", R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": { "link": { "startNode":"n1" } }
        }
    ])json");

    MUST_FAIL("stray startSlot", R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": { "link": { "startSlot":0 } }
        }
    ])json");

    MUST_FAIL("stray endNode", R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": { "link": { "endNode":"n2" } }
        }
    ])json");

    MUST_FAIL("stray endSlot", R"json([
        {
            "n1": { "type":"THRU" },
            "n2": { "type":"THRU" }
        },
        {
            "s1": { "link": { "endSlot":1 } }
        }
    ])json");
}

MUST_FAIL_TEST(IllegalLink, R"json([
    {
        "n1": { "type":"THRU" },
        "n2": { "type":"THRU" }
    },
    {
        "s1": { "startNode":"n1", "startSlot":0,
                "endNode":"n9", "endSlot":1 }
    }
])json")

TEST(IoRead, Metadata) {
    // Base variant

    MUST_PASS("Basic metadata", R"json([
        {},
        {
            "s1": {
                "metadata": {
                    "key1": "value"
                }
            }
        }
    ])json");

    // Empty metadata

    MUST_PASS("Empty metadata", R"json([
        {},
        {
            "s1": {
                "metadata": {}
            }
        }
    ])json");

    // Not an object

    MUST_FAIL("metadata is not an object", R"json([
        {},
        {
            "s1": {
                "metadata": [
                    "value"
                ]
            }
        }
    ])json");

    // Bad values

    MUST_FAIL("value is a number", R"json([
        {},
        {
            "s1": {
                "metadata": {
                    "key1": 123
                }
            }
        }
    ])json");

    MUST_FAIL("value is an object", R"json([
        {},
        {
            "s1": {
                "metadata": {
                    "key1": { "subkey": "subvalue" }
                }
            }
        }
    ])json");

    MUST_FAIL("value is null", R"json([
        {},
        {
            "s1": {
                "metadata": {
                    "key1": null
                }
            }
        }
    ])json");
}
