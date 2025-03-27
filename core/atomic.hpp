#pragma once

#include <atomic>

namespace core {
template<typename T>
using Atomic = std::atomic<T>;

template<typename T>
concept Lockable = requires(T obj){
	{ obj.lock() };
	{ obj.unlock() };
};

inline constexpr auto memory_order_relaxed = std::memory_order_relaxed;
inline constexpr auto memory_order_consume = std::memory_order_consume;
inline constexpr auto memory_order_acquire = std::memory_order_acquire;
inline constexpr auto memory_order_release = std::memory_order_release;
inline constexpr auto memory_order_acq_rel = std::memory_order_acq_rel;
inline constexpr auto memory_order_seq_cst = std::memory_order_seq_cst;

struct Spinlock {
	void lock(){
		while(1){
			if(!state_.exchange(1, memory_order_acquire)){
				break;
			}
			while(state_.load(memory_order_relaxed));
		}
	}

	void unlock(){
		state_.store(0, memory_order_release);
	}

	bool try_lock(){
		return !state_.load(memory_order_relaxed) && !state_.exchange(1, memory_order_acquire);
	}

	constexpr Spinlock() : state_{0} {}
	constexpr Spinlock(Spinlock const&) = delete;

private:
	Atomic<u32> state_{0};
};
};

