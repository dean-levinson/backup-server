#ifndef BACKUP_H_
#define BACKUP_H_

#include <string>
#include <boost/lexical_cast.hpp>
#include "backup.h"

Backup::Backup(std::string user_id): user_id(user_id) {}

Backup::Backup(uint32_t user_id):
    user_id(boost::lexical_cast<std::string>(user_id)) {}

void Backup::backup_file(std::string filename, std::string payload) {
}

void Backup::remove_file(std::string filename) {
}

std::string Backup::restore_file(std::string filename) {
    return std::string("Hello world!");
}

std::string Backup::list_files() {
    return std::string("Hello world!");
}

#endif /* BACKUP_H_ */