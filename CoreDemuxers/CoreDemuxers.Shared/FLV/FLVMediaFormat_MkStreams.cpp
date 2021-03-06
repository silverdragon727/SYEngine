#include "FLVMediaFormat.h"

unsigned FLVMediaFormat::InternalInitStreams(std::shared_ptr<FLVParser::FLVStreamParser>& parser)
{
	unsigned result = PARSER_FLV_OK;

	while (1)
	{
		FLVParser::FLV_STREAM_PACKET packet = {};
		auto ret = parser->ReadNextPacket(&packet);
		if (ret != PARSER_FLV_OK)
		{
			result = ret;
			break;
		}

		parser->GetStreamInfo(&_stream_info);
		if (_stream_count == 1)
		{
			if (_stream_info.video_type == FLVParser::VideoStreamType_AVC)
				if (_stream_info.delay_flush_spec_info.avc_spec_info != nullptr)
					break;

			continue;
		}

		if (_stream_info.audio_type == FLVParser::AudioStreamType_None)
			continue;
		if (_stream_info.video_type == FLVParser::VideoStreamType_None)
			continue;

		if (_stream_info.audio_type == FLVParser::AudioStreamType_AAC)
		{
			if (_stream_info.delay_flush_spec_info.aac_spec_info == nullptr)
				continue;
		}
		if (_stream_info.video_type == FLVParser::VideoStreamType_AVC)
		{
			if (_stream_info.delay_flush_spec_info.avc_spec_info == nullptr)
				continue;
		}

		break;
	}

	return result;
}

bool FLVMediaFormat::MakeAllStreams(std::shared_ptr<FLVParser::FLVStreamParser>& parser)
{
	if (_stream_info.video_type != FLVParser::VideoStreamType_AVC)
		return false;
	if (_stream_info.audio_type != FLVParser::AudioStreamType_AAC &&
		_stream_info.audio_type != FLVParser::AudioStreamType_MP3 &&
		_stream_info.audio_type != FLVParser::AudioStreamType_PCM &&
		_stream_info.no_audio_stream == 0)
		return false;

	std::shared_ptr<IVideoDescription> h264;
	if (!_force_avc1)
		h264 = std::make_shared<X264VideoDescription>
			(_stream_info.delay_flush_spec_info.avc_spec_info,_stream_info.delay_flush_spec_info.avc_info_size);
	else
		h264 = std::make_shared<AVC1VideoDescription>
			(_stream_info.delay_flush_spec_info.avcc,_stream_info.delay_flush_spec_info.avcc_size,
			_stream_info.video_info.width,_stream_info.video_info.height);

	H264_PROFILE_SPEC profile = {};
	h264->GetProfile(&profile);
	if (profile.profile == 0 && !_force_avc1)
		return false;

	VideoBasicDescription vdesc = {};
	h264->GetVideoDescription(&vdesc);
	vdesc.bitrate = _stream_info.video_info.bitrate * 1000;

	if ((profile.variable_framerate || _force_avc1) && _stream_info.video_info.fps != 0.0)
	{
		//process variable_framerate.
		vdesc.frame_rate.den = 10000000;
		vdesc.frame_rate.num = (int)(_stream_info.video_info.fps * 10000000.0);
	}
	h264->ExternalUpdateVideoDescription(&vdesc);

	_video_stream = std::make_shared<FLVMediaStream>(h264,
		MEDIA_CODEC_VIDEO_H264,
		float(_stream_info.video_info.fps));

	if (_stream_info.audio_type == FLVParser::AudioStreamType_AAC)
	{
		std::shared_ptr<IAudioDescription> aac = std::make_shared<ADTSAudioDescription>(
			(unsigned*)_stream_info.delay_flush_spec_info.aac_spec_info,true);

		AudioBasicDescription desc = {};
		aac->GetAudioDescription(&desc);
		if (desc.nch == 0)
			return false;

		if (_stream_info.audio_info.bitrate > 0)
			UpdateAudioDescriptionBitrate(aac.get(),_stream_info.audio_info.bitrate * 1000);
		_audio_stream = std::make_shared<FLVMediaStream>(aac,MEDIA_CODEC_AUDIO_AAC);
	}else if (_stream_info.audio_type == FLVParser::AudioStreamType_MP3)
	{
		unsigned mp3_head = 0;
		while (1)
		{
			FLVParser::FLV_STREAM_PACKET packet = {};
			auto ret = parser->ReadNextPacket(&packet);
			if (ret != PARSER_FLV_OK)
				return false;

			if (packet.type != FLVParser::PacketTypeAudio ||
				packet.data_size < 8 || packet.skip_this == 1)
				continue;

			mp3_head = *(unsigned*)packet.data_buf;
			break;
		}

		if (mp3_head == 0)
			return false;

		std::shared_ptr<IAudioDescription> mp3 = std::make_shared<MPEGAudioDescription>(
			&mp3_head);

		AudioBasicDescription desc = {};
		mp3->GetAudioDescription(&desc);
		if (desc.nch == 0)
			return false;

		_audio_stream = std::make_shared<FLVMediaStream>(mp3,MEDIA_CODEC_AUDIO_MP3);
	}else if (_stream_info.audio_type == FLVParser::AudioStreamType_PCM)
	{
		CommonAudioCore comm = {};
		comm.type = 3;
		comm.desc.bits = _stream_info.audio_info.bits;
		comm.desc.nch = _stream_info.audio_info.nch;
		comm.desc.srate = _stream_info.audio_info.srate;
		comm.desc.wav_block_align = comm.desc.nch * (comm.desc.bits / 8);
		comm.desc.wav_avg_bytes = comm.desc.wav_block_align * comm.desc.srate;

		std::shared_ptr<IAudioDescription> pcm = std::make_shared<CommonAudioDescription>(comm);
		_audio_stream = std::make_shared<FLVMediaStream>(pcm,MEDIA_CODEC_PCM_SINT_LE);
	}

	return true;
}