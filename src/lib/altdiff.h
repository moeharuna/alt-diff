#ifndef ALTDIFF_H_
#define ALTDIFF_H_
#include <string>
#include <map>
#include "../include/json.hpp"

namespace AltDiff {
  using Arch = std::string;

  class Version{
  public:
    Version();
    Version(const std::string&);
    Version(const Version&);
    ~Version();
    Version& operator=(const Version&);
    const std::string &version_string() const;
    std::string &version_string();
    bool operator>(const Version&) const;
    bool operator<(const Version&) const;
    bool operator!=(const Version&) const;
    bool operator==(const Version&) const;

    friend void to_json(nlohmann::json& j, const Version& v);
    friend void from_json(const nlohmann::json& j, Version& v);

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
  };



  class Package{
  public:
    Package(const Package&);
    Package();
    ~Package();
    Package& operator=(const Package&);
    const std::string &name() const;
    const Version &version() const;
    const Arch &arch() const;

    friend void to_json(nlohmann::json& j, const Package& p);
    friend void from_json(const nlohmann::json& j, Package& p);
  private:

    struct Impl;
    std::unique_ptr<Impl> pImpl;
  };

  class VersionMissmatch {
  public:

    VersionMissmatch(const Package& left, const Package &right);
    VersionMissmatch();
    VersionMissmatch(const VersionMissmatch&);
    ~VersionMissmatch();

    VersionMissmatch& operator=(const VersionMissmatch&);
    const Version& left() const;
    const Version& right() const;
    const std::string& name() const;
    const Arch& arch() const;

    friend void from_json(const nlohmann::json& j, VersionMissmatch& vm);
    friend void to_json(nlohmann::json& j, const VersionMissmatch& vm);

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
  };

  class Diff{
  public:
    Diff();
    Diff(const std::vector<Package>& first,
         const std::vector<Package>& second);
    //I need implement explicit copy constructor, and copy assigment
    //for every class because i use unique_ptr for pImpl;
    Diff(const Diff&);
    Diff& operator=(const Diff&);
    ~Diff();

    const std::vector<Package>& left_only() const;
    const std::vector<Package>& right_only() const;
    const std::vector<VersionMissmatch>& version_diff() const;

    friend void from_json(const nlohmann::json& j, Diff& diff);
    friend void to_json(nlohmann::json& j, const Diff& diff);
  private:
    //I use pimpl to hide implimentation of all my classes.
    struct Impl;
    std::unique_ptr<Impl> pImpl;
  };


  //Returns diffrence between two branches in json format
  nlohmann::json get_diff(const std::string& branch1, const std::string& branch2,
                          const std::string &arch="",
                          const std::string &endpoint="https://rdb.altlinux.org/api/export/branch_binary_packages/");
  //You can also parse json using nlogman::json.get instead
  std::map<Arch, Diff> parse_json(nlohmann::json&);
}

#endif // ALTDIFF_H_
