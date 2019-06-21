#pragma once

template<typename Parent, typename Child>
class Dynamic_Cast_Check
{
private:
	static Child*			pChild;
public:
	static bool				IsParent;

private:
	static bool Check(...);
	static short Check(Parent* _Child);
};

template<typename Parent, typename Child>
Child*	Dynamic_Cast_Check<Parent, Child>::pChild = nullptr;

template<typename Parent, typename Child>
bool Dynamic_Cast_Check<Parent, Child>::IsParent = bool(sizeof(Dynamic_Cast_Check<Parent, Child>::Check(Dynamic_Cast_Check<Parent, Child>::pChild)) - 1);


typedef class CReferenceBase
{
	template<typename T>
	friend class SPTR;

private:
	friend class Engine;

private:
	int						m_iRefCount;

private:
	void AddRefCount();
	void ReleaseRef();

public:
	CReferenceBase();
	CReferenceBase(const CReferenceBase& _Other);
	virtual ~CReferenceBase() = 0;
}CRefBase;


template<typename T>
class SPTR
{
public:
	T * PTR;


private:
	const bool CheckRefPointer()
	{
		if (nullptr == PTR)
			return false;

		return true;
	}

	void AddRef()
	{
		if (nullptr == PTR)
			return;
		PTR->AddRefCount();
	}
	void EndRef()
	{
		if (nullptr == PTR)
			return;

		PTR->ReleaseRef();
	}

public:
	T * GetPtr()
	{
		return PTR;
	}

public:
	operator T*() const 
	{
		return PTR;
	}

	SPTR<T>& operator=(const SPTR<T>& _Other)
	{
		if (_Other.PTR == PTR)
			return *this;

		EndRef();
		PTR = _Other.PTR;
		AddRef();

		return *this;
	}

	SPTR& operator=(T* const _P)
	{
		if (_P == PTR)
			return *this;

		EndRef();
		PTR = _P;
		AddRef();

		return *this;
	}
	SPTR& operator=(const std::nullptr_t& _P)
	{
		if (true == CheckRefPointer())
			PTR->ReleaseRef();

		PTR = nullptr;

		return *this;
	}


public:
	/*const T* operator->() const
	{
		return PTR;
	}*/

	T* operator->() const
	{
		return PTR;
	}

public:
	template<typename S>
	operator SPTR<S>()
	{
		if (false == Dynamic_Cast_Check<T, S>::IsParent		// 부모 -> 자식(다운 케스팅용)
			&& false == Dynamic_Cast_Check<S, T>::IsParent)	// 자식 -> 부모(업 케스팅용)
			return nullptr;

		return (S*)PTR;
	}

	template<typename S>
	S* ConvertCheck() const
	{
		return dynamic_cast<S*>(PTR);
	}

public:
	const bool operator==(const SPTR<T>&  _Other) const { return _Other.PTR == PTR; }
	const bool operator==(const T*  _P) const { return _P == PTR; }
	const bool operator==(const std::nullptr_t  _P) const { return _P == PTR; }

	const bool operator!=(const SPTR<T>&  _Other) const { return _Other.PTR != PTR; }
	const bool operator!=(const T*  _P) const { return _P != PTR; }
	const bool operator!=(const std::nullptr_t  _P) const {  return _P != PTR; }


public:
	SPTR() :PTR(nullptr) {}
	SPTR(std::nullptr_t _P) :PTR(_P) {}
	SPTR(T* _P) :PTR(_P)
	{
		AddRef();
	}

	SPTR(const SPTR<T>& _Other) :PTR(nullptr)
	{
		if (nullptr == _Other.PTR)
			return;

		PTR = _Other.PTR;

		AddRef();
	}


	~SPTR()
	{
	
		EndRef();
	}
};

//template<typename T>
//const bool operator==(const T* _Left, const SPTR<T>& _Right)
//{
//	return _Right == _Left;
//}

template<typename T>
const bool operator==(const std::nullptr_t _Left, const SPTR<T>& _Right) 
{
	return _Right == _Left;
}

template<typename T>
const bool operator!=(const std::nullptr_t _Left, const SPTR<T>& _Right) 
{
	return _Right != _Left;
}

//template<typename T>
//const bool operator!=(const T* _Left, const SPTR<T>& _Right)
//{
//	return _Right != _Left;
//}



//template<typename INPTR,typename OUTPTR>
//OUTPTR ChangeOtherType(const INPTR& _Ptr)
//{
//	return _Ptr.GetPointer();
//}