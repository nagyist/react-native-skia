#pragma once

#include "DerivedNodeProp.h"

#include "JsiSkPicture.h"

namespace RNSkia {

class PictureProp:
public DerivedSkProp<SkPicture> {
public:
  PictureProp(PropId name): DerivedSkProp<SkPicture>() {
    _pictureProp = addProperty(std::make_shared<NodeProp>(name));
  }
  
  void updateDerivedValue() override {
    if (_pictureProp->value()->getType() != PropType::HostObject) {
      throw std::runtime_error("Expected SkPicture object for the picture property.");
    }
    
    auto ptr = std::dynamic_pointer_cast<JsiSkPicture>(_pictureProp->value()->getAsHostObject());
    if (ptr == nullptr) {
      throw std::runtime_error("Expected SkPicture object for the picture property.");
    }
    
    setDerivedValue(ptr->getObject());
  }
  
private:
  NodeProp* _pictureProp;
};

}