#pragma once

#include <mapbox/value.hpp>

#include <memory>

namespace mbgl {
namespace platform {

// EXPERIMENTAL_THREAD_PRIORITY object may contain keys defined below.
// The value type for a key is double.
// .
// └── EXPERIMENTAL_THREAD_PRIORITY
//     ├── EXPERIMENTAL_THREAD_PRIORITY_ARG_WORKER
//     ├── EXPERIMENTAL_THREAD_PRIORITY_ARG_FILE
//     ├── EXPERIMENTAL_THREAD_PRIORITY_ARG_NETWORK
//     └── EXPERIMENTAL_THREAD_PRIORITY_ARG_DATABASE

constexpr const char* EXPERIMENTAL_THREAD_PRIORITY = "experimental-thread-priority";
constexpr const char* EXPERIMENTAL_THREAD_PRIORITY_ARG_WORKER = "worker";
constexpr const char* EXPERIMENTAL_THREAD_PRIORITY_ARG_FILE = "file";
constexpr const char* EXPERIMENTAL_THREAD_PRIORITY_ARG_NETWORK = "network";
constexpr const char* EXPERIMENTAL_THREAD_PRIORITY_ARG_DATABASE = "database";

// Settings class provides non-persistent, in-process key-value storage.
class Settings final {
public:
    // Returns singleton instance
    static Settings& getInstance() noexcept;

    // Sets setting value for specified key.
    void set(const std::string& key, mapbox::base::Value value) noexcept;

    // Gets setting value for specified key.
    template <typename First>
    mapbox::base::Value get(const First& key) const noexcept {
        return getValue(key);
    }

    // Gets setting value for specified key by recursing through objects.
    // Example: settings.get("parentObject", "childObject", "property")
    template <typename First, typename... Keys>
    mapbox::base::Value get(const First& key, const Keys&... keys) const noexcept {
        return get(getValue(key), keys...);
    }

private:
    Settings();
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

    template <typename First>
    mapbox::base::Value get(const mapbox::base::Value& value, const First& first) const noexcept {
        return getValue(first, value);
    }

    template <typename First, typename... Keys>
    mapbox::base::Value get(const mapbox::base::Value& value, const First& first, const Keys&... keys) const noexcept {
        return get(getValue(first, value), keys...);
    }

    mapbox::base::Value getValue(const std::string& key) const noexcept;
    mapbox::base::Value getValue(const std::string& key, const mapbox::base::Value& value) const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace platform
} // namespace mbgl
