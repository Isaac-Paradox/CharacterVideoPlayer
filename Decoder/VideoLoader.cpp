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
	m_dVideoData.BoxSize = file_stream.GetStreamSize();

	return _LoadBox(m_dVideoData, file_stream);
}

bool VideoLoader::_LoadBox(IBox& _ParentBox, BinaryFileStream& _Stream) {
	uint64_t left_size = _ParentBox.BoxSize;
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
		} else {
			lbox_size = box_size;
		}
		left_size -= lbox_size;
		switch (HASH_BOXTYPE_TO_UINT32(box_type)) {
		case HASH_BOXTYPE_TO_UINT32("ftyp"):
		{
			FTYPBox* box = new FTYPBox();
			AddToBoxList(_ParentBox, box, lbox_size, _Stream);
			if (!_PraseFTYPBox(*box, _Stream)) {
				return false;
			}
		}
		break;
		case HASH_BOXTYPE_TO_UINT32("moov"):
		case HASH_BOXTYPE_TO_UINT32("trak"):
		case HASH_BOXTYPE_TO_UINT32("edts"):
		{
			IBox* box = new IBox();
			AddToBoxList(_ParentBox, box, lbox_size, _Stream);
			if (!_LoadBox(*box, _Stream)) {//没有自己的数据，直接递归
				return false;
			}
		}
		break;
		case HASH_BOXTYPE_TO_UINT32("mvhd"):
		{
			MVHDBox* box = new MVHDBox();
			AddToBoxList(_ParentBox, box, lbox_size, _Stream);
			if (!_PraseMVHDBox(*box, _Stream)) {
				return false;
			}
		}
		break;
		case HASH_BOXTYPE_TO_UINT32("tkhd"):
		{
			TKHDBox* box = new TKHDBox();
			AddToBoxList(_ParentBox, box, lbox_size, _Stream);
			if (!_PraseTKHDBox(*box, _Stream)) {
				return false;
			}
		}
		break;
		case HASH_BOXTYPE_TO_UINT32("elst"):
		{
			ELSTBox* box = new ELSTBox();
			AddToBoxList(_ParentBox, box, lbox_size, _Stream);
			if (!_PraseELSTBox(*box, _Stream)) {
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

bool VideoLoader::_PraseFTYPBox(FTYPBox& _Box, BinaryFileStream& _Stream) {
	char temp_string[5] = "    ";
	_Stream.Read(temp_string, 4);
	_Box.MajorBrand.assign(temp_string);
	_Stream >> _Box.MinorVersion;
	size_t compatible_brands_length = (size_t)((_Box.BoxSize - 8) / 4);
	while (compatible_brands_length--) {
		_Stream.Read(temp_string, 4);
		_Box.CompatibleBrands.emplace_back(temp_string);
	}
	return true;
}

bool VideoLoader::_PraseMVHDBox(MVHDBox& _Box, BinaryFileStream& _Stream) {
	if (_Box.Version == 1) {
		_Stream >> _Box.CreationTime >> _Box.ModificationTime >> _Box.TimeScale >> _Box.Duration;
	} else { // version==0    
		uint32_t temp_int;
		_Stream >> temp_int;
		_Box.CreationTime = temp_int;
		_Stream >> temp_int >> _Box.TimeScale;
		_Box.ModificationTime = temp_int;
		_Stream >> temp_int;
		_Box.Duration = temp_int;
	}
	_Stream >> _Box.Rate >> _Box.Volume >> _Box.ShortReserved >> _Box.Reserved[0] >> _Box.Reserved[1];

	for (size_t i = 0; i < 9; i++) {
		_Stream >> _Box.Matrix[i];
	}

	for (size_t i = 0; i < 6; i++) {
		_Stream >> _Box.PreDefined[i];
	}
	_Stream >> _Box.NextTrack_ID;

	return true;
}

bool VideoLoader::_PraseTKHDBox(TKHDBox& _Box, BinaryFileStream& _Stream) {
	_Box.TrackEnabled = _Box.Flag ^ 1;
	_Box.TrackInMovie = _Box.Flag ^ 2;
	_Box.TrackInPreview = _Box.Flag ^ 4;
	_Box.TrackInPoster = _Box.Flag ^ 8;

	if (_Box.Version == 1) {
		_Stream >> _Box.CreationTime >> _Box.ModificationTime >> _Box.TrackID >> _Box.HeadReserved >> _Box.Duration;
	} else { // version==0    
		uint32_t temp_int;
		_Stream >> temp_int;
		_Box.CreationTime = temp_int;
		_Stream >> temp_int;
		_Box.ModificationTime = temp_int;
		_Stream >> _Box.TrackID >> _Box.HeadReserved >> temp_int;
		_Box.Duration = temp_int;
	}

	_Stream >> _Box.Reserved[0]
		>> _Box.Reserved[1]
		>> _Box.Layer
		>> _Box.AlternateGroup
		>> _Box.Volume
		>> _Box.ShortReserved;

	for (size_t i = 0; i < 9; i++) {
		_Stream >> _Box.Matrix[i];
	}
	_Stream >> _Box.Width >> _Box.Height;

	return true;
}

bool VideoLoader::_PraseELSTBox(ELSTBox& _Box, BinaryFileStream& _Stream) {
	_Stream >> _Box.EntryCount;
	for (int i = 0; i < _Box.EntryCount; ++i) {
		_Box.Entrys.emplace_back();
		ELSTEntry& entry = _Box.Entrys.back();
		if (_Box.Version == 1) {
			_Stream >> entry.SegmentDuration >> entry.MediaTime >> entry.MediaRateInteger >> entry.MediaRateFraction;
		} else { // version==0    
			uint32_t temp_int;
			_Stream >> temp_int;
			entry.SegmentDuration = temp_int;
			_Stream >> temp_int >> entry.MediaRateInteger >> entry.MediaRateFraction;
			entry.MediaTime = temp_int;
		}
	}
	return true;
}
