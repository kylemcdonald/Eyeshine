uniform sampler2DRect source;
uniform vec2 offsets[5];

void main(void) {
	vec4 c = vec4(0, 0, 0, 0);
	vec2 tc = gl_TexCoord[0].st;
	
	c += texture2DRect(source, tc + offsets[0]) * 1./16.;
	c += texture2DRect(source, tc + offsets[1]) * 4./16.;
	c += texture2DRect(source, tc + offsets[2]) * 6./16.;
	c += texture2DRect(source, tc + offsets[3]) * 4./16.;
	c += texture2DRect(source, tc + offsets[4]) * 1./16.;
	
	gl_FragColor = c;
}