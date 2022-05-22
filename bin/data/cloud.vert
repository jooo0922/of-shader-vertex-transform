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

  /*
    지금 rotated.z 에 값을 할당하지 않은 상태로
    translation 과 더해주면, 구름메쉬가 제대로 렌더링이 안되고 있음.

    정확한 건 아니지만,
    오픈프레임웍스에서는 셰이더 변수를 선언하기만 한다고
    값이 자동으로 초기화되어 할당되는 건 아닌 거 같음.

    GLSL Editor 에서는 그냥 vec3 변수를 선언하기만 해도
    (0, 0, 0) 으로 초기화되서 사용할 수 있는 반면,

    오픈프레임웍스에서 사용하는 셰이더는
    그냥 vec3 변수명;
    이렇게 변수를 '선언'만 한다고 값이 자동으로 '할당' 되지 않는 거 같음.

    그러니 할당되지 않은 값에 translation.z 값을 더해봤자
    뭐가 나올 수 없는 거...

    일단 이 정도로 원인을 파악하고 있고,
    추후에 계속 예제를 진행해보면서 비슷한 상황이 발생할 시
    이런 방식으로 해결하도록 하고, 관련 이슈에 대해서 좀 더 
    리서치해봐야 할 거 같음. 정확한 원인을 알아야 하니까. 
  */
  vec3 rotated = vec3(0.0);
  rotated.x = (cos(rotation) * scaled.x) - (sin(rotation) * scaled.y); // 그 다음 회전 연산
  rotated.y = (cos(rotation) * scaled.y) + (sin(rotation) * scaled.x);

  gl_Position = vec4(rotated + translation, 1.0); // 마지막으로 이동 연산

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