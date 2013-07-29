#pragma once

namespace PTEntity {

template<typename T>
public ref class EntityWrapper
{
public:
	EntityWrapper()
	{
		m_innerItem = new T;
	}
	~EntityWrapper()
	{
		this->!EntityWrapper();
	}
	!EntityWrapper()
	{
		if(m_innerItem != 0)
		{
			delete m_innerItem;
			m_innerItem = 0;
		}
	}

	void From(T* recv)
	{
		m_innerItem->CopyFrom(*recv);
	}

protected:
	T* m_innerItem;
};

}