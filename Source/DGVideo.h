////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2013 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DG_VIDEO_H
#define DG_VIDEO_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

#include <theora/theora.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum DGVideoStates {
    DGVideoInitial,
    DGVideoPlaying,
    DGVideoPaused,
    DGVideoStopped
};

typedef struct {
	int width;
	int	height;
	int depth;
	unsigned char* data;
} DGFrame;

typedef struct {
	ogg_sync_state oy;
	ogg_page og;
	ogg_stream_state to;
	theora_info ti;
	theora_comment tc;
	theora_state td;
	ogg_packet op;
	
    ogg_int64_t bos;
	int long_option_index;
	int c;
	int theora_p;
	int videobuf_ready;
	ogg_int64_t videobuf_granulepos;
	double videobuf_time;
} DGTheoraInfo;

#define DGVideoBuffer 4096

#define DGPutComponent(p, v, i) \
tmp = (unsigned int)(v); \
if (tmp < 0x10000) \
p[i] = tmp >> 8; \
else \
p[i] = (tmp >> 24) ^ 0xff; 

class DGLog;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGVideo : public DGObject {
    DGLog* log;
    
    DGFrame _currentFrame;
    DGTheoraInfo* _theoraInfo;
    
    bool _doesAutoplay;
    double _frameDuration;
    FILE* _handle;
    bool _hasNewFrame;
    bool _hasResource;
    bool _isLoaded;
    bool _isLoopable;
    bool _isSynced;
    double _lastTime;
    int _state;
    
    std::mutex _mutex;
    
    // Eventually all file management will be handled by a DGResourceManager object
    char _resource[DGMaxFileLength];
    
    // Private methods
    size_t _bufferData(ogg_sync_state* oy);
    void _convertToRGB(uint8_t* puc_y, int stride_y,
                       uint8_t* puc_u, uint8_t* puc_v, int stride_uv,
                       uint8_t* puc_out, int width_y, int height_y,
                       unsigned int _stride_out);
    void _initConversionToRGB();
    int _prepareFrame();
    static int _queuePage(DGTheoraInfo* theoraInfo, ogg_page *page);
    
public:
    DGVideo();
    DGVideo(bool doesAutoplay, bool isLoopable, bool isSynced);    
    ~DGVideo();
    
    // Checks
    
    bool doesAutoplay();
    bool hasNewFrame();
    bool hasResource();
    bool isLoaded();
    bool isLoopable();
    bool isPlaying();
    bool isSynced();
    
    // Gets
    
    DGFrame* currentFrame();
    const char* resource();
    
    // Sets
    
    void setAutoplay(bool autoplay);
    void setLoopable(bool loopable);
    void setResource(const char* fromFileName);
    void setSynced(bool synced);    
    
    // State changes
    
    void load();
    void play();
    void pause();
    void stop();
    void unload();
    void update();
};

#endif // DG_VIDEO_H
