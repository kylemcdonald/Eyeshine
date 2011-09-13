uniform sampler2DRect s0,s1,s2,s3,s4,s5;
const float scaleFactor = 2./3.;

void main(void) {
	vec2 tc = gl_TexCoord[0].st;
	gl_FragColor = texture2DRect(s0,tc);
	tc *= scaleFactor;
	gl_FragColor += texture2DRect(s1,tc);
	tc *= scaleFactor;
	gl_FragColor += texture2DRect(s2,tc);
	tc *= scaleFactor;
	gl_FragColor += texture2DRect(s3,tc);
	tc *= scaleFactor;
	gl_FragColor += texture2DRect(s4,tc);
	tc *= scaleFactor;
	gl_FragColor += texture2DRect(s5,tc);
	gl_FragColor /= 6.;
}