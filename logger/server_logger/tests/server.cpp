//
// Created by Des Caldnd on 3/27/2024.
//

#include "server.h"
#include <logger_builder.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <forward_list>
#include <unistd.h>

using json = nlohmann::json;

server::server(uint16_t port) {
    std::cout << "Server pid: " << getpid() << std::endl;
    CROW_ROUTE(app, "/logger/init").methods("POST"_method)([&](const crow::request &req) {
        try {
            json body = json::parse(req.body);

            if (!body.contains("pid") || !body.contains("severity") || !body.contains("path") || !body.contains("console")) {
                return crow::response(400, "Missing one of required fields: pid, severity, path, console");
            }

            int pid = body["pid"];
            logger::severity sev = logger_builder::string_to_severity(body["severity"]);
            std::string path = body["path"];
            bool is_write_console = body["console"];


            std::lock_guard lock(_mut);
            _streams[pid][sev].first.push_front(path);
            _streams[pid][sev].second = is_write_console;

            return crow::response(200);
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Sserver error: ") + e.what());
        }
    });


    CROW_ROUTE(app, "/logger/log").methods("POST"_method)([&](const crow::request &req) {
        try {
            std::cout << "request received!" << req.body << std::endl;
            json body = json::parse(req.body);

            if (!body.contains("pid") || !body.contains("severity") || !body.contains("message")) {
                return crow::response(400, "Missing one of required fields: pid, severity, message");
            }

            int pid = body["pid"];
            logger::severity sev = logger_builder::string_to_severity(body["severity"]);
            std::string message = body["message"];

            std::shared_lock lock(_mut);
            auto it = _streams.find(pid);
            if (it != _streams.end()) {
                auto file_streams_it = it->second.find(sev);
                if (file_streams_it != it->second.end()) {
                    const auto& paths = file_streams_it->second.first;
                    bool console = file_streams_it->second.second;

                    for (const auto& path : paths) {
                        if (!path.empty()) {
                            std::ofstream stream(path, std::ios_base::app);
                            if (stream.is_open()) {
                                stream << message << std::endl;
                            }
                        }
                    }

                    if (console) std::cout << message << std::endl;
                }
            }
            return crow::response(200);
        } catch (const std::exception& e) {
            return crow::response(500, std::string("server error ") + e.what());
        }
    });

    CROW_ROUTE(app, "/logger/stop").methods("POST"_method)([&](const crow::request &req) {
        try {
            json body = json::parse(req.body);
            if (!body.contains("pid")) {
                return crow::response(400, "Missing required field: pid");
            }

            int pid = body["pid"];
            std::cout << "current PID: "<< pid << std::endl;

            std::lock_guard lock(_mut);
            _streams.erase(pid);

            return crow::response(200);
        } catch (const std::exception& e) {
            return crow::response(500, std::string("server error ") + e.what());
        }
    });


    app.port(port).loglevel(crow::LogLevel::Warning).multithreaded();
    app.run();
}
