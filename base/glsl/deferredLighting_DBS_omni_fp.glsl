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
uniform sampler2D	u_NormalMap;
uniform sampler2D	u_SpecularMap;
uniform sampler2D	u_PositionMap;
uniform sampler2D	u_AttenuationMapXY;
uniform sampler2D	u_AttenuationMapZ;
uniform samplerCube	u_ShadowMap;
uniform vec3		u_ViewOrigin;
uniform vec3		u_LightOrigin;
uniform vec3		u_LightColor;
uniform float		u_LightRadius;
uniform float       u_LightScale;
uniform mat4		u_LightAttenuationMatrix;
#if !defined(ATI)
uniform vec4		u_LightFrustum[6];
#endif
uniform int			u_ShadowCompare;
uniform mat4		u_UnprojectMatrix;

void	main()
{
	// calculate the screen texcoord in the 0.0 to 1.0 range
	vec2 st = gl_FragCoord.st * r_FBufScale;
	
	// scale by the screen non-power-of-two-adjust
	st *= r_NPOTScale;
	
#if defined(ATI_flippedImageFix)
	// BUGFIX: the ATI driver flips the image
	st.t = 1.0 - st.t;
#endif
	
#if 0 //defined(GL_EXTX_framebuffer_mixed_formats)
	// compute vertex position in world space
	vec4 P = texture2D(u_PositionMap, st).xyzw;
#else
	// reconstruct vertex position in world space
#if 0
	// gl_FragCoord.z with 32 bit precision
	const vec4 bitShifts = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
	float depth = dot(texture2D(u_PositionMap, st), bitShifts);
#elif 0
	// gl_FragCoord.z with 24 bit precision
	const vec3 bitShifts = vec3(1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
	float depth = dot(texture2D(u_PositionMap, st).rgb, bitShifts);
#else
	float depth = texture2D(u_PositionMap, st).r;
#endif
	
	vec4 P = u_UnprojectMatrix * vec4(gl_FragCoord.xy, depth, 1.0);
	P.xyz /= P.w;
#endif
	
#if 0
	if(distance(P.xyz, u_LightOrigin) > u_LightRadius)
	{
		// position is outside of light volume
		discard;
		return;
	}
#endif

#if !defined(ATI)
	// make sure that the vertex position is inside the light frustum
	for(int i = 0; i < 6; ++i)
	{
		vec4 plane = u_LightFrustum[i];

		float dist = dot(P.xyz, plane.xyz) - plane.w;
		if(dist < 0.0)
		{
			discard;
			return;
		}
	}
#endif

	float shadow = 1.0;

#if defined(VSM)
	if(bool(u_ShadowCompare))
	{
		// compute incident ray
		vec3 I = P.xyz - u_LightOrigin;
	
		vec4 shadowMoments = textureCube(u_ShadowMap, I);
		
		#if defined(VSM_CLAMP)
		// convert to [-1, 1] vector space
		shadowMoments = 2.0 * (shadowMoments - 0.5);
		#endif
	
		float shadowDistance = shadowMoments.r;
		float shadowDistanceSquared = shadowMoments.a;
		
		const float	SHADOW_BIAS = 0.001;
		float vertexDistance = length(I) / u_LightRadius - SHADOW_BIAS;
	
		// standard shadow map comparison
		shadow = vertexDistance <= shadowDistance ? 1.0 : 0.0;
	
		// variance shadow mapping
		float E_x2 = shadowDistanceSquared;
		float Ex_2 = shadowDistance * shadowDistance;
	
		// AndyTX: VSM_EPSILON is there to avoid some ugly numeric instability with fp16
		float variance = min(max(E_x2 - Ex_2, 0.0) + VSM_EPSILON, 1.0);
	
		float mD = shadowDistance - vertexDistance;
		float mD_2 = mD * mD;
		float p = variance / (variance + mD_2);
	
		#if defined(DEBUG_VSM)
		gl_FragColor.r = DEBUG_VSM & 1 ? variance : 0.0;
		gl_FragColor.g = DEBUG_VSM & 2 ? mD_2 : 0.0;
		gl_FragColor.b = DEBUG_VSM & 4 ? p : 0.0;
		gl_FragColor.a = 1.0;
		return;
		#else
		shadow = max(shadow, p);
		#endif
	}
	
	if(shadow <= 0.0)
	{
		discard;
	}
	else
#elif defined(ESM)
	if(bool(u_ShadowCompare))
	{
		// compute incident ray
		vec3 I = P.xyz - u_LightOrigin;
	
		vec4 shadowMoments = textureCube(u_ShadowMap, I);
		
		const float	SHADOW_BIAS = 0.001;
		float vertexDistance = (length(I) / u_LightRadius) * r_ShadowMapDepthScale;// - SHADOW_BIAS;
		
		float shadowDistance = shadowMoments.a;
		
		// exponential shadow mapping
		shadow = clamp(exp(r_OverDarkeningFactor * (shadowDistance - vertexDistance)), 0.0, 1.0);
		
		#if defined(DEBUG_ESM)
		gl_FragColor.r = DEBUG_ESM & 1 ? shadowDistance : 0.0;
		gl_FragColor.g = DEBUG_ESM & 2 ? -(shadowDistance - vertexDistance) : 0.0;
		gl_FragColor.b = DEBUG_ESM & 4 ? 1.0 - shadow : 0.0;
		gl_FragColor.a = 1.0;
		return;
		#endif
	}
	
	if(shadow <= 0.0)
	{
		discard;
	}
	else
#endif
	{
		// compute the diffuse term
		vec4 diffuse = texture2D(u_DiffuseMap, st);
		
		// compute the specular term
		vec4 S = texture2D(u_SpecularMap, st);
	
		// compute normal in world space
		vec3 N = 2.0 * (texture2D(u_NormalMap, st).xyz - 0.5);
		
		//vec3 N;
		//N.x = diffuse.a;
		//N.y = S.a;
		//N.z = P.w;
		//N = 2.0 * (N - 0.5);
		//N.z = sqrt(1.0 - dot(N.xy, N.xy));
		//N.z = sqrt(1.0 - N.x*N.x - N.y*N.y);
		//N.x = sqrt(1.0 - N.y*N.y - N.z*N.z);
		//N = normalize(N);
	
		// compute light direction in world space
		vec3 L = normalize(u_LightOrigin - P.xyz);
	
		// compute view direction in world space
		vec3 V = normalize(u_ViewOrigin - P.xyz);
	
		// compute half angle in world space
		vec3 H = normalize(L + V);
	
		// compute attenuation
		vec3 texAttenXYZ		= (u_LightAttenuationMatrix * vec4(P.xyz, 1.0)).xyz;
		vec3 attenuationXY		= texture2D(u_AttenuationMapXY, texAttenXYZ.xy).rgb;
		vec3 attenuationZ		= texture2D(u_AttenuationMapZ, vec2(texAttenXYZ.z, 0)).rgb;
	
		// compute final color
		vec4 color = diffuse;
		color.rgb *= u_LightColor * clamp(dot(N, L), 0.0, 1.0);
		color.rgb += S.rgb * u_LightColor * pow(clamp(dot(N, H), 0.0, 1.0), r_SpecularExponent) * r_SpecularScale;
		color.rgb *= attenuationXY;
		color.rgb *= attenuationZ;
		color.rgb *= u_LightScale;
		color.rgb *= shadow;
		
		gl_FragColor = color;
	}
}
