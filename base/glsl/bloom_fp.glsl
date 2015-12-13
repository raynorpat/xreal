/*
===========================================================================
Copyright (C) 2006-2008 Robert Beckebans <trebor_7@users.sourceforge.net>

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

uniform sampler2D	u_ColorMap;
uniform sampler2D	u_ContrastMap;
uniform float		u_BlurMagnitude;



#if defined(ATI)

/*
 Fragment shader was successfully compiled to run on hardware.
 Not supported when use temporary array indirect index.
 Not supported when use temporary array indirect index.
 ERROR: Fragment shader(s) failed to link,  vertex shader(s) linked.
 Fragment Shader not supported by HW
 shaders failed to link
 */

void main()
{
	discard;
}
#else
void	main()
{
	vec2 st = gl_FragCoord.st;

	// calculate the screen texcoord in the 0.0 to 1.0 range
	st *= r_FBufScale;

#if defined(ATI_flippedImageFix)
	// BUGFIX: the ATI driver flips the image
	st.t = 1.0 - st.t;
#endif
	
	// scale by the screen non-power-of-two-adjust
	st *= r_NPOTScale;
	
	// we use the Normal-gauss distribution formula
	// f(x) being the formula, we used f(0.5)-f(-0.5); f(1.5)-f(0.5)...

	#if 0
	float gaussFact[3] = float[3](1.0, 2.0, 1.0);
	#elif 1
	float gaussFact[7] = float[7](1.0, 6.0, 15.0, 20.0, 15.0, 6.0, 1.0);
	float gaussSum = 4096.0; // = 64.0^2 = result of sumWeights;
	#else
	float gaussFact[11] = float[11](
	0.0222244, 0.0378346, 0.0755906, 0.1309775, 0.1756663,
	0.1974126,
	0.1756663, 0.1309775, 0.0755906, 0.0378346, 0.0222244
	);
	#endif

	// do a full gaussian blur
	vec4 sumColors = vec4(0.0);
	//float sumWeights = 0.0;

	int tap = 3;
	for(int i = -tap; i < tap; i++)
    {
	    for(int j = -tap; j < tap; j++)
	    {
			float weight = gaussFact[i + 3] * gaussFact[j + 3];
			vec4 color = texture2D(u_ContrastMap, st + vec2(i, j) * u_BlurMagnitude * r_FBufScale) * weight;
			
			sumColors += color;
			//sumWeights += weight;
		}
	}
	
	gl_FragColor = texture2D(u_ColorMap, st) + sumColors / gaussSum;
	//gl_FragColor = sumColors / gaussSum;
}
#endif

