#include "run.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <iostream>

#include "greeter/Greeter.h"

// The core logic is now in a testable function
int run_app([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    // Create a logger that writes to "app_log.txt"
    auto file_logger{spdlog::basic_logger_mt("app_logger", "app_log.txt")};
    spdlog::set_default_logger(file_logger);

    spdlog::info("App starting up");

    Greeter my_greeter;
    std::cout << my_greeter.greet() << '\n';

    // Flush logs to ensure they're written
    spdlog::shutdown();
    return 0;
}
