#pragma once

#include <atomic>
#include <cstddef>

template <typename T, std::size_t Size> class RingBuffer {
  public:
	bool push(const T &item) {
		size_t currentHead = m_head.load(std::memory_order_relaxed);
		size_t nextHead = (currentHead + 1) % Size;
		if(nextHead == m_tail.load(std::memory_order_acquire)) { return false; }

		m_buffer[currentHead] = item;
		m_head.store(nextHead, std::memory_order_release);

		return true;
	}

	bool pop(T &item) {
		size_t currentTail = m_tail.load(std::memory_order_relaxed);
		if(currentTail == m_head.load(std::memory_order_acquire)) { return false; }

		item = m_buffer[currentTail];
		m_tail.store((currentTail + 1) % Size, std::memory_order_release);

		return true;
	}

  private:
	T m_buffer[Size];
	std::atomic<size_t> m_head = 0;
	std::atomic<size_t> m_tail = 0;
};
