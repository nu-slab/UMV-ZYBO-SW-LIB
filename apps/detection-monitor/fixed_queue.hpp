#include <deque>

template <typename T>
class FixedQueue
{
	public:
		typedef typename std::deque<T>::iterator iterator;
		typedef typename std::deque<T>::const_iterator const_iterator;
		int              count;

		FixedQueue()
		{
			count = 0;
		}

		// fixed size
		void fixed_size(size_t size)
		{
			que.resize(size);
		}

		void push_back(const T& t)
		{
			if(que.front()){ count--; }
			if(          t){ count++; }
			que.pop_front();
			que.push_back(t);
		}

		void push_front(const T& t)
		{
			if(que.back()){ count--; }
			if(         t){ count++; }
			que.pop_back();
			que.push_front(t);
		}

		iterator begin() { return que.begin(); }
		const_iterator begin() const { return que.begin(); }

		iterator end() { return que.end(); }
		const_iterator end() const { return que.end(); }

	private:
		std::deque < T > que;
};

