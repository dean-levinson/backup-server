

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

class Backup {
    private:
        //std::string user_id;
        fs::path user_dir;
        std::string get_file_path(std::string filename);
    public:
        Backup(uint32_t user_id);
        Backup(std::string user_id);   
        void backup_file(std::string filename, std::string payload);
        std::string restore_file(std::string filename);
        void remove_file(std::string filename);
        std::string list_files();
};