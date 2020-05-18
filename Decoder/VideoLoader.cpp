#include <fstream>
#include <iostream>

#include "VideoLoader.h"

constexpr std::uint32_t HASH_BOXTYPE_TO_UINT32(const char* data) {
	std::uint32_t h = 0;
	for (int i = 0; i < 4; i++) {
		h = (h << 6) ^ (h >> 26) ^ data[i];
	}
	return h;
}

bool VideoLoader::_Load() {
	BinaryFileStream file_stream(m_sVideoFileName);
	assert(file_stream);
	m_dVideoData.box_size = file_stream.GetStreamSize();

	return _LoadBox(m_dVideoData, file_stream);
}

bool VideoLoader::_LoadBox(IBox& _ParentBox, BinaryFileStream& _Stream) {
	uint64_t left_size = _ParentBox.box_size;
	while (left_size) {
		uint32_t box_size, buf_size = 0;
		uint64_t lbox_size;
		char box_type[5] = "    ";
		_Stream >> box_size;
		if (!box_size) {
			assert(left_size == 4);//判断是否还要没读取完的部分
			return true;
		}
		assert(_Stream.Read(box_type, 4));
		if (box_size == 1) {
			_Stream >> lbox_size;
			left_size -= lbox_size;
		} else {
			left_size -= box_size;
		}
		
		switch (HASH_BOXTYPE_TO_UINT32(box_type)) {
		case HASH_BOXTYPE_TO_UINT32("ftyp"):
		{
			FTYPBox* box = new FTYPBox();
			_ParentBox.boxs.push_back(box);
			if (!_PraseFTYPBox(*box, _Stream)) {
				return false;
			}
		}
		break;
		case HASH_BOXTYPE_TO_UINT32("moov"):
		{
			MOOVBox* box = new MOOVBox();
			_ParentBox.boxs.push_back(box);
			if (!_PraseFullBoxHead(*box, _Stream)) {
				return false;
			}
			if (!_LoadBox(*box, _Stream)) {//moov没有自己的数据，直接递归
				return false;
			}
		}
		break;
		case HASH_BOXTYPE_TO_UINT32("mvhd"):
		{
			MVHDBox* box = new MVHDBox();
			if (!_PraseFullBoxHead(*box, _Stream)) {
				return false;
			}
			_ParentBox.boxs.push_back(box);
			if (!_PraseMVHDBox(*box, _Stream)) {
				return false;
			}
		}
		break;
		default:
			std::cout << "未知type类型:" << box_type << std::endl;
			return false;
		}
	}

	return true;
}

bool VideoLoader::_PraseFullBoxHead(IFullBox& _Box, BinaryFileStream& _Stream) {
	char temp_buff[4];
	memset(temp_buff, 0, 4);
	_Stream.Read(&temp_buff[3], 1);
	memcpy(&_Box.version, temp_buff, 4);
	_Stream.Read(&temp_buff[1], 3);
	std::reverse(&temp_buff[0], &temp_buff[4]);
	memcpy(&_Box.flag, temp_buff, 4);
	return true;
}

bool VideoLoader::_PraseFTYPBox(FTYPBox& _Box, BinaryFileStream& _Stream) {
	char temp_string[5] = "    ";
	_Stream.Read(temp_string, 4);
	_Box.major_brand.assign(temp_string);
	_Stream >> _Box.minor_version;
	size_t compatible_brands_length = (_Box.box_size - 8) / 4;
	while (compatible_brands_length--) {
		_Stream.Read(temp_string, 4);
		_Box.compatible_brands.emplace_back(temp_string);
	}
	return true;
}

bool VideoLoader::_PraseMVHDBox(MVHDBox& _Box, BinaryFileStream& _Stream) {
	if (_Box.version == 1) {
		_Stream >> _Box.creation_time >> _Box.modification_time >> _Box.timescale >> _Box.duration;
	} else { // version==0    
		uint32_t temp_int;
		_Stream >> temp_int;
		_Box.creation_time = temp_int;
		_Stream >> temp_int >> _Box.timescale;
		_Box.modification_time = temp_int;
		_Stream >> temp_int;
		_Box.duration = temp_int;
	}
	_Stream >> _Box.rate >> _Box.volume >> _Box.bit_reserved >> _Box.reserved[0] >> _Box.reserved[1];

	for (size_t i = 0; i < 9; i++) {
		_Stream >> _Box.matrix[i];
	}

	for (size_t i = 0; i < 6; i++) {
		_Stream >> _Box.pre_defined[i];
	}
	_Stream >> _Box.next_track_ID;

	return true;
}