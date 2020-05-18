#pragma once
#include<string>
#include<cassert>

#include"MemoryStream.h"
#include"VideoData.h"

//�����������ݷ����л����ڴ���
//TODO:����������࣬ʹ������չ֧��������Ƶ��ʽ
class VideoLoader {
public:
	VideoLoader(const char* _VideoFile) : m_sVideoFileName(_VideoFile) 
	{ if (!_Load()) { m_bHasError = true; } else { m_bHadPrase = true; } };
	
	MP4RawData& GetVideoData() { assert(!m_bHasError && m_bHadPrase); return m_dVideoData; };

protected:
	bool _Load();

	bool _LoadBox(IBox& _ParentBox, BinaryFileStream& _Stream);

	bool _PraseFullBoxHead(IFullBox& _Box, BinaryFileStream& _Stream);

	bool _PraseFTYPBox(FTYPBox& _Box, BinaryFileStream& _Stream);

	bool _PraseMVHDBox(MVHDBox& _Box, BinaryFileStream& _Stream);

	bool _PraseTRAKBox(MVHDBox& _Box, BinaryFileStream& _Stream);
protected:
	std::string m_sVideoFileName;
	MP4RawData m_dVideoData;
	bool m_bHadPrase = false;
	bool m_bHasError = false;
};