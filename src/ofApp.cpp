#include "ofApp.h"

// 캐릭터 텍스쳐를 입힐 쿼드 메쉬 생성 함수를 따로 밖으로 빼서 정리함.
void buildMesh(ofMesh& mesh, float w, float h, glm::vec3 pos) {
    // 캐릭터 메쉬를 위치시키려는 vec3 pos 값과 width, height 값으로 계산하여
    // 4개 버텍스의 x, y, z 좌표값들을 verts 라는 float 배열에 담아놓음.
    float verts[] = {
        -w + pos.x, -h + pos.y, pos.z,
        -w + pos.x, h + pos.y, pos.z,
        w + pos.x, h + pos.y, pos.z,
        w + pos.x, -h + pos.y, pos.z,
    };
    
    // 4개 버텍스의 uv좌표값들을 uvs 라는 float 배열에 담아놓음
    float uvs[] = {
        0, 0,
        0, 1,
        1, 1,
        1, 0
    };
    
    // for loop 를 돌면서 verts, uvs 배열에 담긴 좌표값들을 꺼내와서 인자로 전달받은 ofMesh 에 버텍스 및 uv좌표를 추가해 줌.
    for (int i = 0; i < 4; ++i) {
        int idx = i * 3;
        int uvIdx = i * 2;
        mesh.addVertex(glm::vec3(verts[idx], verts[idx + 1], verts[idx + 2]));
        mesh.addTexCoord(glm::vec2(uvs[uvIdx], uvs[uvIdx + 1]));
    }
    
    // 4개의 버텍스 순서를 인덱싱해서 어떻게 쿼드 메쉬의 삼각형들을 그려줄 것인지 결정함.
    ofIndexType indices[6] = {0, 1, 2, 2, 3, 0};
    mesh.addIndices(indices, 6);
};

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex(); // 스크린 픽셀 좌표를 사용하는 텍스쳐 관련 오픈프레임웍스 레거시 지원 설정 비활성화
    ofDisableDepthTest(); // 깊이테스트를 활성화하여 z좌표값을 깊이버퍼에 저장해서 z값을 기반으로 앞뒤를 구분하여 렌더링할 수 있도록 함.
    
    buildMesh(charMesh, 0.1, 0.2, glm::vec3(0.0, -0.2, 0.0)); // 캐릭터메쉬 생성
    buildMesh(backgroundMesh, 1.0, 1.0, glm::vec3(0.0, 0.0, 0.5)); // 배경메쉬 생성
    buildMesh(cloudMesh, 0.25, 0.15, glm::vec3(0.0, 0.0, 0.0)); // 구름메쉬 생성
    buildMesh(sunMesh, 1.0, 1.0, glm::vec3(0.0, 0.0, 0.4)); // 태양메쉬 생성
    
    alienImg.load("walk_sheet.png"); // 캐릭터메쉬에 사용할 스프라이트시트 텍스쳐 로드
    backgroundImg.load("forest.png"); // 배경메쉬에 사용할 텍스쳐 로드
    cloudImg.load("cloud.png"); // 구름메쉬에 사용할 텍스쳐 로드
    sunImg.load("sun.png"); // 태양메쉬에 사용할 텍스쳐 로드
    
    spritesheetShader.load("spritesheet.vert", "alphaTest.frag"); // 스프라이트시트 기법을 사용할 캐릭터메쉬에 적용할 셰이더 파일 로드
    alphaTestShader.load("passthrough.vert", "alphaTest.frag"); // 알파테스트 셰이더를 사용할 메쉬들에 적용할 셰이더 파일 로드
    cloudShader.load("cloud.vert", "cloud.frag"); // 구름메쉬에 적용할 셰이더 파일 로드
}

//--------------------------------------------------------------
void ofApp::update(){ // 매 프레임마다 호출하지만, 렌더링 및 드로우콜과 직접적 관련 없는 로직들을 넣는 곳.
    if (walkRight) { // 오른쪽 화살표 키 입력을 감지하여 true 이면 조건문 블록을 수행함.
        float speed = 0.5 * ofGetLastFrameTime(); // 이전 프레임과 현재 프레임의 시간 간격인 '델타타임'을 가져와서 속도값을 구함.
        charPos += glm::vec3(speed, 0, 0); // 속도값 만큼을 x좌표에 더해서 charPos 값을 누적계산함.
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDisableBlendMode(); // 이전 프레임에서 사용한 블렌딩 모드를 비활성화함.
    ofEnableDepthTest(); // 구름메쉬의 투명픽셀에 의해 태양메쉬가 가리는 문제를 해결하고자 밑에서 비활성화했던 깊이테스트를 다시 활성화함. (캐릭터메쉬, 배경메쉬는 깊이를 구분해줘야 하니까)
    
    static float frame = 0.0; // 프레임 변수 초기화
    frame = (frame > 10) ? 0.0 : frame += 0.2; // frame의 정수부분이 5번의 draw() 함수 호출 이후 바뀌도록 프레임 계산
    glm::vec2 spriteSize = glm::vec2(0.28, 0.19); // 스프라이트시트 텍스쳐 사이즈를 프레임 하나 만큼의 사이즈로 조절할 때 필요한 값
    glm::vec2 spriteFrame = glm::vec2((int)frame % 3, (int)frame / 3); // 스프라이트시트 텍스쳐 offset(각각 u, v 방향으로) 적용 시 사용할 vec2값
    
    
    // spritesheetShader 바인딩하여 사용 시작
    spritesheetShader.begin();
    
    spritesheetShader.setUniform2f("size", spriteSize); // 텍스쳐 사이즈 조절값 유니폼 변수에 전송
    spritesheetShader.setUniform2f("offset", spriteFrame); // 프레임 offset 값 유니폼 변수에 전송
    spritesheetShader.setUniformTexture("tex", alienImg, 0); // 스프라이트시트 텍스쳐 이미지 유니폼 변수에 전송
    spritesheetShader.setUniform3f("translation", charPos); // 오른쪽 화살표 키 입력에 따라 캐릭터메쉬를 x축 오른쪽 방향으로 이동시킬 때 사용할 값 유니폰 변수에 전송
    charMesh.draw(); // 캐릭터메쉬 드로우콜 호출하여 그려줌
    
    spritesheetShader.end();
    // spritesheetShader 사용 중단
    
    
    // alphaTestShader 바인딩하여 사용 시작
    alphaTestShader.begin();
    
    alphaTestShader.setUniformTexture("tex", backgroundImg, 0); // 배경 텍스쳐 이미지 유니폼 변수에 전송
    backgroundMesh.draw(); // 배경메쉬 드로우콜 호춣여 그려줌
    
    alphaTestShader.end();
    // alphaTestShader 사용 중단
    
    
    ofDisableDepthTest(); // 투명 픽셀이 깊이버퍼값을 가져서 뒤에 있던 태양메쉬가 가려지지 않도록, 태양메쉬와 구름메쉬를 그리기 전 깊이테스트 비활성화
    ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ALPHA); // 새로운 프래그먼트의 알파값을 이용해서 백 버퍼의 프래그먼트와 색상값을 섞어주는 알파 블렌딩 모드 활성화
    
    
    // cloudShader 바인딩하여 사용 시작
    cloudShader.begin();
    
    cloudShader.setUniformTexture("tex", cloudImg, 0); // 구름 텍스쳐 이미지 유니폼 변수에 전송
    cloudMesh.draw(); // 구름메쉬 드로우콜 호출하여 그려줌
    
    cloudShader.end();
    // cloudShader 사용 중단
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ofKey::OF_KEY_RIGHT) {
        walkRight = true; // 오른쪽 화살표 키 입력 감지 시 walkRight 을 활성화함.
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == ofKey::OF_KEY_RIGHT) {
        walkRight = false; // 오른쪽 화살표 키 입력을 뗏을 때 walkRight 을 비활성화함.
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
