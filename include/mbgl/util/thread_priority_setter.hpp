#pragma once

#include <mbgl/platform/settings.hpp>
#include <mbgl/util/platform.hpp>

namespace mbgl {
namespace util {

// Thread priority strategies
//
// ThreadPrioritySetterLow: Sets thread priority to low. Platform must set
// correct thread priority via implementing platform::makeThreadLowPriority()
//
// ThreadPrioritySetterCustom: Sets thread priority for a thread type based on
// a setting platform::EXPERIMENTAL_THREAD_PRIORITY for a provided thread type.

struct ThreadPrioritySetter {};

struct ThreadPrioritySetterLow final : public ThreadPrioritySetter {
    void setThreadPriority() { platform::makeThreadLowPriority(); }
};

struct ThreadPrioritySetterCustom final : public ThreadPrioritySetter {
    ThreadPrioritySetterCustom(std::string threadType_) : threadType(std::move(threadType_)) {}
    void setThreadPriority() {
        auto& settings = platform::Settings::getInstance();
        auto value = settings.get(platform::EXPERIMENTAL_THREAD_PRIORITY, threadType);
        if (auto* priority = value.getDouble()) {
            platform::setCurrentThreadPriority(*priority);
        } else {
            platform::makeThreadLowPriority();
        }
    }
    std::string threadType;
};

} // namespace util
} // namespace mbgl
