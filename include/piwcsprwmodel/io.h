#ifndef PIWCS_PRW_MODEL_IO
#define PIWCS_PRW_MODEL_IO

#include "fwd.h"
#include "model.h"
#include <iosfwd>
#include <stdexcept>
#include <string>

/**
 * @file
 *
 * This header declares the I/O routines of piwcsprwmodel.
 */

namespace piwcs::prw {

/**
 * Reports that `readModel` attempted to construct an illegal Model.
 *
 * This exception is _not_ raised by any methods of Model itself.
 */
class IllegalModelError : public std::runtime_error {
  public:
    IllegalModelError(const std::string &what_arg)
        : std::runtime_error(what_arg) {}
    IllegalModelError(const char *what_arg) : std::runtime_error(what_arg) {}
};

/**
 * Reads a PRW model definition from the provided `istream`, constructs a new
 * Model object and returns it.
 *
 * Note that this function does not guarantee that the resulting Model is
 * complete, only that it is consistent.
 *
 * @exception std::ios_base::failure if an IO error occurs or if data is
 * formatted incorrectly
 * @exception IllegalModelError if the data describes an inconsistent Model
 *
 * @param in an input stream open in binary mode to read the definition from
 *
 * @return the Model desribed by the data in `in`
 */
Model readModel(std::istream &in);

/**
 * Reads a PRW model definition from the file `filename`, constructs a new
 * Model object and returns it.
 *
 * Note that this function does not guarantee that the resulting Model is
 * complete, only that it is consistent.
 *
 * @exception std::ios_base::failure if an IO error occurs or if data is
 * formatted incorrectly
 * @exception IllegalModelError if the data describes an inconsistent Model
 *
 * @param filename the relative path of the file to read
 *
 * @return the Model desribed by the data in `in`
 */
Model readModel(const std::string &filename);

/**
 * Writes the PRW model definition of the provided Model into the output stream.
 *
 * @exception std::ios_base::failure if an IO error occurs
 *
 * @param out an output stream open in binary mode to write the definition to
 * @param model the Model to serialize
 */
void writeModel(std::ostream &out, const Model &model);

/**
 * Writes the PRW model definition of the provided Model into file `filename`.
 *
 * Existing files will be overwritten silently.
 *
 * @exception std::ios_base::failure if an IO error occurs
 *
 * @param filename the relative path of the file to write into
 * @param model the Model to serialize
 */
void writeModel(const std::string &filename, const Model &model);

} // namespace piwcs::prw

#endif // PIWCS_PRW_MODEL_IO
