# Computer Graphics Assignment1

## Overview
이 프로젝트는 평면과 세 개의 구로 구성된 장면을 레이 트레이싱 알고리즘으로 렌더링하는 과제입니다.
실제 레이 트레이싱 및 음영(흑백) 계산은 직접 구현했으며, OpenGL/GLUT는 최종 결과 이미지를 화면에 출력하는 데에만 사용됩니다.

---

## Table of Contents
- [Compilation Instructions](#compilation-instructions)
- [Run Instructions](#run-instructions)
- [Short Description](#short-description)

---

## Compilation Instructions

### Using Visual Studio
1. **솔루션 열기**  
   `openglviewer.sln` 파일을 Visual Studio에서 엽니다.
2. **프로젝트 설정 확인**  
   - **Include 디렉토리:** `include` 폴더 경로가 포함되어야 합니다.  
   - **Library 디렉토리:** `lib` 폴더 경로가 포함되어야 합니다.
3. **Clean & Rebuild**  
   상단 메뉴에서 **Build > Clean Solution**을 선택한 후, **Build > Rebuild Solution**을 실행하여 최신 코드를 빌드합니다.

---

## Run Instructions
1. 빌드가 완료된 실행 파일은 `bin'폴더에 EmptyViewer로 생성됩니다.
2. 해당 실행 파일을 실행합니다.
3. 프로그램 실행 후 창이 열리며, 소프트웨어로 계산한 최종 이미지가 화면에 출력됩니다.
4. 프로그램 종료는 **ESC** 키 또는 **Q** 키를 누르면 됩니다.

---

## Short Description

### 1. main 함수
- 윈도우를 생성하고 관리하며 resize_callback을 통해 render() 결과를 사이즈에 맞게 출력합니다.

### 2. render 함수
- 1. 카메라 객채를 만들고 eye point, viewing region, image size를 저장해줍니다.
- 2. Scene 객체를 만들고 Plane 객체와 세 개의 구 객체를 만들어 addSurface를 통해 Scene의 surfaces 벡터 리스트에 추가해줍니다.
- 3. 모든 픽셀을 하나씩 순회하며 Ray 객체를 만들어 Camera 객체에서 해당하는 위치의 Ray를 가져와 줍니다.
- 4. Scene의 trace를 통해 검사할 해당하는 픽셀의 Ray와 최소, 최대거리를 인자로 넘겨준다.
- 5. trace에서는 Scene에 있는 각 surface들을 하나씩 검사한다.
- 6. 각 surface의 intersect를 통해 교차하는 지 검사를 하고 흰색인지 검은색인지 return 해준다.
- 7. 색 정보를 OutputImage에 넣어준다.

### Scene 클래스의 trace 함수
- 해당하는 Ray를 인자로 받아서 교차하는 지 각 surface를 순회하고 return해주는 함수다.
- tMin, tMax를 통해 고려할 최소, 최대 거리를 정해주고 만약 Hit하면 closest_t값을 그 거리 값으로 바꿔준다.
- hitSurface 포인터를 만들어 충돌한 surface객체를 저장하고 값이 있으면 흰색 없으면 검은색을 리턴한다.

### Plane intersect 함수
- 1. Ray의 방향과 Plane의 방향이 평행한 지 검사하고 맞으면 false를 반환한다.
- 2. 광선의 원점에서 평면까지의 거리를 계산한다.
- 3. 광선과 평면까지의 거리가 유효하면 true를 반환하고 아닐 시 false를 반환한다.

### Sphere intersect 함수
- 1. 광선의 원점에서 구의 중심 사이 벡터를 계산한다.
- 2. 이차 방적식의 계수를 각각 계산한다.
- 3. 판별식을 만들어서 실근이 존재하는지 확인한다.
- 4. 만약 판별식이 음수이면 false를 반환한다.
- 5. 판별식이 0보다 크다면 의 제곱근을 구해서 각각의 해가 유효범위안의 값인지 확인한다.
- 6. 하나의 근이라도 유효범위 안이라면 true를 반환하고 아닐 시 false를 반환한다.