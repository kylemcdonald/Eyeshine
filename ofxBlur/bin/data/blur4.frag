// float hoffsets[] = {1.2, 0};
// float voffsets[] = {0, 1.2};

uniform sampler2DRect source;
uniform vec2 offsets[1];

void main(void) {
	vec2 tc = gl_TexCoord[0].st;
	gl_FragColor = 0.375 * texture2DRect(source, tc);
	gl_FragColor += 0.3125 *
		(texture2DRect(source, tc - offsets[0]) +
		texture2DRect(source, tc + offsets[0]));
}