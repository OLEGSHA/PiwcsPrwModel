#ifndef CORRECTNESS_H
#define CORRECTNESS_H

#include <piwcsprwmodel/algorithms.h>

namespace piwcs::prw {

bool isLocallyCorrect(const Model &model, IdRef id);

} // namespace piwcs::prw

#endif // CORRECTNESS_H
