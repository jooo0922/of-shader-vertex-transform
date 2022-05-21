// 구름메쉬에 적용할 버텍스 셰이더를 새로 하나 파줌.

#version 410

layout(location = 0) in vec3 pos;
layout(location = 3) in vec2 uv;

uniform vec3 translation;
uniform float rotation;
uniform vec3 scale;

out vec2 fragUV;

void main() {
  vec3 scaled = pos * scale; // 스케일 연산 먼저

  vec3 rotated;
  rotated.x = (cos(rotation) * scaled.x) - (sin(rotation) * scaled.y); // 그 다음 회전 연산
  rotated.y = (cos(rotation) * scaled.y) + (sin(rotation) * scaled.x);

  // 일단 크기, 회전, 이동 연산을 아래와 같이 진행해 줌. 
  // 원래 교재에 나와있는 공식과는 다르기는 한데, 교재에 나온대로 하면 렌더링이 안되서 아래와 같이 translation.x, y 값을 직접 더해줬음.
  vec3 final;
  final.x = rotated.x + translation.x; // 마지막으로 이동 연산
  final.y = rotated.y + translation.y;
  final.z = rotated.z;

  // gl_Position = vec4(rotated + translation, 1.0); // 지금 이런 식으로 + 연산자를 통한 할당을 해주면 렌더링이 안되고 있음. translation.z 값을 더해주면 화면에 안찍히고 있음..ㅠ 뭐가 문제지..
  gl_Position = vec4(final, 1.0);

  fragUV = vec2(uv.x, 1.0 - uv.y);
}

/*
  버텍스 셰이더에서 회전 처리 계산은
  이동이나 스케일 계산보다는 복잡하지만, 
  구체적으로 설명할 필요는 없음.

  왜냐면, 실제로 이런 연산은 변환 행렬을 이용해서 
  한 줄의 코드로 처리하는 간단한 공식을 사용할 것이기 때문!

  대신 여기서 기억해야 할 것은,
  항상 크기 연산 먼저,
  그 다음 회전 연산,
  마지막으로 이동 연산

  이 순서로 연산을 해줘야 한다는 게 중요함!
*/