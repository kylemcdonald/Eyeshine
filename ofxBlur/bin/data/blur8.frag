// float hoffsets[] = {1.33333, 0, 3.11111, 0};
// float voffsets[] = {0, 1.33333, 0, 3.11111};

uniform sampler2DRect source;
uniform vec2 offsets[2];

void main(void) {
	vec2 tc = gl_TexCoord[0].st;
	gl_FragColor = 0.273438 * texture2DRect(source, tc);
	gl_FragColor += 0.328125 *
		(texture2DRect(source, tc - offsets[0]) +
		texture2DRect(source, tc + offsets[0]));
	gl_FragColor += 0.0351562 *
		(texture2DRect(source, tc - offsets[1]) +
		texture2DRect(source, tc + offsets[1]));
}