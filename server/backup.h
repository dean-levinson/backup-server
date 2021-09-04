

#include <string>

class Backup {
    private:
        std::string user_id;
    public:
        Backup(uint32_t user_id);
        Backup(std::string user_id);   
        void backup_file(std::string filename, std::string payload);
        std::string restore_file(std::string filename);
        void remove_file(std::string filename);
        std::string list_files();
};