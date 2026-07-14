#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include "pickup/utils/Observer.h"

using namespace pickup::utils;

namespace {

struct TestArg : ObserverArg {
  explicit TestArg(int v) : value(v) {}
  int value = 0;
};

class TestObservable : public Observable {
 public:
  void triggerChange() { setChanged(); }
  void triggerChange(const ObserverArg* arg) {
    setChanged();
    notifyObservers(arg);
  }
  void triggerNotify() {
    setChanged();
    notifyObservers();
  }
};

class RecordingObserver : public Observer {
 public:
  void update(const Observable* o, const ObserverArg* arg) override {
    ++callCount_;
    lastObservable_ = o;
    if (auto* ta = dynamic_cast<const TestArg*>(arg)) {
      lastValue_ = ta->value;
    }
  }

  int callCount_ = 0;
  const Observable* lastObservable_ = nullptr;
  int lastValue_ = 0;
};

}  // namespace

TEST(ObserverTest, AddObserver) {
  auto observable = std::make_shared<TestObservable>();
  auto observer = std::make_shared<RecordingObserver>();
  observable->addObserver(observer);
  EXPECT_EQ(observable->observerCount(), 1);
}

TEST(ObserverTest, RemoveObserver) {
  auto observable = std::make_shared<TestObservable>();
  auto observer = std::make_shared<RecordingObserver>();
  observable->addObserver(observer);
  EXPECT_EQ(observable->observerCount(), 1);
  observable->removeObserver(observer);
  EXPECT_EQ(observable->observerCount(), 0);
}

TEST(ObserverTest, NotifyWithNoChangeDoesNothing) {
  auto observable = std::make_shared<TestObservable>();
  auto observer = std::make_shared<RecordingObserver>();
  observable->addObserver(observer);
  observable->notifyObservers();
  EXPECT_EQ(observer->callCount_, 0);
}

TEST(ObserverTest, NotifyAfterChange) {
  auto observable = std::make_shared<TestObservable>();
  auto observer = std::make_shared<RecordingObserver>();
  observable->addObserver(observer);
  observable->triggerNotify();
  EXPECT_EQ(observer->callCount_, 1);
}

TEST(ObserverTest, NotifyWithArg) {
  auto observable = std::make_shared<TestObservable>();
  auto observer = std::make_shared<RecordingObserver>();
  observable->addObserver(observer);
  TestArg arg(42);
  observable->triggerChange(&arg);
  EXPECT_EQ(observer->callCount_, 1);
  EXPECT_EQ(observer->lastValue_, 42);
}

TEST(ObserverTest, RemoveAllObservers) {
  auto observable = std::make_shared<TestObservable>();
  auto o1 = std::make_shared<RecordingObserver>();
  auto o2 = std::make_shared<RecordingObserver>();
  observable->addObserver(o1);
  observable->addObserver(o2);
  EXPECT_EQ(observable->observerCount(), 2);
  observable->removeAllObservers();
  EXPECT_EQ(observable->observerCount(), 0);
  observable->triggerNotify();
  EXPECT_EQ(o1->callCount_, 0);
  EXPECT_EQ(o2->callCount_, 0);
}

TEST(ObserverTest, MultipleObservers) {
  auto observable = std::make_shared<TestObservable>();
  auto o1 = std::make_shared<RecordingObserver>();
  auto o2 = std::make_shared<RecordingObserver>();
  observable->addObserver(o1);
  observable->addObserver(o2);
  observable->triggerNotify();
  EXPECT_EQ(o1->callCount_, 1);
  EXPECT_EQ(o2->callCount_, 1);
}

TEST(ObserverTest, AddNullptrDoesNothing) {
  auto observable = std::make_shared<TestObservable>();
  observable->addObserver(nullptr);
  EXPECT_EQ(observable->observerCount(), 0);
}

TEST(ObserverTest, AddDuplicateDoesNothing) {
  auto observable = std::make_shared<TestObservable>();
  auto observer = std::make_shared<RecordingObserver>();
  observable->addObserver(observer);
  observable->addObserver(observer);
  EXPECT_EQ(observable->observerCount(), 1);
}

TEST(ObserverTest, RemovedObserverNotNotified) {
  auto observable = std::make_shared<TestObservable>();
  auto observer = std::make_shared<RecordingObserver>();
  observable->addObserver(observer);
  observable->removeObserver(observer);
  observable->triggerNotify();
  EXPECT_EQ(observer->callCount_, 0);
}
