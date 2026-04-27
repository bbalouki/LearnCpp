#include "run.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <argparse/argparse.hpp>
#include <iostream>
#include <string>

#include "greeter/Greeter.h"
#include "person.pb.h"
#include "tabulate/table.hpp"

// The core logic is now in a testable function
int run_app([[maybe_unused]] int   argc,
            [[maybe_unused]] char* argv[]) {  // NOLINT

    // Create a logger that writes to "app_log.txt"
    auto file_logger {spdlog::basic_logger_mt("app_logger", "app_log.txt")};
    spdlog::set_default_logger(file_logger);

    spdlog::info("App starting up");

    Greeter my_greeter;
    std::cout << my_greeter.greet() << '\n';

    // Create a message
    Person p1;
    p1.set_name("Jane Doe");
    p1.set_id(1234);
    p1.set_email("jane.doe@example.com");

    // Sender serializes the message
    std::string serialized;
    p1.SerializeToString(&serialized);

    // Send the message
    // ...

    // Receiver deserializes and uses the message
    Person p2;
    p2.ParseFromString(serialized);
    std::cout << "Hello " << p2.name() << '\n';
    argparse::ArgumentParser program("GreeterApp");

    program.add_argument("--greeting").help("The greeting message to display");

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << '\n';
        std::cerr << program;
        std::exit(1);
    }

    if (program.present("--greeting")) {
        auto greeting = program.get<std::string>("--greeting");
        std::cout << greeting << " from the GreeterApp\n";
    } else {
        std::cout << "No --greeting argument provided\n";
    }

    tabulate::Table Greetings;
    Greetings.format().locale("C");
    Greetings.add_row({"Language", "Greeting"});
    Greetings.add_row({"English", "Hello, World!"});
    Greetings.add_row({"German", "Hallo, Welt!"});
    Greetings.add_row({"French", "Bonjour, le Monde!"});
    std::cout << Greetings << '\n';

    // Flush logs to ensure they're written
    spdlog::shutdown();
    return 0;
}
