# MCP + UE5 Python 스크립트 실행 가이드

## 개요

MCP(Model Context Protocol)의 `unreal_editor` 서버를 이용하면  
UE5 에디터 안에서 Python 스크립트를 실행해 데이터 에셋, 블루프린트 등을 자동으로 편집할 수 있다.

---

## MCP 도구 종류

| 도구 | 용도 | 비고 |
|---|---|---|
| `check_editor_connection` | 에디터 연결 상태 확인 | 먼저 실행해서 연결 확인 |
| `execute_python` | 단일 라인 Python 실행 | 세미콜론(`;`)으로 구문 연결, 멀티라인 불가 |
| `evaluate_python` | Python 표현식 평가 | 반환값 없음, 결과는 UE Output Log 확인 |
| `run_python_file` | Python 파일 실행 | **멀티라인 스크립트는 반드시 이 도구 사용** |
| `save_all_dirty_assets` | 모든 더티 에셋 저장 | 스크립트 실행 후 추가 저장 시 사용 |
| `find_assets` | 에셋 검색 | 현재 버그 있음 → Glob으로 경로 직접 탐색 |
| `get_asset_info` | 에셋 정보 조회 | 현재 버그 있음 → Python으로 대체 |

---

## 핵심 주의사항

### 1. `execute_python`은 단일 라인만 가능
```python
# ✅ 동작 — 세미콜론으로 연결
obj = unreal.load_object(None, '/Game/.../DA_Foo'); unreal.log(str(obj))

# ❌ 동작 안 함 — 실제 줄바꿈 불가
import unreal
obj = unreal.load_object(...)   # SyntaxError: multiple statements
```

### 2. 멀티라인 스크립트는 `run_python_file` 사용
```
파일을 먼저 생성(Write 도구) → run_python_file로 실행
```

### 3. 실행 결과는 UE Output Log에서 확인
`execute_python` / `run_python_file` 모두 결과를 MCP로 반환하지 않는다.  
→ `Saved/Logs/Aura.log` 파일을 직접 읽거나 UE 에디터 Output Log 창에서 확인.

```bash
# 로그 확인 (Grep 도구 활용)
Grep("pattern", "E:/Git_Project/UE5TopDownRPG/Saved/Logs/Aura.log")
```

### 4. `unreal.log()` 사용 권장
Python `print()`는 출력되지 않을 수 있다. `unreal.log()`를 사용할 것.

```python
# ✅ 권장
unreal.log("결과: " + str(value))
unreal.log_error("오류 발생: " + str(e))

# ⚠️ 불안정
print("결과")
```

---

## 에셋 경로 규칙

| 구분 | 형식 | 예시 |
|---|---|---|
| 파일시스템 경로 | `E:/Git_Project/UE5TopDownRPG/Content/...` | `Content/Blueprints/AbilitySystem/Data/DA_AttributeInfo.uasset` |
| UE 내부 경로 | `/Game/...` (확장자 없음) | `/Game/Blueprints/AbilitySystem/Data/DA_AttributeInfo` |
| 에셋 경로 탐색 | Glob 도구로 파일시스템에서 먼저 확인 | `Glob("**/DA_AttributeInfo*", "E:/...")` |

---

## USTRUCT 인스턴스 편집 방법

UE5 Python에서 USTRUCT의 프로퍼티는 직접 `set_editor_property`가 안 되는 경우가 있다.  
특히 `FGameplayTag`처럼 `TagName`이 read-only인 경우.

### ✅ 해결책: `import_text()` 사용

```python
entry = unreal.AuraAttributeInfo()

# export_text()로 포맷 먼저 확인
print(entry.export_text())
# → (AttributeTag=(TagName=""),AttributeName="",AttributeDescription="",AttributeValue=0.000000)

# 확인한 포맷으로 import_text() 호출
entry.import_text('(AttributeTag=(TagName="Attribute.Primary.Strength"),AttributeName="Strength",AttributeDescription="Increases Physical Damage")')

# 결과 확인
print(entry.export_text())
# → (AttributeTag=(TagName="Attribute.Primary.Strength"),AttributeName="Strength",...)
```

### 포맷 규칙
- `FGameplayTag` → `(TagName="태그.이름")`
- `FText` → `"문자열"` (import_text에서 자동으로 NSLOCTEXT로 변환됨)
- `float` → `0.000000`
- 중첩 구조체 → `PropertyName=(SubProperty=Value,...)`

---

## 데이터 에셋 배열 편집 전체 흐름

```python
import unreal

ASSET_PATH = '/Game/Blueprints/AbilitySystem/Data/DA_AttributeInfo'

# 1. 에셋 로드
data_asset = unreal.load_object(None, ASSET_PATH)
if not data_asset:
    unreal.log_error("에셋 로드 실패: " + ASSET_PATH)

# 2. 구조체 인스턴스 생성 (import_text 방식)
entry = unreal.AuraAttributeInfo()
entry.import_text('(AttributeTag=(TagName="Attribute.Primary.Strength"),AttributeName="Strength",AttributeDescription="Increases Physical Damage")')

# 3. 배열에 추가
entries = [entry, ...]  # 전체 목록

# 4. 데이터 에셋에 설정
data_asset.set_editor_property('attribute_information', entries)

# 5. 저장
unreal.EditorAssetLibrary.save_asset(ASSET_PATH, only_if_is_dirty=False)
unreal.log("저장 완료: {} entries".format(len(entries)))
```

---

## 디버깅 패턴

### 연결 확인
```
mcp__unreal_editor__check_editor_connection()
```

### 구조체 프로퍼티 목록 확인
```python
entry = unreal.SomeStruct()
unreal.log(str([x for x in dir(entry) if not x.startswith('_')]))
unreal.log(entry.export_text())  # 현재 값과 포맷 동시 확인
```

### unreal 모듈 클래스 탐색
```python
results = [x for x in dir(unreal) if 'keyword' in x.lower()]
unreal.log(str(results))
```

### 에셋 경로 탐색 (Python 내에서)
```python
registry = unreal.AssetRegistryHelpers.get_asset_registry()
# → find_assets MCP 도구에 버그가 있을 때 대안
```

---

## 실제 적용 사례: DA_AttributeInfo 속성 추가

- **작업**: `InitializeNativeGameplayTags()`의 16개 Gameplay Tag를 DA_AttributeInfo에 추가
- **스크립트**: `Scripts/populate_da_attribute_info.py`
- **핵심 문제**: `FGameplayTag.TagName`이 read-only → `import_text()`로 우회
- **결과**: Primary 4개 + Secondary 10개 + Vital 2개 = 총 16개 항목 추가 및 저장 완료

---

## 파일 위치

| 파일 | 설명 |
|---|---|
| `Scripts/populate_da_attribute_info.py` | DA_AttributeInfo 16개 속성 추가 스크립트 |
| `Scripts/MCP_UE5_Python_Guide.md` | 이 가이드 문서 |
