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
    /**
     * Constructs a new IllegalModelError.
     *
     * @param what_arg an explanation
     */
    IllegalModelError(const std::string &what_arg)
        : std::runtime_error(what_arg) {}

    /**
     * Constructs a new IllegalModelError.
     *
     * @param what_arg an explanation
     */
    IllegalModelError(const char *what_arg) : std::runtime_error(what_arg) {}
};

/**
 * Reports that `readModel` could not parse input.
 */
class InvalidFormatError : public std::runtime_error {
  public:
    /**
     * Constructs a new InvalidFormatError.
     *
     * @param what_arg an explanation
     */
    InvalidFormatError(const std::string &what_arg)
        : std::runtime_error(what_arg) {}

    /**
     * Constructs a new InvalidFormatError.
     *
     * @param what_arg an explanation
     */
    InvalidFormatError(const char *what_arg) : std::runtime_error(what_arg) {}
};

/**
 * Reads a PRW model definition from the provided `istream`, constructs a new
 * Model object and returns it.
 *
 * Note that this function does not guarantee that the resulting Model is
 * complete, only that it is consistent.
 *
 * @exception std::ios_base::failure if an IO error occurs
 * @exception InvalidFormatError if input could not be parsed
 * @exception IllegalModelError if the data describes an inconsistent Model
 *
 * @param in an input stream open in text mode to read the definition from
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
 * @exception std::ios_base::failure if an IO error occurs
 * @exception InvalidFormatError if input could not be parsed
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
 * @param out an output stream open in text mode to write the definition to
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
