## 파일 목록

| 파일 | 필수 | 설명 |
|---|---|---|
| `.gitattributes` | ✅ 필수 | UE5 바이너리 에셋 깨짐 방지. `.uasset`/`.umap` 등을 binary로 처리해 CRLF 변환 차단 |
| `.gitignore` | ✅ 필수 | Binaries, Intermediate, Saved, IDE 파일 등 불필요한 파일 제외 |
| `.editorconfig` | 권장 | C++ 탭/들여쓰기, 줄바꿈 규칙 (VS/Rider/VSCode 자동 적용) |

## 사용 방법

```
새_프로젝트/
├── Content/
├── Source/
├── .gitattributes   ← 복사
├── .gitignore       ← 복사
├── .editorconfig    ← 복사
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
