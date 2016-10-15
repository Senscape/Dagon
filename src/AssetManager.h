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

#ifndef DAGON_ASSET_MANAGER_H_
#define DAGON_ASSET_MANAGER_H_

#include "Asset.h"
#include "AudioAsset.h"
#include "Config.h"
#include "Object.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace dagon {

class AssetManager {
public:
  static AssetManager& instance() {
    static AssetManager singleton;
    return singleton;
  }

  std::shared_ptr<AudioAsset> asAudioAsset(const std::string& fileName) {
    return asAsset<AudioAsset>(config.path(kPathResources, fileName, kObjectAudio));
  }
private:
  template <class T>
  std::shared_ptr<T> asAsset(const AssetID_t& id) {
    _mutex.lock();

    auto it = _activeAssets.find(id);
    if (it != _activeAssets.end()) {
      if (!it->second.expired()) {
        _mutex.unlock();
        return std::static_pointer_cast<T>(it->second.lock());
      }

      _activeAssets.erase(it);
    }

    auto assetPtr = std::make_shared<T>(id);
    _activeAssets.emplace(id, assetPtr);

    _mutex.unlock();
    return assetPtr;
  }

  AssetManager() : config(Config::instance()){}
  AssetManager(const AssetManager&);
  void operator=(const AssetManager&);

  std::mutex _mutex;

  Config& config;

  std::unordered_map<AssetID_t, std::weak_ptr<Asset>> _activeAssets;
};

}

#endif // DAGON_ASSET_MANAGER_H_
