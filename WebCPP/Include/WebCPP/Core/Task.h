#pragma once

#include "JSValue.h"
#include "JSCallback.h"

#include <coroutine>
#include <memory>
#include <functional>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>
#include <stdexcept>
#include <optional>
#include <chrono>

namespace web
{
	class task_state_base;
	template<typename R> class task;
	template<typename R> class task_state;
	template<typename R> class task_promise;

	class task_resumer
	{
	public:
		virtual ~task_resumer() = default;
		virtual void resume(std::coroutine_handle<> awaiting_coroutine, std::shared_ptr<task_state_base> awaiter) = 0;

		static std::shared_ptr<task_resumer>& get_thread_local();
	};

	class task_state_base
	{
	public:
		task_state_base()
		{
			save_thread_local();
		}

		void restore_thread_local()
		{
			// To do: create some abstraction for restoring thread local variables that should follow the task
		}

		void save_thread_local()
		{
			// To do: create some abstraction for saving thread local variables that should follow the task
		}

		struct
		{
			std::coroutine_handle<> coroutine;
			std::shared_ptr<task_state_base> awaiter;
		} awaiting_task;

		std::atomic<int> refcount = 1;
		std::shared_ptr<task_resumer> resumer = task_resumer::get_thread_local();
	};

	class task_resumer_immediately : public task_resumer
	{
	public:
		void resume(std::coroutine_handle<> awaiting_coroutine, std::shared_ptr<task_state_base> awaiter) override
		{
			awaiter->restore_thread_local();
			awaiting_coroutine.resume();
		}
	};

	class task_resumer_threadpool : public task_resumer
	{
	public:
		task_resumer_threadpool(std::function<void(std::function<void()> task)> queueTask) : queueTask(queueTask)
		{
		}

		void resume(std::coroutine_handle<> awaiting_coroutine, std::shared_ptr<task_state_base> awaiter) override
		{
			queueTask([=]() {
				awaiter->restore_thread_local();
				awaiting_coroutine.resume();
				});
		}

		std::function<void(std::function<void()> task)> queueTask;
	};

	template<typename R = void>
	class task_state : public task_state_base
	{
	public:
		void set_value(const R& v)
		{
			stage_value(v);
			complete();
		}

		void set_value(R&& v)
		{
			stage_value(v);
			complete();
		}

		void set_exception(std::exception_ptr p)
		{
			stage_exception(p);
			complete();
		}

		void stage_value(const R& v)
		{
			std::unique_lock lock(get_mutex());
			value = v;
		}

		void stage_value(R&& v)
		{
			std::unique_lock lock(get_mutex());
			value = v;
		}

		void stage_exception(std::exception_ptr p)
		{
			std::unique_lock lock(get_mutex());
			exception = p;
		}

		void complete()
		{
			get_condvar().notify_all();
			std::unique_lock lock(get_mutex());
			if (awaiting_task.coroutine)
			{
				std::coroutine_handle<> h = awaiting_task.coroutine;
				std::shared_ptr<task_state_base> a = awaiting_task.awaiter;
				awaiting_task = {};
				lock.unlock();
				a->resumer->resume(h, a);
			}
		}

		R get()
		{
			wait();
			std::unique_lock lock(get_mutex());
			if (exception.has_value())
				std::rethrow_exception(exception.value());
			if (value.has_value())
				return value.value();
			throw std::runtime_error("Task is not ready!");
		}

		bool ready() const
		{
			std::unique_lock lock(get_mutex());
			return value.has_value() || exception.has_value();
		}

		bool await_suspend(std::coroutine_handle<> h, std::shared_ptr<task_state_base> awaiter)
		{
			std::unique_lock lock(get_mutex());
			if (value.has_value() || exception.has_value())
			{
				return false;
			}
			else
			{
				awaiter->save_thread_local();
				awaiting_task.coroutine = h;
				awaiting_task.awaiter = awaiter;
				return true;
			}
		}

		void wait()
		{
			std::unique_lock lock(get_mutex());
			if (value.has_value() || exception.has_value())
				return;
			get_condvar().wait(lock, [&]() { return value.has_value() || exception.has_value(); });
		}

		template<class Rep, class Period>
		bool wait_for(const std::chrono::duration<Rep, Period>& rel_time)
		{
			std::unique_lock lock(get_mutex());
			if (value.has_value() || exception.has_value())
				return true;
			return get_condvar().wait_for(lock, rel_time, [&]() { return value.has_value() || exception.has_value(); });
		}

		template<class Clock, class Duration>
		bool wait_until(const std::chrono::time_point<Clock, Duration>& abs_time)
		{
			std::unique_lock lock(get_mutex());
			if (value.has_value() || exception.has_value())
				return true;
			return get_condvar().wait_until(lock, abs_time, [&]() { return value.has_value() || exception.has_value(); });
		}

		const std::shared_ptr<task_resumer>& get_resumer() const { return resumer; }

	private:
		std::optional<R> value;
		std::optional<std::exception_ptr> exception;

		static std::mutex& get_mutex()
		{
			static std::mutex mutex;
			return mutex;
		}

		static std::condition_variable& get_condvar()
		{
			static std::condition_variable condvar;
			return condvar;
		}
	};

	template<>
	class task_state<void> : public task_state_base
	{
	public:
		void set_value()
		{
			stage_value();
			complete();
		}

		void set_exception(std::exception_ptr p)
		{
			stage_exception(p);
			complete();
		}

		void stage_value()
		{
			std::unique_lock lock(get_mutex());
			has_value = true;
		}

		void stage_exception(std::exception_ptr p)
		{
			std::unique_lock lock(get_mutex());
			exception = p;
		}

		void complete()
		{
			get_condvar().notify_all();
			std::unique_lock lock(get_mutex());
			if (awaiting_task.coroutine)
			{
				std::coroutine_handle<> h = awaiting_task.coroutine;
				std::shared_ptr<task_state_base> a = awaiting_task.awaiter;
				awaiting_task = {};
				lock.unlock();
				a->resumer->resume(h, a);
			}
		}

		void get()
		{
			wait();
			std::unique_lock lock(get_mutex());
			if (exception.has_value())
				std::rethrow_exception(exception.value());
			if (!has_value)
				throw std::runtime_error("Task is not ready!");
		}

		bool ready() const
		{
			std::unique_lock lock(get_mutex());
			return has_value || exception.has_value();
		}

		bool await_suspend(std::coroutine_handle<> h, std::shared_ptr<task_state_base> awaiter)
		{
			std::unique_lock lock(get_mutex());
			if (has_value || exception.has_value())
			{
				return false;
			}
			else
			{
				awaiter->save_thread_local();
				awaiting_task.coroutine = h;
				awaiting_task.awaiter = awaiter;
				return true;
			}
		}

		void wait()
		{
			std::unique_lock lock(get_mutex());
			if (has_value || exception.has_value())
				return;
			get_condvar().wait(lock, [&]() { return has_value || exception.has_value(); });
		}

		template<class Rep, class Period>
		bool wait_for(const std::chrono::duration<Rep, Period>& rel_time)
		{
			std::unique_lock lock(get_mutex());
			if (has_value || exception.has_value())
				return true;
			return get_condvar().wait_for(lock, rel_time, [&]() { return has_value || exception.has_value(); });
		}

		template<class Clock, class Duration>
		bool wait_until(const std::chrono::time_point<Clock, Duration>& abs_time)
		{
			std::unique_lock lock(get_mutex());
			if (has_value || exception.has_value())
				return true;
			return get_condvar().wait_until(lock, abs_time, [&]() { return has_value || exception.has_value(); });
		}

		const std::shared_ptr<task_resumer>& get_resumer() const { return resumer; }

	private:
		bool has_value = false;
		std::optional<std::exception_ptr> exception;
		std::shared_ptr<task_resumer> resumer = task_resumer::get_thread_local();

		static std::mutex& get_mutex()
		{
			static std::mutex mutex;
			return mutex;
		}

		static std::condition_variable& get_condvar()
		{
			static std::condition_variable condvar;
			return condvar;
		}
	};

	template<typename R = void>
	class [[nodiscard]] task
	{
	public:
		task() = default;
		task(std::shared_ptr<task_state<R>> state) : state(std::move(state)) {}

		~task()
		{
			// You must call get() or detach() on a started task
			if (state)
				std::terminate();
		}

		bool ready() const { if (state) return state->ready(); else throw std::runtime_error("Cannot get the ready state of an empty task!"); }
		R get() { auto s = std::move(state); if (s) return s->get(); else throw std::runtime_error("Cannot get the value of an empty task!"); }

		void wait() { if (state) return state->wait(); else throw std::runtime_error("Cannot get the value of an empty task!"); }

		void detach() { state.reset(); }

		template<class Rep, class Period>
		bool wait_for(const std::chrono::duration<Rep, Period>& rel_time) { if (state) return state->wait_for(rel_time); else throw std::runtime_error("Cannot wait on an empty task!"); }

		template<class Clock, class Duration>
		bool wait_until(const std::chrono::time_point<Clock, Duration>& abs_time) { if (state) return state->wait_until(abs_time); else throw std::runtime_error("Cannot wait on an empty task!"); }

		using promise_type = task_promise<R>;

	private:
		std::shared_ptr<task_state<R>> state;

		template<typename T>
		friend class task_promise;
	};

	template<>
	class [[nodiscard]] task<void>
	{
	public:
		task() = default;
		task(std::shared_ptr<task_state<void>> state) : state(std::move(state)) {}

		~task()
		{
			// You must call get() or detach() on a started task
			if (state)
				std::terminate();
		}

		bool ready() const { if (state) return state->ready(); else throw std::runtime_error("Cannot get the ready state of an empty task!"); }
		void get() { auto s = std::move(state); if (s) s->get(); else throw std::runtime_error("Cannot get the value of an empty task!"); }

		void wait() { if (state) return state->wait(); else throw std::runtime_error("Cannot get the value of an empty task!"); }

		void detach() { state.reset(); }

		template<class Rep, class Period>
		bool wait_for(const std::chrono::duration<Rep, Period>& rel_time) { if (state) return state->wait_for(rel_time); else throw std::runtime_error("Cannot wait on an empty task!"); }

		template<class Clock, class Duration>
		bool wait_until(const std::chrono::time_point<Clock, Duration>& abs_time) { if (state) return state->wait_until(abs_time); else throw std::runtime_error("Cannot wait on an empty task!"); }

		using promise_type = task_promise<void>;

	private:
		std::shared_ptr<task_state<void>> state;

		template<typename T>
		friend class task_promise;
	};

	template<typename R, typename T>
	class task_await
	{
	public:
		task_await(std::shared_ptr<task_state<R>> awaiter, std::shared_ptr<task_state<T>> task) : awaiter(awaiter), task(std::move(task)) {}

		bool await_ready() { return !task || task->ready(); }
		bool await_suspend(std::coroutine_handle<> h) { return task->await_suspend(h, awaiter); }
		T await_resume() { return task->get(); }

	private:
		std::shared_ptr<task_state<R>> awaiter;
		std::shared_ptr<task_state<T>> task;
	};

	template<typename R>
	class task_await<R, void>
	{
	public:
		task_await(std::shared_ptr<task_state<R>> awaiter, std::shared_ptr<task_state<void>> task) : awaiter(awaiter), task(std::move(task)) {}

		bool await_ready() { return !task || task->ready(); }
		bool await_suspend(std::coroutine_handle<> h) { return task->await_suspend(h, awaiter); }
		void await_resume() { task->get(); }

	private:
		std::shared_ptr<task_state<R>> awaiter;
		std::shared_ptr<task_state<void>> task;
	};

	inline JSValue await_promise(std::shared_ptr<task_state<JSValue>> state, JSValue promise)
	{
		try
		{
			state->set_value(co_await promise);
		}
		catch (...)
		{
			state->set_exception(std::current_exception());
		}
		co_return JSValue::undefined();
	}

	inline task<JSValue> await_promise(JSValue promise)
	{
		std::shared_ptr<task_state<JSValue>> state = std::make_shared<task_state<JSValue>>();
		await_promise(state, std::move(promise));
		return task<JSValue>(state);
	}

	template<typename R>
	class task_initial_suspend
	{
	public:
		bool await_ready() const { return false; }
		void await_suspend(std::coroutine_handle<task_promise<R>> h) { return h.promise().state->get_resumer()->resume(h, h.promise().state); }
		void await_resume() {}
	};

	template<typename R = void>
	class task_promise
	{
	public:
		task<R> get_future() { return { state }; }

		void set_value(const R& value) { state->set_value(value); }
		void set_value(R&& value) { state->set_value(value); }
		void set_exception(std::exception_ptr p) { state->set_exception(p); }

		// coroutine infrastructure
		task<R> get_return_object() { return { state }; }
		task_initial_suspend<R> initial_suspend() { return {}; }
		std::suspend_never final_suspend() noexcept { state->complete(); return {}; }
		void return_value(const R& value) { state->stage_value(value); }
		void return_value(R&& value) { state->stage_value(value); }
		void unhandled_exception() { state->stage_exception(std::current_exception()); }

		template<typename T>
		task_await<R, T> await_transform(task<T>&& task) { auto s = std::move(task.state); if (s) return task_await<R, T>(state, s); else throw std::runtime_error("Cannot await on an empty task!"); }
		task_await<R, JSValue> await_transform(JSValue value) { return task_await<R, JSValue>(state, await_promise(value).state); }

		std::shared_ptr<task_state<R>> state = std::make_shared<task_state<R>>();
	};

	template<>
	class task_promise<void>
	{
	public:
		task<void> get_future() { return { state }; }

		void set_value() { state->set_value(); }
		void set_exception(std::exception_ptr p) { state->set_exception(p); }

		// coroutine infrastructure
		task<> get_return_object() { return { state }; }
		task_initial_suspend<void> initial_suspend() { return {}; }
		std::suspend_never final_suspend() noexcept { state->complete(); return {}; }
		void return_void() { state->stage_value(); }
		void unhandled_exception() { state->stage_exception(std::current_exception()); }

		template<typename T>
		task_await<void, T> await_transform(task<T>&& task) { auto s = std::move(task.state); if (s) return task_await<void, T>(state, s); else throw std::runtime_error("Cannot await on an empty task!"); }
		task_await<void, JSValue> await_transform(JSValue value) { return task_await<void, JSValue>(state, await_promise(value).state); }

		std::shared_ptr<task_state<void>> state = std::make_shared<task_state<void>>();
	};

	class task_execution_context
	{
	public:
		task_execution_context()
		{
			old_resumer = task_resumer::get_thread_local();
		}

		~task_execution_context()
		{
			task_resumer::get_thread_local() = old_resumer;
		}

		task_execution_context(const task_execution_context&) = delete;
		task_execution_context& operator=(const task_execution_context&) = delete;

	private:
		std::shared_ptr<task_resumer> old_resumer;
	};

	class task_execution_immediately : public task_execution_context
	{
	public:
		task_execution_immediately()
		{
			task_resumer::get_thread_local() = std::make_shared<task_resumer_immediately>();
		}
	};

	class task_execution_threadpool : public task_execution_context
	{
	public:
		task_execution_threadpool(std::function<void(std::function<void()> task)> queueTask)
		{
			resumer = std::make_shared<task_resumer_threadpool>(queueTask);
			task_resumer::get_thread_local() = resumer;
		}

	private:
		std::shared_ptr<task_resumer_threadpool> resumer;
	};

	inline std::shared_ptr<task_resumer>& task_resumer::get_thread_local()
	{
		thread_local std::shared_ptr<task_resumer> resumer;
		if (!resumer)
			resumer = std::make_shared<task_resumer_immediately>();
		return resumer;
	}
}
