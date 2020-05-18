#pragma once
#include<vector>

struct IBox {
	uint64_t box_size = 0;

	std::vector<IBox*> boxs;
};

struct IFullBox : public IBox {
	uint32_t version;

	uint32_t flag;
};

struct FTYPBox : public IBox {
	std::string major_brand;
	int minor_version = 0;
	std::vector<std::string> compatible_brands;
};

struct MVHDBox : public IFullBox {
	uint64_t creation_time; // unsigned int(ver == 1 ? 64 : 32) creation_time; 
	uint64_t modification_time;// unsigned int(ver == 1 ? 64 : 32) modification_time; 
	uint32_t timescale; // unsigned int(32) timescale; 
	uint64_t duration; // unsigned int(ver == 1 ? 64 : 32) timescale; 
	int32_t rate; // template int(32)  rate = 0x00010000;
	int16_t volume; // template int(16)  volume = 0x0100;
	int16_t bit_reserved = 0; //const bit(16)  reserved = 0;    
	int32_t reserved[2]; //const unsigned int(32)[2]  reserved = 0; 
	int32_t matrix[9]; //template int(32)[9]  matrix = { 0x00010000,0,0,0,0x00010000,0,0,0,0x40000000 };     // Unity matrix    
	int32_t pre_defined[6]; //bit(32)[6]  pre_defined = 0;    
	int32_t next_track_ID; //unsigned int(32) next_track_ID; 
};

typedef IFullBox MOOVBox;
typedef IBox MP4RawData;

struct TKHDBox : public IBox {

};