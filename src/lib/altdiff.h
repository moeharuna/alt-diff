#ifndef ALTDIFF_H_
#define ALTDIFF_H_
#include <string>
#include <map>
#include "../include/json.hpp"


using Arch = std::string;

class Version{
public:
  Version(const std::string&);
  const std::string &version_string() const;
  bool operator>(const Version&) const;
  bool operator<(const Version&) const;
  friend bool operator!=(const Version&, const Version&);
  friend bool operator==(const Version&, const Version&);

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};



class Package{
public:
  const std::string &name() const;
  const Version &version() const;
  const Arch &arch() const;

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

class VersionMissmatch {
public:
  VersionMissmatch(const Package& p1, const Package &p2);
  const Version& first() const;
  const Version& second() const;
  const std::string& name() const;
  const Arch& arch() const;
private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

class Diff{
  public:

  Diff(const std::vector<Package>& first,
       const std::vector<Package>& second);
  const std::vector<Package> first_only() const;
  const std::vector<Package> second_only() const;
  const std::vector<VersionMissmatch> version_diff() const;
  private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

using DiffByArch = std::map<Arch, Diff>;

void from_json(const nlohmann::json&, Version& v);
void to_json(nlohmann::json&, const Version& v);

void from_json(const nlohmann::json&, Package& p);
void to_json(nlohmann::json&, const Package& p);

void from_json(const nlohmann::json&, Diff& d);
void to_json(nlohmann::json&, const Diff& d);

void from_json(const nlohmann::json&, DiffByArch& ad);
void to_json(const nlohmann::json&, DiffByArch& ad);

nlohmann::json get_diff(const std::string& branch1, const std::string& branch2,
                        const std::string &arch="",
                        const std::string &endpoint="https://rdb.altlinux.org/api/"); //returns diffrence between two branches in json format
std::string get_string_diff(const std::string& branch1, const std::string& branch2,
                            const std::string &arch="",
                            const std::string &endpoint="https://rdb.altlinux.org/api/"); //Returns same json as get_diff but in string.





#endif // ALTDIFF_H_
