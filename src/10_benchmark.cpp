#include <folly/Benchmark.h>
#include <vector>
#include <numeric>


using namespace std;
using namespace folly;

// auto vec = std::vector<std::string>{{"key1", "key2", "key3", "key4", "key5"}};
auto comma = [](std::string a, std::string b) {
    return (a.empty()) ? std::move(b) : std::move(a) + ", " + std::move(b);
};


BENCHMARK(accumulate, n) {
  folly::BenchmarkSuspender suspender;
  std::vector<std::string> vec{};
  for (auto i = 0; i < n; ++i) {
    vec.emplace_back("key" + std::to_string(i));
  }
  suspender.dismiss(); // start measuring benchmark time

  std::accumulate(vec.begin(), vec.end(), std::string{}, comma);

  folly::doNotOptimizeAway(vec);

  suspender.rehire(); // stop measuring benchmark time
  // vec is destructed at this point
  // suspender is destructed last because it is declared first
}

BENCHMARK(reduce, n) {
  folly::BenchmarkSuspender suspender;
  std::vector<std::string> vec{};
  for (auto i = 0; i < n; ++i) {
    vec.emplace_back("key" + std::to_string(i));
  }
  suspender.dismiss();

  std::reduce(vec.begin(), vec.end(), std::string{}, comma);

  folly::doNotOptimizeAway(vec);

  suspender.rehire();
}

// run with --bm_min_iters=100000
int main(int argc, char** argv) {
  // facebook::initFacebook(&argc, &argv);
  runBenchmarks();
}
