#version 150

precision mediump float;

uniform sampler2D tex0;
uniform float width;
uniform float height;
uniform float localX;
uniform float localY;

uniform float elapsedTime;

vec2 remap(vec2 uv, vec2 curvature) {
  uv = uv * 2.0 - 1.0;
  vec2 offset = abs(uv.yx) / curvature;
  uv = uv + uv * offset * offset;
  uv = uv * 0.5 + 0.5;
  return uv;
}

void main() {
  vec2 screenRes = vec2(width, height);
  vec2 localCoord = vec2(localX, localY);
  vec2 vUV = (gl_FragCoord.xy - localCoord) / screenRes;
  vec2 uv = vec2(vUV.x, 1.0-vUV.y);

  //round effect
  vec2 new_uv = remap(uv, vec2(3.5, 3.5));
  vec4 texColor = texture2D(tex0, new_uv);

  //if out of range, pixel is black
  vec4 outColor;
  if(new_uv.x < 0.0 || new_uv.y < 0.0 || new_uv.x > 1.0 || new_uv.y > 1.0) {
    outColor = vec4(0.0, 0.0, 0.0, 1.0);
  } else {
    outColor = texColor;
  }

  // vignettature
  new_uv *= 1.0 - new_uv.xy;
  float vig = new_uv.x*new_uv.y*100; // multiply with sth for intensity
  vig = clamp(pow(vig, 0.4), 0, 1); //change exponent for different rounding effect
  outColor.rgb *= vig;

  //scanline
  float sinValue = (sin(uv.y * screenRes.y *2) + 1) * 0.15 + 1;
  float cosValue = (cos(uv.y * screenRes.y *2) + 1) * 0.10 + 1;
  outColor.g *= sinValue;
  outColor.rb *= cosValue;
  outColor = clamp(outColor, 0.0, 1.0);

  //crt refresh line
  float refreshLine = 0.8 + abs(sin(uv.y*screenRes.y*0.0015 - elapsedTime))*0.2;
  outColor.rgb *= refreshLine;

  gl_FragColor = outColor; 
}