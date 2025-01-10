import { enumKey } from "../../../dom/nodes";
import type { PaintProps } from "../../../dom/types";
import {
  BlendMode,
  PaintStyle,
  StrokeCap,
  StrokeJoin,
} from "../../../skia/types";
import type { SkPaint, Skia } from "../../../skia/types";

export const processColor = (
  Skia: Skia,
  color: number | string | Float32Array | number[]
) => {
  "worklet";
  if (typeof color === "string" || typeof color === "number") {
    return Skia.Color(color);
  } else if (Array.isArray(color) || color instanceof Float32Array) {
    return color instanceof Float32Array ? color : new Float32Array(color);
  } else {
    throw new Error(
      `Invalid color type: ${typeof color}. Expected number, string, or array.`
    );
  }
};

export const setPaintProperties = (
  Skia: Skia,
  paint: SkPaint,
  {
    opacity,
    color,
    blendMode,
    strokeWidth,
    style,
    strokeJoin,
    strokeCap,
    strokeMiter,
    antiAlias,
    dither,
  }: PaintProps
) => {
  "worklet";
  if (opacity !== undefined) {
    paint.setAlphaf(paint.getAlphaf() * opacity);
  }
  if (color !== undefined) {
    const currentOpacity = paint.getAlphaf();
    paint.setShader(null);
    paint.setColor(processColor(Skia, color));
    paint.setAlphaf(currentOpacity * paint.getAlphaf());
  }
  if (blendMode !== undefined) {
    paint.setBlendMode(BlendMode[enumKey(blendMode)]);
  }
  if (strokeWidth !== undefined) {
    paint.setStrokeWidth(strokeWidth);
  }
  if (style !== undefined) {
    paint.setStyle(PaintStyle[enumKey(style)]);
  }
  if (strokeJoin !== undefined) {
    paint.setStrokeJoin(StrokeJoin[enumKey(strokeJoin)]);
  }
  if (strokeCap !== undefined) {
    paint.setStrokeCap(StrokeCap[enumKey(strokeCap)]);
  }
  if (strokeMiter !== undefined) {
    paint.setStrokeMiter(strokeMiter);
  }
  if (antiAlias !== undefined) {
    paint.setAntiAlias(antiAlias);
  }
  if (dither !== undefined) {
    paint.setDither(dither);
  }
};