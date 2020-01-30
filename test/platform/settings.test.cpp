#include <gtest/gtest.h>
#include <mbgl/platform/settings.hpp>

using namespace mbgl;
using namespace mbgl::platform;

TEST(Settings, SetAndGet) {
    auto& settings = Settings::getInstance();
    auto value = settings.get(EXPERIMENTAL_THREAD_PRIORITY);
    EXPECT_TRUE(value.is<mapbox::base::NullValue>());

    mapbox::base::Value lowPrioValue{19.0};
    mapbox::base::ValueObject priorities{{EXPERIMENTAL_THREAD_PRIORITY_ARG_WORKER, lowPrioValue}};
    settings.set(EXPERIMENTAL_THREAD_PRIORITY, std::move(priorities));

    auto threadPriority = settings.get(EXPERIMENTAL_THREAD_PRIORITY);
    EXPECT_FALSE(threadPriority.is<mapbox::base::NullValue>());
    EXPECT_TRUE(threadPriority.is<mapbox::base::ValueObject>());

    auto* object = threadPriority.getObject();
    auto it = object->find(EXPERIMENTAL_THREAD_PRIORITY_ARG_WORKER);
    EXPECT_TRUE(it != object->end());
    EXPECT_EQ(it->second, lowPrioValue);

    auto leafValue = settings.get(EXPERIMENTAL_THREAD_PRIORITY, EXPERIMENTAL_THREAD_PRIORITY_ARG_WORKER);
    EXPECT_EQ(leafValue, lowPrioValue);

    auto nullValue = settings.get(EXPERIMENTAL_THREAD_PRIORITY, "UNKNOWN", "VALUE");
    EXPECT_TRUE(nullValue.is<mapbox::base::NullValue>());

    auto vendorValue = settings.get("vendor-setting", "plugin-name");
    EXPECT_TRUE(vendorValue.is<mapbox::base::NullValue>());
    settings.set(EXPERIMENTAL_THREAD_PRIORITY, mapbox::base::NullValue());
}
