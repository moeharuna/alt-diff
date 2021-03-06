#include <iostream>
#include <map>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <algorithm>
#include "altdiff.h"

void print_package(const AltDiff::Package& package) {
  std::cout<<package.name();
}

void print_ver_missmatch(const AltDiff::VersionMissmatch & vm, size_t left_fill=0) {
  std::cout<<std::left<<std::setw(left_fill);
  std::cout<<vm.name()+": ";
  std::cout<<vm.left().version_string()<<" > "<<vm.right().version_string()<<"\n";
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
    print_ver_missmatch(ver_miss, max_len);
  }
}



void describe_error(const AltDiff::Error& error) {
  if(std::holds_alternative<AltDiff::CurlError>(error)) {
    auto err = std::get<AltDiff::CurlError>(error);
    std::cout<<"Curl error(" <<err.code<<") error desc:\n";
    std::cout<<err.error_desc<<"\n";
  }
  else if(std::holds_alternative<AltDiff::HttpError>(error)) {
    auto err = std::get<AltDiff::HttpError>(error);
    std::cout<<"Unexpexted Http answer: "<<err.http_response_code<<" "<<err.content_type<<"\n";
    std::cout<<err.response_body<<"\n";

  } else if(std::holds_alternative<AltDiff::ExceptionError>(error)) {
    auto err = std::get<AltDiff::ExceptionError>(error);
    std::cout<<"Exception inside library was catched: ";
    std::cout<<err.catched_exception->what();
    std::cout<<"\n";

  } else {
    std::cout<<"Unknown error\n";
  }
}

std::vector<std::string>::iterator find_string(std::vector<std::string> &vec, const std::string& str) {
  return std::find(vec.begin(), vec.end(), str);
}

std::vector<std::string> cpp_args(int argc, char*argv[]) {
  std::vector<std::string> args;
  for(int i=1; i<argc; ++i) {
    args.push_back(argv[i]);
  }
  return args;
}

void human_print(std::map<AltDiff::Arch, AltDiff::Diff> diff) {
  for(const auto& [arch, diff] :diff) {
    std::cout<<"["<<arch<<"] = \n";
    print_diff(diff);
  }
}
int main(int argc, char *argv[]) {
  auto args = cpp_args(argc, argv);
  if(args.size() < 2 || args.size() > 4
     || (find_string(args, "--help") != std::end(args))
     || (find_string(args, "-h"))    != std::end(args)) {
    std::cout<<"Description:\n";
    std::cout<<"Print diffrence between two branches of AltLinux distributive in json\n";
    std::cout<<"Usage: "<<argv[0]<<" [flags] branch1 branch2 <arch>\n";
    std::cout<<"  where flags are:\n";
    std::cout<<"  --help\n";
    std::cout<<"  --human - print in human readable form";
    return 1;
  }

  bool human_flag = false;
  auto it = find_string(args, "--human");
  if(it != std::end(args)) {
    human_flag = true;
    args.erase(it);
  }
  std::string branch1{args[0]};
  std::string branch2{args[1]};
  std::string arch = "";
  if(args.size()>2) {
    std::cout<<arch<<"\n";
    arch = args[2];
  }
  auto r = AltDiff::Request(branch1, branch2);
  r.set_missmatch_type(AltDiff::MissmatchType::GreaterThan)->set_arch(arch);
  auto diff = AltDiff::get_diff(r);
  if(!diff) {
    describe_error(diff.error());
    return 1;
  }
  if(!human_flag) {
    std::cout<<diff.value();
  } else{
    auto diff_map = AltDiff::parse_json(diff.value());
    if(!diff_map) {
      describe_error(diff.error());
      return 1;
    }
    human_print(diff_map.value());
  }
  return 0;
}
