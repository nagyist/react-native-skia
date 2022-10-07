#pragma once

#include <vector>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"

#include <SkCanvas.h>
#include <SkPaint.h>

#pragma clang diagnostic pop

namespace RNSkia {

class DrawingContext: public std::enable_shared_from_this<DrawingContext> {
public:
  /**
   Creates a root drawing context with paint and opacity
   */
  DrawingContext(std::shared_ptr<SkPaint> paint, double opacity) {
    _paint = paint;
    _opacity = opacity;
    _source = "root";
  }
  
  ~DrawingContext() {
    // RNSkLogger::logToConsole("DrawingContext::dtor - " + std::string(_source));
  }
  
  /**
   Initilalizes a new draw context.
   */
  DrawingContext(std::shared_ptr<DrawingContext> parent) {
    _parent = parent;
  }
  
  /**
   Factory for creating a child context
   */
  std::shared_ptr<DrawingContext> inheritContext(const char* source) {
    auto result = std::make_shared<DrawingContext>(shared_from_this());
    result->_source = source;
    _children.push_back(result);
    return result;
  }
  
  void print_debug_description() {
    std::string v = "";
    std::shared_ptr<DrawingContext> cur = _parent;
    while (cur != nullptr) {
      v += "  ";
      cur = cur->_parent;
    }
    v = v + "ctx for " + std::string(_source) + ":";
    
    if (!isInvalid()) {
      auto clr = _paint->getColor();
      auto a = SkColorGetA(clr);
      auto r = SkColorGetR(clr);
      auto g = SkColorGetG(clr);
      auto b = SkColorGetB(clr);
      
      if (r > 0 || g > 0 || b > 0) {
        v += " color:rgba(" + std::to_string(r) + ", " +
          std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a) + ")";
      }
      
      if (_paint->getMaskFilter() != nullptr) {
        v += " maskFilter:set";
      }
      auto blendMode = _paint->getBlendMode_or(SkBlendMode::kSrc);
      if (blendMode != SkBlendMode::kSrc) {
        v += " blendMode:" + std::to_string((size_t)blendMode);        
      }
    } else {
      v += " empty (inherits from parent)";
    }
    
    v = v + "\n";
    
    RNSkLogger::logToConsole(v);    
  }
  
  /**
   Invalidate cache
   */
  void invalidate() {
    _paint = nullptr;
  }
  
  void dispose() {
    if (_parent != nullptr) {
      auto position = std::find(_parent->_children.begin(),
                                _parent->_children.end(),
                                shared_from_this());
      
      if (position != _parent->_children.end()) {
        _parent->_children.erase(position);
      }
      _parent = nullptr;      
    }
  }
  
  /**
   Returns true if the current cache is changed
   */
  bool isInvalid() {
    return _paint == nullptr;
  }
  
  /**
   Get/Sets the canvas object
   */
  SkCanvas *getCanvas() {
    if (_parent != nullptr) {
      return _parent->getCanvas();
    }
    
    return _canvas;
  }
  
  /**
   Sets the canvas
   */
  void setCanvas(SkCanvas* canvas) {
    _canvas = canvas;
  }
  
  /**
   Gets the paint object
   */
  std::shared_ptr<const SkPaint> getPaint() {
    if (_paint != nullptr) {
      return _paint;
    }
    return _parent->getPaint();
  }
  
  /**
   To be able to mutate and change the paint in a context we need to mutate the underlying paint
   object - otherwise we'll just use the parent paint object (to avoid having to create multiple paint
   objects for nodes that does not change the paint).
   
   Calling this accessor implies that the paint is about to be mutatet and will therefore invalidate
   any child contexts to pick up changes from this context as the parent context.
   */
  std::shared_ptr<SkPaint> getMutablePaint() {
    if (_paint == nullptr) {
      _paint = std::make_shared<SkPaint>(*_parent->getPaint());
    }
    invalidateChildren();
    return _paint;
  }
  
  /**
   Sets the paint in the current sub context
   */
  void setMutablePaint(std::shared_ptr<SkPaint> paint) {
    _paint = paint;
  }
  
  /**
   Getd the opacity value
   */
  double getOpacity() {
    if (_paint == nullptr) {
      return _parent->getOpacity();
    }
    return _opacity;
  }
  
  /**
   Sets the opacity value
   */
  void setOpacity(double opacity) {
    if (_parent != nullptr) {
      _opacity = _parent->getOpacity() * opacity;
    } else {
      _opacity = opacity;
    }
    // TODO: Is this enough to set opacity?
    getMutablePaint()->setAlpha(_opacity * 255);
  }
  
private:
  
  void invalidateChildren() {
    for (auto &child: _children) {
      child->invalidate();
    }
  }
  
  std::shared_ptr<SkPaint> _paint;
  double _opacity;
  
  SkCanvas *_canvas = nullptr;
  const char* _source;
  
  std::shared_ptr<DrawingContext> _parent;
  std::vector<std::shared_ptr<DrawingContext>> _children;
};

}