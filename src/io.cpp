#include <piwcsprwmodel/io.h>

#include <fstream>
#include <iostream>

namespace piwcs::prw {

Model readModel(std::istream &in) {
    // TODO implement
    (void)in;
    return {};
}

void writeModel(std::ostream &out, const Model &model) {
    // TODO implement
    (void)out;
    (void)model;
}

Model readModel(const std::string &filename) {
    std::ifstream in(filename, std::ios_base::binary);
    in.exceptions(std::ios_base::failbit); // throws if the file failed to open
    return readModel(in);
}

void writeModel(const std::string &filename, const Model &model) {
    std::ofstream out(filename, std::ios_base::binary);
    out.exceptions(std::ios_base::failbit); // throws if the file failed to open
    writeModel(out, model);
}

} // namespace piwcs::prw
