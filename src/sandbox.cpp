#include <memory>
#include <vector>
#include <iostream>

struct AppPluginWithClient {
  virtual void co_shutdown() {
    std::cout << "shutdown" << std::endl;
  }
};

struct Plugin1 : public AppPluginWithClient {
  virtual void co_shutdown() override {
    std::cout << "shutdown-1" << std::endl;
  }
};
struct Plugin2 : public AppPluginWithClient {
  virtual void co_shutdown() override {
    std::cout << "shutdown-2" << std::endl;
  }
};

template <typename T>
struct PluginTester : public T {
};

struct ParticipantPipelineTester {
  template <typename T>
  std::shared_ptr<PluginTester<T>> makeAndConnectPlugin() {
    return std::make_shared<PluginTester<T>>();
  }
};

struct BaseApperPluginTest {
  std::vector<std::shared_ptr<void>> plugins_;

  template <typename T>
  void makeAndConnectPlugin(const std::shared_ptr<ParticipantPipelineTester>& pp) {
    auto plugin = pp->makeAndConnectPlugin<T>();
    plugins_.push_back(plugin);
  }

  void shutdown() {
    for (const auto& ptr : plugins_) {
      auto plugin =
          std::static_pointer_cast<PluginTester<AppPluginWithClient>>(ptr);
      plugin->co_shutdown();
    }
    plugins_.clear();
  }
};

int main() {
  auto test = BaseApperPluginTest();
  auto pp = std::make_shared<ParticipantPipelineTester>();
  test.makeAndConnectPlugin<Plugin1>(pp);
  test.makeAndConnectPlugin<Plugin2>(pp);
  std::cout << "size: " << test.plugins_.size() << std::endl;
  test.shutdown();
  std::cout << "size: " << test.plugins_.size() << std::endl;

}

