#pragma once
#include<vector>
#include<string>

struct IBox {
	uint64_t BoxSize = 0;

	std::vector<IBox*> Boxs;
};

struct IFullBox : public IBox {
	uint32_t Version;

	uint32_t Flag;
};

struct FTYPBox : public IBox {
	std::string MajorBrand;
	int MinorVersion = 0;
	std::vector<std::string> CompatibleBrands;
};

struct MVHDBox : public IFullBox {
	uint64_t CreationTime; // unsigned int(ver == 1 ? 64 : 32) creation_time; 
	uint64_t ModificationTime;// unsigned int(ver == 1 ? 64 : 32) modification_time; 
	uint32_t TimeScale; // unsigned int(32) timescale; 
	uint64_t Duration; // unsigned int(ver == 1 ? 64 : 32) timescale; 
	int32_t Rate; // template int(32)  rate = 0x00010000;
	int16_t Volume; // template int(16)  volume = 0x0100;
	int16_t ShortReserved = 0; //const bit(16)  reserved = 0;    
	int32_t Reserved[2]; //const unsigned int(32)[2]  reserved = 0; 
	int32_t Matrix[9]; //template int(32)[9]  matrix = { 0x00010000,0,0,0,0x00010000,0,0,0,0x40000000 };     // Unity matrix    
	int32_t PreDefined[6]; //bit(32)[6]  pre_defined = 0;    
	int32_t NextTrack_ID; //unsigned int(32) next_track_ID; 
};

struct TKHDBox : public IFullBox {
	bool TrackEnabled;//Track_enabled: Indicates that the track is enabled.
	bool TrackInMovie;//Track_in_movie : Indicates that the track is used in the presentation.
	bool TrackInPreview;//Track_in_preview : Indicates that the track is used when previewing the presentation.
	bool TrackInPoster;//Track_in_poster : Indicates that the track is used in movie's poster.
	uint64_t CreationTime;//	unsigned int(version == 1 ? 64 : 32) creation_time;
	uint64_t ModificationTime;//	unsigned int(version == 1 ? 64 : 32) modification_time;  
	uint32_t TrackID;//	unsigned int(32) track_ID;      
	uint32_t HeadReserved;//	const unsigned int(32)  reserved = 0;     
	uint64_t Duration;//	unsigned int(version == 1 ? 64 : 32) duration;

	uint32_t Reserved[2];//const unsigned int(32)[2]  reserved = 0;   
	int16_t Layer;//template int(16) layer = 0;    
	int16_t AlternateGroup;//template int(16) alternate_group = 0;   
	int16_t Volume;//template int(16)  volume = {if track_is_audio 0x0100 else 0};  
	int16_t ShortReserved;//const unsigned int(16)  reserved = 0;   
	int32_t Matrix[9];//template int(32)[9]  matrix= { 0x00010000,0,0,0,0x00010000,0,0,0,0x40000000 };     // unity matrix    
	int32_t Width;//unsigned int(32) width;  
	int32_t Height;//unsigned int(32) height; 
};

struct ELSTEntry {
	uint64_t SegmentDuration;//	unsigned int(version == 1 ? 64 : 32) segment_duration;
	uint64_t MediaTime;//	unsigned int(version == 1 ? 64 : 32) media_time;  
	int16_t MediaRateInteger;//	unsigned int(16) media_rate_integer;      
	int16_t MediaRateFraction;//	const unsigned int(16)  media_rate_fraction = 0;     
};

struct ELSTBox : public IFullBox {
	uint32_t EntryCount;//	unsigned int(32) entry_count;
	std::vector<ELSTEntry> Entrys;
};

typedef IBox MOOVBox;
typedef IBox TRAKBox;
typedef IBox EDTSBox;
typedef IBox MP4RawData;