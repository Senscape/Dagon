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

#include <map>
#include <string>

namespace dagon {
  
////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////
  
typedef struct {
  int index; // Temporary fix
  int min;
  int max;
  int value;
} Setting;
  
////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////
  
class Configurable {
 protected:
  typedef std::map<std::string, Setting> SettingCollection;
  SettingCollection _theSettings;
  
 public:
  int operator[](const std::string& theName) {
    return _theSettings[theName].value;
  }
  int get(const std::string& theName) {
    return _theSettings[theName].value;
  }
  int index(const std::string& theName) {
    return _theSettings[theName].index;
  }
  void loadSettings(const SettingCollection& theSettings) {
    _theSettings = theSettings;
  }
  void saveSettings(SettingCollection* theSettings) {
    *theSettings = _theSettings;
  }
  void setValue(const std::string& theName, int theValue) {
    if (_theSettings.find(theName) == _theSettings.end()) {
      static int c = 0;
      Setting newSetting;
      newSetting.index = c;
      newSetting.value = theValue;
      _theSettings.insert(std::pair<std::string, Setting>(theName, newSetting));
      c++;
    }
    else {
      _theSettings[theName].value = theValue;
    }
  }
};

}

#endif // DAGON_CONFIGURABLE_H_
