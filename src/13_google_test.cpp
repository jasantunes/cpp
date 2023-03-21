#include <gmock/gmock.h> // https://github.com/google/googletest/blob/main/docs/gmock_cook_book.md
#include <gtest/gtest.h>

using namespace ::testing;

class Foo {
 public:
  Foo(int value) : value_(value) {}
  virtual ~Foo() = default;
  virtual void setValue(int value) {
    value_ = value;
  }
  virtual int getValue() const {
    return value_;
  }

 private:
  int value_{0};
};

class MockFoo : public Foo {
 public:
  MockFoo() : Foo(0) {}
  MOCK_METHOD(void, setValue, (int));
  MOCK_METHOD(int, getValue, (), (const));
};

class Bar {
 protected:
  std::shared_ptr<Foo> foo_;

 public:
  Bar(const std::shared_ptr<Foo>& foo) : foo_(foo) {}
  void incrementFoo() {
    auto value = foo_->getValue();
    foo_->setValue(value + 1);
  }
  int getFooValue() const {
    return foo_->getValue();
  }
};

class BarTest : public Test {
 protected:
  std::shared_ptr<MockFoo> mockFoo_;
  std::shared_ptr<Bar> bar_;
  BarTest() {
    // You can do set-up work for each test here.
    mockFoo_ = std::make_shared<MockFoo>();
    bar_ = std::make_shared<Bar>(mockFoo_);
  }
  ~BarTest() override {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  void SetUp() override {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(BarTest, TestBarIncrementNoMock) {
  auto bar = Bar(std::make_shared<Foo>(11));
  EXPECT_EQ(bar.getFooValue(), 11);
  bar.incrementFoo();
  EXPECT_EQ(bar.getFooValue(), 12);
}

TEST_F(BarTest, TestBarIncrementWithMock) {
  ON_CALL(*mockFoo_, getValue).WillByDefault(Return(20));

  EXPECT_CALL(*mockFoo_, getValue()).Times(1);
  EXPECT_CALL(*mockFoo_, setValue(21)).Times(1);
  bar_->incrementFoo();
}
