#ifndef DEBUG_H
#define DEBUG_H

#include <cassert>

#ifdef NDEBUG
#define _DEBUG_ONLY(...) while (false) // NOLINT
#else
#define _DEBUG_ONLY(...) __VA_ARGS__ // NOLINT
#endif

#define _ASSERT(test, msg) assert((msg, test)) // NOLINT

#define _FAIL(msg) _ASSERT(msg, false) // NOLINT

#endif // DEBUG_H
