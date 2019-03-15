/*
The MIT License (MIT)

Copyright (c) 2013-2015 SRS(ossrs)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <srs_rtsp_stack.hpp>

#if !defined(SRS_EXPORT_LIBRTMP)

#include <stdlib.h>
#include <map>
using namespace std;

#include <srs_rtmp_io.hpp>
#include <srs_kernel_buffer.hpp>
#include <srs_kernel_error.hpp>
#include <srs_kernel_log.hpp>
#include <srs_kernel_consts.hpp>
#include <srs_core_autofree.hpp>
#include <srs_kernel_utility.hpp>
#include <srs_kernel_stream.hpp>
#include <srs_kernel_codec.hpp>
#include <sys/time.h>

#ifdef SRS_AUTO_STREAM_CASTER

#define SRS_RTSP_BUFFER 4096

u_int32_t convertToRTPTimestamp(struct timeval tv, u_int32_t fTimestampFrequency) {
  // 将 "struct timeval" 单元转换为RTP 时间戳单元:
  u_int32_t timestampIncrement = (fTimestampFrequency*tv.tv_sec);
// 加0.5是为了实现四舍五入
  timestampIncrement += (u_int32_t)(fTimestampFrequency*(tv.tv_usec/1000000.0) + 0.5); // note: rounding

// 生成32位的时间戳
// 时间戳 =  时间戳增量
  u_int32_t const rtpTimestamp = timestampIncrement;

  return rtpTimestamp;
}

string srs_generate_rtsp_status_text(int status)
{
    static std::map<int, std::string> _status_map;
    if (_status_map.empty()) {
        _status_map[SRS_CONSTS_RTSP_Continue                       ] = SRS_CONSTS_RTSP_Continue_str                        ;      
        _status_map[SRS_CONSTS_RTSP_OK                             ] = SRS_CONSTS_RTSP_OK_str                              ;      
        _status_map[SRS_CONSTS_RTSP_Created                        ] = SRS_CONSTS_RTSP_Created_str                         ;      
        _status_map[SRS_CONSTS_RTSP_LowOnStorageSpace              ] = SRS_CONSTS_RTSP_LowOnStorageSpace_str               ;      
        _status_map[SRS_CONSTS_RTSP_MultipleChoices                ] = SRS_CONSTS_RTSP_MultipleChoices_str                 ;      
        _status_map[SRS_CONSTS_RTSP_MovedPermanently               ] = SRS_CONSTS_RTSP_MovedPermanently_str                ;      
        _status_map[SRS_CONSTS_RTSP_MovedTemporarily               ] = SRS_CONSTS_RTSP_MovedTemporarily_str                ;      
        _status_map[SRS_CONSTS_RTSP_SeeOther                       ] = SRS_CONSTS_RTSP_SeeOther_str                        ;      
        _status_map[SRS_CONSTS_RTSP_NotModified                    ] = SRS_CONSTS_RTSP_NotModified_str                     ;      
        _status_map[SRS_CONSTS_RTSP_UseProxy                       ] = SRS_CONSTS_RTSP_UseProxy_str                        ;      
        _status_map[SRS_CONSTS_RTSP_BadRequest                     ] = SRS_CONSTS_RTSP_BadRequest_str                      ;      
        _status_map[SRS_CONSTS_RTSP_Unauthorized                   ] = SRS_CONSTS_RTSP_Unauthorized_str                    ;      
        _status_map[SRS_CONSTS_RTSP_PaymentRequired                ] = SRS_CONSTS_RTSP_PaymentRequired_str                 ;      
        _status_map[SRS_CONSTS_RTSP_Forbidden                      ] = SRS_CONSTS_RTSP_Forbidden_str                       ;      
        _status_map[SRS_CONSTS_RTSP_NotFound                       ] = SRS_CONSTS_RTSP_NotFound_str                        ;      
        _status_map[SRS_CONSTS_RTSP_MethodNotAllowed               ] = SRS_CONSTS_RTSP_MethodNotAllowed_str                ;      
        _status_map[SRS_CONSTS_RTSP_NotAcceptable                  ] = SRS_CONSTS_RTSP_NotAcceptable_str                   ;      
        _status_map[SRS_CONSTS_RTSP_ProxyAuthenticationRequired    ] = SRS_CONSTS_RTSP_ProxyAuthenticationRequired_str     ;      
        _status_map[SRS_CONSTS_RTSP_RequestTimeout                 ] = SRS_CONSTS_RTSP_RequestTimeout_str                  ;      
        _status_map[SRS_CONSTS_RTSP_Gone                           ] = SRS_CONSTS_RTSP_Gone_str                            ;      
        _status_map[SRS_CONSTS_RTSP_LengthRequired                 ] = SRS_CONSTS_RTSP_LengthRequired_str                  ;      
        _status_map[SRS_CONSTS_RTSP_PreconditionFailed             ] = SRS_CONSTS_RTSP_PreconditionFailed_str              ;      
        _status_map[SRS_CONSTS_RTSP_RequestEntityTooLarge          ] = SRS_CONSTS_RTSP_RequestEntityTooLarge_str           ;      
        _status_map[SRS_CONSTS_RTSP_RequestURITooLarge             ] = SRS_CONSTS_RTSP_RequestURITooLarge_str              ;      
        _status_map[SRS_CONSTS_RTSP_UnsupportedMediaType           ] = SRS_CONSTS_RTSP_UnsupportedMediaType_str            ;      
        _status_map[SRS_CONSTS_RTSP_ParameterNotUnderstood         ] = SRS_CONSTS_RTSP_ParameterNotUnderstood_str          ;      
        _status_map[SRS_CONSTS_RTSP_ConferenceNotFound             ] = SRS_CONSTS_RTSP_ConferenceNotFound_str              ;      
        _status_map[SRS_CONSTS_RTSP_NotEnoughBandwidth             ] = SRS_CONSTS_RTSP_NotEnoughBandwidth_str              ;      
        _status_map[SRS_CONSTS_RTSP_SessionNotFound                ] = SRS_CONSTS_RTSP_SessionNotFound_str                 ;      
        _status_map[SRS_CONSTS_RTSP_MethodNotValidInThisState      ] = SRS_CONSTS_RTSP_MethodNotValidInThisState_str       ;      
        _status_map[SRS_CONSTS_RTSP_HeaderFieldNotValidForResource ] = SRS_CONSTS_RTSP_HeaderFieldNotValidForResource_str  ;      
        _status_map[SRS_CONSTS_RTSP_InvalidRange                   ] = SRS_CONSTS_RTSP_InvalidRange_str                    ;      
        _status_map[SRS_CONSTS_RTSP_ParameterIsReadOnly            ] = SRS_CONSTS_RTSP_ParameterIsReadOnly_str             ;      
        _status_map[SRS_CONSTS_RTSP_AggregateOperationNotAllowed   ] = SRS_CONSTS_RTSP_AggregateOperationNotAllowed_str    ;      
        _status_map[SRS_CONSTS_RTSP_OnlyAggregateOperationAllowed  ] = SRS_CONSTS_RTSP_OnlyAggregateOperationAllowed_str   ;      
        _status_map[SRS_CONSTS_RTSP_UnsupportedTransport           ] = SRS_CONSTS_RTSP_UnsupportedTransport_str            ;      
        _status_map[SRS_CONSTS_RTSP_DestinationUnreachable         ] = SRS_CONSTS_RTSP_DestinationUnreachable_str          ;      
        _status_map[SRS_CONSTS_RTSP_InternalServerError            ] = SRS_CONSTS_RTSP_InternalServerError_str             ;      
        _status_map[SRS_CONSTS_RTSP_NotImplemented                 ] = SRS_CONSTS_RTSP_NotImplemented_str                  ;      
        _status_map[SRS_CONSTS_RTSP_BadGateway                     ] = SRS_CONSTS_RTSP_BadGateway_str                      ;     
        _status_map[SRS_CONSTS_RTSP_ServiceUnavailable             ] = SRS_CONSTS_RTSP_ServiceUnavailable_str              ;     
        _status_map[SRS_CONSTS_RTSP_GatewayTimeout                 ] = SRS_CONSTS_RTSP_GatewayTimeout_str                  ;     
        _status_map[SRS_CONSTS_RTSP_RTSPVersionNotSupported        ] = SRS_CONSTS_RTSP_RTSPVersionNotSupported_str         ;     
        _status_map[SRS_CONSTS_RTSP_OptionNotSupported             ] = SRS_CONSTS_RTSP_OptionNotSupported_str              ;        
    }
    
    std::string status_text;
    if (_status_map.find(status) == _status_map.end()) {
        status_text = "Status Unknown";
    } else {
        status_text = _status_map[status];
    }
    
    return status_text;
}

std::string srs_generate_rtsp_method_str(SrsRtspMethod method) 
{
    switch (method) {
        case SrsRtspMethodDescribe: return SRS_METHOD_DESCRIBE;
        case SrsRtspMethodAnnounce: return SRS_METHOD_ANNOUNCE;
        case SrsRtspMethodGetParameter: return SRS_METHOD_GET_PARAMETER;
        case SrsRtspMethodOptions: return SRS_METHOD_OPTIONS;
        case SrsRtspMethodPause: return SRS_METHOD_PAUSE;
        case SrsRtspMethodPlay: return SRS_METHOD_PLAY;
        case SrsRtspMethodRecord: return SRS_METHOD_RECORD;
        case SrsRtspMethodRedirect: return SRS_METHOD_REDIRECT;
        case SrsRtspMethodSetup: return SRS_METHOD_SETUP;
        case SrsRtspMethodSetParameter: return SRS_METHOD_SET_PARAMETER;
        case SrsRtspMethodTeardown: return SRS_METHOD_TEARDOWN;
        default: return "Unknown";
    }
}

SrsRtpPacket::SrsRtpPacket()
{
    version = 2;
    padding = 0;
    extension = 0;
    csrc_count = 0;
    marker = 1;

    payload_type = 0;
    sequence_number = 0;
    timestamp = 0;
    ssrc = 0;
    nalu_type = 0;
    fragment_st = 0;
	video_codec_type = 0;

    payload = new SrsSimpleBuffer();
    audio_samples = new SrsCodecSample();
    chunked = false;
    completed = false;
}

SrsRtpPacket::~SrsRtpPacket()
{
    srs_freep(payload);
    srs_freep(audio_samples);
}

void SrsRtpPacket::copy(SrsRtpPacket* src)
{
    version = src->version;
    padding = src->padding;
    extension = src->extension;
    csrc_count = src->csrc_count;
    marker = src->marker;
    payload_type = src->payload_type;
    sequence_number = src->sequence_number;
    timestamp = src->timestamp;
    ssrc = src->ssrc;

    chunked = src->chunked;
    completed = src->completed;
    audio_samples = new SrsCodecSample();
    video_codec_type = src->video_codec_type;
}

void SrsRtpPacket::reap(SrsRtpPacket* src)
{
    copy(src);

    srs_freep(payload);
    payload = src->payload;
    src->payload = NULL;
    
    srs_freep(audio_samples);
    audio_samples = src->audio_samples;
    src->audio_samples = NULL;
}

int SrsRtpPacket::decode(SrsStream* stream)
{
    int ret = ERROR_SUCCESS;
    struct timeval tv; 

    gettimeofday(&tv, NULL);

    // 12bytes header
    if (!stream->require(12)) {
        ret = ERROR_RTP_HEADER_CORRUPT;
        srs_error("rtsp: rtp header corrupt. ret=%d", ret);
        return ret;
    }

    //we need to uncompress the RTP header
    version = (stream->bytes[0] & 0xC0) >> 6;
    if (version != 2)
        return ERROR_RTP_HEADER_CORRUPT;

    padding = (stream->bytes[0] & 0x20) >> 5;
    extension = (stream->bytes[0] & 0x10) >> 4;
    csrc_count = stream->bytes[0] & 0x0F;
    marker = (stream->bytes[1] & 0x80) >> 7;
    payload_type = stream->bytes[1] & 0x7F;

    /*we don't support multiple CSRC now. Only one source (the server) is allowed*/
    if (csrc_count)
        return ERROR_RTP_HEADER_CORRUPT;
    /*SeqNum*/
    sequence_number = ((stream->bytes[2] << 8) & 0xFF00) | (stream->bytes[3] & 0xFF);
    /*TS*/
//    timestamp = (u_int32_t)((stream->bytes[4]<<24) &0xFF000000) | ((stream->bytes[5]<<16) & 0xFF0000) | ((stream->bytes[6]<<8) & 0xFF00) | ((stream->bytes[7]) & 0xFF);
    timestamp = convertToRTPTimestamp( tv, 90000);
    /*SSRC*/
    ssrc = (u_int32_t)((stream->bytes[8]<<24) &0xFF000000) | ((stream->bytes[9]<<16) & 0xFF0000) | ((stream->bytes[10]<<8) & 0xFF00) | ((stream->bytes[11]) & 0xFF);

    // TODO: FIXME: check sequence number.

    // video codec.
    if (payload_type == 96) {
		if (video_codec_type == 0)   // h.264
        	return decode_h264(stream);
		else if(video_codec_type == 1) // h.265
			return decode_h265(stream);
		
    } else if (payload_type == 97) {
        return decode_97(stream);
    }

    return ret;
}

int SrsRtpPacket::decode_97(SrsStream* stream)
{
    int ret = ERROR_SUCCESS;

    unsigned short au_index_delta, samples, au_header_length, au_size, au_header_loop, au_index;
    au_header_length = (unsigned char)((stream->bytes[12]) & 0xFF);
    au_header_length <<= 8;
    au_header_length |= (unsigned char)((stream->bytes[13]) & 0xFF);
    //printf("SSSS decode 97  au_header_length %d %d %d\n", au_header_length, (stream->bytes[12]) & 0xFF, (stream->bytes[13]) & 0xFF);
    au_header_length = (au_header_length + 7) / 8;

    samples = 14 + au_header_length;
    // append left bytes to payload.
    payload->append(&(stream->bytes[samples]), stream->size() - samples);
    for (au_header_loop = 0; au_header_loop < au_header_length; au_header_loop += 2)
    {
        au_size = (unsigned char)((stream->bytes[14 + au_header_loop]) & 0xFF);
        au_size <<= 8;
        au_size |= (unsigned char)((stream->bytes[15 + au_header_loop]) & 0xFF);
        au_size >>= 3;
        au_index_delta = (unsigned char)((stream->bytes[15 + au_header_loop]) & 0x7);
        if (au_header_loop == 0)
            au_index = au_index_delta;
        else
            au_index = au_index + au_index_delta + 1;

        //printf("SSSS decode 97 au_size %d au_index_delta %d au_index %d\n", au_size, au_index_delta, au_index);

        if ((ret = audio_samples->add_sample_unit(&stream->bytes[samples], au_size)) != ERROR_SUCCESS)
        {
            srs_error("rtsp: rtp type97 add sample failed. ret=%d", ret);
            return ret;
        }
        samples += au_size;
    }

    // parsed ok.
    completed = true;

    return ret;
}

#ifndef AV_RB16
#define AV_RB16(x)                      \
    ((((const uint8_t *)(x))[0] << 8) | \
     ((const uint8_t *)(x))[1])
#endif

int SrsRtpPacket::decode_h264(SrsStream *stream)

{

    int ret = ERROR_SUCCESS;

    static uint8_t start_sequence[] = {0, 0, 0, 1};



    uint8_t nal_type = stream->bytes[12] & 0x1F;

    /* Simplify the case (these are all the NAL types used internally by

       * the H.264 codec). */

    if (nal_type >= 1 && nal_type <= 23)

        nal_type = 1;



    switch (nal_type)

    {

    case 0: // undefined, but pass them through

    case 1:

        //      payload->append((const char*)start_sequence, sizeof(start_sequence));

        payload->append((const char *)&stream->bytes[12], stream->size() - 12);

        chunked = true;

        completed = true;

        nalu_type = 1;

        break;



    case 24: // STAP-A (one packet, multiple nals)

        // consume the STAP-A NAL

        int pass;

        int total_length;

        total_length = 0;

        // first we are going to figure out the total size

        for (pass = 0; pass < 2; pass++)

        {

            const char *src = (const char *)&stream->bytes[13];

            int src_len = stream->size() - 13;



            while (src_len > 2)

            {

                uint16_t nal_size = AV_RB16(src);



                // consume the length of the aggregate

                src += 2;

                src_len -= 2;



                if (nal_size <= src_len)

                {

                    if (pass == 0)

                    {

                        // counting

                        total_length += sizeof(start_sequence) + nal_size;

                    }

                    else

                    {

                        // copying

                        payload->append((const char *)start_sequence, sizeof(start_sequence));

                        payload->append((const char *)src, nal_size);

                    }

                }



                // eat what we handled

                src += nal_size;

                src_len -= nal_size;

            }

        }
            chunked = true;
            completed = true;
        break;



    case 28: // FU-A (fragmented nal)

        uint8_t fu_indicator, fu_header, start_bit, end_bit, nal_type, nal;



        const char *buf = (const char *)&stream->bytes[12];

        int len = stream->size() - 12;

        nalu_type = 28;



        //      +---------------+

        //      |0|1|2|3|4|5|6|7|

        //      +-+-+-+-+-+-+-+-+

        //      |S|E|R|  Type   |

        //      +---------------+



        fu_indicator = buf[0];

        fu_header = buf[1];

        start_bit = fu_header >> 7;

        end_bit = (fu_header & 0x40) >> 6;

        nal_type = fu_header & 0x1f;

        nal = fu_indicator & 0xe0 | nal_type;

        // skip the fu_indicator and fu_header

        buf += 2;

        len -= 2;

        chunked = true;

        completed = false;

        fragment_st = 0;

        if ((start_bit == 1) && (end_bit == 0))

        {

            payload->append((const char *)start_sequence, sizeof(start_sequence));

            payload->append((const char *)&nal, 1);

            fragment_st = 1;

        }

        payload->append(buf, len);

        if ((start_bit == 0) && (end_bit == 1))

        {

            chunked = true;

            completed = true;

            fragment_st = 2;

        }

        break;

    }



    //    completed = true;



    return ret;

}

int SrsRtpPacket::decode_h265(SrsStream *stream)
{
    int ret = ERROR_SUCCESS;
    static uint8_t start_sequence[] = {0, 0, 0, 1};

	uint8_t nal_type =  (stream->bytes[12] >> 1) & 0x3f;
    uint8_t lid  = ((stream->bytes[12] << 5) & 0x20) | ((stream->bytes[13] >> 3) & 0x1f);
    uint8_t tid  =   stream->bytes[13] & 0x07;

    /*Single NALU*/
    if (nal_type <= 40)	nal_type = 1;

    switch (nal_type)
    {
    case 1: /*Single NALU*/
//        payload->append((const char*)start_sequence, sizeof(start_sequence));
        payload->append((const char *)&stream->bytes[12], stream->size() - 12);
        chunked = true;
        completed = true;
		/*signal NALU size on 4 bytes*/
		//to do 
        nalu_type = 1;
        break;

    case 48: /*AP NALU*/
        /* aggregated packet (AP) - with two or more NAL units */
	    /* Not support the HEVC DONL field this version */
        int pass;
        int total_length;
        total_length = 0;
		nalu_type = 48;
        // first we are going to figure out the total size
        for (pass = 0; pass < 2; pass++)
        {
            const char *src = (const char *)&stream->bytes[14];
            int src_len = stream->size() - 14;

            while (src_len > 2)
            {
                uint16_t nal_size = AV_RB16(src);

                // consume the length of the aggregate
                src += 2;
                src_len -= 2;

                if (nal_size <= src_len && nal_size > 0)
                {
                    if (pass == 0)
                    {
                        // counting
                        total_length += sizeof(start_sequence) + nal_size;
                    }
                    else
                    {
                        // copying
                        payload->append((const char *)start_sequence, sizeof(start_sequence));
                        payload->append((const char *)src, nal_size);
                    }
                }

                // eat what we handled
                src += nal_size;
                src_len -= nal_size;
            }
        }
		chunked = true;
        completed = true;
        break;

    case 49: /*FU NALU*/
		    /* Not support the HEVC DONL field this version */
			uint8_t fu_type, fu_header, start_bit, end_bit, nal_type, nal[2];
				
			const char* buf = (const char*)&stream->bytes[14];
			int len = stream->size()-14;
			nalu_type = 49;
				
			/*
			 *	  decode the FU header
			 *
			 *	   0 1 2 3 4 5 6 7
			 *	  +-+-+-+-+-+-+-+-+
			 *	  |S|E|  FuType   |
			 *	  +---------------+
			 *
			 *		 Start fragment (S): 1 bit
			 *		 End fragment (E): 1 bit
			 *		 FuType: 6 bits
			 */
				
 			start_bit = ((buf[0] & 0x80)>>7)&0x1;
			end_bit   = ((buf[0] & 0x40)>>6)&0x1;
			fu_type   = buf[0] & 0x3f;
		
		
		
			nal[0] = (stream->bytes[12] & 0x81) | (fu_type << 1);
			nal[1] = stream->bytes[13];
				
			// skip the fu_header
			buf += 1;
			len -= 1;

			chunked = true;
			completed = false;
			fragment_st = 0;
			if ((start_bit == 1 ) && (end_bit == 0 )) {
//				payload->append((const char*)start_sequence, sizeof(start_sequence));
				payload->append((const char*)&nal, 2);
					
				fragment_st = 1;
				
			}
				
			if (len > 0) {
				payload->append( buf, len);
			}
				
			if ((start_bit == 0 ) && (end_bit == 1 )) {
				chunked = true;
				completed = true;
				fragment_st = 2;
					
			}
				
			break;

    }

    //    completed = true;

    return ret;
}

SrsRtspSdp::SrsRtspSdp()
{
    state = SrsRtspSdpStateOthers;
}

SrsRtspSdp::~SrsRtspSdp()
{
}

int SrsRtspSdp::parse(string token)
{
    int ret = ERROR_SUCCESS;

    if (token.empty()) {
        srs_info("rtsp: ignore empty token.");
        return ret;
    }
    
    size_t pos = string::npos;

    char* start = (char*)token.data();
    char* end = start + (int)token.length();
    char* p = start;

    // key, first 2bytes.
    // v=0
    // o=- 0 0 IN IP4 127.0.0.1
    // s=No Name
    // c=IN IP4 192.168.43.23
    // t=0 0
    // a=tool:libavformat 53.9.0
    // m=video 0 RTP/AVP 96
    // b=AS:850
    // a=rtpmap:96 H264/90000
    // a=fmtp:96 packetization-mode=1; sprop-parameter-sets=Z2QAKKzRwFAFu/8ALQAiEAAAAwAQAAADAwjxgxHg,aOmrLIs=
    // a=control:streamid=0
    // m=audio 0 RTP/AVP 97
    // b=AS:49
    // a=rtpmap:97 MPEG4-GENERIC/44100/2
    // a=fmtp:97 profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3; config=139056E5A0
    // a=control:streamid=1

	//  v=0
	//  o=- 0 0 IN IP4 127.0.0.1
	//  s=No Name
	//  c=IN IP4 127.0.0.1
	//  t=0 0
	//  a=tool:libavformat 58.26.101
	//  m=video 0 RTP/AVP 96
	//  a=rtpmap:96 H265/90000
	//  a=fmtp:96 sprop-vps=QAEMAf//AWAAAAMAkAAAAwAAAwA/lZgJ; sprop-sps=QgEBAWAAAAMAkAAAAwAAAwA/oAUCAWnXllZpJMr/8MIQwCAQAAADABAAAAMB4IA=; sprop-pps=RAHBcrRiQA==
	//  a=control:streamid=0
	//  m=audio 0 RTP/AVP 97
	//  b=AS:128
	//  a=rtpmap:97 MPEG4-GENERIC/44100/2
	//  a=fmtp:97 profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3; config=121056E500
	//  a=control:streamid=1


    char key = p[0];
    p += 2;

    // left bytes as attr string.
    std::string attr_str;
    if (end - p) {
        attr_str.append(p, end - p);
    }

    // parse the attributes from left bytes.
    std::vector<std::string> attrs;
    while (p < end) {
        // parse an attribute, split by SP.
        char* pa = p;
        for (; p < end && p[0] != SRS_RTSP_SP; p++) {
        }
        std::string attr;
        if (p > pa) {
            attr.append(pa, p - pa);
            attrs.push_back(attr);
        }
        p++;
    }

    // parse the first attr as desc, update the first elem for desc.
    // for example, the value can be "tool", "AS", "rtpmap", "fmtp", "control"
    std::string desc_key;
    if (attrs.size() > 0) {
        std::string attr = attrs.at(0);
        if ((pos = attr.find(":")) != string::npos) {
            desc_key = attr.substr(0, pos);
            attr = attr.substr(pos + 1);
            attr_str = attr_str.substr(pos + 1);
            attrs[0] = attr;
        } else {
            desc_key = attr;
        }
    }

    // interpret the attribute according by key.
    switch (key) {
        case 'v': version = attr_str; break;
        case 'o':
            owner_username = (attrs.size() > 0)? attrs[0]:"";
            owner_session_id = (attrs.size() > 1)? attrs[1]:"";
            owner_session_version = (attrs.size() > 2)? attrs[2]:"";
            owner_network_type = (attrs.size() > 3)? attrs[3]:"";
            owner_address_type = (attrs.size() > 4)? attrs[4]:"";
            owner_address = (attrs.size() > 5)? attrs[5]:"";
            break;
        case 's': session_name = attr_str; break;
        case 'c':
            connection_network_type = (attrs.size() > 0)? attrs[0]:"";
            connection_address_type = (attrs.size() > 0)? attrs[0]:"";
            connection_address = (attrs.size() > 0)? attrs[0]:"";
            break;
        case 'a':
            if (desc_key == "tool") {
                tool = attr_str;
            } else if (desc_key == "rtpmap") {
                if (state == SrsRtspSdpStateVideo) {
                    video_codec = (attrs.size() > 1)? attrs[1]:"";
                    if ((pos = video_codec.find("/")) != string::npos) {
                        video_sample_rate = video_codec.substr(pos + 1);
                        video_codec = video_codec.substr(0, pos);
                    }
                } else if (state == SrsRtspSdpStateAudio) {
                    audio_codec = (attrs.size() > 1)? attrs[1]:"";
                    if ((pos = audio_codec.find("/")) != string::npos) {
                        audio_sample_rate = audio_codec.substr(pos + 1);
                        audio_codec = audio_codec.substr(0, pos);
                    }
                    if ((pos = audio_sample_rate.find("/")) != string::npos) {
                        audio_channel = audio_sample_rate.substr(pos + 1);
                        audio_sample_rate = audio_sample_rate.substr(0, pos);
                    }
                }
            } else if (desc_key == "fmtp") {
                for (int i = 1; i < (int)attrs.size(); i++) {
                    std::string attr = attrs.at(i);
                    if ((ret = parse_fmtp_attribute(attr)) != ERROR_SUCCESS) {
                        srs_error("rtsp: parse fmtp failed, attr=%s. ret=%d", attr.c_str(), ret);
                        return ret;
                    }
                }
            } else if (desc_key == "control") {
                for (int i = 0; i < (int)attrs.size(); i++) {
                    std::string attr = attrs.at(i);
                    if ((ret = parse_control_attribute(attr)) != ERROR_SUCCESS) {
                        srs_error("rtsp: parse control failed, attr=%s. ret=%d", attr.c_str(), ret);
                        return ret;
                    }
                }
            }
            break;
        case 'm':
            if (desc_key == "video") {
                state = SrsRtspSdpStateVideo;
                video_port = (attrs.size() > 1)? attrs[1]:"";
                video_protocol = (attrs.size() > 2)? attrs[2]:"";
                video_transport_format = (attrs.size() > 3)? attrs[3]:"";
            } else if (desc_key == "audio") {
                state = SrsRtspSdpStateAudio;
                audio_port = (attrs.size() > 1)? attrs[1]:"";
                audio_protocol = (attrs.size() > 2)? attrs[2]:"";
                audio_transport_format = (attrs.size() > 3)? attrs[3]:"";
            }
            break;
        case 'b':
            if (desc_key == "AS") {
                if (state == SrsRtspSdpStateVideo) {
                    video_bandwidth_kbps = (attrs.size() > 0)? attrs[0]:"";
                } else if (state == SrsRtspSdpStateAudio) {
                    audio_bandwidth_kbps = (attrs.size() > 0)? attrs[0]:"";
                }
            }
            break;
        case 't':
        default: break;
    }

    return ret;
}

int SrsRtspSdp::parse_fmtp_attribute(string attr)
{
    int ret = ERROR_SUCCESS;
    
    size_t pos = string::npos;
    std::string token = attr;

    while (!token.empty()) {
        std::string item = token;
        if ((pos = item.find(";")) != string::npos) {
            item = token.substr(0, pos);
            token = token.substr(pos + 1);
        } else {
            token = "";
        }

        std::string item_key = item, item_value;
        if ((pos = item.find("=")) != string::npos) {
            item_key = item.substr(0, pos);
            item_value = item.substr(pos + 1);
        }

        if (state == SrsRtspSdpStateVideo) {
            if (item_key == "packetization-mode") {
                video_packetization_mode = item_value;
				  // packetization-mode 的值为 0 时或不存在时, 必须使用单一 NALU 单元模式.
  				  // packetization-mode 的值为 1 时必须使用非交错(non-interleaved)封包模式.
  				  // packetization-mode 的值为 2 时必须使用交错(interleaved)封包模式.
            } else if (item_key == "sprop-parameter-sets") {
                video_sps = item_value;
                if ((pos = video_sps.find(",")) != string::npos) {
                    video_pps = video_sps.substr(pos + 1);
                    video_sps = video_sps.substr(0, pos);
                }
                // decode the sps/pps by base64
                video_sps = base64_decode(video_sps);
                video_pps = base64_decode(video_pps);
            } else if (item_key == "sprop-vps") {
				video_vps = item_value;
				video_vps = base64_decode(video_vps);
           	} else if (item_key == "sprop-sps") {
				video_sps = item_value;
				video_sps = base64_decode(video_sps);
           	} else if (item_key == "sprop-pps") {
				video_pps = item_value;
				video_pps = base64_decode(video_pps);
           	} else if (item_key == "sprop-sei") {
				srs_warn("sprop-sei not support this time");
           	} else if (item_key == "sprop-max-don-diff") {
				srs_warn("sprop-max-don-diff not support this time");
           	}
				
        } else if (state == SrsRtspSdpStateAudio) {
            if (item_key == "profile-level-id") {
                audio_profile_level_id = item_value;
            } else if (item_key == "mode") {
                audio_mode = item_value;
            } else if (item_key == "sizelength") {
                audio_size_length = item_value;
            } else if (item_key == "indexlength") {
                audio_index_length = item_value;
            } else if (item_key == "indexdeltalength") {
                audio_index_delta_length = item_value;
            } else if (item_key == "config") {
                if (item_value.length() <= 0) {
                    ret = ERROR_RTSP_AUDIO_CONFIG;
                    srs_error("rtsp: audio config failed. ret=%d", ret);
                    return ret;
                }

                char* tmp_sh = new char[item_value.length()];
                SrsAutoFreeA(char, tmp_sh);
                int nb_tmp_sh = ff_hex_to_data((u_int8_t*)tmp_sh, item_value.c_str());
                srs_assert(nb_tmp_sh > 0);
                audio_sh.append(tmp_sh, nb_tmp_sh);
            }
        }
    }

    return ret;
}

int SrsRtspSdp::parse_control_attribute(string attr)
{
    int ret = ERROR_SUCCESS;
    
    size_t pos = string::npos;
    std::string token = attr;

    while (!token.empty()) {
        std::string item = token;
        if ((pos = item.find(";")) != string::npos) {
            item = token.substr(0, pos);
            token = token.substr(pos + 1);
        } else {
            token = "";
        }

        std::string item_key = item, item_value;
        if ((pos = item.find("=")) != string::npos) {
            item_key = item.substr(0, pos);
            item_value = item.substr(pos + 1);
        }

        if (state == SrsRtspSdpStateVideo) {
            if (item_key == "streamid") {
                video_stream_id = item_value;
            }
        } else if (state == SrsRtspSdpStateAudio) {
            if (item_key == "streamid") {
                audio_stream_id = item_value;
            }
        }
    }

    return ret;
}

string SrsRtspSdp::base64_decode(string value)
{
    if (value.empty()) {
        return "";
    }

    int nb_output = (int)(value.length() * 2);
    u_int8_t* output = new u_int8_t[nb_output];
    SrsAutoFreeA(u_int8_t, output);

    int ret = srs_av_base64_decode(output, (char*)value.c_str(), nb_output);
    if (ret <= 0) {
        return "";
    }

    std::string plaintext;
    plaintext.append((char*)output, ret);
    return plaintext;
}

SrsRtspTransport::SrsRtspTransport()
{
    client_port_min = 0;
    client_port_max = 0;
}

SrsRtspTransport::~SrsRtspTransport()
{
}

int SrsRtspTransport::parse(string attr)
{
    int ret = ERROR_SUCCESS;
    
    size_t pos = string::npos;
    std::string token = attr;

    while (!token.empty()) {
        std::string item = token;
        if ((pos = item.find(";")) != string::npos) {
            item = token.substr(0, pos);
            token = token.substr(pos + 1);
        } else {
            token = "";
        }

        std::string item_key = item, item_value;
        if ((pos = item.find("=")) != string::npos) {
            item_key = item.substr(0, pos);
            item_value = item.substr(pos + 1);
        }

        if (transport.empty()) {
            transport = item_key;
            if ((pos = transport.find("/")) != string::npos) {
                profile = transport.substr(pos + 1);
                transport = transport.substr(0, pos);
            }
            if ((pos = profile.find("/")) != string::npos) {
                lower_transport = profile.substr(pos + 1);
                profile = profile.substr(0, pos);
            }
        }

        if (item_key == "unicast" || item_key == "multicast") {
            cast_type = item_key;
        } else if (item_key == "mode") {
            mode = item_value;
        } else if (item_key == "client_port") {
            std::string sport = item_value;
            std::string eport = item_value;
            if ((pos = eport.find("-")) != string::npos) {
                sport = eport.substr(0, pos);
                eport = eport.substr(pos + 1);
            }
            client_port_min = ::atoi(sport.c_str());
            client_port_max = ::atoi(eport.c_str());
        }
    }

    return ret;
}

SrsRtspRequest::SrsRtspRequest()
{
    seq = 0;
    content_length = 0;
    stream_id = 0;
    sdp = NULL;
    transport = NULL;
}

SrsRtspRequest::~SrsRtspRequest()
{
    srs_freep(sdp);
    srs_freep(transport);
}

bool SrsRtspRequest::is_options()
{
    return method == SRS_METHOD_OPTIONS;
}

bool SrsRtspRequest::is_describe()
{
    return method == SRS_METHOD_DESCRIBE;
}

bool SrsRtspRequest::is_play()
{
    return method == SRS_METHOD_PLAY;
}

bool SrsRtspRequest::is_announce()
{
    return method == SRS_METHOD_ANNOUNCE;
}

bool SrsRtspRequest::is_setup()
{
    return method == SRS_METHOD_SETUP;
}

bool SrsRtspRequest::is_record()
{
    return method == SRS_METHOD_RECORD;
}

SrsRtspResponse::SrsRtspResponse(int cseq)
{
    seq = cseq;
    status = SRS_CONSTS_RTSP_OK;
}

SrsRtspResponse::~SrsRtspResponse()
{
}

int SrsRtspResponse::encode(stringstream& ss)
{
    int ret = ERROR_SUCCESS;

    // status line
    ss << SRS_RTSP_VERSION << SRS_RTSP_SP 
        << status << SRS_RTSP_SP 
        << srs_generate_rtsp_status_text(status) << SRS_RTSP_CRLF;

    // cseq
    ss << SRS_RTSP_TOKEN_CSEQ << ":" << SRS_RTSP_SP << seq << SRS_RTSP_CRLF;

    // others.
    ss << "Cache-Control: no-store" << SRS_RTSP_CRLF
        << "Pragma: no-cache" << SRS_RTSP_CRLF
        << "Server: " << RTMP_SIG_SRS_SERVER << SRS_RTSP_CRLF;

    // session if specified.
    if (!session.empty()) {
        ss << SRS_RTSP_TOKEN_SESSION << ":" << session << SRS_RTSP_CRLF;
    }

    if ((ret = encode_header(ss)) != ERROR_SUCCESS) {
        srs_error("rtsp: encode header failed. ret=%d", ret);
        return ret;
    };

    // header EOF.
    ss << SRS_RTSP_CRLF;

    return ret;
}

int SrsRtspResponse::encode_header(std::stringstream& ss)
{
    return ERROR_SUCCESS;
}

SrsRtspOptionsResponse::SrsRtspOptionsResponse(int cseq) : SrsRtspResponse(cseq)
{
    methods = (SrsRtspMethod)(SrsRtspMethodDescribe | SrsRtspMethodOptions 
        | SrsRtspMethodPause | SrsRtspMethodPlay | SrsRtspMethodSetup | SrsRtspMethodTeardown
        | SrsRtspMethodAnnounce | SrsRtspMethodRecord);
}

SrsRtspOptionsResponse::~SrsRtspOptionsResponse()
{
}

int SrsRtspOptionsResponse::encode_header(stringstream& ss)
{
    SrsRtspMethod rtsp_methods[] = {
        SrsRtspMethodDescribe,
        SrsRtspMethodAnnounce,
        SrsRtspMethodGetParameter,
        SrsRtspMethodOptions,
        SrsRtspMethodPause,
        SrsRtspMethodPlay,
        SrsRtspMethodRecord,
        SrsRtspMethodRedirect,
        SrsRtspMethodSetup,
        SrsRtspMethodSetParameter,
        SrsRtspMethodTeardown,
    };

    ss << SRS_RTSP_TOKEN_PUBLIC << ":" << SRS_RTSP_SP;

    bool appended = false;
    int nb_methods = (int)(sizeof(rtsp_methods) / sizeof(SrsRtspMethod));
    for (int i = 0; i < nb_methods; i++) {
        SrsRtspMethod method = rtsp_methods[i];
        if (((int)methods & (int)method) != (int)method) {
            continue;
        }

        if (appended) {
            ss << ", ";
        }
        ss << srs_generate_rtsp_method_str(method);
        appended = true;
    }
    ss << SRS_RTSP_CRLF;

    return ERROR_SUCCESS;
}

SrsRtspDescribeResponse::SrsRtspDescribeResponse(int cseq) : SrsRtspResponse(cseq)
{
    video_id = 0;
    audio_id = 0;
    stream_ip = "";
    stream_name = "";
    ts_duration = 0.0;
}

SrsRtspDescribeResponse::~SrsRtspDescribeResponse()
{
}

int SrsRtspDescribeResponse::encode(stringstream &ss)
{
    int ret = ERROR_SUCCESS;
    //printf("SSSS SrsRtspDescribeResponse encode\n");
    // status line
    ss << SRS_RTSP_VERSION << SRS_RTSP_SP
       << status << SRS_RTSP_SP
       << srs_generate_rtsp_status_text(status) << SRS_RTSP_CRLF;

    // cseq
    ss << SRS_RTSP_TOKEN_CSEQ << ":" << SRS_RTSP_SP << seq << SRS_RTSP_CRLF;

    // others.
    ss << "Cache-Control: no-store" << SRS_RTSP_CRLF
       << "Pragma: no-cache" << SRS_RTSP_CRLF
       << "Server: " << RTMP_SIG_SRS_SERVER << SRS_RTSP_CRLF;

    // session if specified.
    if (!session.empty())
    {
        ss << SRS_RTSP_TOKEN_SESSION << ":" << session << SRS_RTSP_CRLF;
    }

    if ((ret = encode_header(ss)) != ERROR_SUCCESS)
    {
        srs_error("rtsp: encode header failed. ret=%d", ret);
        return ret;
    };

    return ret;
}

/*
v=0
o=StreamingServer 3712379394 1502962304000 IN IP4 192.168.234.128
s=/sample_h264_300kbit.mp4
u=http:///
e=admin@
c=IN IP4 0.0.0.0
b=AS:2097200
t=0 0
a=control:*
a=isma-compliance:2,2.0,2
a=range:npt=0-  70.00000
m=video 0 RTP/AVP 96
b=AS:2097151
a=3GPP-Adaptation-Support:1
a=rtpmap:96 H264/90000
a=control:trackID=3
a=cliprect:0,0,480,380
a=framesize:96 380-480
a=fmtp:96 packetization-mode=1;profile-level-id=4D401E;sprop-parameter-sets=J01AHqkYMB73oA==,KM4NiA==
a=mpeg4-esid:201
m=audio 0 RTP/AVP 97
b=AS:48
a=3GPP-Adaptation-Support:1
a=rtpmap:97 mpeg4-generic/22050/2
a=control:trackID=4
a=fmtp:97 profile-level-id=15;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3;config=1390
a=mpeg4-esid:101
*/

int SrsRtspDescribeResponse::encode_header(stringstream &ss)
{
    printf("SrsRtspDescribeResponse::encode_header\n");
    
    stringstream ss_sdp;
    ts_duration = ts_duration / 100.0;

    ss_sdp << SRS_RTSP_CRLF; 
    ss_sdp << "v=0" << SRS_RTSP_CRLF;
    ss_sdp << "o=StreamingServer 3712379394 1502962304000 IN IP4 " << stream_ip << SRS_RTSP_CRLF;
    ss_sdp << "s=/" << stream_name << SRS_RTSP_CRLF;
    ss_sdp << "u=http:///" << SRS_RTSP_CRLF;
    ss_sdp << "e=admin@" << SRS_RTSP_CRLF;
    ss_sdp << "c=IN IP4 0.0.0.0" << SRS_RTSP_CRLF;
    ss_sdp << "b=AS:2097200" << SRS_RTSP_CRLF;
    ss_sdp << "t=0 0" << SRS_RTSP_CRLF;
    ss_sdp << "a=control:*" << SRS_RTSP_CRLF;
    ss_sdp << "a=isma-compliance:2,2.0,2" << SRS_RTSP_CRLF;
    ss_sdp << "a=range:npt=0-  "  <<  ts_duration << SRS_RTSP_CRLF;    
    ss_sdp << "m=video 0 RTP/AVP 96" << SRS_RTSP_CRLF;
    ss_sdp << "b=AS:2097151" << SRS_RTSP_CRLF;
    ss_sdp << "a=3GPP-Adaptation-Support:1" << SRS_RTSP_CRLF;
    ss_sdp << "a=rtpmap:96 H264/90000" << SRS_RTSP_CRLF;
    ss_sdp << "a=control:trackID=" << video_id << SRS_RTSP_CRLF;
    ss_sdp << "a=cliprect:0,0,480,640" << SRS_RTSP_CRLF;
    ss_sdp << "a=framesize:96 640-480" << SRS_RTSP_CRLF;
    ss_sdp << "a=fmtp:96 packetization-mode=1;profile-level-id=4D401E;sprop-parameter-sets=J01AHqkYMB73oA==,KM4NiA==" << SRS_RTSP_CRLF;
    ss_sdp << "a=mpeg4-esid:201" << SRS_RTSP_CRLF;
    ss_sdp << "m=audio 0 RTP/AVP 97" << SRS_RTSP_CRLF;
    ss_sdp << "b=AS:48" << SRS_RTSP_CRLF;
    ss_sdp << "a=3GPP-Adaptation-Support:1" << SRS_RTSP_CRLF;
    ss_sdp << "a=rtpmap:97 mpeg4-generic/22050/2" << SRS_RTSP_CRLF;
    ss_sdp << "a=control:trackID=" << audio_id << SRS_RTSP_CRLF;
    ss_sdp << "a=fmtp:97 profile-level-id=15;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3;config=1390" << SRS_RTSP_CRLF;
    ss_sdp << "a=mpeg4-esid:101" << SRS_RTSP_CRLF;

    ss_sdp.seekp(0, ios::end);
    stringstream::pos_type offset = ss_sdp.tellp();
    printf("SSS offset %d %.5f\n",offset,ts_duration);

ss << SRS_RTSP_TOKEN_CONTENT_TYPE << ":" << SRS_RTSP_SP << "application/sdp" << SRS_RTSP_CRLF;
ss << SRS_RTSP_TOKEN_CONTENT_LENGTH << ":" << SRS_RTSP_SP << offset << SRS_RTSP_CRLF;
ss << SRS_RTSP_CRLFCRLF; 
ss << "v=0" << SRS_RTSP_CRLF;
ss << "o=StreamingServer 3712379394 1502962304000 IN IP4 " << stream_ip << SRS_RTSP_CRLF;
ss << "s=/" << stream_name << SRS_RTSP_CRLF;
ss << "u=http:///" << SRS_RTSP_CRLF;
ss << "e=admin@" << SRS_RTSP_CRLF;
ss << "c=IN IP4 0.0.0.0" << SRS_RTSP_CRLF;
ss << "b=AS:2097200" << SRS_RTSP_CRLF;
ss << "t=0 0" << SRS_RTSP_CRLF;
ss << "a=control:*" << SRS_RTSP_CRLF;
ss << "a=isma-compliance:2,2.0,2" << SRS_RTSP_CRLF;
ss << "a=range:npt=0-  " << ts_duration << SRS_RTSP_CRLF;
ss << "m=video 0 RTP/AVP 96" << SRS_RTSP_CRLF;
ss << "b=AS:2097151" << SRS_RTSP_CRLF;
ss << "a=3GPP-Adaptation-Support:1" << SRS_RTSP_CRLF;
ss << "a=rtpmap:96 H264/90000" << SRS_RTSP_CRLF;
ss << "a=control:trackID=" << video_id << SRS_RTSP_CRLF;
ss << "a=cliprect:0,0,480,640" << SRS_RTSP_CRLF;
ss << "a=framesize:96 640-480" << SRS_RTSP_CRLF;
ss << "a=fmtp:96 packetization-mode=1;profile-level-id=4D401E;sprop-parameter-sets=J01AHqkYMB73oA==,KM4NiA==" << SRS_RTSP_CRLF;
ss << "a=mpeg4-esid:201" << SRS_RTSP_CRLF;
ss << "m=audio 0 RTP/AVP 97" << SRS_RTSP_CRLF;
ss << "b=AS:48" << SRS_RTSP_CRLF;
ss << "a=3GPP-Adaptation-Support:1" << SRS_RTSP_CRLF;
ss << "a=rtpmap:97 mpeg4-generic/22050/2" << SRS_RTSP_CRLF;
ss << "a=control:trackID=" << audio_id << SRS_RTSP_CRLF;
ss << "a=fmtp:97 profile-level-id=15;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3;config=1390" << SRS_RTSP_CRLF;
ss << "a=mpeg4-esid:101" << SRS_RTSP_CRLF;
    return ERROR_SUCCESS;
}

SrsRtspPlayResponse::SrsRtspPlayResponse(int cseq) : SrsRtspResponse(cseq)
{
}

SrsRtspPlayResponse::~SrsRtspPlayResponse()
{
}

int SrsRtspPlayResponse::encode_header(stringstream &ss)
{

    return ERROR_SUCCESS;
}

SrsRtspSetupResponse::SrsRtspSetupResponse(int seq) : SrsRtspResponse(seq)
{
    local_port_min = 0;
    local_port_max = 0;
}

SrsRtspSetupResponse::~SrsRtspSetupResponse()
{
}

int SrsRtspSetupResponse::encode_header(stringstream& ss)
{
    ss << SRS_RTSP_TOKEN_SESSION << ":" << SRS_RTSP_SP << session << SRS_RTSP_CRLF;
    ss << SRS_RTSP_TOKEN_TRANSPORT << ":" << SRS_RTSP_SP 
        << "RTP/AVP;unicast;client_port=" << client_port_min << "-" << client_port_max << ";"
        << "server_port=" << local_port_min << "-" << local_port_max
        << SRS_RTSP_CRLF;
    return ERROR_SUCCESS;
}

SrsRtspStack::SrsRtspStack(ISrsProtocolReaderWriter* s)
{
    buf = new SrsSimpleBuffer();
    skt = s;
}

SrsRtspStack::~SrsRtspStack()
{
    srs_freep(buf);
}

int SrsRtspStack::recv_message(SrsRtspRequest** preq)
{
    int ret = ERROR_SUCCESS;

    SrsRtspRequest* req = new SrsRtspRequest();
    if ((ret = do_recv_message(req)) != ERROR_SUCCESS) {
        srs_freep(req);
        return ret;
    }

    *preq = req;

    return ret;
}

int SrsRtspStack::send_message(SrsRtspResponse* res)
{
    int ret = ERROR_SUCCESS;

    std::stringstream ss;
    // encode the message to string.
    res->encode(ss);

    std::string str = ss.str();
    srs_assert(!str.empty());

    if ((ret = skt->write((char*)str.c_str(), (int)str.length(), NULL)) != ERROR_SUCCESS) {
        if (!srs_is_client_gracefully_close(ret)) {
            srs_error("rtsp: send response failed. ret=%d", ret);
        }
        return ret;
    }
    srs_info("rtsp: send response ok");

    return ret;
}

int SrsRtspStack::do_recv_message(SrsRtspRequest* req)
{
    int ret = ERROR_SUCCESS;

    // parse request line.
    if ((ret = recv_token_normal(req->method)) != ERROR_SUCCESS) {
        if (!srs_is_client_gracefully_close(ret)) {
            srs_error("rtsp: parse method failed. ret=%d", ret);
        }
        return ret;
    }

    if ((ret = recv_token_normal(req->uri)) != ERROR_SUCCESS) {
        if (!srs_is_client_gracefully_close(ret)) {
            srs_error("rtsp: parse uri failed. ret=%d", ret);
        }
        return ret;
    }

    if ((ret = recv_token_eof(req->version)) != ERROR_SUCCESS) {
        if (!srs_is_client_gracefully_close(ret)) {
            srs_error("rtsp: parse version failed. ret=%d", ret);
        }
        return ret;
    }

    // parse headers.
    for (;;) {
        // parse the header name
        std::string token;
        if ((ret = recv_token_normal(token)) != ERROR_SUCCESS) {
            if (ret == ERROR_RTSP_REQUEST_HEADER_EOF) {
                ret = ERROR_SUCCESS;
                srs_info("rtsp: message header parsed");
                break;
            }
            if (!srs_is_client_gracefully_close(ret)) {
                srs_error("rtsp: parse token failed. ret=%d", ret);
            }
            return ret;
        }

        // parse the header value according by header name
        if (token == SRS_RTSP_TOKEN_CSEQ) {
            std::string seq;
            if ((ret = recv_token_eof(seq)) != ERROR_SUCCESS) {
                if (!srs_is_client_gracefully_close(ret)) {
                    srs_error("rtsp: parse %s failed. ret=%d", SRS_RTSP_TOKEN_CSEQ, ret);
                }
                return ret;
            }
            req->seq = ::atol(seq.c_str());
        } else if (token == SRS_RTSP_TOKEN_CONTENT_TYPE) {
            std::string ct;
            if ((ret = recv_token_eof(ct)) != ERROR_SUCCESS) {
                if (!srs_is_client_gracefully_close(ret)) {
                    srs_error("rtsp: parse %s failed. ret=%d", SRS_RTSP_TOKEN_CONTENT_TYPE, ret);
                }
                return ret;
            }
            req->content_type = ct;
        } else if (token == SRS_RTSP_TOKEN_CONTENT_LENGTH) {
            std::string cl;
            if ((ret = recv_token_eof(cl)) != ERROR_SUCCESS) {
                if (!srs_is_client_gracefully_close(ret)) {
                    srs_error("rtsp: parse %s failed. ret=%d", SRS_RTSP_TOKEN_CONTENT_LENGTH, ret);
                }
                return ret;
            }
            req->content_length = ::atol(cl.c_str());
        } else if (token == SRS_RTSP_TOKEN_TRANSPORT) {
            std::string transport;
            if ((ret = recv_token_eof(transport)) != ERROR_SUCCESS) {
                if (!srs_is_client_gracefully_close(ret)) {
                    srs_error("rtsp: parse %s failed. ret=%d", SRS_RTSP_TOKEN_TRANSPORT, ret);
                }
                return ret;
            }
            if (!req->transport) {
                req->transport = new SrsRtspTransport();
            }
            if ((ret = req->transport->parse(transport)) != ERROR_SUCCESS) {
                srs_error("rtsp: parse transport failed, transport=%s. ret=%d", transport.c_str(), ret);
                return ret;
            }
        } else if (token == SRS_RTSP_TOKEN_SESSION) {
            if ((ret = recv_token_eof(req->session)) != ERROR_SUCCESS) {
                if (!srs_is_client_gracefully_close(ret)) {
                    srs_error("rtsp: parse %s failed. ret=%d", SRS_RTSP_TOKEN_SESSION, ret);
                }
                return ret;
            }
        }
        else if (token == SRS_RTSP_TOKEN_RANGE)
        {
            std::string range;
            std::string range_target;
            size_t pos = string::npos;
            size_t pos1 = string::npos;
            if ((ret = recv_token_eof(range)) != ERROR_SUCCESS)
            {
                if (!srs_is_client_gracefully_close(ret))
                {
                    srs_error("rtsp: parse %s failed. ret=%d", SRS_RTSP_TOKEN_RANGE, ret);
                }
                return ret;
            }
            if ((pos = range.find("=")) != string::npos)
            {
                if ((pos1 = range.find("-")) != string::npos)
                {
                    range_target = range.substr(pos + 1,pos1-pos -1);
                }
            }
            req->range = ::atof(range_target.c_str());
        }
        else if (token == SRS_RTSP_TOKEN_SCALE)
        {
            std::string scale;
            if ((ret = recv_token_eof(scale)) != ERROR_SUCCESS)
            {
                if (!srs_is_client_gracefully_close(ret))
                {
                    srs_error("rtsp: parse %s failed. ret=%d", SRS_RTSP_TOKEN_SCALE, ret);
                }
                return ret;
            }
            req->scale = ::atof(scale.c_str());
        }
        else
        {
            // unknown header name, parse util EOF.
            SrsRtspTokenState state = SrsRtspTokenStateNormal;
            while (state == SrsRtspTokenStateNormal) {
                std::string value;
                if ((ret = recv_token(value, state)) != ERROR_SUCCESS) {
                    if (!srs_is_client_gracefully_close(ret)) {
                        srs_error("rtsp: parse token failed. ret=%d", ret);
                    }
                    return ret;
                }
                srs_trace("rtsp: ignore header %s=%s", token.c_str(), value.c_str());
            }
        }
    }

    // for setup, parse the stream id from uri.
    if (req->is_setup()) {
        size_t pos = string::npos;
        std::string stream_id;
        if ((pos = req->uri.rfind("/")) != string::npos) {
            stream_id = req->uri.substr(pos + 1);
        }
        if ((pos = stream_id.find("=")) != string::npos) {
            stream_id = stream_id.substr(pos + 1);
        }
        req->stream_id = ::atoi(stream_id.c_str());
        srs_info("rtsp: setup stream id=%d", req->stream_id);
    }

    // parse rdp body.
    long consumed = 0;
    while (consumed < req->content_length) {
        if (!req->sdp) {
            req->sdp = new SrsRtspSdp();
        }

        int nb_token = 0;
        std::string token;
        if ((ret = recv_token_util_eof(token, &nb_token)) != ERROR_SUCCESS) {
            if (!srs_is_client_gracefully_close(ret)) {
                srs_error("rtsp: parse sdp token failed. ret=%d", ret);
            }
            return ret;
        }
        consumed += nb_token;

        if ((ret = req->sdp->parse(token)) != ERROR_SUCCESS) {
            srs_error("rtsp: sdp parse token failed, token=%s. ret=%d", token.c_str(), ret);
            return ret;
        }
        srs_info("rtsp: %s", token.c_str());
    }
    srs_info("rtsp: sdp parsed, size=%d", consumed);
    return ret;
}

int SrsRtspStack::recv_token_normal(std::string& token)
{
    int ret = ERROR_SUCCESS;

    SrsRtspTokenState state;

    if ((ret = recv_token(token, state)) != ERROR_SUCCESS) {
        if (ret == ERROR_RTSP_REQUEST_HEADER_EOF) {
            return ret;
        }
        if (!srs_is_client_gracefully_close(ret)) {
            srs_error("rtsp: parse token failed. ret=%d", ret);
        }
        return ret;
    }

    if (state != SrsRtspTokenStateNormal) {
        ret = ERROR_RTSP_TOKEN_NOT_NORMAL;
        srs_error("rtsp: parse normal token failed, state=%d. ret=%d", state, ret);
        return ret;
    }

    return ret;
}

int SrsRtspStack::recv_token_eof(std::string& token)
{
    int ret = ERROR_SUCCESS;

    SrsRtspTokenState state;

    if ((ret = recv_token(token, state)) != ERROR_SUCCESS) {
        if (ret == ERROR_RTSP_REQUEST_HEADER_EOF) {
            return ret;
        }
        if (!srs_is_client_gracefully_close(ret)) {
            srs_error("rtsp: parse token failed. ret=%d", ret);
        }
        return ret;
    }

    if (state != SrsRtspTokenStateEOF) {
        ret = ERROR_RTSP_TOKEN_NOT_NORMAL;
        srs_error("rtsp: parse eof token failed, state=%d. ret=%d", state, ret);
        return ret;
    }

    return ret;
}

int SrsRtspStack::recv_token_util_eof(std::string& token, int* pconsumed)
{
    int ret = ERROR_SUCCESS;

    SrsRtspTokenState state;

    // use 0x00 as ignore the normal token flag.
    if ((ret = recv_token(token, state, 0x00, pconsumed)) != ERROR_SUCCESS) {
        if (ret == ERROR_RTSP_REQUEST_HEADER_EOF) {
            return ret;
        }
        if (!srs_is_client_gracefully_close(ret)) {
            srs_error("rtsp: parse token failed. ret=%d", ret);
        }
        return ret;
    }

    if (state != SrsRtspTokenStateEOF) {
        ret = ERROR_RTSP_TOKEN_NOT_NORMAL;
        srs_error("rtsp: parse eof token failed, state=%d. ret=%d", state, ret);
        return ret;
    }

    return ret;
}

int SrsRtspStack::recv_token(std::string& token, SrsRtspTokenState& state, char normal_ch, int* pconsumed)
{
    int ret = ERROR_SUCCESS;

    // whatever, default to error state.
    state = SrsRtspTokenStateError;

    // when buffer is empty, append bytes first.
    bool append_bytes = buf->length() == 0;

    // parse util token.
    for (;;) {
        // append bytes if required.
        if (append_bytes) {
            append_bytes = false;

            char buffer[SRS_RTSP_BUFFER];
            ssize_t nb_read = 0;
            if ((ret = skt->read(buffer, SRS_RTSP_BUFFER, &nb_read)) != ERROR_SUCCESS) {
                if (!srs_is_client_gracefully_close(ret)) {
                    srs_error("rtsp: io read failed. ret=%d", ret);
                }
                return ret;
            }
            srs_info("rtsp: io read %d bytes", nb_read);

            buf->append(buffer, (int32_t)nb_read);
        }

        // parse one by one.
        char* start = buf->bytes();
        char* end = start + buf->length();
        char* p = start;

        // find util SP/CR/LF, max 2 EOF, to finger out the EOF of message.
        for (; p < end && p[0] != normal_ch && p[0] != SRS_RTSP_CR && p[0] != SRS_RTSP_LF; p++) {
        }

        // matched.
        if (p < end) {
            // finger out the state.
            if (p[0] == normal_ch) {
                state = SrsRtspTokenStateNormal;
            } else {
                state = SrsRtspTokenStateEOF;
            }
            
            // got the token.
            int nb_token = p - start;
            // trim last ':' character.
            if (nb_token && p[-1] == ':') {
                nb_token--;
            }
            if (nb_token) {
                token.append(start, nb_token);
            } else {
                ret = ERROR_RTSP_REQUEST_HEADER_EOF;
            }

            // ignore SP/CR/LF
            for (int i = 0; i < 2 && p < end && (p[0] == normal_ch || p[0] == SRS_RTSP_CR || p[0] == SRS_RTSP_LF); p++, i++) {
            }

            // consume the token bytes.
            srs_assert(p - start);
            buf->erase(p - start);
            if (pconsumed) {
                *pconsumed = p - start;
            }
            break;
        }

        // append more and parse again.
        append_bytes = true;
    }

    return ret;
}

#endif

#endif

