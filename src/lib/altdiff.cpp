#include "altdiff.h"
#include <map>
#include <string>
#include <vector>
#include <optional>
#include <curl/curl.h>
#include <future>
#include "../include/json.hpp"
#include <algorithm>
#include <memory>
using namespace nlohmann;

namespace AltDiff {
  struct Version::Impl {
    std::string version_string_;
  };

  Version::Version() {
    pImpl = std::make_unique<Impl>();
  }
  Version::Version(const std::string& ver_str) {
    pImpl = std::make_unique<Impl>();
    pImpl->version_string_ = ver_str;
  }

  Version::Version(const Version &old) {
    pImpl = std::make_unique<Impl>(*old.pImpl);
  }


  Version& Version::operator=(const Version& other) {
    pImpl = std::make_unique<Impl>(*other.pImpl);
    return *this;
  }

  const std::string& Version::version_string() const{
    return pImpl->version_string_;
  }

  bool Version::operator>(const Version &b) const {
    return version_string()>b.version_string();
  }

  bool Version::operator<(const Version &b) const {
    return version_string()<b.version_string();
  }

  bool Version::operator!=(const Version &other) const{
    return version_string()!=other.version_string();
  }

  bool Version::operator==(const Version& b) const{
    return !(*this!=b);
  }

  void to_json(nlohmann::json& j, const Version &v) {
    j = v.pImpl->version_string_;
  }

  void from_json(const nlohmann::json&j, Version &v) {
    j.get_to(v.pImpl->version_string_);
  }

  struct Package::Impl {
    std::string name_;
    Version version_;
    Arch arch_;
  };

  Package::Package() {
    pImpl = std::make_unique<Impl>();
  }
  Package::Package(const Package& p) {
    this->pImpl = std::make_unique<Impl>(*p.pImpl);
  }

  Package& Package::operator=(const Package& p) {
    this->pImpl = std::make_unique<Impl>(*p.pImpl);
    return *this;
  }

  const std::string& Package::name() const {
    return pImpl->name_;
  }

  const Version& Package::version() const {
    return pImpl->version_;
  }

  const Arch& Package::arch() const {
    return pImpl->arch_;
  }

  void to_json(json &j, const Package &p) {
    j= {{"name", p.pImpl->name_},
         {"arch", p.pImpl->arch_},
         {"version", p.pImpl->version_}};
  }

  void from_json(const json &j, Package &p) {
    p.pImpl->name_= j.at("name").get<std::string>();
    p.pImpl->arch_ = j.at("arch").get<Arch>();
    p.pImpl->version_ = j.at("version");
  }

  using Packages = std::vector<Package>;



  struct VersionMissmatch::Impl {
    std::string name_;
    Arch arch_;
    Version left_ver_;
    Version right_ver_;
  };

  VersionMissmatch::VersionMissmatch(const Package& left, const Package& right) {
    assert(left.name()==right.name());
    assert(left.arch()==right.arch());

    pImpl = std::make_unique<Impl>();

    pImpl->name_ = left.name();
    pImpl->arch_ = left.arch();
    pImpl->left_ver_ = left.version();
    pImpl->right_ver_ = right.version();
  }

  VersionMissmatch::VersionMissmatch() {
    pImpl = std::make_unique<Impl>();
  }

  VersionMissmatch::VersionMissmatch(const VersionMissmatch& other) {
    pImpl = std::make_unique<Impl>(*other.pImpl);
  }

  VersionMissmatch& VersionMissmatch::operator=(const VersionMissmatch& other) {
    pImpl = std::make_unique<Impl>(*other.pImpl);
    return *this;
  }

  const Version& VersionMissmatch::left() const{
    return pImpl->left_ver_;
  }

  const Version& VersionMissmatch::right() const{
    return pImpl->right_ver_;
  }

  const std::string& VersionMissmatch::name() const {
    return pImpl->name_;
  }

  const std::string& VersionMissmatch::arch() const {
    return pImpl->arch_;
  }

  void from_json(const json &j, VersionMissmatch &vm) {
    vm.pImpl->name_ = j.at("name").get<std::string>();
    vm.pImpl->arch_ = j.at("arch").get<Arch>();
    vm.pImpl->left_ver_ = j.at("left").get<Version>();
    vm.pImpl->right_ver_ = j.at("right").get<Version>();
  }

  void to_json(json &j, const VersionMissmatch &vm) {
    j = {{"name",  vm.pImpl->name_},
         {"arch",  vm.pImpl->arch_},
         {"left",  vm.pImpl->left_ver_},
         {"right", vm.pImpl->right_ver_}};
  }

  std::vector<VersionMissmatch> version_set_diffrence(const Packages &left, const Packages &right) {
    std::vector<VersionMissmatch> result{};
    auto first_iter = left.begin();
    auto second_iter = right.begin();

    while(first_iter!=left.end()) {
      bool ver_check =
        second_iter->name()    == first_iter->name() &&
        second_iter->arch()    == first_iter->arch() &&
        second_iter->version() != first_iter->version();
      if(ver_check) {
        result.emplace_back(VersionMissmatch{*first_iter, *second_iter});
      } else {
        if(second_iter->name() >= first_iter->name()) {
          ++first_iter;
        }
        ++second_iter;
      }
    }
    return result;
  }


  struct Diff::Impl {
    Packages only_left_;
    Packages only_right_;
    std::vector<VersionMissmatch> version_missmatch_;
    static bool name_comp(const Package& first, const Package& second) {
      return first.name() < second.name();
    }
  };

  Diff::Diff(const Packages &left,
             const Packages &right) {
    pImpl = std::make_unique<Impl>();
    assert(std::is_sorted(left.begin(),  left.end(), pImpl->name_comp));
    assert(std::is_sorted(right.begin(), right.end(), pImpl->name_comp));

    std::set_difference(left.begin(), left.end(),
                        right.begin(), right.end(),
                        std::back_inserter(pImpl->only_left_), pImpl->name_comp);
    std::set_difference(right.begin(), right.end(),
                        left.begin(), left.end(),
                        std::back_inserter(pImpl->only_right_), pImpl->name_comp);
    pImpl->version_missmatch_ = version_set_diffrence(left, right);
  }

  Diff::Diff() {
    pImpl = std::make_unique<Impl>();
  }
  Diff::Diff(const Diff& d) {
    pImpl = std::make_unique<Impl>(*d.pImpl);
  }

  Diff& Diff::operator=(const Diff& d) {
    pImpl = std::make_unique<Impl>(*d.pImpl);
    return *this;
  }

  void from_json(const json& j, Diff& diff) {
    diff.pImpl->only_left_ = j.at("left").get<Packages>();
    diff.pImpl->only_right_ = j.at("right").get<Packages>();
    diff.pImpl->version_missmatch_ = j.at("version").get<std::vector<VersionMissmatch>>();
  }

  void to_json(json& j, const Diff& diff) {
    j= {{"left", diff.pImpl->only_left_},
        {"right", diff.pImpl->only_right_},
        {"version", diff.pImpl->version_missmatch_}};
  }


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
                      const std::string &endpoint="https://rdb.altlinux.org/api//export/branch_binary_packages/") {
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
                                                 const std::string &endpoint = "https://rdb.altlinux.org/api//export/branch_binary_packages/") {
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
    std::map<Arch, Diff> result;
    for(const auto& [key, _] : pack1) {
      result.at(key) = Diff(pack1.at(key), pack2.at(key));
    }
    return result;
  }

  json json_diff(const std::map<Arch, Packages>& pack1,
                 const std::map<Arch, Packages>& pack2) {
    json result{diff_by_arch(pack1, pack2)};
    return result;
  }

  json json_diff(const std::string& branch_name1,
                 const std::string& branch_name2) {
    auto pack1 = packages_by_arch(get_branch(branch_name1));
    auto pack2 = packages_by_arch(get_branch(branch_name2));
    return json_diff(pack1, pack2);
  }

  std::map<Arch, Diff> parse_json(nlohmann::json& j) {
    return j.get<std::map<Arch, Diff>>();
  }
}
