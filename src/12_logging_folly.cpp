#include <folly/init/Init.h>
#include <folly/logging/Init.h>
#include <folly/logging/xlog.h>
#include <fmt/format.h>
#include <folly/Range.h>

class ExampleObject {
 public:
  explicit ExampleObject(folly::StringPiece str) : value_{str.str()} {
    // All XLOG() statements in this file will log to the category
    // folly.logging.example.lib
    XLOGF(DBG1, "ExampleObject({}) constructed at {}", value_, fmt::ptr(this));
  }
  ~ExampleObject() {
    // All XLOG() statements in this file will log to the category
    // folly.logging.example.lib
    XLOGF(DBG1, "ExampleObject({}) at {} destroyed", value_, fmt::ptr(this));
  }

  void doStuff();

 private:
  std::string value_;
};
// Configure folly to enable INFO+ messages, and everything else to
// enable WARNING+.

// Set the default log handler to log asynchronously by default
FOLLY_INIT_LOGGING_CONFIG(".=WARNING,folly=INFO; default:async=true,sync_level=WARNING");

int main(int argc, char* argv[]) {    // Load XML configuration file using DOMConfigurator
    // Using log macros before calling folly::initLogging() will use the default
    // log settings defined by folly::initializeLoggerDB().  The default behavior
    // is to log INFO+ messages to stderr.
    XLOG(DBG) << "log messages less than INFO will be ignored before initLogging";
    XLOG(ERR) << "error messages before initLogging() will be logged to stderr";

    // folly::Init() will automatically initialize the logging settings based on
    // the FOLLY_INIT_LOGGING_CONFIG declaration above and the --logging command
    // line flag.
    folly::Init init(&argc, &argv);

    // All XLOG() statements in this file will log to the category
    // folly.logging.example.main
    XLOG(INFO, "now the normal log settings have been applied");

    XLOG(DBG1, "log arguments are concatenated: ", 12345, ", ", 92.0);
    XLOGF(DBG1, "XLOGF supports {}-style formatting: {:.3f}", "python", 1.0 / 3);
    XLOG(DBG2) << "streaming syntax is also supported: " << 1234;
    XLOG(DBG2, "if you really want, ", "you can even")
        << " mix function-style and streaming syntax: " << 42;
    XLOGF(DBG3, "and {} can mix {} style", "you", "format") << " and streaming";

    ExampleObject("foo");
    XLOG(INFO) << "main returning";


    return 0;
}
