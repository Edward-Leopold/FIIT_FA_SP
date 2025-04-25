#include <not_implemented.h>
#include <httplib.h>
#include "../include/server_logger.h"

#ifdef _WIN32
#include <process.h>
#elif
#include <unistd.h>
#endif

server_logger::~server_logger() noexcept
{
    throw not_implemented("server_logger::~server_logger() noexcept", "your code should be here...");
}

logger& server_logger::log(
    const std::string &text,
    logger::severity severity) &
{
    throw not_implemented("const logger& server_logger::log(const std::string &, logger::severity) const &", "your code should be here...");
}

server_logger::server_logger(const std::string& dest,
                             const std::unordered_map<logger::severity, std::pair<std::string, bool>> &streams)
{
    throw not_implemented("server_logger::server_logger(const std::string& ,const std::unordered_map<logger::severity, std::pair<std::string, bool>> &)", "your code should be here...");
}

int server_logger::inner_getpid()
{
#ifdef _WIN32
    return ::_getpid();
#elif
    return getpid();
#endif
}

server_logger::server_logger(const server_logger &other)
{
    throw not_implemented("server_logger::server_logger(const server_logger &other)", "your code should be here...");
}

server_logger &server_logger::operator=(const server_logger &other)
{
    throw not_implemented("server_logger &server_logger::operator=(const server_logger &other)", "your code should be here...");
}

server_logger::server_logger(server_logger &&other) noexcept
{
    throw not_implemented("server_logger::server_logger(server_logger &&other) noexcept", "your code should be here...");
}

server_logger &server_logger::operator=(server_logger &&other) noexcept
{
    throw not_implemented("server_logger &server_logger::operator=(server_logger &&other) noexcept", "your code should be here...");
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