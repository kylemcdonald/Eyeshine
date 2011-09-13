// float hoffsets[] = {1.38462, 0, 3.23077, 0, 5.07692, 0};
// float voffsets[] = {0, 1.38462, 0, 3.23077, 0, 5.07692};

uniform sampler2DRect source;
uniform vec2 offsets[3];

void main(void) {
	vec2 tc = gl_TexCoord[0].st;
	gl_FragColor = 0.225586 * texture2DRect(source, tc);
	gl_FragColor += 0.314209 *
		(texture2DRect(source, tc - offsets[0]) +
		texture2DRect(source, tc + offsets[0]));
	gl_FragColor += 0.0698242 *
		(texture2DRect(source, tc - offsets[1]) +
		texture2DRect(source, tc + offsets[1]));
	gl_FragColor += 0.00317383 *
		(texture2DRect(source, tc - offsets[2]) +
		texture2DRect(source, tc + offsets[2]));
}