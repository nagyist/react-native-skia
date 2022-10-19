#pragma once

#include "JsiDomDrawingNode.h"
#include "SvgProp.h"
#include "RectProp.h"

namespace RNSkia {

class JsiImageSvgNode : public JsiDomDrawingNode, public JsiDomNodeCtor<JsiImageSvgNode> {
public:
  JsiImageSvgNode(std::shared_ptr<RNSkPlatformContext> context) :
  JsiDomDrawingNode(context, "skImageSvg") {}
    
protected:
  void draw(DrawingContext* context) override {
    auto svgDom = _svgDomProp->getDerivedValue();
    auto rect = _rectProp->getDerivedValue();
    
    context->getCanvas()->save();
    context->getCanvas()->translate(rect->x(), rect->y());
    svgDom->setContainerSize(SkSize::Make(rect->width(), rect->height()));
    svgDom->render(context->getCanvas());
    context->getCanvas()->restore();
  }
  
  void defineProperties(NodePropsContainer* container) override {
    JsiDomDrawingNode::defineProperties(container);
    _svgDomProp = container->defineProperty(std::make_shared<SvgProp>(JsiPropId::get("svg")));
    _rectProp = container->defineProperty(std::make_shared<RectProps>(JsiPropId::get("rect")));
    
    _svgDomProp->require();
  }
  
private:
  SvgProp* _svgDomProp;
  RectProps* _rectProp;
};

}

