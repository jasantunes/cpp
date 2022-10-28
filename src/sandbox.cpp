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
  std::unique_ptr<PluginTester<T>> makeAndConnectPlugin() {
    return std::make_unique<PluginTester<T>>();
  }
};

struct BaseApperPluginTest {
  std::vector<std::unique_ptr<PluginTester<AppPluginWithClient>>> plugins_;

  template <typename T>
  void makeAndConnectPlugin(const std::unique_ptr<ParticipantPipelineTester>& pp) {
    auto plugin = pp->makeAndConnectPlugin<T>();
    // plugin is type: std::unique_ptr<PluginTester<T>>
    // T is subclass of AppPluginWithClient
    // How to convert std::unique_ptr<PluginTester<T>> -> std::unique_ptr<PluginTester<AppPluginWithClient>>?
    auto* ptr1 = reinterpret_cast<PluginTester<AppPluginWithClient>*>(plugin.release());
    plugins_.push_back(std::unique_ptr<PluginTester<AppPluginWithClient>>(ptr1));
  }

  void shutdown() {
    for (const auto& plugin : plugins_) {
      plugin->co_shutdown();
    }
    plugins_.clear();
  }
};

int main() {
  auto test = BaseApperPluginTest();
  auto pp = std::make_unique<ParticipantPipelineTester>();
  test.makeAndConnectPlugin<Plugin1>(pp);
  test.makeAndConnectPlugin<Plugin2>(pp);
  std::cout << "size: " << test.plugins_.size() << std::endl;
  test.shutdown();
  std::cout << "size: " << test.plugins_.size() << std::endl;
}

