#ifndef BACKUP_H_
#define BACKUP_H_

#include <exception>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <cstdio>
#include "backup.h"

namespace fs = boost::filesystem;

FileNotFound::FileNotFound(std::string filename): filename(filename) {};

const char * FileNotFound::what() const throw()  {  
            std::string tmp_string = filename;
            tmp_string += "is not exist";
            return &tmp_string[0];  
}  

Backup::Backup(std::string user_id) {
    fs::path backup_dir("backup_files");
    fs::path user_id_path(user_id);
    user_dir = backup_dir / user_id_path;
    if (!fs::exists(user_dir)) {
       fs::create_directories(user_dir);
    }
}

Backup::Backup(uint32_t user_id):
    Backup(boost::lexical_cast<std::string>(user_id)) {}

std::string Backup::get_file_path(std::string filename) {
    fs::path basename(filename);
    fs::path full_path = user_dir / basename;
    return full_path.string();
}

void Backup::backup_file(std::string filename, std::string payload) {
    std::string filepath = get_file_path(filename); 
    std::ofstream new_file(filepath, std::ios::binary);
    new_file << payload;
    new_file.close();
}

void Backup::remove_file(std::string filename) {
    std::string filepath = get_file_path(filename); 
    if (fs::exists(filepath)) {
       std::remove(&filepath[0]);
    }
}

std::string Backup::restore_file(std::string filename) {
    std::string filepath = get_file_path(filename); 
    std::stringstream buffer;
    if (!fs::exists(filepath)) {
       throw FileNotFound(filepath);
    }

    std::ifstream restore_file(filepath, std::ios::binary);
    buffer << restore_file.rdbuf();
    return buffer.str();
}

std::string Backup::list_files() {
    std::string files_list;
    for (fs::directory_iterator itr(user_dir); itr != fs::directory_iterator(); ++itr) {
        if (fs::is_regular_file(itr->path())) {
            // assign current file name to current_file and echo it out to the console.
            files_list += itr->path().filename().string() + '\n';
        }
    }
    return files_list;
}

#endif /* BACKUP_H_ */