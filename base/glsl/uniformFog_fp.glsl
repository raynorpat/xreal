/*
===========================================================================
Copyright (C) 2007 Robert Beckebans <trebor_7@users.sourceforge.net>

This file is part of XreaL source code.

XreaL source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

XreaL source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with XreaL source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

uniform sampler2D	u_CurrentMap;
uniform sampler2D	u_PositionMap;
uniform vec3		u_ViewOrigin;
uniform float		u_FogDensity;
uniform vec3		u_FogColor;
uniform mat4		u_UnprojectMatrix;

//varying vec3		var_Vertex;

void	main()
{
	// calculate the screen texcoord in the 0.0 to 1.0 range
	vec2 st = gl_FragCoord.st * r_FBufScale;

#if defined(ATI_flippedImageFix)
	// BUGFIX: the ATI driver flips the image
	st.t = 1.0 - st.t;
#endif
	
	// scale by the screen non-power-of-two-adjust
	st *= r_NPOTScale;
	
#if 0 //defined(r_deferredShading)

#if defined(GL_EXTX_framebuffer_mixed_formats)
	// compute vertex position in world space
	vec4 P = texture2D(u_PositionMap, st).xyzw;
#else
	// reconstruct vertex position in world space
#if 0
	// gl_FragCoord.z with 32 bit precision
	const vec4 bitShifts = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
	float depth = dot(texture2D(u_PositionMap, st), bitShifts);
#else
	// gl_FragCoord.z with 24 bit precision
	const vec3 bitShifts = vec3(1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
	float depth = dot(texture2D(u_PositionMap, st).rgb, bitShifts);
	
	vec4 P = u_UnprojectMatrix * vec4(gl_FragCoord.xy, depth, 1.0);
	P.xyz /= P.w;
#endif
#endif

#else // no r_deferredShading
	float depth = texture2D(u_PositionMap, st).r;
	
	vec4 P = u_UnprojectMatrix * vec4(gl_FragCoord.xy, depth, 1.0);
	P.xyz /= P.w;
#endif

	// calculate fog distance
	float fogDistance = distance(P.xyz, u_ViewOrigin);
	
	// calculate fog exponent
	float fogExponent = fogDistance * u_FogDensity;
	
	// calculate fog factor
	float fogFactor = exp2(-abs(fogExponent));

	// get current color
	vec3 currentColor = texture2D(u_CurrentMap, st).rgb;
	
	// compute final color, lerp between fog color and current color by fog factor
	vec4 color;
	color.r = (1.0 - fogFactor) * u_FogColor.r + currentColor.r * fogFactor;
	color.g = (1.0 - fogFactor) * u_FogColor.g + currentColor.g * fogFactor;
	color.b = (1.0 - fogFactor) * u_FogColor.b + currentColor.b * fogFactor;
	color.a = 1.0;
	
	gl_FragColor = color;
}
