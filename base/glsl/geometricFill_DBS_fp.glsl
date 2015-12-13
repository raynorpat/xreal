/*
===========================================================================
Copyright (C) 2007-2008 Robert Beckebans <trebor_7@users.sourceforge.net>

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

#extension GL_ARB_draw_buffers : enable

uniform sampler2D	u_DiffuseMap;
uniform sampler2D	u_NormalMap;
uniform sampler2D	u_SpecularMap;
uniform float		u_AlphaTest;
uniform vec3		u_ViewOrigin;
uniform vec3        u_AmbientColor;
uniform float		u_DepthScale;
uniform mat4		u_ModelMatrix;

varying vec3		var_Vertex;
varying vec2		var_TexDiffuse;
varying vec2		var_TexNormal;
varying vec2		var_TexSpecular;
varying vec4		var_Tangent;
varying vec4		var_Binormal;
varying vec4		var_Normal;


float RayIntersectDisplaceMap(vec2 dp, vec2 ds)
{
	const int linearSearchSteps = 16;
	const int binarySearchSteps = 6;

	float depthStep = 1.0 / float(linearSearchSteps);

	// current size of search window
	float size = depthStep;

	// current depth position
	float depth = 0.0;

	// best match found (starts with last position 1.0)
	float bestDepth = 1.0;

	// search front to back for first point inside object
	for(int i = 0; i < linearSearchSteps - 1; ++i)
	{
		depth += size;
		
		vec4 t = texture2D(u_NormalMap, dp + ds * depth);

		if(bestDepth > 0.996)		// if no depth found yet
			if(depth >= t.w)
				bestDepth = depth;	// store best depth
	}

	depth = bestDepth;
	
	// recurse around first point (depth) for closest match
	for(int i = 0; i < binarySearchSteps; ++i)
	{
		size *= 0.5;

		vec4 t = texture2D(u_NormalMap, dp + ds * depth);
		
		if(depth >= t.w)
		#ifdef RM_DOUBLEDEPTH
			if(depth <= t.z)
		#endif
			{
				bestDepth = depth;
				depth -= 2.0 * size;
			}

		depth += size;
	}

	return bestDepth;
}


void	main()
{
#if 0 //defined(PARALLAX)

	// construct tangent-world-space-to-tangent-space 3x3 matrix
#if defined(ATI)

	mat3 worldToTangentMatrix;
	/*
	for(int i = 0; i < 3; ++i)
	{
		for(int j = 0; j < 3; ++j)
			worldToTangentMatrix[i][j] = var_TangentToWorldMatrix[j][i];
	}
	*/
	
	worldToTangentMatrix = mat3(var_TangentToWorldMatrix[0][0], var_TangentToWorldMatrix[1][0], var_TangentToWorldMatrix[2][0],
								var_TangentToWorldMatrix[0][1], var_TangentToWorldMatrix[1][1], var_TangentToWorldMatrix[2][1], 
								var_TangentToWorldMatrix[0][2], var_TangentToWorldMatrix[1][2], var_TangentToWorldMatrix[2][2]);
#else
	mat3 worldToTangentMatrix = transpose(var_TangentToWorldMatrix);
#endif

	// compute view direction in tangent space
	vec3 V = worldToTangentMatrix * (u_ViewOrigin - var_Vertex.xyz);
	V = normalize(V);
	
	// ray intersect in view direction
	
	// size and start position of search in texture space
	vec2 S = V.xy * -u_DepthScale / V.z;
		
	float depth = RayIntersectDisplaceMap(var_TexNormal, S);
	
	// compute texcoords offset
	vec2 texOffset = S * depth;
	
	vec4 diffuse = texture2D(u_DiffuseMap, var_TexDiffuse + texOffset);
	vec3 specular = texture2D(u_SpecularMap, var_TexSpecular + texOffset).rgb;
	
	// compute normal in tangent space from normalmap
	vec3 N = 2.0 * (texture2D(u_NormalMap, var_TexNormal + texOffset).xyz - 0.5);
	//N.z = sqrt(1.0 - dot(N.xy, N.xy));
	#if defined(r_NormalScale)
	N.z *= r_NormalScale;
	normalize(N);
	#endif
	
	// transform normal into world space
	N = var_TangentToWorldMatrix * N;
	
	// convert normal back to [0,1] color space
	N = N * 0.5 + 0.5;
	
	// transform vertex position into world space
	//vec3 P = var_Vertex.xyz;

	// transform parallax offset world space
	//P += (u_ModelMatrix * vec4(texOffset, 0, 1)).xyz;

	gl_FragData[0] = vec4(diffuse.rgb, 0.0);
	gl_FragData[1] = vec4(N, 0.0);
	gl_FragData[2] = vec4(specular, 0.0);
	
#if defined(GL_EXTX_framebuffer_mixed_formats)
	gl_FragData[3] = var_Vertex;
#else
	// compute depth instead of world vertex position in a [0..1] range
	depth = gl_FragCoord.z;
	
#if 0
	// 32 bit precision
	const vec4 bitSh = vec4(256 * 256 * 256,	256 * 256,				256,         1);
	const vec4 bitMsk = vec4(			0,		1.0 / 256.0,    1.0 / 256.0,    1.0 / 256.0);
	
	vec4 comp;
	comp = depth * bitSh;
	comp = fract(comp);
	comp -= comp.xxyz * bitMsk;
	gl_FragData[3] = comp;
#else
	// 24 bit precision
	const vec3 bitSh = vec3(256 * 256,			256,		1);
	const vec3 bitMsk = vec3(		0,	1.0 / 256.0,		1.0 / 256.0);
	
	vec3 comp;
	comp = depth * bitSh;
	comp = fract(comp);
	comp -= comp.xxy * bitMsk;
	gl_FragData[3] = vec4(comp, 0.0);
#endif // precision
#endif // 


#else // defined(PARALLAX)
	vec4 diffuse = texture2D(u_DiffuseMap, var_TexDiffuse);
	
	if(diffuse.a <= u_AlphaTest)
	{
		discard;
		return;
	}
	
	vec4 depthColor = diffuse;
	depthColor.rgb *= u_AmbientColor;
	
	vec3 specular = texture2D(u_SpecularMap, var_TexSpecular).rgb;

	// compute normal in tangent space from normalmap
	vec3 N = 2.0 * (texture2D(u_NormalMap, var_TexNormal).xyz - 0.5);
	#if defined(r_NormalScale)
	N.z *= r_NormalScale;
	normalize(N);
	#endif
	
	// invert tangent space for twosided surfaces
	mat3 tangentToWorldMatrix;
	if(gl_FrontFacing)
		tangentToWorldMatrix = mat3(-var_Tangent.xyz, -var_Binormal.xyz, -var_Normal.xyz);
	else
		tangentToWorldMatrix = mat3(var_Tangent.xyz, var_Binormal.xyz, var_Normal.xyz);
	
	// transform normal into world space
	N = tangentToWorldMatrix * N;
	
	//N = normalize(N);
	
	// convert normal back to [0,1] color space
	N = N * 0.5 + 0.5;
	
	gl_FragData[0] = vec4(diffuse.rgb, 0.0);
	gl_FragData[1] = vec4(N, 0.0);
	gl_FragData[2] = vec4(specular, 0.0);
	
/*
#if defined(GL_EXTX_framebuffer_mixed_formats)
	// transform vertex position into world space
	gl_FragData[3] = (u_ModelMatrix * var_Vertex).xyzw;
#else
	// compute depth instead of world vertex position in a [0..1] range
	float depth = gl_FragCoord.z;
	
#if 0
	// 32 bit precision
	const vec4 bitSh = vec4(256 * 256 * 256,	256 * 256,				256,         1);
	const vec4 bitMsk = vec4(			0,		1.0 / 256.0,    1.0 / 256.0,    1.0 / 256.0);
	
	vec4 comp;
	comp = depth * bitSh;
	comp = fract(comp);
	comp -= comp.xxyz * bitMsk;
	gl_FragData[3] = comp;
#elif 0
	// 24 bit precision
	const vec3 bitSh = vec3(256 * 256,			256,		1);
	const vec3 bitMsk = vec3(		0,	1.0 / 256.0,		1.0 / 256.0);
	
	vec3 comp;
	comp = depth * bitSh;
	comp = fract(comp);
	comp -= comp.xxy * bitMsk;
	gl_FragData[3] = vec4(comp, 0.0);
#else
	// DO NOTHING
#endif // precision
#endif // 
*/

#endif
}


