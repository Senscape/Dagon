////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2016 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DAGON_AUDIO_ASSET_H_
#define DAGON_AUDIO_ASSET_H_

#include "Asset.h"
#include "Config.h"

#include <fstream>

namespace dagon {

class AudioAsset : public Asset {
public:
  AudioAsset(const AssetID_t& id) : Asset(id){}
  ~AudioAsset() {
    if (loaded()) {
      delete[] _data;
    }
  }

  char* data() const {
    return _data;
  }

  virtual size_t size() {
    return _dataSize;
  }
private:
  size_t _dataSize;
  char* _data;
protected:
  virtual bool _load() {
    std::ifstream file(id(), std::ifstream::binary | std::ifstream::ate);
    if (file.good()) {
      goto READ_FILE;
    }
    else {
      auto pos = id().find_last_of('/');
      if (pos != std::string::npos) {
        file = std::ifstream(Config::instance().defAssetPath(id().substr(pos), kObjectAudio),
                             std::ifstream::binary | std::ifstream::ate);
        if (file.good()) {
          goto READ_FILE;
        }
      }

      return false;
    }

  READ_FILE:
    _dataSize = file.tellg();
    _data = new char[_dataSize];

    file.seekg(file.beg);
    file.read(_data, _dataSize);
    file.close();
    return true;
  }
};

}

#endif // DAGON_AUDIO_ASSET_H_
