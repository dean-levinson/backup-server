#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <random>
#include "handler.h"
#include "protocol.h"

using boost::asio::ip::tcp;

#define SERVER_VERSION (1)

Handler::Handler(tcp::iostream& stream): request(stream), backup(request.user_id) {};

string Handler::handle_request() {
    string response;
    switch (request.op) {
        case BACKUP_FILE:
            response = backup_file(); break;
        case RESTORE_FILE:
            response = restore_file(); break;
        case REMOVE_FILE:
            response = remove_file(); break;
        case LIST_FILES:
            response = list_files();
    }
    return response;
}

string Handler::backup_file() {
    backup.backup_file(request.filename, request.payload);
    ResponseBuilder builder(SERVER_VERSION, BACKUP_OR_REMOVE_SUCCESS, request.filename);   
    return builder.get_response();
}

string Handler::restore_file() {
    std::string payload = backup.restore_file(request.filename);
    ResponseBuilder builder(SERVER_VERSION, RESTORE_SUCCESS, request.filename, payload);   
    return builder.get_response();
}

string Handler::remove_file() {
    backup.remove_file(request.filename);
    ResponseBuilder builder(SERVER_VERSION, BACKUP_OR_REMOVE_SUCCESS, request.filename);   
    return builder.get_response();
}

string Handler::list_files() {
    std::string payload = backup.list_files(); 
    std::string filename = generate_filename();
    ResponseBuilder builder(SERVER_VERSION, LIST_FILES_SUCCESS, filename, payload);   
    return builder.get_response();
}

std::string Handler::generate_filename() {
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::string random_string;

    for (std::size_t i = 0; i < 32; ++i) {
        random_string += CHARACTERS[distribution(generator)];
    }

    return random_string;
}
