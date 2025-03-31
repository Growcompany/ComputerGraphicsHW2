# Computer Graphics Assignment1

## Overview

이 프로젝트는 평면과 세 개의 구로 구성된 장면을 레이 트레이싱 알고리즘으로 렌더링하는 과제입니다. 
Phong 모델을 기반으로 조명을 계산하며, 광원, 재질 특성(ambient, diffuse, specular), 그림자 효과 등을 모두 직접 구현합니다.

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
   상단 메뉴에서 **Build > Clean Solution**을 선택한 후, **Build > Rebuild Solution**을 실행하여 최신 코드를 빌드합니다. 또는 단축키(Ctrl + Shift + B)로 빌드 가능

---

## Run Instructions

### 1. 실행 파일로 실행 (권장)

- 빌드가 완료되면 실행 파일이 `bin` 폴더에 생성됩니다:
  - `ComputerGraphicsHW2_Q1.exe`
  - `ComputerGraphicsHW2_Q2.exe`
  - `ComputerGraphicsHW2_Q3.exe`
- 원하는 `.exe` 파일을 더블 클릭하여 실행합니다.

---

### 2. Visual Studio에서 실행

- 솔루션에는 총 3개의 프로젝트가 포함되어 있습니다.
- 특정 프로젝트만 실행하려면:
  1. **솔루션 탐색기**에서 원하는 프로젝트를 마우스 오른쪽 클릭
  2. **"Set as Startup Project"** 선택
  3. `Ctrl + F5`를 눌러 실행 (디버깅 없이 실행)

---

### 3. 실행 시 동작

- 프로그램 실행 후, 계산된 최종 이미지가 창에 출력됩니다.
- 프로그램 종료는 키보드에서 **ESC 키** 또는 **Q 키**를 누르면 됩니다.


---

## Short Description

### Q1 - Material struct 추가
- ka, kd, ks, specularPower의 정보를 담는 struct를 추가해준다.

### Q1 - Sphere Class에 intersect 함수의 인자로 법선벡터(normal)  추가
- 카메라의 광선이 Sphere에 맞는 위치에서 구의 중심 위치를 빼서 법석벡터를 구한 값을 참조를 통해 값을 넘겨겨준다.

### Q1 - Plane Class에 intersect 함수의 인자로 법선벡터(normal)  추가
- 여기서의 Plane은 y값만 인자로 받는 Plane이기 때문에 법선벡터는 vec3(0.0f, 1.0f, 0.0f) 고정이다. 이 값을 인자로 넘겨준다.

### Q1 - Scene Class에 trace함수에서 흑백처리 -> 컬러처리(Lighting, Shawdow, Phong illumination)로 수정
- 1. 기존의 카메라광선과 물체가 hit할 시 흰 색으로 return하던 부분을 빼준다.
- 2. 빛의 정보를 추가해준다.
- 3. 빛의 색상을 흰색으로 설정한다.
- 4. 빛의 방향을 빛의 위치에서 카메라광선과 물체가 hit한 point를 빼주고 단위벡터로 만들어준다.
- 5. 자신을 향한 방향의 단위벡터를 카메라광선의 반대방향으로 구해준다.
- 6. 빛까지의 반대 방향 벡터와 카메라광선과 충돌한 각 물체의 지점에서의 법선벡터를 reflect를 통해 빛이 반사된 reflectDir 벡터를 알아낸다.
- 7. 그림자를 처리하기 위해 각 물체의 충돌한 지점에서 빛을 향하는 Ray를 하나 만들어준다.
- 8. 이때 충돌한 지점에서 빛까지의 방향으로 Ray를 만들면 중복처리등의 문제가 발생할 수도 있기에 법선벡터방향으로 아주 작은 값을 시작지점에 더해줘서 문제를 없애준다.
- 9. 모든 surfaces를 돌면서 그림자 광선이 충돌하는 surfaces가 있는지 확인한다.
- 10. 이때 빛과 시작지점 사이에 물체가 있는지 확인하는 과정이므로 이 사이값안에서만 intersect하는 지 확인한다.
- 11. 만약 intersect하는 surface가 있다면 색상값에서 ka값만 즉, ambient값만 반환해준다.
- 12. 그림자 처리를 지나고 그림자가 지는 공간이 아니라면 나머지 값들도 계산해준다.
- 13. ambient는 각 물체에 설정한 ka값대로 설정한다.
- 14. diffuse값은 물체의 법선벡터와 빛의 방향과 내적을 통해 최소 0보다 큰 값으로 설정한다.
- 15. specular는 카메라광선의 반대방향인 보이고 있는 viewDir과 빛이 반사된 reflectDir과 내적을 한 0보다 큰 값을 미리 설정한 specularPower값만큼 제곱해주어 물체의 재질등을 표현해준다.
- 16. 한 픽셀에서의 색상정보는 ambient와 diffuse, specular값을 더한 값으로 넘겨준다.
- 17. 넘겨받은 값을 이미지에 넣어서 결과물을 완성한다.

### Q2 - Gamma correction (γ = 2.2)
- render함수에서 각 픽셀의 색을 OutputImage에 넣기 전 r,g,b의 각각의 성분에 (color의 r,g,b)^(1/2.2)을 통해 제곱을 해주어 Gamma correction을 구현하였다. 

### Q3 - Antialiasing (N = 64)
- 1. Camera클래스의 GetRay함수에서 각 픽셀의 위치를 구하는 과정에서 ix, iy에 0.5를 더하는 값을 0 ~ 1 사이의 랜덤한 float형 값으로 바꿔준다.
- 2. render함수에서 각 픽셀에서의 컬러를 구할 때 하나의 정해진 픽셀의 위치에서 나오는 컬러값에서 Getray함수에 랜덤한 offsetX, offsetY값을 넣어주어 여기서 나온 color값을 구한다. 총 이 과정을 64번 반복하여 나온 color값을 64로 나눠주면 된다.
- 3. 이러한 과정을 통해 한 픽셀내에서 랜덤한 64개의 위치에서의 color값의 평균을 내어 Antialiasing을 구현할 수 있다. 