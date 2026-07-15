#pragma once

#include <chrono>
#include <condition_variable>
#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <stdexcept>
#include <thread>

#include "pickup/timer/TimerTask.h"

namespace pickup {
namespace timer {

/**
 * @brief 将 std::function 适配为 TimerTask 的适配器
 */
class TimerTaskAdapter final : public TimerTask {
 public:
  explicit TimerTaskAdapter(std::function<void()> func) : func_(std::move(func)) {}
  void run() final { func_(); }

 private:
  std::function<void()> func_;
};

/**
 * @brief 定时器，支持一次性 / 重复执行的任务调度
 *
 * 内部维护一个专用工作线程，顺序执行到期的任务。
 *
 * 线程与重入：所有任务在同一工作线程上【顺序】执行，单个慢任务会延后其它到期任务。
 * 可在任务内部安全调用 schedule/reschedule/scheduleRepeated/scheduleAtFixedRate/
 * cancel（这些接口在任务执行期间不持锁）；但【不可】在任务内调用 stop()。stop() 与
 * 析构应由单一拥有者线程执行，二者并发调用非安全。
 *
 * @note 所有调度接口的 delay/period 必须非负且不致时间点溢出——这是调用方须满足
 *       的前置条件，违反在调试期触发断言、release 期为未定义行为（不作为异常报告）。
 *       而"定时器已销毁""任务已重复调度"属可恢复的运行期错误，仍以异常报告。
 */
class Timer {
 public:
  Timer();
  ~Timer();

  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;

  /**
   * @brief 停止定时器并等待工作线程退出
   * @note 不可在定时任务内部调用，否则抛出 std::runtime_error。
   * @note 阻塞直至【正在执行】的任务返回（join 语义），长任务会相应延长本调用。
   */
  void stop();

  /**
   * @brief 在指定延迟后执行一个函数
   * @tparam Rep 延迟时间类型
   * @tparam Period 延迟时间精度
   * @param func  要执行的函数
   * @param delay 延迟时间
   * @return 内部创建的任务句柄，可用于 cancel()（在其触发前）
   */
  template <class Rep, class Period>
  TimerTaskPtr schedule(std::function<void()> func, const std::chrono::duration<Rep, Period>& delay) {
    auto task = std::make_shared<TimerTaskAdapter>(std::move(func));
    schedule(task, delay);
    return task;
  }

  /**
   * @brief 在指定延迟后执行一个定时任务
   * @tparam Rep 延迟时间类型
   * @tparam Period 延迟时间精度
   * @param task  定时任务
   * @param delay 延迟时间
   */
  template <class Rep, class Period>
  void schedule(const TimerTaskPtr& task, const std::chrono::duration<Rep, Period>& delay) {
    addTask(task, std::chrono::duration_cast<std::chrono::nanoseconds>(delay), std::nullopt, false, false);
  }

  /**
   * @brief 重新调度一个任务：取消其现有排期（若有），改为在 delay 后【一次性】执行
   * @tparam Rep 延迟时间类型
   * @tparam Period 延迟时间精度
   * @param task  要重新调度的任务
   * @param delay 新的延迟时间
   * @note 即使 task 之前未调度，也会将其加入调度。
   * @note 无论此前是否为重复任务，重排后一律变为一次性；如需改变重复任务的周期，
   *       请先 cancel() 再 scheduleRepeated() / scheduleAtFixedRate()。
   */
  template <class Rep, class Period>
  void reschedule(const TimerTaskPtr& task, const std::chrono::duration<Rep, Period>& delay) {
    addTask(task, std::chrono::duration_cast<std::chrono::nanoseconds>(delay), std::nullopt, true, false);
  }

  /**
   * @brief 以固定延迟重复执行任务（fixed-delay）
   * @tparam Rep 延迟时间类型
   * @tparam Period 延迟时间精度
   * @param task  要重复执行的任务
   * @param delay 首次延迟，以及每次执行【完成后】到下次执行的间隔
   * @note 下次执行以上次【完成时刻】为基准，故实际周期会累积任务执行耗时的漂移。
   *       需要稳定平均频率请用 scheduleAtFixedRate()。
   */
  template <class Rep, class Period>
  void scheduleRepeated(const TimerTaskPtr& task, const std::chrono::duration<Rep, Period>& delay) {
    const auto period = std::chrono::duration_cast<std::chrono::nanoseconds>(delay);
    addTask(task, period, period, false, false);
  }

  /**
   * @brief 以固定延迟重复执行一个函数（fixed-delay）
   * @return 内部创建的任务句柄，用于 cancel()（重复任务通常需保留以便取消）
   * @see scheduleRepeated(const TimerTaskPtr&, const std::chrono::duration&)
   */
  template <class Rep, class Period>
  TimerTaskPtr scheduleRepeated(std::function<void()> func, const std::chrono::duration<Rep, Period>& delay) {
    auto task = std::make_shared<TimerTaskAdapter>(std::move(func));
    scheduleRepeated(task, delay);
    return task;
  }

  /**
   * @brief 以固定频率重复执行任务（fixed-rate）
   * @tparam Rep 周期时间类型
   * @tparam Period 周期时间精度
   * @param task   要重复执行的任务
   * @param period 首次延迟，以及相邻两次【计划执行时刻】之间的间隔
   * @note 下次执行以上次【计划时刻】+ period 为基准，不受本次执行耗时影响，长期
   *       平均频率稳定。若某次执行超时导致落后，后续会连续补触发以追赶进度。
   * @warning 单工作线程：若任务单次执行经常超过 period，追赶式的连续补触发会持续
   *          占用工作线程，饿死其它任务。此类负载应改用 scheduleRepeated 或缩短任务。
   */
  template <class Rep, class Period>
  void scheduleAtFixedRate(const TimerTaskPtr& task, const std::chrono::duration<Rep, Period>& period) {
    const auto p = std::chrono::duration_cast<std::chrono::nanoseconds>(period);
    addTask(task, p, p, false, true);
  }

  /**
   * @brief 以固定频率重复执行一个函数（fixed-rate）
   * @return 内部创建的任务句柄，用于 cancel()（重复任务通常需保留以便取消）
   * @see scheduleAtFixedRate(const TimerTaskPtr&, const std::chrono::duration&)
   */
  template <class Rep, class Period>
  TimerTaskPtr scheduleAtFixedRate(std::function<void()> func, const std::chrono::duration<Rep, Period>& period) {
    auto task = std::make_shared<TimerTaskAdapter>(std::move(func));
    scheduleAtFixedRate(task, period);
    return task;
  }

  /**
   * @brief 取消一个任务
   * @param task 要取消的任务
   * @return 成功取消返回 true；任务尚未执行、已取消或从未调度返回 false
   */
  bool cancel(const TimerTaskPtr& task) noexcept;

  /** @brief 检查任务是否已调度 */
  bool isScheduled(const TimerTaskPtr& task) const;

 protected:
  /**
   * @brief 执行定时任务
   * @param task 要执行的任务
   * @note 子类可重写以自定义执行逻辑
   */
  virtual void executeTask(const TimerTaskPtr& task);

  /**
   * @brief 任务执行抛出异常时的处理钩子
   * @param task  抛出异常的任务
   * @param error 捕获到的异常（可 std::rethrow_exception 后 catch 以获取详情）
   * @note 默认实现吞掉异常，保证定时线程不因单个任务失败而中断；子类可重写以
   *       记录日志等。库本身不向任何流输出，输出通道由使用方决定。
   * @warning 本钩子在定时线程、锁外调用，且【不得】抛出异常（否则 terminate）。
   */
  virtual void onException(const TimerTaskPtr& task, std::exception_ptr error) noexcept;

 private:
  struct Token {
    std::chrono::steady_clock::time_point scheduledTime;
    std::optional<std::chrono::nanoseconds> delay;
    TimerTaskPtr task;
    bool fixedRate = false;  ///< true=固定频率(基于计划时刻)，false=固定延迟(基于完成时刻)

    // 排序仅依据 (scheduledTime, task)，delay/fixedRate 不参与比较——cancelNoSync
    // 据此可用默认的 delay/fixedRate 重建键来定位并删除条目。
    bool operator<(const Token& other) const {
      if (scheduledTime < other.scheduledTime) return true;
      if (scheduledTime > other.scheduledTime) return false;
      return task < other.task;
    }
  };

  // schedule/reschedule/scheduleRepeated/scheduleAtFixedRate 的公共实现：加锁校验、
  // 登记并按需唤醒。repeat 非空表示重复任务；reschedule 为 true 时先移除旧的同一
  // 任务；fixedRate 区分重复任务的重排基准。
  void addTask(const TimerTaskPtr& task, std::chrono::nanoseconds delay,
               std::optional<std::chrono::nanoseconds> repeat, bool reschedule, bool fixedRate);
  void runLoop();
  bool cancelNoSync(const TimerTaskPtr& task) noexcept;

  mutable std::mutex mutex_;
  std::condition_variable condition_;
  std::set<Token> tokens_;
  std::map<TimerTaskPtr, std::chrono::steady_clock::time_point> tasks_;
  bool destroyed_{false};
  std::chrono::steady_clock::time_point wakeUpTime_;
  std::thread worker_;
};

}  // namespace timer
}  // namespace pickup