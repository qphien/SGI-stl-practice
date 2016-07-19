#include <algorithm>
template <typename t, typename alloc = alloca>
class self_vector
{
public:
	typedef t value_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef const pointer const_pointer;
	typedef const reference const_reference;
	typedef value_type* iterator;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
protected:
	typedef simple_alloc<t, alloc> data_allocator;
	iterator allocate_and_fill(size_type n, const t &x);
	void fill_n(size_type n, const t &x)
	{
		start = allocate_and_fill(n, x);
		finish = end_of_storage = start + n;
	}
	void insert_aux(iterator position, const t &x);
	void insert_n_aux(iterator position, size_type n, const t &x);
	void deallocate() { data_allocator::deallocate(start, end_of_storage - start); }
private:
	iterator start;
	iterator finish;
	iterator end_of_storage;
public:
	self_vector() : start(0), finish(0), end_of_storage(0) {}
	self_vecor(size_type n, const t &x) { fill_n(n, x); }
	self_vector(int n, const t &x) { fill_n(n, x); }
	self_vector(long n, const t &x) { fill_n(n, x); }

	iterator begin() { return start; }
	iterator end() { return finish; }
	reference operator*(size_type n) { return *(begin() + n); }
	void push_back(const t &x);
	void pop_back();
	reference front() { return *begin(); }
	reference back() { return *(end() - ); }

	void erase(iterator position);
	void erase(iterator position_start, iterator position_end);
	void clear() { erase(begin(), end()); }
	void insert(iterator position, const t &x);
	void insert(iterator position, size_type n, const t &x);
	template<typename insert_iterator>
	void insert(iterator position, insert_iterator begin, insert_iterator end);
};
template <typename t, typename alloc>
typename self_vector<t, alloc>::iterator self_vector<t, alloc>::allocate_and_fill(size_type n, const t &x)
{
	start = data_allocator::allocate(n);
	uninitialized_fill_n(start, n, x);
	finish = end_of_storage = start + n;
}
template <typename t, typename alloc>
void self_vector<t, alloc>::push_back(const t &x)
{
	if (finish != end_of_storage)
		construct(finish++, x);
	else
		insert_aux(finish, x);
}
template <typename t, typename alloc>
void self_vector<t, allloc>::pop_back()
{
	destroy(--finish);
}
template <typename t, typename alloc>
void self_vector<t, alloc>::erase(iterator position)
{
	if (position < finish)
	{
		copy(position + 1, finish, position);
		--finish;
		destroy(finish);
	}
}
template <typename t, typename alloc>
void self_vector<t, alloc>::erase(iterator positon_start, iterator position_end)
{
	if (position_end <= finish)
	{
		iterator i = copy(position_end, finish, position_start);
		destroy(i, finish);
		finish -= positon_start - position_end;
	}
}
template <typename t, typename alloc>
void self_vector<t, alloc>::insert(iterator position, size_type n, const t &x)
{
	if (end_of_storage - finish < n)
		insert_n_aux(position, n, x);
	else
	{
		if (n <= finish - position)
		{
			uninitialized_copy(finish - n, finish, finish);
			copy_backward(positon, finish - n, finish);
			finish += n;
		}
		else
		{
			iteartor i = unintialized_fill_n(finish, n - (finish - position), x);
			finish = unitialized_copy(position, finish, i);
			fill(position, finish - position, x);
		}
	}
}
template <typename t, typename alloc>
void self_vector<t, alloc>::insert_n_aux(iterator position, size_type n, const t &x)
{
	try
	{
		size_type old_size = end_of_storage - start;
		size_type new_size = old_size ? old_size * 2 : 1;
		iterator new_start = data_allocator::allocate(new_size);
		end_of_storage = new_start + new_size;
		iterator new_fininsh = uninitialized_copy(start, position, new_start);
		new_fininsh = uninitialized_fill_n(new_finish, n, x);
		new_finish = unitialized_copy(new_fininsh, position, finish);
		destroy(start, finish);
		dellocate();
		start = new_start;
		finish = new_finish;
	}
	catch (...)
	{
		destroy(new_start, new_finish);
		data_allocator::deallocate(new_start, new_size);
		throw;
	}
}