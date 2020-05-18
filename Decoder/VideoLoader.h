#pragma once
#include<string>
#include<cassert>

#include"MemoryStream.h"
#include"VideoData.h"

//将二进制数据反序列化到内存中
//TODO:抽象出加载类，使可以扩展支持其它视频格式
class VideoLoader {
public:
	VideoLoader(const char* _VideoFile) : m_sVideoFileName(_VideoFile) 
	{ if (!_Load()) { m_bHasError = true; } else { m_bHadPrase = true; } };
	
	MP4RawData& GetVideoData() { assert(!m_bHasError && m_bHadPrase); return m_dVideoData; };

protected:
	bool _Load();

	bool _LoadBox(IBox& _ParentBox, BinaryFileStream& _Stream);

	inline bool AddToBoxList(IBox& _ParentBox, IBox* _NewBox, uint64_t _BoxSize,BinaryFileStream& _Stream){
		_NewBox->BoxSize = _BoxSize - 8;
		_ParentBox.Boxs.push_back(_NewBox);
		return true;
	}

	inline bool AddToBoxList(IBox& _ParentBox, IFullBox* _NewBox, uint64_t _BoxSize, BinaryFileStream& _Stream) {
		_NewBox->BoxSize = _BoxSize - 12;
		char temp_buff[4];
		memset(temp_buff, 0, 4);
		_Stream.Read(&temp_buff[3], 1);
		memcpy(&_NewBox->Version, temp_buff, 4);
		_Stream.Read(&temp_buff[1], 3);
		std::reverse(&temp_buff[0], &temp_buff[4]);
		memcpy(&_NewBox->Flag, temp_buff, 4);
		_ParentBox.Boxs.push_back(_NewBox);
		return true;
	}

	//bool _PraseFullBoxHead(IFullBox& _Box, BinaryFileStream& _Stream);

	bool _PraseFTYPBox(FTYPBox& _Box, BinaryFileStream& _Stream);

	bool _PraseMVHDBox(MVHDBox& _Box, BinaryFileStream& _Stream);

	bool _PraseTKHDBox(TKHDBox& _Box, BinaryFileStream& _Stream);

	bool _PraseELSTBox(ELSTBox& _Box, BinaryFileStream& _Stream);
protected:
	std::string m_sVideoFileName;
	MP4RawData m_dVideoData;
	bool m_bHadPrase = false;
	bool m_bHasError = false;
};