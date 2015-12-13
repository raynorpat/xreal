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

uniform sampler2D	u_AttenuationMapXY;
uniform sampler2D	u_AttenuationMapZ;
uniform sampler2D	u_ShadowMap;
uniform vec3		u_LightOrigin;
uniform float		u_LightRadius;
uniform float       u_ShadowTexelSize;
uniform float       u_ShadowBlur;
uniform int         u_ShadowInverse;

varying vec4		var_Vertex;
varying vec4		var_TexAtten;
varying vec4		var_TexShadow;
varying vec4        var_Color;


#if defined(VSM) || defined(ESM)
vec4 PCF(vec4 SP, float filterWidth, float samples)
{
	// compute step size for iterating through the kernel
	float stepSize = 2.0 * filterWidth / samples;
	
	vec4 moments = vec4(0.0, 0.0, 0.0, 0.0);
	for(float i = -filterWidth; i < filterWidth; i += stepSize)
	{
		for(float j = -filterWidth; j < filterWidth; j += stepSize)
		{
			//moments += texture2DProj(u_ShadowMap, vec3(SP.xy + vec2(i, j), SP.w));
			moments += texture2D(u_ShadowMap, SP.xy / SP.w + vec2(i, j));
		}
	}
	
	// return average of the samples
	moments *= (1.0 / (samples * samples));
	return moments;
}
#endif


void	main()
{
	float shadow = 1.0;

	if(var_TexAtten.q <= 0.0)
	{
		discard;
	}

#if defined(VSM)
	// compute incident ray
	vec3 I = var_Vertex.xyz - u_LightOrigin;
		
	const float	SHADOW_BIAS = 0.001;
	float vertexDistance = length(I) / u_LightRadius - SHADOW_BIAS;
		
	#if defined(PCF_2X2)
	vec4 shadowMoments = PCF(var_TexShadow, u_ShadowTexelSize * u_ShadowBlur, 2.0);
	#elif defined(PCF_3X3)
	vec4 shadowMoments = PCF(var_TexShadow, u_ShadowTexelSize * u_ShadowBlur, 3.0);
	#elif defined(PCF_4X4)
	vec4 shadowMoments = PCF(var_TexShadow, u_ShadowTexelSize * u_ShadowBlur, 4.0);
	#elif defined(PCF_5X5)
	vec4 shadowMoments = PCF(var_TexShadow, u_ShadowTexelSize * u_ShadowBlur, 5.0);
	#elif defined(PCF_6X6)
	vec4 shadowMoments = PCF(var_TexShadow, u_ShadowTexelSize * u_ShadowBlur, 6.0);
	#else
	vec4 shadowMoments = texture2DProj(u_ShadowMap, var_TexShadow.xyw);
	#endif
	
	#if defined(VSM_CLAMP)
	// convert to [-1, 1] vector space
	shadowMoments = 2.0 * (shadowMoments - 0.5);
	#endif
		
	float shadowDistance = shadowMoments.r;
	float shadowDistanceSquared = shadowMoments.a;
	
	// standard shadow map comparison
	shadow = vertexDistance <= shadowDistance ? 1.0 : 0.0;
			
	// variance shadow mapping
	float E_x2 = shadowDistanceSquared;
	float Ex_2 = shadowDistance * shadowDistance;
	
	// AndyTX: VSM_EPSILON is there to avoid some ugly numeric instability with fp16
	float variance = min(max(E_x2 - Ex_2, 0.0) + VSM_EPSILON, 1.0);
	//float variance = smoothstep(VSM_EPSILON, 1.0, max(E_x2 - Ex_2, 0.0));
	
	float mD = shadowDistance - vertexDistance;
	float mD_2 = mD * mD;
	float p = variance / (variance + mD_2);
	p = smoothstep(0.0, 1.0, p);
		
	#if defined(DEBUG_VSM)
	gl_FragColor.r = DEBUG_VSM & 1 ? variance : 0.0;
	gl_FragColor.g = DEBUG_VSM & 2 ? mD_2 : 0.0;
	gl_FragColor.b = DEBUG_VSM & 4 ? p : 0.0;
	gl_FragColor.a = 1.0;
	return;
	#else
	shadow = max(shadow, p);
	#endif
		
	shadow = clamp(shadow, 0.0, 1.0);
	shadow = 1.0 - shadow;
		
	if(shadow <= 0.0)
	{
		discard;
	}
#elif defined(ESM)
	// compute incident ray
	vec3 I = var_Vertex.xyz - u_LightOrigin;
		
	const float	SHADOW_BIAS = 0.001;
	float vertexDistance = (length(I) / u_LightRadius) * r_ShadowMapDepthScale; // - SHADOW_BIAS;
		
	#if defined(PCF_2X2)
	vec4 shadowMoments = PCF(var_TexShadow, u_ShadowTexelSize * u_ShadowBlur, 2.0);
	#elif defined(PCF_3X3)
	vec4 shadowMoments = PCF(var_TexShadow, u_ShadowTexelSize * u_ShadowBlur, 3.0);
	#elif defined(PCF_4X4)
	vec4 shadowMoments = PCF(var_TexShadow, u_ShadowTexelSize * u_ShadowBlur, 4.0);
	#elif defined(PCF_5X5)
	vec4 shadowMoments = PCF(var_TexShadow, u_ShadowTexelSize * u_ShadowBlur, 5.0);
	#elif defined(PCF_6X6)
	vec4 shadowMoments = PCF(var_TexShadow, u_ShadowTexelSize * u_ShadowBlur, 6.0);
	#else
	// no filter
	vec4 shadowMoments = texture2DProj(u_ShadowMap, var_TexShadow.xyw);
	#endif
		
	float shadowDistance = shadowMoments.a;
	
	// exponential shadow mapping
	//shadow = vertexDistance <= shadowDistance ? 1.0 : 0.0;
	shadow = clamp(exp(r_OverDarkeningFactor * (shadowDistance - vertexDistance)), 0.0, 1.0);
	//shadow = smoothstep(0.0, 1.0, shadow);
		
	shadow = 1.0 - shadow;
		
	#if defined(DEBUG_ESM)
	gl_FragColor.r = DEBUG_ESM & 1 ? shadowDistance : 0.0;
	gl_FragColor.g = DEBUG_ESM & 2 ? -(shadowDistance - vertexDistance) : 0.0;
	gl_FragColor.b = DEBUG_ESM & 4 ? shadow : 0.0;
	gl_FragColor.a = 1.0;
	return;
	#endif
	
	if(shadow <= 0.0)
	{
		discard;
	}
#endif
	// compute attenuation
	vec3 attenuationXY = texture2DProj(u_AttenuationMapXY, var_TexAtten.xyw).rgb;
	vec3 attenuationZ  = texture2D(u_AttenuationMapZ, vec2(clamp(var_TexAtten.z, 0.0, 1.0), 0.0)).rgb;

	// compute final color
	vec4 color = var_Color; //vec4(0.6, 0.6, 0.6, 1.0);
	color.rgb *= attenuationXY;
	color.rgb *= attenuationZ;
	color.rgb *= shadow;
	
	gl_FragColor = color;
}
