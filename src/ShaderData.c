/*
 DAGON - An Adventure Game Engine
 Copyright (c) 2011-2014 Senscape s.r.l.
 All rights reserved.
 
 This Source Code Form is subject to the terms of the
 Mozilla Public License, v. 2.0. If a copy of the MPL was
 not distributed with this file, You can obtain one at
 http://mozilla.org/MPL/2.0/.
 */

/*
 * Embedded GL shader data
 */

const char kShaderData[] =
  "\n // Base texture and coordinates shared by all functions"
  "\n "
  "\n uniform sampler2D tex;"
  "\n vec2 uv;"
  "\n "
  "\n // Adjust parameters"
  "\n "
  "\n uniform bool AdjustEnabled;"
  "\n uniform float AdjustBrightness;"
  "\n uniform float AdjustSaturation;"
  "\n uniform float AdjustContrast;"
  "\n "
  "\n // Adjust function (brightness, saturation, contrast)"
  "\n // (adapted from TGM's shader pack)"
  "\n "
  "\n vec4 Adjust(vec4 base, float brt, float sat, float con) {"
  "\n   // Increase or decrease theese values to adjust r, g and b color channels seperately"
  "\n   const float AvgLumR = 0.5;"
  "\n   const float AvgLumG = 0.5;"
  "\n   const float AvgLumB = 0.5;"
  "\n   "
  "\n   const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);"
  "\n     "
  "\n     vec3 pixel = vec3(base.x, base.y, base.z);"
  "\n   "
  "\n   vec3 AvgLumin = vec3(AvgLumR, AvgLumG, AvgLumB);"
  "\n   vec3 brtColor = pixel * brt;"
  "\n   vec3 intensity = vec3(dot(brtColor, LumCoeff));"
  "\n   vec3 satColor = mix(intensity, brtColor, sat);"
  "\n   vec3 conColor = mix(AvgLumin, satColor, con);"
  "\n     "
  "\n   return vec4(conColor, 1.0);"
  "\n }"
  "\n "
  "\n // Motion Blur parameters"
  "\n "
  "\n uniform bool MotionBlurEnabled;"
  "\n uniform float MotionBlurIntensity;"
  "\n uniform float MotionBlurOffsetX;"
  "\n uniform float MotionBlurOffsetY;"
  "\n "
  "\n // Motion Blur function"
  "\n "
  "\n vec4 MotionBlur(float offsetx, float offsety, float intensity) {"
  "\n   vec2 offsetx_aux = vec2(offsetx / intensity, 0.0);"
  "\n   vec2 offsety_aux = vec2(0.0, offsety / intensity);"
  "\n     "
  "\n   vec4 c0 = texture2D(tex, uv);"
  "\n   vec4 c1 = texture2D(tex, uv - offsety_aux);"
  "\n   vec4 c2 = texture2D(tex, uv + offsety_aux);"
  "\n   vec4 c3 = texture2D(tex, uv - offsetx_aux);"
  "\n   vec4 c4 = texture2D(tex, uv + offsetx_aux);"
  "\n     "
  "\n   vec4 motion = c0 * 0.2 + c1 * 0.2 + c2 * 0.2 + c3 * 0.2 + c4 * 0.2;"
  "\n     "
  "\n     return motion;"
  "\n }"
  "\n "
  "\n // Noise parameters"
  "\n "
  "\n uniform bool NoiseEnabled;"
  "\n uniform float NoiseIntensity;"
  "\n uniform float NoiseRand;"
  "\n "
  "\n // Noise function"
  "\n "
  "\n vec4 Noise(vec4 base, float rand, float intensity) {"
  "\n     float fract = fract(sin(dot(uv, vec2(12.9898 + rand, 78.233 + rand))) * 43758.5453);"
  "\n   vec3 noise = vec3(fract * vec3(1.0, 1.0, 1.0));"
  "\n     "
  "\n     return mix(base, vec4(noise, 1.0), intensity);"
  "\n }"
  "\n "
  "\n // Sepia parameters"
  "\n "
  "\n uniform bool SepiaEnabled;"
  "\n uniform float SepiaIntensity;"
  "\n "
  "\n // Sepia function"
  "\n "
  "\n vec4 Sepia(vec4 base, float intensity) {"
  "\n     vec3 pixel = vec3(base.x, base.y, base.z);"
  "\n     vec3 color;"
  "\n     "
  "\n     color.x = pixel.x * 0.3588 + pixel.y * 0.7044 + pixel.z * 0.1368;"
  "\n     color.y = pixel.x * 0.2990 + pixel.y * 0.5870 + pixel.z * 0.1140;"
  "\n     color.z = pixel.x * 0.2392 + pixel.y * 0.4696 + pixel.z * 0.0912;"
  "\n     "
  "\n     vec3 blend = (intensity * color) + ((1.0 - intensity) * pixel);"
  "\n     "
  "\n   return vec4(blend, 1.0);"
  "\n }"
  "\n "
  "\n // Sharpen parameters"
  "\n "
  "\n uniform bool SharpenEnabled;"
  "\n uniform float SharpenIntensity;"
  "\n uniform float SharpenRatio;"
  "\n "
  "\n // Sharpen function"
  "\n "
  "\n vec4 Sharpen(vec4 base, float ratio, float intensity) {"
  "\n     vec4 pixel = texture2D(tex, uv);"
  "\n     "
  "\n   pixel -= texture2D(tex, uv + (ratio / 1000.0));"
  "\n   pixel += texture2D(tex, uv - (ratio / 1000.0));"
  "\n     "
  "\n     return mix(base, pixel, intensity);"
  "\n }"
  "\n "
  "\n void main() {"
  "\n     uv = gl_TexCoord[0].xy;"
  "\n     "
  "\n     vec4 pass;"
  "\n     "
  "\n     // Motion blur is always the first pass"
  "\n     if (MotionBlurEnabled)"
  "\n         pass = MotionBlur(MotionBlurOffsetX, MotionBlurOffsetY, MotionBlurIntensity);"
  "\n     else"
  "\n         pass = texture2D(tex, uv); // Otherwise keep the base texture"
  "\n     "
  "\n     if (SharpenEnabled)"
  "\n         pass = Sharpen(pass, SharpenRatio, SharpenIntensity);"
  "\n     "
  "\n     if (AdjustEnabled)"
  "\n         pass = Adjust(pass, AdjustBrightness, AdjustSaturation, AdjustContrast);"
  "\n     "
  "\n     if (NoiseEnabled)"
  "\n         pass = Noise(pass, NoiseRand, NoiseIntensity);"
  "\n     "
  "\n     if (SepiaEnabled)"
  "\n         pass = Sepia(pass, SepiaIntensity);"
  "\n     "
  "\n     gl_FragColor = pass;"
  "\n }";
