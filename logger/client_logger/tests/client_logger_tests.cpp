#include <gtest/gtest.h>
#include "../include/client_logger.h"
#include "../include/client_logger_builder.h"

#include <filesystem>

int main(int argc, char *argv[])
{
    try {
        testing::InitGoogleTest(&argc, argv);

        client_logger_builder builder;

        builder.add_file_stream("a.txt" ,logger::severity::trace).
                add_file_stream("b.txt" ,logger::severity::trace).
                add_file_stream("b.txt" ,logger::severity::trace).
                add_file_stream("a.txt", logger::severity::debug).
                add_file_stream("../tests/a.txt", logger::severity::debug).
                add_console_stream(logger::severity::trace).
                set_format("[%d %t][%s] %m");

        builder.transform_with_configuration("../tests/set.json", "log");

        std::unique_ptr<logger> log(builder.build());

        log->trace("it is a very long message!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!").debug("dd");

        std::unique_ptr<logger> logger2(builder.build());

        logger2->trace("From second logger");
        logger2->trace("Another message from second logger");

        logger2->error("ERROR MSG second logger");

        return RUN_ALL_TESTS();
    } catch (const std::exception &ex) {
        std::cerr << "exception : " << ex.what() << std::endl;
        return 0;
    }
}