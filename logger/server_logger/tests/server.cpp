//
// Created by Des Caldnd on 3/27/2024.
//

#include "server.h"
#include <logger_builder.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

server::server(uint16_t port) {
    CROW_ROUTE(app, "/logger/init").methods("POST"_method)([&](const crow::request &req) {
        json body = json::parse(req.body);
        int pid = body["pid"];
        logger::severity sev = logger_builder::string_to_severity(body["severity"]);
        std::string path = body["path"];
        bool console = body["console"];

        {
            std::lock_guard lock(_mut);
            _streams[pid][sev] = { path, console };
        }
        return crow::response(204);
    });

    CROW_ROUTE(app, "/logger/log").methods("POST"_method)([&](const crow::request &req) {
        json body = json::parse(req.body);
        int pid = body["pid"];
        logger::severity sev = logger_builder::string_to_severity(body["severity"]);
        std::string message = body["message"];

        std::shared_lock lock(_mut);
        auto it = _streams.find(pid);
        if (it != _streams.end()) {
            auto inner_it = it->second.find(sev);
            if (inner_it != it->second.end()) {
                const std::string &path = inner_it->second.first;
                bool console = inner_it->second.second;

                if (!path.empty()) {
                    std::ofstream stream(path, std::ios_base::app);
                    if (stream.is_open()) stream << message << std::endl;
                }
                if (console) {
                    std::cout << message << std::endl;
                }
            }
        }
        return crow::response(204);
    });

    CROW_ROUTE(app, "/logger/stop").methods("POST"_method)([&](const crow::request &req) {
        json body = json::parse(req.body);
        int pid = body["pid"];
        {
            std::lock_guard lock(_mut);
            _streams.erase(pid);
        }
        return crow::response(204);
    });

    app.port(port).loglevel(crow::LogLevel::Warning).multithreaded();
    app.run();
}
