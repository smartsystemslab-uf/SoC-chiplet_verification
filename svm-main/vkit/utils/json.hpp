#pragma once

//
// Minimal wrapper so we can include JSON without external dependencies.
// Drops in the single-header nlohmann JSON implementation.
//

#include <nlohmann/json.hpp>

using json = nlohmann::json;
