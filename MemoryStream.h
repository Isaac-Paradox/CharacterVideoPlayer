#pragma once
#include <streambuf>
#include <fstream>
#include <cassert>

enum class BinaryStreamEndian {
	BigEndian,
	LittleEndian
};

class MemoryStream;
class IBinaryStream;
class BinaryFileStream;
class BinaryMemoryStream;

class MemoryStream : protected std::streambuf{
	friend class IBinaryStream;
	friend class BinaryFileStream;
	friend class BinaryMemoryStream;
public:
	MemoryStream(uint32_t _BufferSize)
		:m_nBufferSize(_BufferSize) {
		m_pBuffer = new char[m_nBufferSize];
		_InitBuffer();
	}
	MemoryStream(char* _Buffer, uint32_t _BufferSize)
		:m_nBufferSize(_BufferSize), m_pBuffer(_Buffer) {
		_InitBuffer();
	}
	~MemoryStream() { if (m_bNeedDelete) { delete m_pBuffer; } }

	inline uint32_t GetSize() { return m_nBufferSize; }

protected:
	void _InitBuffer() { setg(m_pBuffer, m_pBuffer, m_pBuffer + m_nBufferSize); };

protected:
	uint32_t m_nBufferSize;

	char* m_pBuffer;

	bool m_bNeedDelete = false;
};

class IBinaryStream {
public:
	~IBinaryStream() { delete[]m_pTempBuffer; }

	template<typename T>
	IBinaryStream& operator >> (T& _Val);
	
	virtual bool Read(char* _Buffer, uint32_t _Count) {
		if (!_OutOfRangeBoundCheck(_Count)) {
			return false;//自己先做一次检查
		}
		std::streamsize has_read = _GetStream().sgetn(_Buffer, _Count);
		m_nLocate += has_read;
		return has_read == _Count;//实际读完再做一次检查
	};

	bool IsEOF() { return m_nLocate == m_nStreamSize; }

	uint64_t GetStreamSize() { return m_nStreamSize; }
protected:
	IBinaryStream(BinaryStreamEndian _Endian) :m_eEndian(_Endian) {}	

	inline bool _OutOfRangeBoundCheck(uint32_t _ReadSize)
	{ return m_nStreamSize >= m_nLocate + _ReadSize; }

	inline void _UpdateTempBuffer(uint32_t _BufferSize) {
		if (m_nTempBufferLength < _BufferSize) {
			delete[] m_pTempBuffer;
			m_pTempBuffer = new char[_BufferSize];
			m_nTempBufferLength = _BufferSize;
		}
	}

	inline virtual std::streambuf& _GetStream() = 0;
protected:
	BinaryStreamEndian m_eEndian;

	uint64_t m_nStreamSize = 0;

	uint64_t m_nLocate = 0;

	char* m_pTempBuffer = new char[4];

	uint32_t m_nTempBufferLength = 4;
};

class BinaryFileStream : public IBinaryStream {
public:
	BinaryFileStream(std::string& _Filename, BinaryStreamEndian _Endian = BinaryStreamEndian::BigEndian);

	operator bool() const { return m_sStream.is_open(); };

	inline std::streambuf& _GetStream() override { return m_sStream; }
protected:
	std::filebuf m_sStream;
};

class BinaryMemoryStream : public IBinaryStream {
	//TODO:内存流也支持long long长的二进制流
public:
	BinaryMemoryStream(MemoryStream& _Stream, BinaryStreamEndian _Endian = BinaryStreamEndian::BigEndian);

protected:
	inline std::streambuf& _GetStream() override { return m_sStream; }

protected:
	MemoryStream& m_sStream;
};

template IBinaryStream& IBinaryStream::operator>>(char& _Val);
template IBinaryStream& IBinaryStream::operator>>(short& _Val);
template IBinaryStream& IBinaryStream::operator>>(int& _Val);
template IBinaryStream& IBinaryStream::operator>>(unsigned int& _Val);
template IBinaryStream& IBinaryStream::operator>>(long long& _Val);
template IBinaryStream& IBinaryStream::operator>>(unsigned long long& _Val);