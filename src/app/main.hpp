#include "run.h"

// The main function is just a thin entry point
int main(int argc, char* argv[])  // NOLINT
{
  return run_app(argc, argv);
}
// #include "greeter/Greeter.h"
// #include "person.pb.h"
// #include "spdlog/spdlog.h"

// // #include "tabulate/table.hpp"
// // #include <boost/program_options.hpp>
// #include <iostream>
// #include <string>

// int main() {
//     spdlog::info(get_greeting());
//     Greeter my_greeter;
//     std::cout << my_greeter.greet();

//     // Create a message
//     Person p1;
//     p1.set_name("Jane Doe");
//     p1.set_id(1234);
//     p1.set_email("jane.doe@example.com");

//     // Sender serializes the message
//     std::string serialized;
//     p1.SerializeToString(&serialized);

//     // Send the message
//     // ...

//     // Receiver deserializes and uses the message
//     Person p2;
//     p2.ParseFromString(serialized);
//     std::cout << "Hello " << p2.name() << '\n';
// std::string greeting;
// boost::program_options::options_description desc;
// desc.add_options()(
//     "greeting",
//     boost::program_options::value<std::string>(&greeting));

// boost::program_options::variables_map vm;
// boost::program_options::store(
//     boost::program_options::parse_command_line(
//         argc, argv, desc),
//     vm);

// boost::program_options::notify(vm);

// if (vm.count("greeting")) {
//     std::cout << greeting << " from the GreeterApp\n";
//     spdlog::info(get_greeting());
// } else {
//     std::cout << "No --greeting argument provided";
// }

// tabulate::Table Greetings;
// Greetings.format().locale("C");
// Greetings.add_row({"Language", "Greeting"});
// Greetings.add_row({"English", "Hello, World!"});
// Greetings.add_row({"German", "Hallo, Welt!"});
// Greetings.add_row({"French", "Bonjour, le Monde!"});
// std::cout << Greetings << '\n';

//     return 0;
// }
