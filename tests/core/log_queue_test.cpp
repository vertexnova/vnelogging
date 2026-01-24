/* ---------------------------------------------------------------------
 * Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   JULY-2024
 *
 * Autodoc:   yes
 * ----------------------------------------------------------------------
 */

#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include "vertexnova/logging/core/log_queue.h"

using namespace vne;

class LogQueueTest : public ::testing::Test {
   protected:
    // Utility function to simulate a logging task that modifies a shared atomic variable
    std::function<void()> createLogTask(std::atomic<int>& counter, int id) {
        return [&counter, id]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            counter += id;
        };
    }

   protected:
    log::LogQueue log_queue_;
};

TEST_F(LogQueueTest, TestPushAndPop) {
    std::atomic<int> counter(0);

    auto log_task1 = createLogTask(counter, 1);
    auto log_task2 = createLogTask(counter, 2);

    // Test push
    log_queue_.push(log_task1);
    log_queue_.push(log_task2);

    // Test empty
    ASSERT_FALSE(log_queue_.empty());

    // Test pop and execute tasks
    auto popped_task1 = log_queue_.pop();
    auto popped_task2 = log_queue_.pop();

    popped_task1();
    popped_task2();

    // Ensure the tasks were executed correctly by checking the counter value
    ASSERT_EQ(counter.load(), 3);

    // Test empty after popping all tasks
    ASSERT_TRUE(log_queue_.empty());
}

TEST_F(LogQueueTest, TestEmptyInitially) {
    ASSERT_TRUE(log_queue_.empty());
}

TEST_F(LogQueueTest, TestPopBlocksWhenEmpty) {
    std::atomic<int> counter(0);
    auto log_task = createLogTask(counter, 1);

    std::thread t([&]() {
        // This will block until a task is pushed
        auto popped_task = log_queue_.pop();
        popped_task();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_TRUE(log_queue_.empty());

    log_queue_.push(log_task);

    t.join();
    ASSERT_TRUE(log_queue_.empty());
    ASSERT_EQ(counter.load(), 1);
}

TEST_F(LogQueueTest, TestConcurrentPushAndPop) {
    std::atomic<int> counter(0);

    auto log_task1 = createLogTask(counter, 1);
    auto log_task2 = createLogTask(counter, 2);

    std::thread t1([&]() { log_queue_.push(log_task1); });

    std::thread t2([&]() { log_queue_.push(log_task2); });

    std::thread t3([&]() {
        auto popped_task1 = log_queue_.pop();
        popped_task1();
    });

    std::thread t4([&]() {
        auto popped_task2 = log_queue_.pop();
        popped_task2();
    });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(log_queue_.empty());
    ASSERT_EQ(counter.load(), 3);
}
