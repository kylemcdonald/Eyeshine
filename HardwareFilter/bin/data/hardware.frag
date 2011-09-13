uniform sampler2DRect source;
uniform vec2 offset;

void main(void) {
	vec4 c = vec4(0, 0, 0, 0);
	vec2 tc = gl_TexCoord[0].st;
	
	c += texture2DRect(source, tc - offset) * 5./16.; // 1, 4
	c += texture2DRect(source, tc) * 6./16.; // 6
	c += texture2DRect(source, tc + offset) * 5./16.; // 1, 4
	
	gl_FragColor = c;
}