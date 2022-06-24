#include <iostream>
#include "../lib/altdiff.h"

void print_diff(const Diff &diff) {
  print_left();
  move_cursor_half_screen();
  print("|")
  print_right()
}

void print_json_diff(const JSON &diff) {
  auto map_diff = from_json(diff);
  for(const auto&[arch, diff] : map_diff) {
    std::cout<<arch<<"\n";
    print_diff(diff);
  }
}

int main(int argc, char *argv[]) {
  if(argc!=3) {
    std::cout<<"Usage: "<<argv[0]<<" <branch1> <branch2>";
    return 1;
  }
  std::string branch1{argv[1]};
  std::string branch2{argv[2]};
  auto diff = json_diff(branch1, branch2);

  print_json_diff(diff);

  return 0;
}
