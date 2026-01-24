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
#include "vertexnova/logging/core/log_queue_worker.h"

using namespace vne;

class LogQueueWorkerTest : public ::testing::Test {
   public:
    LogQueueWorkerTest()
        : worker_(log_queue_) {}

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
    log::LogQueueWorker worker_;
};

TEST_F(LogQueueWorkerTest, TestStartAndStop) {
    std::atomic<int> counter(0);
    auto log_task1 = createLogTask(counter, 1);
    auto log_task2 = createLogTask(counter, 2);

    // Start the worker
    worker_.start();

    // Push tasks
    log_queue_.push(log_task1);
    log_queue_.push(log_task2);

    // Give some time for the worker to process the tasks
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Stop the worker
    worker_.stop();

    // Ensure the tasks were executed correctly by checking the counter value
    ASSERT_EQ(counter.load(), 3);

    // Ensure no tasks are left in the queue
    ASSERT_TRUE(log_queue_.empty());
}

TEST_F(LogQueueWorkerTest, TestFlush) {
    std::atomic<int> counter(0);
    auto log_task1 = createLogTask(counter, 1);
    auto log_task2 = createLogTask(counter, 2);

    // Push tasks
    log_queue_.push(log_task1);
    log_queue_.push(log_task2);

    // Flush tasks
    worker_.flush();

    // Ensure the tasks were executed correctly by checking the counter value
    ASSERT_EQ(counter.load(), 3);

    // Ensure no tasks are left in the queue
    ASSERT_TRUE(log_queue_.empty());
}

TEST_F(LogQueueWorkerTest, TestConcurrentStartAndStop) {
    std::atomic<int> counter(0);
    auto log_task1 = createLogTask(counter, 1);
    auto log_task2 = createLogTask(counter, 2);

    // Start the worker
    worker_.start();

    std::thread t1([&]() { log_queue_.push(log_task1); });

    std::thread t2([&]() { log_queue_.push(log_task2); });

    std::thread t3([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        worker_.stop();
    });

    t1.join();
    t2.join();
    t3.join();

    // Ensure the tasks were executed correctly by checking the counter value
    ASSERT_EQ(counter.load(), 3);

    // Ensure no tasks are left in the queue
    ASSERT_TRUE(log_queue_.empty());
}
