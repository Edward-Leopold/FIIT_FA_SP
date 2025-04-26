#include "server.h"
#include <server_logger_builder.h>
#include <unistd.h>

int main() {
    std::cout << "PID of client is: " << getpid() << std::endl;

    server_logger_builder builder;

    builder.add_file_stream("a.txt", logger::severity::trace).add_file_stream("b.txt", logger::severity::debug).
            add_console_stream(logger::severity::trace).add_file_stream("a.txt", logger::severity::information);

    std::unique_ptr<logger> log(builder.build());

    log->trace("good").debug("debug");

    log->trace("IT is a very long strange message !!!!!!!!!!%%%%%%%%\tzdtjhdjh").
		information("bfldknbpxjxjvpxvjbpzjbpsjbpsjkgbpsejegpsjpegesjpvbejpvjzepvgjs");
}