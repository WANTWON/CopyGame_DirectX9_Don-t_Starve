#pragma once

namespace Engine
{
	template<typename T>
	void Safe_Delete(T& Temp)
	{
		if (Temp)
		{
			delete Temp;
			Temp = nullptr;
		}
	}

	template<typename T>
	unsigned long Safe_Release(T& Temp)
	{
		unsigned long dwRefCnt = 0;

		if (Temp)
		{
			dwRefCnt = Temp->Release();
			Temp = nullptr;
		}

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_AddRef(T& Temp)
	{
		unsigned long dwRefCnt = 0;

		if (Temp)	
			dwRefCnt = Temp->AddRef();	

		return dwRefCnt;
	}


	template<typename T>
	void Safe_Delete_Array(T& Temp)
	{
		if (Temp)
		{
			delete[] Temp;
			Temp = nullptr;
		}
	}


	class CTag_Finder
	{
	private:
		const TCHAR*	m_pStr;

	public:
		CTag_Finder(const TCHAR* pStr)
			: m_pStr(pStr)
		{

		}

	public:
		template<typename T>
		bool	operator()(T& Pair)
		{
			if (!lstrcmp(m_pStr, Pair.first))
				return true;

			return false;
		}
	};


}