# Unreal Engine 5 — 프로젝트 기본 설정 템플릿

새 UE5 프로젝트 생성 시 이 폴더의 파일들을 **프로젝트 루트**에 복사해서 사용하세요.

## 파일 목록

| 파일 | 필수 | 설명 |
|---|---|---|
| `.gitattributes` | ✅ 필수 | UE5 바이너리 에셋 깨짐 방지. `.uasset`/`.umap` 등을 binary로 처리해 CRLF 변환 차단 |
| `.gitignore` | ✅ 필수 | Binaries, Intermediate, Saved, IDE 파일 등 불필요한 파일 제외 |
| `.editorconfig` | 권장 | C++ 탭/들여쓰기, 줄바꿈 규칙 (VS/Rider/VSCode 자동 적용) |
| `.vsconfig` | 선택 | Visual Studio 필수 컴포넌트 목록. 팀원이 VS 설치 시 자동으로 필요한 워크로드 제안 |

## 사용 방법

```
새_프로젝트/
├── Content/
├── Source/
├── .gitattributes   ← 복사
├── .gitignore       ← 복사
├── .editorconfig    ← 복사
└── .vsconfig        ← 복사 (VS 사용 시)
```

## 주의 사항

### `.gitignore` 커스터마이징
프로젝트에서 `Saved/` 내 특정 파일을 트래킹해야 할 경우 예외 추가:
```gitignore
Saved/
!Saved/MyImportantFile.txt
```

### Git LFS
팀 협업 환경에서 `.uasset` 용량이 클 경우 Git LFS 도입 권장:
```bash
git lfs install
git lfs track "*.uasset"
git lfs track "*.umap"
```
LFS 사용 시 `.gitattributes`의 binary 라인을 LFS filter로 교체.

### `.vsconfig` 버전
현재 파일은 **UE 5.7 + VS 2022 기준**입니다.
다른 엔진 버전 사용 시 Epic 런처 → 엔진 우클릭 → "Visual Studio 필수 구성 요소 확인"으로 최신 목록 확인.
