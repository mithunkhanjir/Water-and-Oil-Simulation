varying out vec4 gradientColor;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    gl_FrontColor = gl_Color;

    vec4 lightYellow = vec4(0.85, 0.53,0.1, 1.0);
    vec4 Yellow = vec4(1, 1, 0, 1.0);

    float alpha = (gl_Position.y + 1.0) * 2.0 ;

    gradientColor = mix(Yellow, lightYellow, alpha);
}