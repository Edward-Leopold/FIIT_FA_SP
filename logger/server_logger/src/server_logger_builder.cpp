#include <not_implemented.h>
#include "../include/server_logger_builder.h"
#include <fstream>
#include <filesystem>

logger_builder& server_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity) &
{
    auto iter = _output_streams.find(severity);
    if (iter == _output_streams.end()) {
        iter = _output_streams.emplace(severity, std::make_pair(stream_file_path, false)).first;
    }

    iter->second.first = stream_file_path;
    return *this;
}

logger_builder& server_logger_builder::add_console_stream(
    logger::severity severity) &
{
    auto iter = _output_streams.find(severity);
    if (iter == _output_streams.end()){
        iter = _output_streams.emplace(severity, std::make_pair(std::string(), false)).first;
    }
    iter->second.second = true;
    return *this;
}

logger_builder& server_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path) &
{
    using json = nlohmann::json;

    std::ifstream config_file(configuration_file_path);
    if (!config_file.is_open()) {
        throw std::runtime_error("Failed to open configuration file");
    }

    json config = json::parse(config_file);
    if (config.is_discarded() || config.is_null()) {
        throw std::runtime_error("Failed to parse configuration file");
    }

    // json::json_pointer config_path(configuration_path);
    if (config.contains(configuration_path)) {
        json& settings = config[configuration_path];

        // { trace, debug, information, warning, error, critical };
        if (settings.contains("trace")) parse_severity(logger::severity::trace, settings["trace"]);
        if (settings.contains("debug")) parse_severity(logger::severity::debug, settings["debug"]);
        if (settings.contains("information")) parse_severity(logger::severity::information, settings["information"]);
        if (settings.contains("warning")) parse_severity(logger::severity::warning, settings["warning"]);
        if (settings.contains("error")) parse_severity(logger::severity::error, settings["error"]);
        if (settings.contains("critical")) parse_severity(logger::severity::critical, settings["critical"]);

        if (settings.contains("format")) {
            _format = settings["format"].get<std::string>();
        }
    } else {
        throw std::runtime_error("Failed to parse configuration file. No such configuration_path");
    }

    return *this;
}

logger_builder& server_logger_builder::clear() &
{
    throw not_implemented("logger_builder& server_logger_builder::clear() &", "your code should be here...");
}

logger *server_logger_builder::build() const
{
    throw not_implemented("logger *server_logger_builder::build() const", "your code should be here...");
}

logger_builder& server_logger_builder::set_destination(const std::string& dest) &
{
    throw not_implemented("logger_builder& server_logger_builder::set_destination(const std::string&) &", "your code should be here...");
}

logger_builder& server_logger_builder::set_format(const std::string &format) &
{
    throw not_implemented("logger_builder& server_logger_builder::set_format(const std::string &) &", "your code should be here...");
}
