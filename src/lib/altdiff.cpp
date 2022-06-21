#include "altdiff.h"
#include <map>
#include <string>
#include <vector>
#include <optional>
class Version {
  std::vector<std::string> version_string;
  bool operator<(const Version &b) {
    return true;
  }
};


using Arch = std::string;
class Package {
  std::string name;
  Version version;
  Arch arch;
};
using Packages = std::vector<Package>;
class Diff{

};

Packages get_branch(const std::string &branch_name, const std::string &arch="") {

};


std::map<Arch, Packages> packages_by_arch(const Packages& packages) {

}

Diff diff_packages(const Packages &first,
                   const Packages &second) {

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
