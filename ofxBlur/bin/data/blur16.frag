// float hoffsets[] = {1.41176, 0, 3.29412, 0, 5.17647, 0, 7.05882, 0};
// float voffsets[] = {0, 1.41176, 0, 3.29412, 0, 5.17647, 0, 7.05882};

uniform sampler2DRect source;
uniform vec2 offsets[4];

void main(void) {
	vec2 tc = gl_TexCoord[0].st;
	gl_FragColor = 0.196381 * texture2DRect(source, tc);
	gl_FragColor += 0.296753 *
		(texture2DRect(source, tc - offsets[0]) +
		texture2DRect(source, tc + offsets[0]));
	gl_FragColor += 0.0944214 *
		(texture2DRect(source, tc - offsets[1]) +
		texture2DRect(source, tc + offsets[1]));
	gl_FragColor += 0.010376 *
		(texture2DRect(source, tc - offsets[2]) +
		texture2DRect(source, tc + offsets[2]));
	gl_FragColor += 0.000259399 *
		(texture2DRect(source, tc - offsets[3]) +
		texture2DRect(source, tc + offsets[3]));
}