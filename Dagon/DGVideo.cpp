////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGLog.h"
#include "DGSystem.h"
#include "DGVideo.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// FIXME: The lookup table is shared by all objects but it should go in the manager

struct DGLookUpTable{
    int32_t m_plY[256];
    int32_t m_plRV[256];
    int32_t m_plGV[256];
    int32_t m_plGU[256];
    int32_t m_plBU[256];
};

static struct DGLookUpTable _lookUpTable;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGVideo::DGVideo() {
    log = &DGLog::getInstance();
    this->setType(DGObjectVideo);
    
    _hasNewFrame = false;
    _hasResource = false;
    _isLoaded = false;
    _state = DGVideoIdle;
    
    _doesAutoplay = true;
    _isLoopable = false;
    _isSynced = false;
    
    _theoraInfo = new DGTheoraInfo;
    
    _initConversionToRGB();
}

DGVideo::DGVideo(bool doesAutoplay, bool isLoopable, bool isSynced) {
    log = &DGLog::getInstance();
    this->setType(DGObjectVideo);
    
    _hasResource = false;
    _isLoaded = false;
    _state = DGVideoIdle;
    
    _doesAutoplay = doesAutoplay;
    _isLoopable = isLoopable;
    _isSynced = isSynced;
    
    _theoraInfo = new DGTheoraInfo;
    
    _theoraInfo->bos = 0;
	_theoraInfo->theora_p = 0;
	_theoraInfo->videobuf_ready = 0;
	_theoraInfo->videobuf_granulepos -= 1;
	_theoraInfo->videobuf_time = 0;
    
    _initConversionToRGB();
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGVideo::~DGVideo() {
    this->unload();
    delete _theoraInfo;
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool DGVideo::doesAutoplay() {
    return _doesAutoplay;
}

bool DGVideo::hasNewFrame() {
    if (_hasNewFrame) {
        _hasNewFrame = false;
        return true;
    }
    
    return false;
}

bool DGVideo::hasResource() {
    return _hasResource;
}

bool DGVideo::isLoaded() {
    return _isLoaded;
}

bool DGVideo::isLoopable() {
    return _isLoopable;
}

bool DGVideo::isPlaying() {
    return (_state == DGVideoPlaying);
}

bool DGVideo::isSynced() {
    return _isSynced;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

DGFrame* DGVideo::currentFrame() {
    return &_currentFrame;
}

const char* DGVideo::resource() {
    return _resource;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void DGVideo::setAutoplay(bool autoplay) {
    _doesAutoplay = autoplay;
}

void DGVideo::setLoopable(bool loopable) {
    _isLoopable = loopable;
}

void DGVideo::setResource(const char* fromFileName) {
    strncpy(_resource, fromFileName, DGMaxFileLength);
    _hasResource = true;
}

void DGVideo::setSynced(bool synced) {
    _isSynced = synced;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void DGVideo::load() {
    int stateFlag = 0;
    
    if (!_hasResource) {
        log->error(DGModVideo, "%s", DGMsg280005);
        return;
    }
    
    //log->trace(DGModVideo, "%s %s", DGMsg080001, _resource);
    
    _handle = fopen(_resource, "rb");
    
    if (_handle == NULL) {
        log->error(DGModVideo, "%s: %s", DGMsg280002, _resource);
        return;
    }
    
    ogg_sync_init(&_theoraInfo->oy);
    
    theora_comment_init(&_theoraInfo->tc);
    theora_info_init(&_theoraInfo->ti);
    
    while (!stateFlag) {
        int ret = _bufferData(&_theoraInfo->oy);
        
        if (ret == 0)
            break;
        
        while (ogg_sync_pageout(&_theoraInfo->oy, &_theoraInfo->og) > 0) {
            ogg_stream_state test;
            
            if (!ogg_page_bos(&_theoraInfo->og)) {
                _queuePage(_theoraInfo, &_theoraInfo->og);
                stateFlag = 1;
                break;
            }
            
            ogg_stream_init(&test, ogg_page_serialno(&_theoraInfo->og));
            ogg_stream_pagein(&test, &_theoraInfo->og);
            ogg_stream_packetout(&test, &_theoraInfo->op);
            
            if (!_theoraInfo->theora_p && 
                theora_decode_header(&_theoraInfo->ti, &_theoraInfo->tc, &_theoraInfo->op) >= 0) {
                memcpy(&_theoraInfo->to, &test, sizeof(test));
                _theoraInfo->theora_p = 1;
            } else {
                ogg_stream_clear(&test);
            }
        }
    }
    
    while (_theoraInfo->theora_p && _theoraInfo->theora_p < 3) {
        int ret;
        
        while (_theoraInfo->theora_p && (_theoraInfo->theora_p < 3) && 
               (ret = ogg_stream_packetout(&_theoraInfo->to, &_theoraInfo->op))) {
            if (ret < 0) {
                log->error(DGModVideo, "%s", DGMsg280003);
                return;
            }
            
            if (theora_decode_header(&_theoraInfo->ti, &_theoraInfo->tc, &_theoraInfo->op)) {
                log->error(DGModVideo, "%s", DGMsg280003);
                return;
            }
            
            _theoraInfo->theora_p++;
            if (_theoraInfo->theora_p == 3)
                break;
        }
        
        if (ogg_sync_pageout(& _theoraInfo->oy, & _theoraInfo->og) > 0)
            _queuePage(_theoraInfo, &_theoraInfo->og);
        else {
            int ret = _bufferData(&_theoraInfo->oy);
            if (ret == 0) {
                log->error(DGModVideo, "%s", DGMsg280004);
                return;
            }
        }
    }
    
    if (_theoraInfo->theora_p) {
        theora_decode_init(&_theoraInfo->td, &_theoraInfo->ti);
    } else {
        theora_info_clear(&_theoraInfo->ti);
        theora_comment_clear(&_theoraInfo->tc);
    }
    
    _currentFrame.width = _theoraInfo->ti.width;
    _currentFrame.height = _theoraInfo->ti.height;
    _currentFrame.depth = 24; // NOTE: We only support flat RGB for now
    _currentFrame.data = (unsigned char*)malloc((_theoraInfo->ti.width * _theoraInfo->ti.height) * 3);
    
    while (ogg_sync_pageout(&_theoraInfo->oy, &_theoraInfo->og)>0) {
        _queuePage(_theoraInfo, &_theoraInfo->og);
    }
    
    _frameDuration = (float)(1/((double)_theoraInfo->ti.fps_numerator / _theoraInfo->ti.fps_denominator));
    _isLoaded = true;
}

void DGVideo::play() {
    if (_state == DGVideoIdle)
        _lastTime = DGSystem::getInstance().wallTime() - (time_t)(_frameDuration * CLOCKS_PER_SEC);
    
    _state = DGVideoPlaying;
}

void DGVideo::pause() {
    if (_state == DGVideoPlaying)
        _state = DGVideoPaused;
}

void DGVideo::stop() {
    if (_state != DGVideoIdle) {
        _state = DGVideoIdle;
        
        // Rewind
        fseek(_handle, (long)_theoraInfo->bos * 8, SEEK_SET);
        ogg_stream_reset(&_theoraInfo->to);
    }
}

void DGVideo::unload() {
    if (_isLoaded) {
        _isLoaded = false;
        _state = DGVideoIdle;
        
        _theoraInfo->videobuf_ready = 0;
        _theoraInfo->videobuf_granulepos -= 1;
        _theoraInfo->videobuf_time = 0;
        
        if (_theoraInfo->theora_p) {
            // Rewind and reset
            fseek(_handle, (long)_theoraInfo->bos * 8, SEEK_SET);
            ogg_stream_reset(&_theoraInfo->to);
            ogg_stream_clear(&_theoraInfo->to);
            theora_clear(&_theoraInfo->td);
            theora_comment_clear(&_theoraInfo->tc);
            theora_info_clear(&_theoraInfo->ti);
        }
        
        ogg_sync_clear(&_theoraInfo->oy);
        
        _theoraInfo->theora_p = 0;
        
        free(_currentFrame.data);
        fclose(_handle);
    }
}

void DGVideo::update() {
    if (_state == DGVideoPlaying) {
        time_t currentTime = DGSystem::getInstance().wallTime();
        double duration = (double)(currentTime - _lastTime) / CLOCKS_PER_SEC;
        
        if (duration >= _frameDuration) {
            yuv_buffer yuv;
            
            // Skip frames?
            //int frames = (int)floor(duration / _frameDuration);
            //for (int i = 1; i <= frames; i++)
                _prepareFrame();
            
            theora_decode_YUVout(&_theoraInfo->td, &yuv);
            _convertToRGB(yuv.y, yuv.y_stride,
                       yuv.u, yuv.v, yuv.uv_stride,
                       _currentFrame.data, _theoraInfo->ti.width, _theoraInfo->ti.height, _theoraInfo->ti.width);
            
            _lastTime = currentTime;
            
            _hasNewFrame = true;
        }
	}
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

int DGVideo::_bufferData(ogg_sync_state* oy) {
    char *buffer = ogg_sync_buffer(oy, DGVideoBuffer);
	int bytes = fread(buffer, 1, DGVideoBuffer, _handle);
	
	ogg_sync_wrote(oy, bytes);
	
	return(bytes);
}

void DGVideo::_convertToRGB(uint8_t* puc_y, int stride_y,
                   uint8_t* puc_u, uint8_t* puc_v, int stride_uv,
                   uint8_t* puc_out, int width_y, int height_y,
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
			
			R = _lookUpTable.m_plRV[*pU];
			G = _lookUpTable.m_plGV[*pU];
			pU++;
			G += _lookUpTable.m_plGU[*pV];
			B = _lookUpTable.m_plBU[*pV];
			pV++;
			Y = _lookUpTable.m_plY[*pY];
			pY++;
			DGPutComponent(puc_out, R+Y, 0);
			DGPutComponent(puc_out, G+Y, 1);
			DGPutComponent(puc_out, B+Y, 2);
			Y = _lookUpTable.m_plY[*pY];
			pY++;
			DGPutComponent(puc_out, R+Y, 3);
			DGPutComponent(puc_out, G+Y, 4);
			DGPutComponent(puc_out, B+Y, 5);
			Y = _lookUpTable.m_plY[*pY1];
			pY1++;
			DGPutComponent(pOut2, R+Y, 0);
			DGPutComponent(pOut2, G+Y, 1);
			DGPutComponent(pOut2, B+Y, 2);
			Y = _lookUpTable.m_plY[*pY1];
			pY1++;
			DGPutComponent(pOut2, R+Y, 3);
			DGPutComponent(pOut2, G+Y, 4);
			DGPutComponent(pOut2, B+Y, 5);
			puc_out += 6;
			pOut2 += 6;
		}
		
		puc_y   += 2 * stride_y;
		puc_u   += stride_uv;
		puc_v   += stride_uv;
		puc_out += stride_diff;
	}
}

void DGVideo::_initConversionToRGB() {
	for (int i = 0; i < 256; i++) {
		if (i >= 16)
			if (i > 240)
				_lookUpTable.m_plY[i] = _lookUpTable.m_plY[240];
			else
				_lookUpTable.m_plY[i] = 298 * (i - 16);
			else
				_lookUpTable.m_plY[i] = 0;
		
		if ((i >= 16) && (i <= 240)) {
			_lookUpTable.m_plRV[i] = 408 * (i - 128);
			_lookUpTable.m_plGV[i] = -208 * (i - 128);	// Green tweaked -2?
			_lookUpTable.m_plGU[i] = -100 * (i - 128);
			_lookUpTable.m_plBU[i] = 517 * (i - 128);
		}
		else if (i < 16) {
			_lookUpTable.m_plRV[i] = 408 * (16 - 128);
			_lookUpTable.m_plGV[i] = -208 * (16 - 128);	// Green tweaked -2?
			_lookUpTable.m_plGU[i] = -100 * (16 - 128);
			_lookUpTable.m_plBU[i] = 517 * (16 - 126);
		}
		else {
			_lookUpTable.m_plRV[i] = _lookUpTable.m_plRV[240];
			_lookUpTable.m_plGV[i] = _lookUpTable.m_plGV[240];	// Green tweaked -2?
			_lookUpTable.m_plGU[i] = _lookUpTable.m_plGU[240];
			_lookUpTable.m_plBU[i] = _lookUpTable.m_plBU[240];
		}
	}
}

int DGVideo::_prepareFrame() {
    while (_state == DGVideoPlaying) {
		while (_theoraInfo->theora_p && !_theoraInfo->videobuf_ready) {
			if (ogg_stream_packetout(&_theoraInfo->to, &_theoraInfo->op) > 0) {
				theora_decode_packetin(&_theoraInfo->td, &_theoraInfo->op);
				_theoraInfo->videobuf_granulepos = _theoraInfo->td.granulepos;
				_theoraInfo->videobuf_time = theora_granule_time(&_theoraInfo->td, _theoraInfo->videobuf_granulepos);
				_theoraInfo->videobuf_ready = 1;
                
                if (!_theoraInfo->bos) {
                    _theoraInfo->bos = _theoraInfo->td.granulepos;
                }
			} else
				break;
		}
		
		if (!_theoraInfo->videobuf_ready && feof(_handle)) {
			if (_isLoopable) {
                // This is the begin of stream (granule position) * 8 bits (in bytes) 
				fseek(_handle, (long)_theoraInfo->bos * 8, SEEK_SET);
				ogg_stream_reset(&_theoraInfo->to);
			}
			else {
				_state = DGVideoIdle;
			}
            
			break;
		}
		
		if (!_theoraInfo->videobuf_ready) {
			_bufferData(&_theoraInfo->oy);
			while (ogg_sync_pageout(&_theoraInfo->oy, &_theoraInfo->og) > 0) {
				_queuePage(_theoraInfo, &_theoraInfo->og);
			}
            
		} else {
			_theoraInfo->videobuf_ready = 0;
			
			return 1;
		}
		_theoraInfo->videobuf_ready = 0;
	}
	
	return 0;
}

int DGVideo::_queuePage(DGTheoraInfo* theoraInfo, ogg_page *page) {
	if (theoraInfo->theora_p) ogg_stream_pagein(&theoraInfo->to, page);
	
	return 0;
}
