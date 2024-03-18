#ifndef PIWCS_PRW_MODEL_METADATA
#define PIWCS_PRW_MODEL_METADATA

#include "fwd.h"
#include "idmap.h"
#include "util.h"
#include <optional>

/**
 * @file
 *
 * This header declares internals for metadata support.
 */

namespace piwcs::prw {

/**
 * A container of metadata.
 */
using Metadata = IdMap<std::string>;

namespace detail {

/**
 * A base class for objects that may have metadata.
 */
class HasMetadata {

  private:
    std::optional<Metadata> m_meta{};

  public:
    /**
     * Checks whether there are any metadata records for this object.
     *
     * @return true if and only if there is at least one metadata record
     */
    [[nodiscard]] bool hasMetadata() const;

    /**
     * Provides access to the backing metadata storage.
     *
     * The lifetime of this reference matches the lifetime of this object. It
     * can be used to modify metadata, and all changes to metadata will be
     * visible through it.
     *
     * If this object did not have any existing metadata records, calling this
     * method will likely result in memory allocation. Use hasMetadata() to
     * avoid unneeded allocations.
     *
     * @return a live writable view of the metadata of this object
     */
    [[nodiscard]] Metadata &metadata();

    /**
     * Returns the metadata of this object as a map.
     *
     * The lifetime of this reference is at least the lifetime of this object.
     * If no metadata records exist for this object, the returned map may not
     * reflect future changes to metadata; if there is at least one metadata
     * record, the returned map is guaranteed to reflect future changes.
     *
     * Unlike the non-const counterpart of this method, calls to this method
     * cannot result in memory allocation.
     *
     * @return a map view of the metadata of this object
     */
    [[nodiscard]] const Metadata &metadata() const;

    /**
     * Checks whether a metadata record with the given key exists.
     *
     * @param key the key to look up
     *
     * @return whether key maps to a metadata value
     */
    [[nodiscard]] bool hasMetadata(IdRef key) const;

    /**
     * Obtains a writable metadata record for the given key.
     *
     * This reference is valid until the next call to non-const metadata(IdRef).
     * It may be used to read or write the value for the record. If the record
     * did not exist previously, it is created.
     *
     * @param key the key to look up
     *
     * @return a writable reference to the requested metadata record
     */
    [[nodiscard]] std::string &metadata(IdRef key);

    /**
     * Returns the value of the metadata record for the given key.
     *
     * This view is valid until the next call to non-const metadata(IdRef).
     *
     * If the record was not found, an empty string ("") is returned.
     *
     * @param key the key to look up
     *
     * @return a view of the metadata value for the requested record or ""
     */
    [[nodiscard]] std::string_view metadata(IdRef key) const;
};

} // namespace detail

} // namespace piwcs::prw

#endif // PIWCS_PRW_MODEL_METADATA
