#ifndef DB_UTILS_H
#define DB_UTILS_H
#include <string>
#include <fstream>

inline std::string find_db_path() {
    const char* paths[] = {"DB.csv", "../DB.csv"};
    for(const char* p : paths) {
        std::ifstream file(p);
        if(file.good())
            return p;
    }
    return paths[0];
}

inline std::string find_data_file(const std::string& name) {
    const char* prefixes[] = {"", "../"};
    for(const char* pre : prefixes) {
        std::string path = std::string(pre) + name;
        std::ifstream file(path);
        if(file.good())
            return path;
    }
    return name;
}

#endif // DB_UTILS_H