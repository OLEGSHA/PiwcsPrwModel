#include <piwcsprwmodel/io.h>

#include <fstream>
#include <iostream>

namespace piwcs::prw {

void writeModel(std::ostream &out, const Model &model) {
    // TODO implement
    (void)out;
    (void)model;
}

void writeModel(const std::string &filename, const Model &model) {
    std::ofstream out(filename);
    out.exceptions(std::ios_base::failbit); // throws if the file failed to open
    writeModel(out, model);
}

} // namespace piwcs::prw
