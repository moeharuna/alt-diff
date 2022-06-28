#include <iostream>
#include "../lib/altdiff.h"

struct Position {
  int x, y;
};

void print_diff(const AltDiff::Diff &diff, int terminal_width) {
  std::cout<<"\033["<<terminal_width/2<<"C"<<"|\n";
}

Position read_terminal_size() {

}

int main(int argc, char *argv[]) {
  if(argc!=3 || strcmp(argv[1], "-h")==0 || strcmp(argv[1], "--help")==0) {
    std::cout<<"Usage: "<<argv[0]<<" <branch1> <branch2>";
    return 1;
  }
  std::string branch1{argv[1]};
  std::string branch2{argv[2]};
  auto diff = AltDiff::get_diff(branch1, branch2, "aarch64");

  std::map<AltDiff::Arch, AltDiff::Diff> diff_map = diff;
  for(const auto& [arch, diff] :diff_map) {
    for(const auto& package:diff.version_diff()) {
      std::cout<<package.name()<<" "<<package.left().version_string()
               <<":"<<package.right().version_string()<<"\n";
    }
  }

  return 0;
}
