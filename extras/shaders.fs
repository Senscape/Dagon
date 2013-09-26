// Base texture and coordinates shared by all functions

uniform sampler2D tex;
vec2 uv;

// Adjust parameters

uniform bool AdjustEnabled;
uniform float AdjustBrightness;
uniform float AdjustSaturation;
uniform float AdjustContrast;

// Adjust function (brightness, saturation, contrast)
// (adapted from TGM's shader pack)

vec4 Adjust(vec4 base, float brt, float sat, float con) {
	// Increase or decrease theese values to adjust r, g and b color channels seperately
	const float AvgLumR = 0.5;
	const float AvgLumG = 0.5;
	const float AvgLumB = 0.5;
	
	const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);
    
    vec3 pixel = vec3(base.x, base.y, base.z);
	
	vec3 AvgLumin = vec3(AvgLumR, AvgLumG, AvgLumB);
	vec3 brtColor = pixel * brt;
	vec3 intensity = vec3(dot(brtColor, LumCoeff));
	vec3 satColor = mix(intensity, brtColor, sat);
	vec3 conColor = mix(AvgLumin, satColor, con);
    
	return vec4(conColor, 1.0);
}

// Motion Blur parameters

uniform bool MotionBlurEnabled;
uniform float MotionBlurIntensity;
uniform float MotionBlurOffsetX;
uniform float MotionBlurOffsetY;

// Motion Blur function

vec4 MotionBlur(float offsetx, float offsety, float intensity) {
	vec2 offsetx_aux = vec2(offsetx / intensity, 0.0);
	vec2 offsety_aux = vec2(0.0, offsety / intensity);
    
	vec4 c0 = texture2D(tex, uv);
	vec4 c1 = texture2D(tex, uv - offsety_aux);
	vec4 c2 = texture2D(tex, uv + offsety_aux);
	vec4 c3 = texture2D(tex, uv - offsetx_aux);
	vec4 c4 = texture2D(tex, uv + offsetx_aux);
    
	vec4 motion = c0 * 0.2 + c1 * 0.2 + c2 * 0.2 + c3 * 0.2 + c4 * 0.2;
    
    return motion;
}

// Noise parameters

uniform bool NoiseEnabled;
uniform float NoiseIntensity;
uniform float NoiseRand;

// Noise function

vec4 Noise(vec4 base, float rand, float intensity) {
    float fract = fract(sin(dot(uv, vec2(12.9898 + rand, 78.233 + rand))) * 43758.5453);
	vec3 noise = vec3(fract * vec3(1.0, 1.0, 1.0));
    
    return mix(base, vec4(noise, 1.0), intensity);
}

// Sepia parameters

uniform bool SepiaEnabled;
uniform float SepiaIntensity;

// Sepia function

vec4 Sepia(vec4 base, float intensity) {
    vec3 pixel = vec3(base.x, base.y, base.z);
    vec3 color;
    
    color.x = pixel.x * 0.3588 + pixel.y * 0.7044 + pixel.z * 0.1368;
    color.y = pixel.x * 0.2990 + pixel.y * 0.5870 + pixel.z * 0.1140;
    color.z = pixel.x * 0.2392 + pixel.y * 0.4696 + pixel.z * 0.0912;
    
    vec3 blend = (intensity * color) + ((1.0 - intensity) * pixel);
    
	return vec4(blend, 1.0);
}

// Sharpen parameters

uniform bool SharpenEnabled;
uniform float SharpenIntensity;
uniform float SharpenRatio;

// Sharpen function

vec4 Sharpen(vec4 base, float ratio, float intensity) {
    vec4 pixel = texture2D(tex, uv);
    
	pixel -= texture2D(tex, uv + (ratio / 1000.0));
	pixel += texture2D(tex, uv - (ratio / 1000.0));
    
    return mix(base, pixel, intensity);
}

void main() {
    uv = gl_TexCoord[0].xy;
    
    vec4 pass;
    
    // Motion blur is always the first pass
    if (MotionBlurEnabled)
        pass = MotionBlur(MotionBlurOffsetX, MotionBlurOffsetY, MotionBlurIntensity);
    else
        pass = texture2D(tex, uv); // Otherwise keep the base texture
    
    if (SharpenEnabled)
        pass = Sharpen(pass, SharpenRatio, SharpenIntensity);
    
    if (AdjustEnabled)
        pass = Adjust(pass, AdjustBrightness, AdjustSaturation, AdjustContrast);
    
    if (NoiseEnabled)
        pass = Noise(pass, NoiseRand, NoiseIntensity);

    if (SepiaEnabled)
        pass = Sepia(pass, SepiaIntensity);

    gl_FragColor = pass;
}
