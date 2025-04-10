#include <filesystem>
#include <utility>
#include <not_implemented.h>
#include "../include/client_logger_builder.h"
#include <not_implemented.h>

logger_builder& client_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity) &
{

    // if (!std::filesystem::exists(stream_file_path)) {
    //     std::ofstream create_file(stream_file_path);
    //     if (!create_file) {
    //         throw std::runtime_error("Unable to create file: " + stream_file_path);
    //     }
    // }

    // std::filesystem::path new_file_path;
    // try {
    //     new_file_path = std::filesystem::canonical(stream_file_path);
    // } catch (const std::filesystem::filesystem_error& e) {
    //     throw std::runtime_error("Canonical path for: " + stream_file_path + " threw exception:\n" + e.what() + "\n");
    // }


    auto iter = _output_streams.find(severity);
    if (iter == _output_streams.end()) {
        std::forward_list<client_logger::refcounted_stream> list;
        list.emplace_front(stream_file_path);
        _output_streams[severity] = std::make_pair(std::move(list), false);
    } else {

        // for (const auto& file_stream : iter->second.first) {
        //     try {
        //         std::filesystem::path existing_file_path = std::filesystem::canonical(file_stream._stream.first);
        //         if (existing_file_path == new_file_path) {
        //             return *this; // файл уже добавлен
        //         }
        //     } catch (const std::filesystem::filesystem_error& e) {
        //         throw std::runtime_error("error in canonical");
        //     }
        // }
        //
        // iter->second.first.emplace_front(stream_file_path);

        auto& streams = iter->second.first;
        bool is_existing = std::any_of(
            streams.begin(),
            streams.end(),
            [&](const client_logger::refcounted_stream &s) {
                return std::filesystem::absolute(s._stream.first) == std::filesystem::absolute(stream_file_path);;
            });


        if (!is_existing) {
            streams.emplace_front(stream_file_path);
        }
    }
    return *this;
}

using namespace nlohmann;

logger_builder& client_logger_builder::add_console_stream(
    logger::severity severity) &
{
    auto iter = _output_streams.find(severity);
    if (iter == _output_streams.end()) {
        _output_streams[severity] = std::make_pair(std::forward_list<client_logger::refcounted_stream>(), true);
    } else {
        iter->second.second = true;
    }
    return *this;
}

logger_builder& client_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path) &
{
    std::ifstream config_file(configuration_file_path);
    if (!config_file.is_open()) {
        throw std::runtime_error("Failed to open configuration file");
    }

    json config = json::parse(config_file);
    if (config.is_discarded() || config.is_null()) {
        throw std::runtime_error("Failed to parse configuration file");
    }

    json::json_pointer config_path(configuration_path);
    if (config.contains(config_path)) {
        json& settings = config[config_path];

        // { trace, debug, information, warning, error, critical };
        if (settings.contains("trace")) parse_severity(logger::severity::information, settings["trace"]);
        if (settings.contains("debug")) parse_severity(logger::severity::information, settings["debug"]);
        if (settings.contains("information")) parse_severity(logger::severity::information, settings["information"]);
        if (settings.contains("warning")) parse_severity(logger::severity::information, settings["warning"]);
        if (settings.contains("error")) parse_severity(logger::severity::information, settings["error"]);
        if (settings.contains("critical")) parse_severity(logger::severity::information, settings["critical"]);

        if (settings.contains("format")) {
            _format = settings["format"].get<std::string>();
        }
    } else {
        throw std::runtime_error("Failed to parse configuration file. No such configuration_path");
    }

    return *this;
}

logger_builder& client_logger_builder::clear() &
{
    _format = "%m";
    _output_streams.clear();
    return *this;
}

logger *client_logger_builder::build() const
{
    return new client_logger(_output_streams, _format);
}

logger_builder& client_logger_builder::set_format(const std::string &format) &
{
    _format = format;
    return *this;
}

void client_logger_builder::parse_severity(logger::severity sev, nlohmann::json& j)
{
    if (!j.contains("file_paths") || !j["file_paths"].is_array() || !j.contains("console") || !j["console"].is_boolean()) {
        throw std::runtime_error("Failed to parse configuration file. Invalid format of severity, the severity object must contain 'file_paths' and 'console'");
    }

    for (const auto & file_path : j["file_paths"]) {
        if (!file_path.is_string()) {
            throw std::runtime_error("Failed to parse configuration file. Each file path must be a string");
        }

        std::string path = file_path.get<std::string>();
        add_file_stream(path, sev);
    }

    if (j['console'].get<bool>()) {
        add_console_stream(sev);
    }
}

logger_builder& client_logger_builder::set_destination(const std::string &format) &
{
    throw not_implemented("logger_builder *client_logger_builder::set_destination(const std::string &format)", "invalid call");
}
