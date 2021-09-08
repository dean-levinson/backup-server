

#include <string>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class FileNotFound : public std::exception {
    private:
    std::string filename;  
    public:
        FileNotFound(std::string filename);
        const char * what() const throw();
};

/**
 * Responsible for all the interaction with the filesystem on the server.
 * All the operations that are supported on the protocol will be implemented here. 
 */
class Backup {
    private:
        fs::path user_dir;
        std::string get_file_path(std::string filename);
    public:
        Backup(uint32_t user_id);
        Backup(std::string user_id);
        /**
         * Backup the given file to the user's directory.
         * 
         * @param filename - the file's base name.
         * @param payload the file's content.
         */
        void backup_file(std::string filename, std::string payload);

        /**
         * Returns a string that contains the file's content.
         * Raises FileNotFound exception if the file is not found.
         * 
         * @param filename - the file's base name. 
         */
        std::string restore_file(std::string filename);

        /**
         * Removes the file from the user's directory.
         * 
         * @param filename 
         */
        void remove_file(std::string filename);
        std::string list_files();
};