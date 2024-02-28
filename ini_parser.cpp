
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>

class ini_parser {
private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> data;

    static void trim(std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        size_t end = str.find_last_not_of(" \t\r\n");

        if (start != std::string::npos && end != std::string::npos) {
            str = str.substr(start, end - start + 1);
        }

        else {
            str.clear();
        }
    }

    static void trim_comment_to_value(std::string& value) {
        size_t pos = value.find(';');
        if (pos != std::string::npos) {
            value = value.substr(0, pos);
        }
    }

    std::string get_values_name(std::string& section_name) {
        std::string name_list;
        for (auto& elem : data[section_name]) {
            name_list += elem.first + '\n';
        }
        return name_list;
    }

public:

    ini_parser(const std::string& file_name) {
        std::ifstream file(file_name);

        if (!file.is_open()) {
            throw std::runtime_error("file is not opened");
        }

        std::string line;
        std::string section_name;

        while (std::getline(file, line)) {
            trim(line);

            if (line.empty() || line[0] == ';') {
                continue;
            }

            if (line[0] == '[' && line[line.length() - 1] == ']') {
                section_name = line.substr(1, line.length() - 2);
                trim(section_name);
                continue;
            }

            std::string value_name;
            std::string value;
            std::istringstream ss(line);

            if (std::getline(ss, value_name, '=') && std::getline(ss, value)) {
                trim(value_name);
                trim(value);
                trim_comment_to_value(value);
                data[section_name][value_name] = value;
            }

        }
        file.close();
    }


    template<class T>
    T get_value(const std::string& key) {

        size_t dot = key.find_first_of('.');
        std::string section_name = key.substr(0,dot);
        std::string value_name = key.substr(dot + 1,key.length() - 1);

        if (data.count(section_name) > 0 && data.at(section_name).count(value_name) > 0) {

            T value = data[section_name][value_name];  
            return value; 
        }

        else {
            throw std::runtime_error("Value not found for this section\nValues for this section:\n" + get_values_name(section_name));
        }
    }

};

int main()
{

    try {
        ini_parser parser("test_file.ini");
        auto value = parser.get_value<std::string>("Section1.var2");
        std::cout << value;
    }
    catch (const std::runtime_error& ex) {
        std::cout << ex.what() << std::endl;
    }
}

