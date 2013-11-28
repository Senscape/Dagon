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

#ifndef DAGON_CONFIGURABLE_H_
#define DAGON_CONFIGURABLE_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <cassert>
#include <map>
#include <string>

namespace dagon {
  
////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////
  
typedef struct {
  int min;
  int max;
  int value;
} Setting;
  
typedef std::map<std::string, Setting> SettingCollection;
  
////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////
  
template <typename T>
class Configurable {
 protected:
  std::map <std::string, T> SettingAlias;
  typename std::map <std::string, T>::const_iterator _aliasIterator;

  SettingCollection _theSettings;
  
 public:
  Configurable() {};
  
  bool beginIteratingSettings() {
    if (!_theSettings.empty()) {
      _aliasIterator = SettingAlias.begin();
      return true;
    }
    return false;
  }
  
  bool iterateSettings() {
    ++_aliasIterator;
    if (_aliasIterator == SettingAlias.end() || SettingAlias.empty()) {
      return false;
    } else {
      return true;
    }
  }
  
  std::string getCurrentSetting(Setting* pointerToSetting) {
    assert(_aliasIterator != SettingAlias.end());
    pointerToSetting->min = _theSettings[_aliasIterator->first].min;
    pointerToSetting->max = _theSettings[_aliasIterator->first].max;
    pointerToSetting->value = _theSettings[_aliasIterator->first].value;
    return _aliasIterator->first;
  }
  
  void initAliases(int numberOfAliases, const char* listOfAliases[]) {
    _numberOfAliases = numberOfAliases;
    for (int i = 0; i < _numberOfAliases; ++i) {
      SettingAlias[listOfAliases[i]] = T(i);
    }
  }
  
  T indexOf(const std::string &value) {
    typename std::map <std::string, T>::const_iterator iValue = SettingAlias.find(value);
    if (iValue  == SettingAlias.end())
      throw 0;
    return iValue->second;
  }
  
  int operator[](const std::string& theName) {
    return _theSettings[theName].value;
  }
  int get(const std::string& theName) {
    return _theSettings[theName].value;
  }
  int numberOfAliases() {
    return _numberOfAliases;
  }
  void loadSettings(const SettingCollection& theSettings) {
    _theSettings = theSettings;
  }
  void saveSettings(SettingCollection* theSettings) {
    *theSettings = _theSettings;
  }
  void set(const std::string& theName, int theValue) {
    _theSettings[theName].value = theValue;
  }
  
 private:
  int _numberOfAliases;
};

}

#endif // DAGON_CONFIGURABLE_H_
