#pragma once

#include "core.h"

namespace nbn::core::interfaces {

/**
 * @file interfaces/Interface.h
 * @brief Root interface for polymorphic core contracts.
 */

/**
 * @struct Interface
 * @brief Base polymorphic interface type.
 */

struct nbn_export_core Interface {
    virtual ~Interface() = default;
};

}  // namespace nbn::core::interfaces
