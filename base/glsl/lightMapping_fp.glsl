/*
===========================================================================
Copyright (C) 2008 Robert Beckebans <trebor_7@users.sourceforge.net>

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

uniform sampler2D	u_DiffuseMap;
uniform sampler2D	u_LightMap;
uniform float		u_AlphaTest;

varying vec2		var_TexDiffuse;
varying vec2		var_TexLight;

void	main()
{
	// compute the diffuse term
	vec4 diffuse = texture2D(u_DiffuseMap, var_TexDiffuse);
	if(diffuse.a <= u_AlphaTest)
	{
		discard;
		return;
	}

#if defined(r_showLightMaps)
	gl_FragColor = texture2D(u_LightMap, var_TexLight);
#else
	// compute light color from object space lightmap
	vec4 lightColor = texture2D(u_LightMap, var_TexLight);
	
	
	diffuse.rgb *= lightColor.rgb;
	
	gl_FragColor = diffuse;
#endif
}
