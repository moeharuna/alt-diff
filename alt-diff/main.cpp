#include <iostream>
#include <sys/ioctl.h>
#include "altdiff.h"
#include <stdlib.h>
void print_package(const AltDiff::Package& package) {
  std::cout<<package.name();
}
void print_ver_missmatch(const AltDiff::VersionMissmatch & vm, size_t left_fill=0) {
  std::cout<<std::left<<std::setw(left_fill);
  std::cout<<vm.name()+": ";
  std::cout<<vm.left().version_string()<<" < "<<vm.right().version_string()<<"\n";
}

void print_diff(const AltDiff::Diff &diff) {
  size_t max_len = 0;
  for(const auto& pack: diff.left_only()) {
    max_len = std::max(pack.name().length(), max_len);
  }

  size_t left=0, right=0;
  while(left< diff.left_only().size() ||
        right< diff.right_only().size()) {
    if(left < diff.left_only().size()) {
      std::cout<<std::left<<std::setw(max_len);
      print_package(diff.left_only().at(left));
    } else {
      std::cout<<std::right<<std::setw(max_len+2);
    }
    //move_cursor_to_center();
    std::cout<<"| ";
    if(right < diff.right_only().size()) {
      print_package(diff.right_only().at(right));
    }
    std::cout<<"\n";
    left++;
    right++;
  }
  for(const auto& ver_miss: diff.version_diff()) {
    max_len = std::max(ver_miss.name().length(), max_len);
  }
  for(const auto& ver_miss : diff.version_diff()) {
    if(ver_miss.left() < ver_miss.right()) {
      print_ver_missmatch(ver_miss, max_len);
    }
  }
}

int main(int argc, char *argv[]) {
  if(argc < 3 || argc > 4 || strcmp(argv[1], "-h")==0 || strcmp(argv[1], "--help")==0) {
    std::cout<<"Usage: "<<argv[0]<<" <branch1> <branch2> <arch>(optional)";
    return 1;
  }
  std::string branch1{argv[1]};
  std::string branch2{argv[2]};
  std::string arch = "";
  if(argc==4) {
    arch = argv[3];
  }
  try {
    auto diff = AltDiff::get_diff(branch1, branch2, arch);
    std::map<AltDiff::Arch, AltDiff::Diff> diff_map = diff;
    for(const auto& [arch, diff] :diff_map) {
      std::cout<<"["<<arch<<"] = \n";
      print_diff(diff);
    }
  } catch(const std::exception& e) {
    std::cout<<e.what()<<"\n";
  }

  return 0;
}
