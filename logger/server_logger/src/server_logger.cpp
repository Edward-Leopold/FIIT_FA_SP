#include <not_implemented.h>
#include <httplib.h>
#include "../include/server_logger.h"
#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

using json = nlohmann::json;

server_logger::~server_logger() noexcept
{
    nlohmann::json body = {
        {"pid", inner_getpid()}
    };
    httplib::Headers headers = {{"Content-Type", "application/json"}};
    _client.Post("/logger/stop", headers, body.dump(), "application/json");
}

logger& server_logger::log(
    const std::string &text,
    logger::severity severity) &
{
    std::string formatted = make_format(text, severity);
    nlohmann::json body = {
        {"pid", inner_getpid()},
        {"severity", severity_to_string(severity)},
        {"message", formatted}
    };
    httplib::Headers headers = {{"Content-Type", "application/json"}};
    _client.Post("/logger/log", headers, body.dump(), "application/json");

    auto it = _streams.find(severity);
    if (it != _streams.end()) {
        if (it->second.second) {
            std::cout << formatted << std::endl;
        }
    }
    return *this;
}

server_logger::server_logger(const std::string& dest,
                             const std::unordered_map<logger::severity, std::pair<std::forward_list<std::string>, bool>> &streams, const std::string &format):
    _client(dest),
    _streams(streams),
    _dest(dest),
    _format(format)
{
    std::cout << "builded!" << std::endl;
    int pid = inner_getpid();
    for (const auto &[sev, stream_info]: streams) {
        for (const auto& path : stream_info.first) {
            json body = {
                {"pid", pid},
                {"severity", severity_to_string(sev)},
                {"path", path},
                {"console", stream_info.second}
            };
            httplib::Headers headers = {{"Content-Type", "application/json"}};
            _client.Post("/logger/init", headers, body.dump(), "application/json");
        }
    }

}

int server_logger::inner_getpid()
{
#ifdef _WIN32
    return ::_getpid();
#else
    return getpid();
#endif
}


server_logger::server_logger(server_logger &&other) noexcept:
    _client(std::move(other._client)),
    _streams(std::move(other._streams)),
    _dest(std::move(other._dest)),
    _format(std::move(other._format))
{
}

server_logger &server_logger::operator=(server_logger &&other) noexcept
{
    if (this != &other) {
        //closing (this) connection with old destination
        int this_pid = inner_getpid();
        nlohmann::json body_for_this_old = {{"pid", this_pid}};
        _client.Post("/logger/stop", "application/json", body_for_this_old.dump());

        _client = std::move(other._client);
        _streams = std::move(other._streams);
        _dest = std::move(other._dest);
        _format = std::move(other._format);

        //openning new connection with moved destination
        for (const auto &[sev, stream_info]: _streams) {
            for (const auto& path : stream_info.first) {
                json body = {
                    {"pid", this_pid},
                    {"severity", severity_to_string(sev)},
                    {"path", path},
                    {"console", stream_info.second}
                };
                httplib::Headers headers = {{"Content-Type", "application/json"}};
                _client.Post("/logger/init", headers, body.dump(), "application/json");
            }
        }
    }
    return *this;
}

server_logger::flag server_logger::char_to_flag(const char c) noexcept
{
    // { DATE, TIME, SEVERITY, MESSAGE, NO_FLAG };
    switch (c) {
        case 'd':
            return flag::DATE;
        case 't':
            return flag::TIME;
        case 's':
            return flag::SEVERITY;
        case 'm':
            return flag::MESSAGE;
        default:
            return flag::NO_FLAG;
    }
}

std::string server_logger::make_format(const std::string &message, severity sev) const
{
    std::stringstream res;
    for (size_t i = 0; i < _format.size(); ++i) {
        if (_format[i] == '%' && i + 1 < _format.size()) {
            switch (char_to_flag(_format[i + 1])) {
                case flag::DATE:
                    res << current_date_to_string();
                break;
                case flag::TIME:
                    res << current_time_to_string();
                break;
                case flag::SEVERITY:
                    res << severity_to_string(sev);
                break;
                case flag::MESSAGE:
                    res << message;
                break;
                default:
                    break;
            }
            ++i;
        } else {
            res << _format[i];
        }
    }

    return res.str();
}