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
uniform sampler2D	u_PortalMap;
uniform vec3		u_ViewOrigin;
uniform float		u_RefractionIndex;
uniform float		u_FresnelPower;
uniform float		u_FresnelScale;
uniform float		u_FresnelBias;
uniform mat4		u_ModelMatrix;

varying vec3		var_Vertex;
varying vec3		var_Normal;
varying vec4		var_Color;

void	main()
{
	vec4 color;

	// compute incident ray
	vec3 I = normalize(u_ViewOrigin - var_Vertex);
	
	// compute normal
	vec3 N = normalize(var_Normal);
			
	// compute fresnel term
	float fresnel = clamp(u_FresnelBias + pow(1.0 - dot(I, N), u_FresnelPower) * u_FresnelScale, 0.0, 1.0);
	
	// calculate the screen texcoord in the 0.0 to 1.0 range
	vec2 st = gl_FragCoord.st * r_FBufScale;
	
	// scale by the screen non-power-of-two-adjust
	st *= r_NPOTScale;
	
	color = texture2D(u_CurrentMap, st);
	
	vec3 refractColor = texture2D(u_CurrentMap, st).rgb;
	vec3 reflectColor = texture2D(u_PortalMap, st).rgb;

	// compute final color
	color.r = (1.0 - fresnel) * refractColor.r + reflectColor.r * fresnel;
	color.g = (1.0 - fresnel) * refractColor.g + reflectColor.g * fresnel;
	color.b = (1.0 - fresnel) * refractColor.b + reflectColor.b * fresnel;
	color.a = 1.0;
	
	color *= var_Color;
	gl_FragColor = color;
}
