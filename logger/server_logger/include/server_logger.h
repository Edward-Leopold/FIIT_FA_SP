#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H

#include <logger.h>
#include <unordered_map>
#include <httplib.h>
#include <forward_list>

class server_logger_builder;
class server_logger final:
    public logger
{
    httplib::Client _client;
    std::unordered_map<logger::severity, std::pair<std::forward_list<std::string>, bool>> _streams;
    std::string _dest;
    std::string _format;
public:
    enum class flag
    { DATE, TIME, SEVERITY, MESSAGE, NO_FLAG };
private:
    server_logger(const std::string& dest, const std::unordered_map<logger::severity ,std::pair<std::forward_list<std::string>, bool>>& streams, const std::string &format);

    std::string make_format(const std::string& message, severity sev) const;
    static flag char_to_flag(char c) noexcept;

    friend server_logger_builder;

    static int inner_getpid();


public:

    server_logger(server_logger const &other) = delete;

    server_logger &operator=(server_logger const &other) = delete;

    server_logger(server_logger &&other) noexcept;

    server_logger &operator=(server_logger &&other) noexcept;

    ~server_logger() noexcept final;

public:

    [[nodiscard]] logger& log(
        const std::string &message,
        logger::severity severity) & override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H