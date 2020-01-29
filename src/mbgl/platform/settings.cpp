#include <mbgl/platform/settings.hpp>

#ifndef NDEBUG
#include <algorithm>
#include <array>
#include <mapbox/eternal.hpp>
#endif

#include <mutex>

namespace mbgl {
namespace platform {

#ifndef NDEBUG
namespace {

constexpr const char* VENDOR_PREFIX = "vendor-";

using ValidatorFN = bool (*)(const mapbox::base::Value&);
bool validateThreadPriority(const mapbox::base::Value&);

MAPBOX_ETERNAL_CONSTEXPR const auto supportedSettings = mapbox::eternal::hash_map<mapbox::eternal::string, ValidatorFN>(
    {{EXPERIMENTAL_THREAD_PRIORITY, validateThreadPriority}});

bool validateThreadPriority(const mapbox::base::Value& value) {
    if (value.is<mapbox::base::NullValue>()) return true;
    const std::array<std::string, 4> supportedTypes{EXPERIMENTAL_THREAD_PRIORITY_ARG_WORKER,
                                                    EXPERIMENTAL_THREAD_PRIORITY_ARG_FILE,
                                                    EXPERIMENTAL_THREAD_PRIORITY_ARG_NETWORK,
                                                    EXPERIMENTAL_THREAD_PRIORITY_ARG_DATABASE};

    if (auto* obj = value.getObject()) {
        for (auto pair : *obj) {
            if (!pair.second.getDouble() ||
                std::find(supportedTypes.begin(), supportedTypes.end(), pair.first) == supportedTypes.end()) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool isValidSetting(const std::string& key, mapbox::base::Value& value) {
    auto it = supportedSettings.find(key.c_str());
    if (it != supportedSettings.end()) {
        return it->second(value);
    }
    return false;
}

bool isValidKey(const std::string& key) {
    if (key.rfind(VENDOR_PREFIX, 0) != std::string::npos) {
        return true;
    }
    return supportedSettings.find(key.c_str()) != supportedSettings.end();
}
} // namespace
#endif

class Settings::Impl {
public:
    mapbox::base::Value settings = mapbox::base::ValueObject{};
    std::mutex mutex;
};

Settings::Settings() : impl(std::make_unique<Impl>()) {}

Settings& Settings::getInstance() noexcept {
    static Settings instance;
    return instance;
}

void Settings::set(const std::string& key, mapbox::base::Value value) noexcept {
    assert(isValidSetting(key, value));
    assert(impl->settings.getObject());

    std::lock_guard<std::mutex> lock(impl->mutex);
    impl->settings.getObject()->operator[](key) = std::move(value);
}

mapbox::base::Value Settings::getValue(const std::string& key, const mapbox::base::Value& value) const noexcept {
    if (value.is<mapbox::base::NullValue>() || !value.is<mapbox::base::ValueObject>()) {
        return value;
    }
    auto* object = value.getObject();
    auto it = object->find(key);
    if (it != object->end()) {
        return it->second;
    }
    return {};
}

mapbox::base::Value Settings::getValue(const std::string& key) const noexcept {
    assert(isValidKey(key));
    std::lock_guard<std::mutex> lock(impl->mutex);
    return getValue(key, impl->settings);
}

} // namespace platform
} // namespace mbgl
