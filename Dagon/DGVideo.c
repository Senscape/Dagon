/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGArray.h"
#include "DGConfig.h"
#include "DGLog.h"
#include "DGSystem.h"
#include "DGVideo.h"

#include <theora/theora.h>

////////////////////////////////////////////////////////////////////////////////
///// Private Data														   /////
////////////////////////////////////////////////////////////////////////////////

struct lookuptable
{
	int32_t m_plY[256];
	int32_t m_plRV[256];
	int32_t m_plGV[256];
	int32_t m_plGU[256];
	int32_t m_plBU[256];
};

static struct lookuptable lut;

typedef struct {
	ogg_sync_state      oy;
	ogg_page            og;
	ogg_stream_state    to;
	theora_info         ti;
	theora_comment      tc;
	theora_state        td;
	ogg_packet          op;
	
    ogg_int64_t     bos;
	int				long_option_index;
	int				c;
	int             theora_p;
	int             videobuf_ready;
	ogg_int64_t     videobuf_granulepos;
	double          videobuf_time;
} DG_THEORA;

typedef struct {
	char file[DG_MAX_OBJNAME];
	FILE* fh;
	float fps_factor;
	DG_BOOL autoplay;
	DG_BOOL has_frame;
	DG_BOOL	has_reset;
	DG_BOOL loaded;
	DG_BOOL loop;
    DG_BOOL is_frame_ready;
	DG_BOOL is_playing;
	DG_BOOL sync;
	DG_FRAME frame;
	DG_THEORA theora;
    clock_t lastTime;
} DG_VIDEO;

DG_ARRAY* videos;

static DG_BOOL active = DG_NO;

////////////////////////////////////////////////////////////////////////////////
///// Private Prototypes												   /////
////////////////////////////////////////////////////////////////////////////////

int buffer_data(FILE *in, ogg_sync_state *oy);

int prepare_frame(DG_VIDEO* video);

static int queue_page(DG_THEORA* theora, ogg_page *page);

void init_yuv2rgb(void);

void yuv2rgb_24(uint8_t *puc_y, int stride_y,
				uint8_t *puc_u, uint8_t *puc_v, int stride_uv,
				uint8_t *puc_out, int width_y, int height_y,
				unsigned int _stride_out);

DG_VIDEO* _tovideo(DG_OBJECT* video);

////////////////////////////////////////////////////////////////////////////////
///// Init																   /////
////////////////////////////////////////////////////////////////////////////////

void dg_video_init() {
	dg_log_trace(DG_MOD_VIDEO, "Initializing video manager...");
	
	init_yuv2rgb();
	
	videos = dg_array_new(0);
    
    active = DG_YES;
}

void dg_video_terminate() {
    active = DG_NO;
}

////////////////////////////////////////////////////////////////////////////////
///// Standard New / Release											   /////
////////////////////////////////////////////////////////////////////////////////

DG_OBJECT* dg_video_new(const char* name, DG_BOOL autoplay, DG_BOOL loop, DG_BOOL sync) {
	DG_OBJECT* video;
	DG_VIDEO video_data;
	
	strcpy(video_data.file, name);
	video_data.autoplay = autoplay;
	video_data.loaded = DG_NO;
	video_data.loop = loop;
	video_data.has_frame = DG_NO;
	video_data.has_reset = DG_NO;
    video_data.is_frame_ready = DG_NO;
	video_data.is_playing = DG_NO;
	video_data.sync = sync;
	
    video_data.theora.bos = 0;
	video_data.theora.theora_p = 0;
	video_data.theora.videobuf_ready = 0;
	video_data.theora.videobuf_granulepos -= 1;
	video_data.theora.videobuf_time = 0;
	
	// Store data into object and return
	video = dg_object_new(DG_OBJ_VIDEO, &video_data, sizeof(video_data));
	dg_array_add_object(videos, video);
	
	return video;
}

void dg_video_release(DG_OBJECT* video) {
	DG_VIDEO* video_data = _tovideo(video);
	
	if (video_data) {
		if (video_data->loaded)
			dg_video_unload(video);
		
		dg_object_release(video);
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation													   /////
////////////////////////////////////////////////////////////////////////////////

void dg_video_load(DG_OBJECT* video) {
	DG_VIDEO* video_data = _tovideo(video);
	
	if (video_data) {
		int stateflag = 0;
		
		dg_log_trace(DG_MOD_VIDEO, "Loading %s", video_data->file);
		
		video_data->fh = fopen(dg_config_get_path(DG_RES, video_data->file), "rb");
		
		if (video_data->fh == NULL) {
			dg_log_error(DG_MOD_VIDEO, "File %s not found", video_data->file);
			return;
		}
		
		ogg_sync_init(&video_data->theora.oy);
		
		theora_comment_init(&video_data->theora.tc);
		theora_info_init(&video_data->theora.ti);
		
		while (!stateflag) {
			int ret = buffer_data(video_data->fh, &video_data->theora.oy);
			if (ret == 0)
				break;
			
			while (ogg_sync_pageout(&video_data->theora.oy, &video_data->theora.og) > 0) {
				ogg_stream_state test;
				
				if (!ogg_page_bos(&video_data->theora.og)) {
					queue_page(&video_data->theora, &video_data->theora.og);
					stateflag = 1;
					break;
				}
				
				ogg_stream_init(&test, ogg_page_serialno(&video_data->theora.og));
				ogg_stream_pagein(&test, &video_data->theora.og);
				ogg_stream_packetout(&test, &video_data->theora.op);
				
				if (!video_data->theora.theora_p && theora_decode_header(&video_data->theora.ti, 
																		 &video_data->theora.tc, &video_data->theora.op) >= 0) {
					memcpy(&video_data->theora.to, &test, sizeof(test));
					video_data->theora.theora_p = 1;
				} else {
					ogg_stream_clear(&test);
				}
			}
		}
		
		while (video_data->theora.theora_p && video_data->theora.theora_p < 3) {
			int ret;
			
			while (video_data->theora.theora_p && (video_data->theora.theora_p < 3) && 
				   (ret = ogg_stream_packetout(&video_data->theora.to, &video_data->theora.op))) {
				if (ret < 0) {
					dg_log_error(DG_MOD_VIDEO, "Error parsing stream headers");
					return;
				}
				if (theora_decode_header(&video_data->theora.ti, &video_data->theora.tc, &video_data->theora.op)) {
					dg_log_error(DG_MOD_VIDEO, "Error parsing stream headers");
					return;
				}
				video_data->theora.theora_p++;
				if (video_data->theora.theora_p == 3)
					break;
			}
			
			if (ogg_sync_pageout(&video_data->theora.oy, &video_data->theora.og) > 0) {
				queue_page(&video_data->theora, &video_data->theora.og);
			} else {
				int ret = buffer_data(video_data->fh, &video_data->theora.oy);
				if (ret == 0) {
					dg_log_error(DG_MOD_VIDEO, "End of file while searching for codec headers");
					return;
				}
			}
		}
		
		if (video_data->theora.theora_p) {
			theora_decode_init(&video_data->theora.td, &video_data->theora.ti);
		} else {
			theora_info_clear(&video_data->theora.ti);
			theora_comment_clear(&video_data->theora.tc);
		}
		
		video_data->frame.width = video_data->theora.ti.width;
		video_data->frame.height = video_data->theora.ti.height;
		video_data->frame.bpp = 24;
		video_data->frame.fps = (int)((double)video_data->theora.ti.fps_numerator / video_data->theora.ti.fps_denominator);
		video_data->frame.data = (unsigned char*)malloc((video_data->theora.ti.width * video_data->theora.ti.height) * 3);
		
		while (ogg_sync_pageout(&video_data->theora.oy, &video_data->theora.og)>0) {
			queue_page(&video_data->theora, &video_data->theora.og);
		}
		
		video_data->has_reset = DG_YES;
		video_data->has_frame = DG_NO;
		video_data->fps_factor = (float)(1/((double)video_data->theora.ti.fps_numerator/video_data->theora.ti.fps_denominator));
		video_data->loaded = DG_YES;
	}
}

void dg_video_unload(DG_OBJECT*	video) {
	DG_VIDEO* video_data = _tovideo(video);
	
	if (video_data) {
		if (video_data->loaded) {
			dg_system_lock();
			video_data->loaded = DG_NO;
            video_data->is_frame_ready = DG_NO;
			video_data->is_playing = DG_NO;
			
			video_data->theora.videobuf_ready = 0;
			video_data->theora.videobuf_granulepos -= 1;
			video_data->theora.videobuf_time = 0;
			
			if (video_data->theora.theora_p) {
                // Rewind
                fseek(video_data->fh, (long)video_data->theora.bos*8, SEEK_SET);
                ogg_stream_reset(&video_data->theora.to);
				ogg_stream_clear(&video_data->theora.to);
				theora_clear(&video_data->theora.td);
				theora_comment_clear(&video_data->theora.tc);
				theora_info_clear(&video_data->theora.ti);
			}
			
			ogg_sync_clear(&video_data->theora.oy);
			
			video_data->theora.theora_p = 0;
			
			free(video_data->frame.data);
			dg_system_unlock();
		}
	}
	
	return;
}

DG_FRAME* dg_video_get_frame(DG_OBJECT* video) {
	DG_VIDEO* video_data = _tovideo(video);
	
	if (video_data) {
		static DG_FRAME frame;
		
		frame.width = video_data->frame.width;
		frame.height = video_data->frame.height;
		frame.bpp = video_data->frame.bpp;
		frame.data = video_data->frame.data;
		
		return &frame ;
	}
	
	return NULL;
}

DG_BOOL dg_video_has_frame(DG_OBJECT* video) {
	DG_VIDEO* video_data = _tovideo(video);
	
	if (video_data) {
		if (video_data->has_frame) {
			dg_system_lock();
			video_data->has_frame = DG_NO;
			dg_system_unlock();
			return DG_YES;
		}
	}
	
	return DG_NO;
}

DG_BOOL dg_video_has_loop(DG_OBJECT* video) {
	DG_VIDEO* video_data = _tovideo(video);
	
	if (video_data) {
		if (video_data->loop) {
			return DG_YES;
		}
	}
	
	return DG_NO;
}

DG_BOOL	dg_video_has_reset(DG_OBJECT* video) {
	DG_VIDEO* video_data = _tovideo(video);
	
	if (video_data) {
		return video_data->has_reset;
	}
	
	return DG_NO;
}

DG_BOOL dg_video_is_frame_ready(DG_OBJECT* video) {
	DG_VIDEO* video_data = _tovideo(video);
	
	if (video_data) {
		return video_data->is_frame_ready;
	}
	
	return DG_NO;
}

DG_BOOL dg_video_is_playing(DG_OBJECT* video) {
	DG_VIDEO* video_data = _tovideo(video);
	
	if (video_data) {
		return video_data->is_playing;
	}
	
	return DG_NO;
}

DG_BOOL	dg_video_is_synced(DG_OBJECT* video) {
	DG_VIDEO* video_data = _tovideo(video);
	
	if (video_data)
		return video_data->sync;
	
	return DG_NO;
}

DG_BOOL dg_video_loop() {
	clock_t         currentTime;
	double          duration;
	int				i, frames;
	
    if (active) {
        int idx, num_vids = dg_array_count(videos);
        
        for (idx = 0; idx < num_vids; idx++) {		
            DG_VIDEO* video_data = _tovideo(dg_array_get_object(videos, idx));
            
            if (video_data->loaded && video_data->is_playing) {
                if (video_data->has_reset) {
                    video_data->lastTime = clock() - (clock_t)(video_data->fps_factor * CLOCKS_PER_SEC);
                    video_data->has_reset = DG_NO;
                }
                
                currentTime = clock();
                duration = (double)(currentTime - video_data->lastTime) / CLOCKS_PER_SEC;
                
                if (duration >= video_data->fps_factor) {
                    yuv_buffer yuv;
                    
                    dg_system_lock();
                    frames = (int)floor(duration / video_data->fps_factor);
                    for (i = 1; i <= frames; i++)
                        prepare_frame(video_data);
                    
                    //if (video_data->has_frame) {
                        theora_decode_YUVout(&video_data->theora.td,&yuv);
                        yuv2rgb_24(yuv.y, yuv.y_stride,
                                   yuv.u, yuv.v, yuv.uv_stride,
                                   video_data->frame.data, video_data->theora.ti.width, video_data->theora.ti.height, video_data->theora.ti.width);
                    
                        video_data->has_frame = DG_YES;
                        video_data->is_frame_ready = DG_YES;
                    //}
                    dg_system_unlock();
                    
                    video_data->lastTime = currentTime;
                }
                //
            }
        }
	}
    
	return active;
}

void dg_video_play(DG_OBJECT* video) {
	DG_VIDEO* video_data = _tovideo(video);
	
	if (video_data) {
		video_data->is_playing = DG_YES;
	}
}

void dg_video_pause(DG_OBJECT* video) {
	DG_VIDEO* video_data = _tovideo(video);
	
	if (video_data) {
		if (video_data->is_playing)
			video_data->is_playing = DG_NO;
	}	
}

void dg_video_stop(DG_OBJECT* video) {
	DG_VIDEO* video_data = _tovideo(video);
	
	if (video_data) {
		//if (video_data->is_playing) {
			video_data->is_playing = DG_NO;
			
			// Rewind
            fseek(video_data->fh, (long)video_data->theora.bos*8, SEEK_SET);
            ogg_stream_reset(&video_data->theora.to);
		//}
	}	
}

////////////////////////////////////////////////////////////////////////////////
///// Private Functions													   /////
////////////////////////////////////////////////////////////////////////////////

int buffer_data(FILE *in, ogg_sync_state *oy) {
	char *buffer = ogg_sync_buffer(oy, 4096);
	int bytes = fread(buffer, 1, 4096, in);
	
	ogg_sync_wrote(oy, bytes);
	
	return(bytes);
}

int prepare_frame(DG_VIDEO* video) {
	while (video->is_playing) {
		while (video->theora.theora_p && !video->theora.videobuf_ready) {
			if (ogg_stream_packetout(&video->theora.to, &video->theora.op) > 0) {
				theora_decode_packetin(&video->theora.td, &video->theora.op);
				video->theora.videobuf_granulepos = video->theora.td.granulepos;
				video->theora.videobuf_time = theora_granule_time(&video->theora.td, video->theora.videobuf_granulepos);
				video->theora.videobuf_ready = 1;
                
                if (!video->theora.bos) {
                    video->theora.bos = video->theora.td.granulepos;

                }
			} else
				break;
		}
		
		if (!video->theora.videobuf_ready && feof(video->fh)) {
			if (video->loop) {
                // This is the begin of stream (granule position) * 8 bits (in bytes) 
				fseek(video->fh, (long)video->theora.bos*8, SEEK_SET);
				ogg_stream_reset(&video->theora.to);
			}
			else {
				video->is_playing = DG_NO;
				video->has_reset = DG_YES;
			}

			break;
		}
		
		if (!video->theora.videobuf_ready) {
			buffer_data(video->fh, &video->theora.oy);
			while (ogg_sync_pageout(&video->theora.oy, &video->theora.og) > 0) {
				queue_page(&video->theora, &video->theora.og);
			}

		} else {
			video->theora.videobuf_ready = 0;
			
			return 1;
		}
		video->theora.videobuf_ready = 0;
	}
	
	return 0;
}

static int queue_page(DG_THEORA* theora, ogg_page *page) {
	if (theora->theora_p) ogg_stream_pagein(&theora->to, &theora->og);
	
	return 0;
}

void init_yuv2rgb() {
	int i;
	
	for (i = 0; i < 256; i++) {
		if(i >= 16)
			if(i > 240)
				lut.m_plY[i] = lut.m_plY[240];
			else
				lut.m_plY[i] = 298 * (i - 16);
			else
				lut.m_plY[i] = 0;
		
		if ((i >= 16) && (i <= 240)) {
			lut.m_plRV[i] = 408 * (i - 128);
			lut.m_plGV[i] = -208 * (i - 128);	// Green tweaked -2?
			lut.m_plGU[i] = -100 * (i - 128);
			lut.m_plBU[i] = 517 * (i - 128);
		}
		else if (i < 16) {
			lut.m_plRV[i] = 408 * (16 - 128);
			lut.m_plGV[i] = -208 * (16 - 128);	// Green tweaked -2?
			lut.m_plGU[i] = -100 * (16 - 128);
			lut.m_plBU[i] = 517 * (16 - 126);
		}
		else {
			lut.m_plRV[i] = lut.m_plRV[240];
			lut.m_plGV[i] = lut.m_plGV[240];	// Green tweaked -2?
			lut.m_plGU[i] = lut.m_plGU[240];
			lut.m_plBU[i] = lut.m_plBU[240];
		}
	}
}

#define PUT_COMPONENT(p, v, i) \
tmp = (unsigned int)(v); \
if (tmp < 0x10000) \
p[i] = tmp >> 8; \
else \
p[i] = (tmp >> 24) ^ 0xff; 

void yuv2rgb_24(uint8_t *puc_y, int stride_y, 
				uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
				uint8_t *puc_out, int width_y, int height_y,
				unsigned int _stride_out) {
	
	int x, y;
	int stride_diff = 6 * _stride_out - 3 * width_y;
	
	if (height_y < 0) {
		/* we are flipping our output upside-down */
		height_y  = -height_y;
		puc_y     += (height_y     - 1) * stride_y ;
		puc_u     += (height_y / 2 - 1) * stride_uv;
		puc_v     += (height_y / 2 - 1) * stride_uv;
		stride_y  = -stride_y;
		stride_uv = -stride_uv;
	}
	
	for (y = 0; y < height_y; y += 2) {
		uint8_t* pY = puc_y;
		uint8_t* pY1 = puc_y+stride_y;
		uint8_t* pU = puc_u;
		uint8_t* pV = puc_v;
		uint8_t* pOut2 = puc_out + 3 * _stride_out;
		
		for (x = 0; x < width_y; x += 2) {
			int R, G, B;
			int Y;
			unsigned int tmp;
			
			R = lut.m_plRV[*pU];
			G = lut.m_plGV[*pU];
			pU++;
			G += lut.m_plGU[*pV];
			B = lut.m_plBU[*pV];
			pV++;
			Y = lut.m_plY[*pY];
			pY++;
			PUT_COMPONENT(puc_out, R+Y, 0);
			PUT_COMPONENT(puc_out, G+Y, 1);
			PUT_COMPONENT(puc_out, B+Y, 2);
			Y = lut.m_plY[*pY];
			pY++;
			PUT_COMPONENT(puc_out, R+Y, 3);
			PUT_COMPONENT(puc_out, G+Y, 4);
			PUT_COMPONENT(puc_out, B+Y, 5);
			Y = lut.m_plY[*pY1];
			pY1++;
			PUT_COMPONENT(pOut2, R+Y, 0);
			PUT_COMPONENT(pOut2, G+Y, 1);
			PUT_COMPONENT(pOut2, B+Y, 2);
			Y = lut.m_plY[*pY1];
			pY1++;
			PUT_COMPONENT(pOut2, R+Y, 3);
			PUT_COMPONENT(pOut2, G+Y, 4);
			PUT_COMPONENT(pOut2, B+Y, 5);
			puc_out += 6;
			pOut2 += 6;
		}
		
		puc_y   += 2 * stride_y;
		puc_u   += stride_uv;
		puc_v   += stride_uv;
		puc_out += stride_diff;
	}
}

DG_VIDEO* _tovideo(DG_OBJECT* video) {
	if (dg_object_check(video, DG_OBJ_VIDEO)) {
		DG_VIDEO* video_data = (DG_VIDEO*)dg_object_data(video);
		return video_data;
	}
	else {
		// raise error
		return NULL;
	}
}
