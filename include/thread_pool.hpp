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
	std::atomic<size_t> _busy_thread_num;
	// 活着的线程数
	std::atomic<size_t> _alive_thread_num;
	// 添加/取出任务互斥量
	std::mutex _task_mtx;
	// 任务队列为空的条件变量
	std::condition_variable _empty_cond;
public:
	thread_pool() {
		_running = true;
		_busy_thread_num = 0;
		_alive_thread_num = 0;
	}

	thread_pool(const thread_pool &) = delete;

	thread_pool &operator=(const thread_pool &) = delete;

	thread_pool(thread_pool &&rvalue) noexcept {
		*this = std::forward<thread_pool &&>(rvalue);
	}

	~thread_pool() {
		destroy();
	}

	thread_pool &operator=(thread_pool &&rhs) noexcept {
		_thread_list = std::move(rhs._thread_list);
		_task_q = std::move(rhs._task_q);
		_running.exchange(rhs._running);
		_busy_thread_num.exchange(rhs._busy_thread_num);
		_alive_thread_num.exchange(rhs._alive_thread_num);
		return *this;
	}

	// add task
	template<typename Callable, typename... Args>
	auto push_task(Callable &&call, Args &&...args)->std::future<decltype(call(args...))> {
		// deduce return type
		using RetType = decltype(call(args...));
		auto *task = new std::packaged_task<RetType()>(
				std::bind(std::forward<Callable>(call), std::forward<Args>(args)...));

		std::unique_lock<std::mutex> lk(_task_mtx);
		// add task
		_task_q.push([task] {
			(*task)();
			delete task;
		});
		_empty_cond.notify_one();
		return task->get_future();
	}

	// init thread pool
	void start(int create_num) {
		for (int i = 0; i < create_num; ++i) {
			_thread_list.emplace_back(thread_pool::routine, this);
		}
		_alive_thread_num.fetch_add(create_num);
	}

	// thread destroy
	void destroy() {
		_running = false;
		for (auto &t : _thread_list) {
			std::thread::id id = t.get_id();
			_empty_cond.notify_all();
			if (t.joinable()) {
				t.join();
				std::cout << "jobs thread " << id << " exiting..." << std::endl;
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
				task = pool->_task_q.front();
				pool->_task_q.pop();
			}

			++pool->_busy_thread_num;
			task();
			--pool->_busy_thread_num;

			// thread exit
			if (!pool->_running) return;
		}
	}
};

#endif //THREAD_POOL_THREAD_POOL_HPP








