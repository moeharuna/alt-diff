#include "altdiff.h"
#include <map>
#include <string>
#include <vector>
#include <optional>
#include <curl/curl.h>
#include <future>
#include "../include/json.hpp"
#include <algorithm>
using namespace nlohmann;

struct Version {
  public:
  std::string &version_string() {
    return version_string_;
  }

  const std::string &version_string() const{
    return version_string_;
  }

  friend bool operator!=(const Version &a, const Version &b) {
    return a.version_string()!=b.version_string();
  }

  bool operator>(const Version &b) const {
    return version_string()>b.version_string();
  }
  private:
  std::string version_string_;

};

void from_json(const json &j, Version& v) {
  j.get_to(v.version_string());
}
void to_json(json &j, const Version&v) {
  j = json(v.version_string());
}
using Arch = std::string;
struct Package {
  public:
  std::string &name() {
    return name_;
  }

  const std::string &name() const {
    return name_;
  }
  Version &version() {
    return version_;
  }

  const Version &version() const {
    return version_;
  }

  Arch &arch() {
    return arch_;
  }

  const Arch &arch() const {
    return arch_;
  }

  private:
  std::string name_;
  Version version_;
  Arch arch_;
};
using Packages = std::vector<Package>;

void from_json(const json& j, Package& p) {
  j.at("name").get_to(p.name());
  j.at("arch").get_to(p.arch());
  j.at("version").get_to(p.version());
}

void to_json(json& j, const Package& p) {
  j = json{{"name", p.name()},
           {"arch", p.arch()},
           {"version", p.version()}};
}

class VersionDiff {
  public:
  VersionDiff(const Package& first, const Package& second) {
    assert(first.name()==second.name());
    assert(first.arch()==second.arch());

    package_ = first;
    second_ver_ = second.version();
  }
  const Version &first_ver() const{
    return package_.version();
  }

  Version &first_ver() {
    return package_.version();
  }

  const Version &second_ver() const{
    return second_ver_;
  }

  Version &second_ver() {
    return second_ver_;
  }

  const std::string& name() const {
    return package_.name();
  }

  std::string& name() {
    return package_.name();
  }

  bool is_first_larger() const{
    return package_.version()>second_ver();
  }

  private:
  Package package_;
  Version second_ver_;
};

std::vector<VersionDiff> version_set_diffrence(const Packages &first, const Packages &second) {
  std::vector<VersionDiff> result{};
  auto first_iter = first.begin();
  auto second_iter = second.begin();

  while(first_iter!=first.end()) {
    bool ver_check =
      second_iter->name()    == first_iter->name() &&
      second_iter->arch()    == first_iter->arch() &&
      second_iter->version() != first_iter->version();
    if(ver_check) {
      result.emplace_back(VersionDiff{*first_iter, *second_iter});
    } else {
      if(second_iter->name() >= first_iter->name()) {
        ++first_iter;
      }
      ++second_iter;
    }
  }
  return result;
}



class Diff{
  public:
  Diff(const Packages &first,
       const Packages &second) {

    assert(std::is_sorted(first.begin(),  first.end(), name_comp));
    assert(std::is_sorted(second.begin(), second.end(), name_comp));

    std::set_difference(first.begin(), first.end(),
                        second.begin(), second.end(),
                        std::back_inserter(only_first_), name_comp);
    std::set_difference(second.begin(), second.end(),
                        first.begin(), first.end(),
                        std::back_inserter(only_second_), name_comp);
    version_missmatch_ = version_set_diffrence(first, second);
  }
  private:
  static bool name_comp(const Package& first, const Package& second) {
    return first.name() < second.name();
  }

  Packages only_first_;
  Packages only_second_;
  std::vector<VersionDiff> version_missmatch_;
};

size_t curl_callback(char *data, size_t size, size_t nmemb, void *strptr) {
  size_t real_size = nmemb*size;
  std::string *result_str =reinterpret_cast<std::string*>(strptr);

  *result_str+=data;
  return real_size;
}

std::string curl_get(const std::string &url) {
  CURL* curl;
  curl = curl_easy_init();
  if(curl==NULL) {
    throw "TODO: Curl is NULL";
  }
  std::string result{};
  result.reserve(10'000);

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
  curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  return result;
}


Packages get_branch(const std::string &branch_name, const std::string &arch="",
                    const std::string &endpoint="https://rdb.altlinux.org/api/") {
  std::string result_request = endpoint+branch_name;
  if(arch!="") {
    result_request+="?arch="+arch;
  }
  auto json = json::parse(curl_get(result_request));
  return json.at("packages").get<Packages>();
}

std::pair<Packages, Packages> get_branch_async(const std::string &branch1,
                                               const std::string &branch2,
                                               const std::string &arch = "",
                                               const std::string &endpoint = "https://rdb.altlinux.org/api/") {
  auto f1 = std::async(std::launch::async,  get_branch, branch1, arch, endpoint);
  auto f2 = std::async(std::launch::async,  get_branch, branch1, arch, endpoint);
  if(f1.valid() && f2.valid()) {
    return std::make_pair(f1.get(), f2.get());
  } else {
    throw "TODO: Future is invalid";
  }

}

std::map<Arch, Packages> packages_by_arch(const Packages& packages) {
  std::map<Arch, Packages> result;
  for(const auto &package : packages) {
    if(result.count(package.arch())==0)
      result[package.arch()] = std::vector<Package>(512);
    else {
      result[package.arch()].emplace_back(package);
    }
  }
  return result;
}


bool package_name_comp(const Package& first, const Package& second) {
  return first.name()<second.name();
}

std::map<Arch, Diff> diff_by_arch(const std::map<Arch, Packages>& pack1,
                                  const std::map<Arch, Packages>& pack2) {

}

std::map<Arch, Diff> fromJson(JSON diff) {

}


JSON json_diff(const std::map<Arch, Packages>& pack1,
               const std::map<Arch, Packages>& pack2) {
  return toJson(diff_by_arch(pack1, pack2));
}

JSON json_diff(const std::string& branch_name1,
               const std::string& branch_name2) {
  auto pack1 = packages_by_arch(get_branch(branch_name1));
  auto pack2 = packages_by_arch(get_branch(branch_name2));
  return json_diff(pack1, pack2);
}
