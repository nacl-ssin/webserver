//
// Created by 26372 on 2022/3/10.
//

#ifndef THREAD_POOL_THREAD_POOL_HPP
#define THREAD_POOL_THREAD_POOL_HPP

#include <list>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <future>
#include <iostream>
#include <functional>
#include <condition_variable>
#include "log.h"


class thread_pool {
	using task_type = std::function<void()>;
public:
	// 任务队列
	std::queue<task_type> _task_q;
	// 工作线程
	std::list<std::thread> _thread_list;
	// 线程池是否运行
	std::atomic<bool> _running;
	// 正在工作的线程数
	std::atomic<std::size_t> _busy_thread_num;
	// 活着的线程数
	std::atomic<std::size_t> _alive_thread_num;
	// 添加/取出任务互斥量
	std::mutex _task_mtx;
	// 任务队列为空的条件变量
	std::condition_variable _empty_cond;


public:
	thread_pool() : _running(true), _busy_thread_num(0), _alive_thread_num(0) {

	}

	thread_pool(const thread_pool &) = delete;

	thread_pool(thread_pool &&) = delete;

	~thread_pool() {
		if (_running) {
			destroy();
		}
	}

public:
	// add task
	template<typename Callable, typename... Args>
	auto push_task(Callable &&call, Args &&...args)->std::future<decltype(call(args...))> {
		// deduce return type
		using RetType = decltype(call(args...));
		auto task = std::make_shared<std::packaged_task<RetType()>>(
				std::bind(std::forward<Callable>(call), std::forward<Args>(args)...));

		//LOG_INFO("push a task");

		{
			std::unique_lock<std::mutex> lk(_task_mtx);
			// add task
			_task_q.push([task] {
				(*task)();
			});
		}

		//LOG_INFO("push a task size = %d", _task_q.size());
		_empty_cond.notify_one();
		return task->get_future();
	}


	// init thread pool
	void start(std::size_t create_num = std::thread::hardware_concurrency() * 10) {
		//LOG_INFO("create num = %lu", create_num);
		for (size_t i = 0; i < create_num; ++i) {
			_thread_list.emplace_back(thread_pool::routine, this);
		}
		_alive_thread_num.fetch_add(create_num);
		LOG_INFO("thread pool init success num = %d", create_num);
	}

	// thread destroy
	void destroy() {
		_running = false;
		for (auto &t : _thread_list) {
			_empty_cond.notify_all();
			if (t.joinable()) {
				t.join();
				LOG_INFO("jobs thread %lu exiting...", pthread_self());
			}
		}
	}

	//  now busy thread num
	size_t busy_num() const {
		return _busy_thread_num;
	}

	// alive thread num
	size_t alive_num() const {
		return _alive_thread_num;
	}

public:
	thread_pool &operator=(const thread_pool &) = delete;

	thread_pool &operator=(const thread_pool &&) = delete;

private:
	// thread routine
	static void routine(thread_pool *pool) {
		while (pool->_running) {
			thread_pool::task_type task;
			{
				std::unique_lock<std::mutex> lk(pool->_task_mtx);
				while (pool->_task_q.empty()) {
					// wait push task
					pool->_empty_cond.wait(lk);
					// thread exit
					if (!pool->_running) {
						return;
					}
				}
				// get one task
				task = std::move(pool->_task_q.front());
				pool->_task_q.pop();
			}
			//LOG_INFO("get a task");

			++pool->_busy_thread_num;
			//LOG_INFO("thread id = %lu running...", pthread_self());
			task();
			--pool->_busy_thread_num;
			LOG_INFO("task run end");

			// thread exit
			if (!pool->_running) return;
		}
	}
};

#endif //THREAD_POOL_THREAD_POOL_HPP








