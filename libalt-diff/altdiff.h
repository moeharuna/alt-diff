#ifndef ALTDIFF_H_
#define ALTDIFF_H_
#include <string>
#include <map>
#include <variant>
#include <boost/json.hpp>
#include <boost/outcome/outcome.hpp>
#include <curl/curl.h>
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

    friend void    tag_invoke(boost::json::value_from_tag, boost::json::value& jv, Version const&);
    friend Version tag_invoke(boost::json::value_to_tag<Version>, boost::json::value const& jv);

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

    friend void    tag_invoke(boost::json::value_from_tag, boost::json::value& jv, Package const&);
    friend Package tag_invoke(boost::json::value_to_tag<Package>, boost::json::value const& jv);
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

    friend void             tag_invoke(boost::json::value_from_tag, boost::json::value& jv, VersionMissmatch const&);
    friend VersionMissmatch tag_invoke(boost::json::value_to_tag<VersionMissmatch>, boost::json::value const& jv);

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

    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, Diff const&);
    friend Diff tag_invoke(boost::json::value_to_tag<Diff>, boost::json::value const& jv);
  private:
    //I use pimpl to hide implimentation of all my classes.
    struct Impl;
    std::unique_ptr<Impl> pImpl;
  };


  struct CurlError {
    public:
    CurlError(CURLcode code, std::string&& error_desc);
    CurlError();
    CURLcode code;
    std::string error_desc;
  };


  struct HttpError {
    public:
    HttpError(long http_response_code,
              std::string&& response_body,
              std::string&& content_type);
    long http_response_code;
    std::string response_body;
    std::string content_type;
  };

  struct ExceptionError {
    public:
    ExceptionError(std::exception&);
    std::shared_ptr<std::exception> catched_exception;
  };

  using Error = std::variant<CurlError, HttpError, ExceptionError>;
  //Returns diffrence between two branches in json format
  boost::outcome_v2::result<boost::json::value, Error> get_diff(const std::string& branch1, const std::string& branch2,
                                               const std::string &arch="",
                                               const std::string &endpoint="https://rdb.altlinux.org/api/export/branch_binary_packages/") noexcept;
  //Parse json without exceptions. Use boost::json::value_from if you want exceptions instead of result.
  boost::outcome_v2::result<std::map<Arch, Diff>, Error> parse_json(boost::json::value&) noexcept;
}

#endif // ALTDIFF_H_
