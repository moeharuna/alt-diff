#include "altdiff.h"
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <future>
#include <algorithm>
#include <memory>
#include <cstdlib>
#include <boost/outcome/outcome.hpp>
#include <boost/json/src.hpp>
using namespace boost;



namespace AltDiff {

  struct Version::Impl {
    std::string version_string_;
    std::vector<std::variant<std::string, int>> version_vec;
    std::vector<std::string> str_split(std::string str, std::string delim=" ") {
      std::vector<std::string> result{};
      size_t pos = 0;
      std::string token;
      while ((pos = str.find(delim)) != std::string::npos) {
        token = str.substr(0, pos);
        result.push_back(token);
        str.erase(0, pos+delim.length());
      }
      if(str!="") result.push_back(str);
      return result;
    }
    void parse_version_vec() {
      std::vector<std::string> str_vec = str_split(version_string_, ".");

      version_vec = {};

      for (const auto &str : str_vec) {
        int try_int = atoi(str.c_str());
        if(try_int!=0)
          version_vec.emplace_back(try_int);
        else {
          version_vec.emplace_back(str);
        }
      }
    }
  };

  Version::Version() {
    pImpl = std::make_unique<Impl>();
  }
  Version::Version(const std::string& ver_str) {
    pImpl = std::make_unique<Impl>();
    pImpl->version_string_ = ver_str;
    pImpl->parse_version_vec();
  }

  Version::Version(const Version &old) {
    pImpl = std::make_unique<Impl>(*old.pImpl);
  }
  Version::~Version() = default;


  Version& Version::operator=(const Version& other) {
    pImpl = std::make_unique<Impl>(*other.pImpl);
    return *this;
  }

  const std::string& Version::version_string() const{
    return pImpl->version_string_;
  }

  bool Version::operator<(const Version &rhs) const {
    for(size_t i=0; i< pImpl->version_vec.size(); ++i) {
      if(i >= rhs.pImpl->version_vec.size()) {
        return false;
      }
      if(pImpl->version_vec[i].index() != rhs.pImpl->version_vec[i].index()) {
        return pImpl->version_string_ < rhs.version_string();
      }
      if(pImpl->version_vec[i] < rhs.pImpl->version_vec[i]) {
        return true;
      } else if(pImpl->version_vec[i] > rhs.pImpl->version_vec[i]){
        return false;
      }
    }
    return false;
  }

  bool Version::operator!=(const Version &other) const{
    return version_string()!=other.version_string();
  }

  bool Version::operator==(const Version& b) const{
    return !(*this!=b);
  }


  void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, Version const& v) {
    jv = v.pImpl->version_string_;
  }
  Version tag_invoke(boost::json::value_to_tag<Version>, boost::json::value const& jv) {
    Version v = Version{std::string(jv.as_string().subview())};
    v.pImpl->parse_version_vec();
    return v;
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
  Package::~Package() =  default;

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

  void  tag_invoke(boost::json::value_from_tag, boost::json::value& jv, Package const& p) {
    jv= {{"name", p.pImpl->name_},
         {"arch", p.pImpl->arch_},
         {"version", p.pImpl->version_}};
  }
  Package tag_invoke(boost::json::value_to_tag<Package>, boost::json::value const& jv) {
    Package p;
    p.pImpl->name_= std::string(jv.at("name").as_string().subview());
    p.pImpl->arch_ =std::string(jv.at("arch").as_string().subview());
    p.pImpl->version_ = json::value_to<Version>(jv.at("version"));
    return p;
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
  VersionMissmatch::~VersionMissmatch() = default;

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

  void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, VersionMissmatch const& vm) {
    jv = {{"name",  vm.pImpl->name_},
         {"arch",  vm.pImpl->arch_},
         {"left",  vm.pImpl->left_ver_},
         {"right", vm.pImpl->right_ver_}};

  }

  VersionMissmatch tag_invoke(boost::json::value_to_tag<VersionMissmatch>, boost::json::value const& jv) {
    VersionMissmatch vm;
    vm.pImpl->name_ = std::string(jv.at("name").as_string().subview());
    vm.pImpl->arch_ = std::string(jv.at("arch").as_string().subview());
    vm.pImpl->left_ver_ = json::value_to<Version>(jv.at("left"));
    vm.pImpl->right_ver_ = json::value_to<Version>(jv.at("right"));
    return vm;
  }

  std::vector<VersionMissmatch> version_set_diffrence(const Packages &first, const Packages &second) {
    std::vector<VersionMissmatch> result{};
    auto first_iter = first.begin();
    auto second_iter = second.begin();

    while(first_iter!=first.end() && second_iter!=second.end()) {
      bool ver_check =
        first_iter->name()    == second_iter->name() &&
        first_iter->arch()    == second_iter->arch() &&
        first_iter->version() <  second_iter->version();
     if(ver_check) {
       result.push_back(VersionMissmatch(*first_iter, *second_iter));
       ++second_iter;
     }
     else if(first_iter->name() < second_iter->name()) {
       ++first_iter;
     }
     else {
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

  Diff::Diff(const Packages &first,
             const Packages &second) {
    pImpl = std::make_unique<Impl>();
    assert(std::is_sorted(first.begin(),  first.end(), pImpl->name_comp));
    assert(std::is_sorted(second.begin(), second.end(), pImpl->name_comp));

    std::set_difference(first.begin(), first.end(),
                        second.begin(), second.end(),
                        std::back_inserter(pImpl->only_left_), pImpl->name_comp);
    std::set_difference(second.begin(), second.end(),
                        first.begin(), first.end(),
                        std::back_inserter(pImpl->only_right_), pImpl->name_comp);
    pImpl->version_missmatch_ = version_set_diffrence(first, second);
  }

  Diff::Diff() {
    pImpl = std::make_unique<Impl>();
  }
  Diff::Diff(const Diff& d) {
    pImpl = std::make_unique<Impl>(*d.pImpl);
  }

  Diff::~Diff() = default;

  Diff& Diff::operator=(const Diff& d) {
    pImpl = std::make_unique<Impl>(*d.pImpl);
    return *this;
  }

  const std::vector<Package>& Diff::left_only() const {
    return pImpl->only_left_;
  }
  const std::vector<Package>& Diff::right_only() const {
    return pImpl->only_right_;
  }
  const std::vector<VersionMissmatch>& Diff::version_diff() const {
    return pImpl->version_missmatch_;
  }

  void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, Diff const& diff) {
    jv= {{"left", diff.pImpl->only_left_},
        {"right", diff.pImpl->only_right_},
        {"version", diff.pImpl->version_missmatch_}};
  }

  Diff tag_invoke(boost::json::value_to_tag<Diff>, boost::json::value const& jv) {
    Diff diff;
    diff.pImpl->only_left_ = json::value_to<Packages>(jv.at("left"));
    diff.pImpl->only_right_ = json::value_to<Packages>(jv.at("right"));
    diff.pImpl->version_missmatch_ = json::value_to<std::vector<VersionMissmatch>>(jv.at("version"));
    return diff;
  }




  size_t curl_callback(char *data, size_t size, size_t nmemb, void *strptr) {
    size_t real_size = nmemb*size;
    std::string *result_str =reinterpret_cast<std::string*>(strptr);

    *result_str+=data;
    return real_size;
  }

  CurlError::CurlError(CURLcode code, std::string&& error_desc) :code{code}, error_desc{error_desc} {}
  CurlError::CurlError() {
    code = CURLE_OK;
    error_desc = "Error on curl init, error_msg cannot be supplied";
  }
  HttpError::HttpError(long http_response_code,
                       std::string&& body,
                       std::string&& content_type)
    :http_response_code{http_response_code},
     response_body{body},
     content_type{content_type} {}
  ExceptionError::ExceptionError(std::exception& except)
    :catched_exception{std::make_shared<std::exception>(except)} {}


  outcome_v2::result<std::string, Error> curl_get(const std::string &url) {
    char curl_error_buffer[CURL_ERROR_SIZE];
    long http_response;
    CURL* curl;
    char *ct = NULL;
    std::string content_type{};
    curl = curl_easy_init();
    if(curl==NULL) {
      return CurlError{};
    }
    std::string response;
    response.reserve(10'000);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error_buffer);
    curl_error_buffer[0] = 0;
    CURLcode err_code =  curl_easy_perform(curl);

    if(err_code != CURLE_OK) {
      std::string error_str{curl_error_buffer};
      return CurlError{err_code, std::move(error_str)};
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_response);
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
    if(ct != NULL)
      content_type = ct;
    if(http_response!=200) {
      return HttpError{http_response,
                       std::move(response),
                       std::move(content_type)};
    }


    curl_easy_cleanup(curl);

    return response;
  }


  outcome_v2::result<Packages, Error> get_branch(const std::string &branch_name,
                                                 const std::string &arch,
                                                 const std::string &endpoint) {
    std::string result_request = endpoint+branch_name;
    if(arch!="") {
      result_request+="?arch="+arch;
    }

    auto get_result = curl_get(result_request);

    if(!get_result) {
      return get_result.error();
    }

    auto json = json::parse(get_result.value());

    Packages packages;
    packages = json::value_to<Packages>(json.at("packages"));
    return packages;
  }

  outcome_v2::result<std::pair<Packages, Packages>, Error> get_branch_async(const std::string &branch1,
                               const std::string &branch2,
                               const std::string &arch,
                               const std::string &endpoint) {
    std::pair<Packages, Packages> result;
    auto f1 = std::async(std::launch::async,  get_branch, branch1, arch, endpoint);
    auto f2 = std::async(std::launch::async,  get_branch, branch2, arch, endpoint);

    outcome_v2::result<Packages, Error> first =  f1.valid() ? f1.get() : get_branch(branch1, arch, endpoint);
    outcome_v2::result<Packages, Error> second = f2.valid() ? f2.get() : get_branch(branch2, arch, endpoint);

    if(!first) {
      return first.error();
    }
    if(!second) {
      return second.error();
    }
    return std::make_pair(first.value(), second.value());
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


  std::map<Arch, Diff> diff_by_arch(const Packages& packages1,
                                    const Packages& packages2) {
    auto pack1 = packages_by_arch(packages1);
    auto pack2 = packages_by_arch(packages2);
    std::map<Arch, Diff> result;
    for(const auto& [key, _] : pack1) {
      result[key] = Diff(pack1.at(key), pack2.at(key));
    }
    return result;
  }



  outcome_v2::result<std::map<Arch, Diff>, Error> parse_json(json::value &jv) noexcept{
    try {
      return json::value_to<std::map<Arch, Diff>>(jv);
    } catch(std::exception& e) {
      return ExceptionError{e};
    }
  }

  outcome_v2::result<json::value, Error> get_diff(const std::string& branch1, const std::string& branch2,
                const std::string &arch,
                const std::string &endpoint) noexcept{
    try {
      auto pair = get_branch_async(branch1, branch2, arch, endpoint);
      if(!pair) {
        return pair.error();
      }
      auto diffs = diff_by_arch(pair.value().first, pair.value().second);
      return json::value_from(diffs);
    } catch(std::exception& e) {
      return ExceptionError{e};
    }
  }
}
