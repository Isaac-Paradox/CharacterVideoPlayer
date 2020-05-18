#include <algorithm>

#include "MemoryStream.h"

template<typename T>
IBinaryStream& IBinaryStream::operator>>(T& _Val) {
	_UpdateTempBuffer(sizeof(T));
	assert(Read(m_pTempBuffer, sizeof(T)));
	if (m_eEndian == BinaryStreamEndian::BigEndian) {
		std::reverse(m_pTempBuffer, m_pTempBuffer + sizeof(T));
	}
	memcpy(&_Val, m_pTempBuffer, sizeof(T));
	return *this;
}

template<>
IBinaryStream& IBinaryStream::operator>>(char*& _Val) {
	uint32_t string_length = 0;
	*this >> (string_length);
	assert(Read(_Val, sizeof(string_length)));
	_Val[string_length] = '\0';
	return *this;
}

template<>
IBinaryStream& IBinaryStream::operator>>(std::string& _Val) {
	uint32_t string_length = 0;
	*this >> (string_length);
	_UpdateTempBuffer(string_length);
	assert(Read(m_pTempBuffer, string_length));
	m_pTempBuffer[string_length] = '\0';
	_Val = m_pTempBuffer;
	return *this;
}

template<>
IBinaryStream& IBinaryStream::operator>>(MemoryStream& _Val) {
	uint32_t buffer_size = _Val.GetSize();
	assert(Read(_Val.m_pBuffer, buffer_size));
	return *this;
}

BinaryFileStream::BinaryFileStream(std::string& _Filename, BinaryStreamEndian _Endian)
	: IBinaryStream(_Endian) {
	m_sStream.open(_Filename, std::ios::binary | std::ios::in);
	m_nStreamSize = m_sStream.pubseekoff(0, std::ios::end, std::ios_base::in);
	m_sStream.pubseekoff(0, std::ios::beg, std::ios_base::in);
}

BinaryMemoryStream::BinaryMemoryStream(MemoryStream& _Stream, BinaryStreamEndian _Endian)
	: IBinaryStream(_Endian), m_sStream(_Stream) {
	m_nStreamSize = m_sStream.GetSize();
}
