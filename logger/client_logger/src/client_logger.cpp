#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include "../include/client_logger.h"
#include <not_implemented.h>

std::unordered_map<std::string, std::pair<size_t, std::ofstream>> client_logger::refcounted_stream::_global_streams;


logger& client_logger::log(
    const std::string &text,
    logger::severity severity) &
{
    std::string log_text = make_format(text, severity);

    auto iter = _output_streams.find(severity);
    if (iter == _output_streams.end()) {
        throw std::runtime_error("No such severity");
    }

    const auto &file_streams = iter->second.first;
    const bool is_write_to_console = iter->second.second;
    for (const refcounted_stream& stream : file_streams) {
        *(stream._stream.second) << log_text << std::endl;
    }
    if (is_write_to_console) {
        std::cout << log_text << std::endl;
    }

    return *this;
}

std::string client_logger::make_format(const std::string &message, severity sev) const
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
        } else {
            res << _format[i];
        }
    }

    return res.str();
}

client_logger::client_logger(
        const std::unordered_map<logger::severity, std::pair<std::forward_list<refcounted_stream>, bool>> &streams,
        std::string format)
        : _output_streams(streams), _format(std::move(format))
{}

client_logger::flag client_logger::char_to_flag(char c) noexcept
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

client_logger::client_logger(const client_logger &other) : _output_streams(other._output_streams), _format(other._format){}

client_logger &client_logger::operator=(const client_logger &other)
{
    if (this != &other) {
        _format = other._format;
        _output_streams = other._output_streams;
    }
    return *this;
}

client_logger::client_logger(client_logger &&other) noexcept
{
    _format = std::move(other._format);
    _output_streams = std::move(other._output_streams);
}

client_logger &client_logger::operator=(client_logger &&other) noexcept
{
    if (this != &other) {
        _format = std::move(other._format);
        _output_streams = std::move(other._output_streams);
    }
    return *this;
}

client_logger::~client_logger() noexcept
{
    // seems it's okay
}

client_logger::refcounted_stream::refcounted_stream(const std::string &path)
{
    std::cout << "stream path constructor: " << path << std::endl;
    _stream.first = path;
    _stream.second = nullptr;
}

client_logger::refcounted_stream::refcounted_stream(const client_logger::refcounted_stream &oth)
{
    std::cout << "stream copy constructor: " << oth._stream.first << std::endl;
    auto it = _global_streams.find(oth._stream.first);
    if (it == _global_streams.end()){
        std::ofstream out;
        out.open(oth._stream.first);
        if (out.is_open()) {
            _global_streams.insert({oth._stream.first, std::make_pair(1, std::move(out))});
            it = _global_streams.find(oth._stream.first);
            _stream.first = oth._stream.first;
            _stream.second = &(it->second.second);
        } else {
            throw std::runtime_error("Unable to open file " + oth._stream.first);
        }
    } else{
        _global_streams.at(oth._stream.first).first++;
        _stream.first = oth._stream.first;
        _stream.second = &(it->second.second);
    }
}

client_logger::refcounted_stream &
client_logger::refcounted_stream::operator=(const client_logger::refcounted_stream &oth)
{
    std::cout << "stream assign copy constructor: " << oth._stream.first << std::endl;
    if (this != &oth) {
        _global_streams.at(_stream.first).first--;
        if (_global_streams.at(_stream.first).first == 0) {
            _global_streams.at(_stream.first).second.close();
            _global_streams.erase(_stream.first);
        }

        _stream.first = oth._stream.first;
        _stream.second = oth._stream.second;
        _global_streams.at(_stream.first).first++;
    }
    return *this;
}

client_logger::refcounted_stream::refcounted_stream(client_logger::refcounted_stream &&oth) noexcept
{
    _stream = std::move(oth._stream);
    oth._stream = {"", nullptr};
}

client_logger::refcounted_stream &client_logger::refcounted_stream::operator=(client_logger::refcounted_stream &&oth) noexcept
{
    if (this != &oth) {
        _stream = std::move(oth._stream);
        oth._stream = {"", nullptr};
    }
    return *this;
}

client_logger::refcounted_stream::~refcounted_stream()
{
    std::cout << "stream destructor: " << _stream.first << std::endl;
    if (_stream.second) {
        _global_streams.at(_stream.first).first--;
        if (_global_streams.at(_stream.first).first == 0) {
            _global_streams.at(_stream.first).second.close();
            _global_streams.erase(_stream.first);
        }
    }

}
