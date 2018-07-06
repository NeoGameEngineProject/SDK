#ifndef NEO_HANDLE_H
#define NEO_HANDLE_H

#include <cstdint>
#include <cstddef>
#include <cassert>

namespace Neo 
{

typedef size_t handle_t;

template<typename OBJECT, typename MANAGER>
class Handle
{
	MANAGER* m_manager = nullptr;
	handle_t m_handle = -1;
	
public:
	Handle() = default;
	Handle(MANAGER* mgr, handle_t handle):
		m_manager(mgr),
		m_handle(handle) {}
		
	Handle(const Handle<OBJECT, MANAGER>& other) { *this = other; }
	
	bool operator==(const Handle<OBJECT, MANAGER>& other) const
	{
		return other.m_handle == m_handle && other.m_manager == m_manager;
	}
	
	bool operator!=(const Handle<OBJECT, MANAGER>& other) const
	{
		return !(*this == other);
	}

	OBJECT* operator->()
	{
		return get();
	}
	
	OBJECT& operator*()
	{
		return *get();
	}
	
	OBJECT* get() 
	{ 
		assert(m_manager && m_handle != -1 && m_handle < m_manager->size());
		return &(*m_manager)[m_handle]; 
	}
	bool empty() { return m_manager == nullptr; }
};

}

#endif
