/*
===========================================================================
Copyright (C) 2006 Robert Beckebans <trebor_7@users.sourceforge.net>

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
uniform sampler2D	u_NormalMap;
uniform sampler2D	u_SpecularMap;
uniform float		u_AlphaTest;
uniform vec3		u_ViewOrigin;
uniform vec3		u_AmbientColor;
uniform vec3		u_LightDir;
uniform vec3		u_LightColor;
uniform float		u_SpecularExponent;

varying vec3		var_Vertex;
varying vec2		var_TexDiffuse;
varying vec2		var_TexNormal;
varying vec2		var_TexSpecular;
varying vec3		var_Tangent;
varying vec3		var_Binormal;
varying vec3		var_Normal;


void	main()
{
	// compute the diffuse term
	vec4 diffuse = texture2D(u_DiffuseMap, var_TexDiffuse);
	if(diffuse.a <= u_AlphaTest)
	{
		discard;
		return;
	}

	// construct object-space-to-tangent-space 3x3 matrix
	mat3 OS2TSMatrix;
	if(gl_FrontFacing)
	{
		OS2TSMatrix = mat3( -var_Tangent.x, -var_Binormal.x, -var_Normal.x,
							-var_Tangent.y, -var_Binormal.y, -var_Normal.y,
							-var_Tangent.z, -var_Binormal.z, -var_Normal.z	);
	}
	else
	{
		OS2TSMatrix = mat3(	var_Tangent.x, var_Binormal.x, var_Normal.x,
							var_Tangent.y, var_Binormal.y, var_Normal.y,
							var_Tangent.z, var_Binormal.z, var_Normal.z	);
	}

	// compute view direction in tangent space
	vec3 V = normalize(OS2TSMatrix * (u_ViewOrigin - var_Vertex));

	// compute light direction in tangent space
	vec3 L = normalize(OS2TSMatrix * u_LightDir);
	
	// compute half angle in tangent space
	vec3 H = normalize(L + V);
	
	// compute normal in tangent space from normalmap
	vec3 N = 2.0 * (texture2D(u_NormalMap, var_TexNormal).xyz - 0.5);
	#if defined(r_NormalScale)
	N.z *= r_NormalScale;
	normalize(N);
	#endif
	
	// compute the specular term
	vec3 specular = texture2D(u_SpecularMap, var_TexSpecular).rgb * u_LightColor * pow(clamp(dot(N, H), 0.0, 1.0), r_SpecularExponent) * r_SpecularScale;
	
	// compute the light term
	vec3 light = u_AmbientColor + u_LightColor * clamp(dot(N, L), 0.0, 1.0);
	clamp(light, 0.0, 1.0);
	
	// compute final color
	vec4 color = diffuse;
	color.rgb *= light;
	color.rgb += specular;
	
	gl_FragColor = color;
}
