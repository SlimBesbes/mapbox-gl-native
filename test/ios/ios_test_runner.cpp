#include "ios_test_runner.hpp"

#include <mbgl/test.hpp>

#include <mbgl/util/logging.hpp>

#include <vector>

#define EXPORT __attribute__((visibility("default")))

EXPORT
bool TestRunner::startTest() {
    bool success = false;

    std::vector<std::string> arguments = {"mbgl-test-runner", "--gtest_output=xml:test/results.xml"};
    std::vector<char*> argv;
    for (const auto& arg : arguments) {
        argv.push_back((char*)arg.data());
    }
    argv.push_back(nullptr);

    mbgl::Log::Info(mbgl::Event::General, "Start TestRunner");
    int status = mbgl::runTests(argv.size(), argv.data());
    mbgl::Log::Info(mbgl::Event::General, "TestRunner finished with status: '%d'", status);
    success = status ? false : true;

    return success;
}
