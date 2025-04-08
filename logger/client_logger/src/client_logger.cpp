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
    throw not_implemented("const logger& client_logger::log(const std::string &,logger::severity) const &", "your code should be here...");
}

std::string client_logger::make_format(const std::string &message, severity sev) const
{
    throw not_implemented("std::string client_logger::make_format(const std::string &, severity) const", "your code should be here...");
}

client_logger::client_logger(
        const std::unordered_map<logger::severity, std::pair<std::forward_list<refcounted_stream>, bool>> &streams,
        std::string format)
{

}

client_logger::flag client_logger::char_to_flag(char c) noexcept
{
    throw not_implemented("client_logger::flag client_logger::char_to_flag(char) noexcept", "your code should be here...");
}

client_logger::client_logger(const client_logger &other)
{
    throw not_implemented("client_logger::client_logger(const client_logger &other)", "your code should be here...");
}

client_logger &client_logger::operator=(const client_logger &other)
{
    throw not_implemented("client_logger::flag client_logger::char_to_flag(char) noexcept", "your code should be here...");
}

client_logger::client_logger(client_logger &&other) noexcept
{
    throw not_implemented("client_logger &client_logger::operator=(const client_logger &other)", "your code should be here...");
}

client_logger &client_logger::operator=(client_logger &&other) noexcept
{
    throw not_implemented("client_logger &client_logger::operator=(client_logger &&other) noexcept", "your code should be here...");
}

client_logger::~client_logger() noexcept
{
    throw not_implemented("client_logger::~client_logger() noexcept", "your code should be here...");
}

client_logger::refcounted_stream::refcounted_stream(const std::string &path)
{
    _stream.first = path;
    _stream.second = nullptr;
    // auto it = _global_streams.find(path);
    // if (it == _global_streams.end()){
    //     std::ofstream out;
    //     out.open(path);
    //     if (out.is_open()) {
    //         _global_streams.insert({path, std::make_pair(1, std::move(out))});
    //         it = _global_streams.find(path);
    //         _stream.first = path;
    //         _stream.second = &(it->second.second);
    //     } else {
    //         throw std::runtime_error("Unable to open file " + path);
    //     }
    // } else{
    //     _global_streams.at(path).first++;
    //     _stream.first = path;
    //     _stream.second = &(it->second.second);
    // }
}

client_logger::refcounted_stream::refcounted_stream(const client_logger::refcounted_stream &oth)
{
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
    // _stream.first = oth._stream.first;
    // _stream.second = oth._stream.second;
    // _global_streams.at(_stream.first).first++;
}

client_logger::refcounted_stream &
client_logger::refcounted_stream::operator=(const client_logger::refcounted_stream &oth)
{
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
    _global_streams.at(_stream.first).first--;
    if (_global_streams.at(_stream.first).first == 0) {
        _global_streams.at(_stream.first).second.close();
        _global_streams.erase(_stream.first);
    }
}
