# Soft Ray Tracer

OpenGL의 Compute Shader를 기반으로 소프트웨어 레이 트레이싱을 구현하고, 성능 최적화와 관련된 실험적 분석을 수행하는 학술대회를 위한 연구 프로젝트입니다. 광선 생성과 교차 판정, 가속 구조 탐색을 셰이더에서 직접 구현하며, 알고리즘과 GPU 데이터 처리 방식에 따른 성능 차이를 측정하고 분석하는 것을 목표로 합니다.

## 🛠️ 기술 스택

| 구분 | 라이브러리 및 API | 용도 |
| :--- | :--- | :--- |
| 언어 | C++20 / GLSL | 프레임워크 및 셰이더 구현 |
| 그래픽스 API | OpenGL 4.6 Core / Compute Shader | GPU 연산 및 레이 트레이싱 구현 |
| 윈도우 및 입력 | GLFW / GLAD | 윈도우·컨텍스트 생성, 입력 처리 및 OpenGL 함수 로딩 |
| 에셋 로딩 | Assimp / KTX | 3D 모델 임포트 및 KTX 텍스처 로딩 |
| 수학 | GLM | 벡터·행렬 및 좌표 변환 연산 |
| 디버그 UI | Dear ImGui | 디버그 화면 및 실험 파라미터 조절 |
| 로깅 | spdlog | 실행 로그 및 디버깅 |

## 📦 에셋 및 외부 라이브러리 준비

`Assets`와 `ThirdParty`는 Git 저장소에 포함되어 있지 않습니다. 빌드 및 실행 전에 [Google Drive 다운로드 폴더](https://drive.google.com/drive/folders/1ifNkKWmHThTi0-CJdv82mU9xuHt9YDmv?usp=sharing)에서 두 폴더를 내려받고, 압축된 경우 압축을 해제해 아래와 같이 배치해 주세요.

두 폴더는 **`soft_ray_tracer.slnx`가 있는 프로젝트 루트**에 위치해야 합니다. 루트 안의 `soft_ray_tracer/` 소스 폴더가 아니라, 솔루션 파일과 같은 위치에 두세요.

```text
soft_ray_tracer/              # 저장소 루트
├─ soft_ray_tracer.slnx
├─ README.md
├─ Assets/                   # 다운로드한 에셋 폴더
├─ ThirdParty/               # 다운로드한 외부 라이브러리 폴더
│  ├─ include/
│  ├─ lib/
│  └─ src/
├─ Core/
├─ Editor/
├─ Pipelines/
├─ Rendering/
├─ Resources/
├─ Runtime/
└─ soft_ray_tracer/          # 실행 프로젝트 소스 폴더
```

다운로드 과정에서 폴더가 중첩되지 않도록 `Assets/Assets/` 또는 `ThirdParty/ThirdParty/` 형태가 아닌지 확인해 주세요. 폴더 내부의 파일과 하위 폴더 구조는 그대로 유지해야 합니다.

## 🗺️ 향후 로드맵

아래 항목은 향후 구현 및 실험 계획이며, 개발 과정에서 조정할 예정입니다.

1. **Compute Shader 기반 레이 트레이싱 구현**: 카메라 광선 생성, 기본 도형 및 삼각형 교차 판정, 결과 이미지 출력
2. **장면 데이터 구성 및 셰이딩**: 모델·재질·광원 데이터를 GPU에 전달하고 기본 조명, 그림자 및 반사 구현
3. **PBR 기반 레이 트레이싱 구현**: 물리 기반 BRDF와 Metallic·Roughness 재질 모델을 적용하여 재질에 따른 빛의 반사와 표면 표현 구현
4. **가속 구조 도입**: BVH 구축 및 탐색을 구현하고, 전체 도형을 순회하는 방식과 성능 비교
5. **GPU 연산 최적화**: 워크그룹 크기, 메모리 배치 및 접근 패턴, 분기와 광선 처리 방식에 따른 성능 비교
6. **성능 측정 및 실험적 분석**: 동일한 장면과 렌더링 조건에서 최적화 전후의 GPU 실행 시간과 프레임 시간을 측정하고, 해상도·삼각형 수·광선 수에 따른 병목과 성능 변화 기록
