/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGCommonGL.h"
#include "DGConfig.h"
#include "DGEffects.h"
#include "DGLog.h"
#include "DGMemory.h"


char fs[] = 
"uniform sampler2D tex;\n"
"uniform float gamma;\n"
"uniform float offsetx;\n"
"uniform float offsety;\n"
"\n"
"void main()\n"
"{\n"
"   vec4 pixel = texture2D(tex,gl_TexCoord[0].st);\n"
"	\n"
"	// Sharpen\n"
"	pixel -= texture2D(tex,gl_TexCoord[0].st + 0.0001);\n"
"	pixel += texture2D(tex,gl_TexCoord[0].st - 0.0001);\n"
"    \n"
"	// Motion Blur\n"
"	vec2 offx = vec2(offsetx, 0.0);\n"
"	vec2 offy = vec2(0.0, offsety);\n"
"	\n"
"	vec4 c0 = texture2D(tex,gl_TexCoord[0].st);\n"
"	vec4 c1 = texture2D(tex,gl_TexCoord[0].st - offy);\n"
"	vec4 c2 = texture2D(tex,gl_TexCoord[0].st + offy);\n"
"	vec4 c3 = texture2D(tex,gl_TexCoord[0].st - offx);\n"
"	vec4 c4 = texture2D(tex,gl_TexCoord[0].st + offx);\n"
"	vec4 motion = c0 * 0.2 + c1 * 0.2 + c2 * 0.2 + c3 * 0.2 + c4 * 0.2;\n"
"	\n"
"	// Noise\n"
"	float fract = fract(sin(dot(gl_TexCoord[0].xy, vec2(12.9898,78.233))) * 43758.5453);\n"
"	vec4 noise = vec4(fract * vec4(1.0, 0.8, 0.8, 1.0)) * 0.085 + motion * 0.925; // 0.085 is the intensity\n"
"	\n"
"	// Contrast\n"
"	float gray = noise.r * 0.39 + noise.g * 0.50 + noise.b * 0.11;\n"
"   vec4 contrast = ((gray * 0.1 + noise * 0.9) * gamma) - vec4(0.15, 0.15, 0.15, 1.0);\n" // Has further brightness 2.5
"  	\n"
"  	gl_FragColor = mix(contrast, pixel, 0.5);\n"
"}\n";

GLuint f;
GLuint p;

char *textFileRead(char *fn);

static DG_BOOL active = DG_NO;

void dg_effects_init() {
#ifndef DG_PLATFORM_IOS
	const char * ff = fs;
	//const char * ff = fs_file;

	dg_log_trace(DG_MOD_EFFECTS, "Initializing effects manager...");
	
	f = glCreateShader(GL_FRAGMENT_SHADER);	
	
	//char* fs_file;
	//fs_file = textFileRead("test.frag");
	
	glShaderSource(f, 1, &ff, NULL);
	
	//dg_free(fs_file);
	
	glCompileShader(f);
	
	p = glCreateProgram();
	
	glAttachShader(p, f);
	
	glLinkProgram(p);

	if (config.effects) {
		glUseProgram(p);
		
		active = DG_YES;
	}
#endif
}

void dg_effects_pause() {
#ifndef DG_PLATFORM_IOS  
	if (active) {
		glUseProgram(0);
		active = DG_NO;
	}
#endif    
}

void dg_effects_play() {
#ifndef DG_PLATFORM_IOS    
	if (config.effects && !active) {
		glUseProgram(p);
		active = DG_YES;
	}
#endif    
}

void dg_effects_release() {
#ifndef DG_PLATFORM_IOS    
	if (config.effects && active) {
		glUseProgram(0);
	
		glDetachShader(p, f);
		glDeleteShader(f);
		glDeleteProgram(p);
		active = DG_NO;
	}
#endif    
}

void dg_effects_set_blur(int offsetx, int offsety) {
#ifndef DG_PLATFORM_IOS    
	if (config.effects && active) {
		GLint ox;
		GLint oy;
        
        GLfloat ox_aux;
        GLfloat oy_aux;
        
        if (config.scare) {
            ox_aux = (GLfloat)abs(offsetx) / (GLfloat)config.display_width / 5.0f; // Last value is intensity...
            oy_aux = (GLfloat)abs(offsety) / (GLfloat)config.display_height / 10.0f; // Last value is intensity...
        }
        else {
            ox_aux = (GLfloat)abs(offsetx) / (GLfloat)config.display_width / 20.0f; 
            oy_aux = (GLfloat)abs(offsety) / (GLfloat)config.display_height / 40.0f;
        }
		
		ox = glGetUniformLocation(p, "offsetx");
		glUniform1f(ox, ox_aux);
		oy = glGetUniformLocation(p, "offsety");
		glUniform1f(oy, oy_aux);
	}
#endif    
}

void dg_effects_set_gamma(float level) {
#ifndef DG_PLATFORM_IOS    
	if (config.effects && active) {
		GLint ol;
		
		ol = glGetUniformLocation(p, "gamma");
		glUniform1f(ol, level);
	}
#endif
}

// Modified from Lighthouse 3D
char *textFileRead(char *fn) {
	FILE *fp;
	char *content = NULL;
	
	int count=0;
	
	if (fn != NULL) {
		fp = fopen(fn,"rt");
		
		if (fp != NULL) {
			
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			
			if (count > 0) {
				content = (char *)dg_alloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}
