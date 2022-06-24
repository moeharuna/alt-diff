#ifndef ALTDIFF_H_
#define ALTDIFF_H_
#include <string>
#include <map>
#include "../include/json.hpp"

class Diff{};
class ArchDiff{};

nlohmann::json get_diff(const std::string& branch1, const std::string& branch2,
                        const std::string &arch="",
                        const std::string &endpoint="https://rdb.altlinux.org/api/");  //returns diffrence between two branches in json format
std::string get_string_diff(const std::string& branch1, const std::string& branch2,
                            const std::string &arch="",
                            const std::string &endpoint="https://rdb.altlinux.org/api/"); //Returns same json as get_diff but in string if you don't want to depend on nlohmann json library
std::map<Arch, Diff> from_json(const nlohmann::json& j, Diff& d);




#endif // ALTDIFF_H_
