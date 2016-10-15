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

#ifndef DAGON_ASSET_H_
#define DAGON_ASSET_H_

#include <mutex>
#include <string>

namespace dagon {

typedef std::string AssetID_t;

class Asset {
public:
  Asset(const AssetID_t& id) : _id(id), _isLoaded(false) {}

  AssetID_t id() const {
    return _id;
  }

  bool loaded() const {
    return _isLoaded;
  }

  void load() {
    _loadMutex.lock();
    if (!_isLoaded) {
      _isLoaded = _load();
    }
    _loadMutex.unlock();
  }

  // The returned size may not be meaningful until the asset has been loaded.
  virtual size_t size() = 0;
private:
  std::mutex _loadMutex;

  AssetID_t _id;
  bool _isLoaded;
protected:
  virtual bool _load() = 0;
};

struct AssetPtrHash {
  size_t operator()(const std::shared_ptr<Asset>& ptr) const {
    return std::hash<std::string>()(ptr->id());
  }
};
}

#endif // DAGON_ASSET_H_
