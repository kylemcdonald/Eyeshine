uniform sampler2DRect source;
uniform float coefficients[25];

void main(void) {
	// 2 = int(5 / 2)
	float left = gl_TexCoord[0].s - 2.;
	float top = gl_TexCoord[0].t - 2.;
	vec2 tc = vec2(left, top);
	vec4 c = vec4(0, 0, 0, 0);
	
	c += coefficients[ 0] * texture2DRect(source, tc); tc.x++;
	c += coefficients[ 1] * texture2DRect(source, tc); tc.x++;
	c += coefficients[ 2] * texture2DRect(source, tc); tc.x++;
	c += coefficients[ 3] * texture2DRect(source, tc); tc.x++;
	c += coefficients[ 4] * texture2DRect(source, tc); tc.y++;
	tc.x = left;
	c += coefficients[ 5] * texture2DRect(source, tc); tc.x++;
	c += coefficients[ 6] * texture2DRect(source, tc); tc.x++;
	c += coefficients[ 7] * texture2DRect(source, tc); tc.x++;
	c += coefficients[ 8] * texture2DRect(source, tc); tc.x++;
	c += coefficients[ 9] * texture2DRect(source, tc); tc.y++;
	tc.x = left;
	c += coefficients[10] * texture2DRect(source, tc); tc.x++;
	c += coefficients[11] * texture2DRect(source, tc); tc.x++;
	c += coefficients[12] * texture2DRect(source, tc); tc.x++;
	c += coefficients[13] * texture2DRect(source, tc); tc.x++;
	c += coefficients[14] * texture2DRect(source, tc); tc.y++;
	tc.x = left;
	c += coefficients[15] * texture2DRect(source, tc); tc.x++;
	c += coefficients[16] * texture2DRect(source, tc); tc.x++;
	c += coefficients[17] * texture2DRect(source, tc); tc.x++;
	c += coefficients[18] * texture2DRect(source, tc); tc.x++;
	c += coefficients[19] * texture2DRect(source, tc); tc.y++;
	tc.x = left;
	c += coefficients[20] * texture2DRect(source, tc); tc.x++;
	c += coefficients[21] * texture2DRect(source, tc); tc.x++;
	c += coefficients[22] * texture2DRect(source, tc); tc.x++;
	c += coefficients[23] * texture2DRect(source, tc); tc.x++;
	c += coefficients[24] * texture2DRect(source, tc);
	
	gl_FragColor = c;
}
