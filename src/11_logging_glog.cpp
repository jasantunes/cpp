// Remove all strings which fall below a certain severity level.
// This must go before the #include!
// #define GOOGLE_STRIP_LOG 0   // FATAL, ERROR, WARNING, INFO
// #define GOOGLE_STRIP_LOG 1   // FATAL, ERROR, WARNING
// #define GOOGLE_STRIP_LOG 2   // FATAL, ERROR
// #define GOOGLE_STRIP_LOG 3   // FATAL

#include <glog/logging.h>
#include <folly/logging/xlog.h>
#include <iomanip>      // std::setw
#include <chrono> // sleep
#include <thread> // sleep

// Configure folly to enable INFO+ messages, and everything else to
// enable WARNING+.
//
// Set the default log handler to log asynchronously by default
FOLLY_INIT_LOGGING_CONFIG(".=WARNING,folly=INFO; default:async=true,sync_level=WARNING");

/* This function writes a prefix that matches glog's default format.
* (The third parameter can be used to receive user-supplied data, and is
* NULL by default.)
*/
void CustomPrefix(std::ostream &s, const google::LogMessageInfo &l, void*) {
    s << "["
    // << std::setw(4) << 1900 + l.time.year()
    // << std::setw(2) << 1 + l.time.month()
    // << std::setw(2) << l.time.day()
    // << ' '
    << std::setw(2) << l.time.hour() << ':'
    << std::setw(2) << l.time.min()  << ':'
    << std::setw(2) << l.time.sec() << "."
    << std::setw(3) << l.time.usec() / 1000
    // << ' '
    // << std::setfill(' ') << std::setw(5)
    // << l.thread_id << std::setfill('0')
    // << ' '
    // << l.filename << ':' << l.line_number
    << ' ' << l.severity[0] << ']';
}

using namespace std::chrono_literals;

int main(int argc, char* argv[]) {    // Load XML configuration file using DOMConfigurator
    // google::EnableLogCleaner(3); // keep your logs for 3 days
    // google::InitGoogleLogging(argv[0]);
    google::InitGoogleLogging(argv[0], &CustomPrefix);

    // Also log to console.
    FLAGS_logtostderr = 1;

    // Log levels.
    LOG(INFO) << "You can log INFO messages";
    LOG(WARNING) << "You can log WARNING messages";
    LOG(ERROR) << "You can log ERROR messages";
    // LOG(FATAL) << "You can log FATAL messages, which also aborts.";

    // Verbose logging.
    VLOG(1) << "I’m printed when you run the program with --v=1 or higher";
    VLOG(2) << "I’m printed when you run the program with --v=2 or higher";

    // Normal logging.
    int num_cookies = 20;
    LOG(INFO) << "Found " << num_cookies << " cookies";

    // Conditional logging.
    LOG_IF(INFO, num_cookies > 10) << "Got lots of cookies";

    // Output a message every nth time.
    // Note that the special google::COUNTER value is used to identify which repetition is happening.
    LOG(INFO) << "LOG_EVERY_N(INFO, 10)";
    for (int i = 0; i < num_cookies; i++) {
        LOG_EVERY_N(INFO, 10) << "Got the " << google::COUNTER << "th cookie";
    }

    // Limit the output to the first n occurrences.
    LOG(INFO) << "LOG_FIRST_N(INFO, 3)";
    for (int i = 0; i < num_cookies; i++) {
        LOG_FIRST_N(INFO, 3) << "Got the " << google::COUNTER << "th cookie";
    }

    // Output a message every nth second.
    for (int i = 0; i < num_cookies; i++) {
        LOG_EVERY_T(INFO, 0.1) << "Got a cookie every 100ms";
        std::this_thread::sleep_for(10ms);
    }

    // Only in debug mode
    DLOG(INFO) << "Use DLOG() to log only when compiled in debug mode (-DNDEBUG)";
    DLOG(INFO) << "This won't log or slow down if compiled with -DNDEBUG";

    // CHECK asserts conditions regardless of compilation mode.
    CHECK(num_cookies > 0) << "We got no cookies!";

    // There are various helper macros for equality/inequality checks:
    // CHECK_EQ, CHECK_NE, CHECK_LE, CHECK_LT, CHECK_GE, and CHECK_GT.
    // They compare two values, and log a FATAL message including the
    // two values when the result is not as expected.
    CHECK_EQ(num_cookies, 20) << "I was expecting 20 cookies!";
    // Check failed: num_cookies == xx (20 vs. xx) : I was expecting xx cookies!

    CHECK_NE(num_cookies, 19) << "I was not expecting 19 cookies!";
    CHECK_LE(num_cookies, 20) << "I was expecting ≤ 20 cookies!";
    CHECK_GE(num_cookies, 20) << "I was expecting ≥ 20 cookies!";
    CHECK_LT(num_cookies, 21) << "I was expecting < 21 cookies!";
    CHECK_GT(num_cookies, 19) << "I was expecting > 19 cookies!";

    // Check pointers.
    CHECK_NOTNULL(std::make_shared<int>());

    // Check C strings: CHECK_STREQ, CHECK_STRNE, CHECK_STRCASEEQ, and CHECK_STRCASENE.
    // The CASE versions are case-insensitive. You can safely pass NULL pointers for this macro.
    CHECK_STREQ("foo", "foo");

    return 0;
}
