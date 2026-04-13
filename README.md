# Aura - UE5 Top-Down RPG (Gameplay Ability System)

Unreal Engine 5 기반의 탑다운 RPG 프로젝트입니다.
GAS(Gameplay Ability System)를 활용한 AAA급 코드 아키텍처를 학습하는 것이 목표입니다.

---

## 마감 기한

2026.3.26 ~ 2026. 4.26

Week1:
1. 기본적인 언리얼 프로젝트 세팅
2. 캐릭터 및 NPC 애니메이션
3. GAS(GameplayAbilitySystem) 시스템 기반 코드 작성
4. UI Widget 컨트롤러 및 기반 작업

Week2:
1. Targeting 기능 구현
2. Spline 기반 Auto Run 구현
3. Gameplay Effect 시스템 구현
4. 스킬 시스템 구현

Week3:
1.

##

---

## 프로젝트 개요

| 항목 | 내용 |
|---|---|
| 엔진 버전 | Unreal Engine 5.7 |
| 프로젝트명 | Aura |
| 장르 | Top-Down RPG |
| 핵심 시스템 | Gameplay Ability System (GAS) |
| 멀티플레이어 | 지원 (Replicated) |

---

## 주요 학습 내용

### 캐릭터 시스템
- OOP 및 상속을 활용한 캐릭터 클래스 계층 구조
- 플레이어 캐릭터 **Aura** 및 다양한 적 캐릭터
- RPG 직업 클래스: **Ranger**, **Warrior**, **Elementalist**

### Gameplay Ability System (GAS)
- GAS 핵심 클래스 및 컴포넌트 설정
- 커스텀 **Ability System Component** 및 **Attribute Set**
- Attribute 복제(Replication) 처리

### 스탯(Attribute) 시스템

**Primary Attributes**
- Strength (물리 피해 증가)
- Intelligence (마법 피해 증가)
- Resilience (방어구 및 방어 관통력 증가)
- Vigor (최대 체력 증가)

**Secondary Attributes**
- Armor / Armor Penetration
- Block Chance / Critical Hit Chance / Critical Hit Damage / Critical Hit Resistance
- Health Regeneration / Mana Regeneration
- Max Health / Max Mana

### 전투 시스템
- 피해 타입별 저항 및 디버프 (화염, 번개 등)
- 플로팅 데미지 텍스트 (치명타, 막기 등 시각 피드백)
- 넉백 및 기절(번개 피해), 화상 상태이상(화염 피해)

### 스펠(Ability) 시스템
- **FireBolt**, **Electrocute**, **Arcane Shards**, **FireBlast** 등 공격 스펠
- 패시브 스펠 (장착 중 지속 발동)
- 마나 소모 및 쿨다운 관리
- 스펠 메뉴에서 입력키 자유 배정

### UI 시스템
- MVC / MVVM 패턴 적용
- HUD: 장착 스펠, 쿨다운 타이머, XP 바, 체력/마나 글로브
- Attribute 메뉴 및 Spell Tree 메뉴
- 로드 화면(Load Screen)

### 경험치 & 레벨업
- 적 처치 시 XP 획득
- 레벨업 시 Attribute Points / Spell Points 획득
- 레벨업 이펙트 및 체력/마나 회복

### 적 AI
- Behavior Tree + EQS(Environment Query System)
- 근거리(Warrior), 원거리(Ranger), 마법사(Elementalist) 적 타입
- 적의 스펠 시전 및 AI 미니언 소환

### 기타
- 저장 및 레벨 전환 시스템
- 카메라를 가리는 지형 페이드 처리
- 커스텀 Async Tasks, Ability Tasks, Blueprint Function Libraries
- Asset Manager, Gameplay Effect Context, Net Serialization

---

## 에셋 구성

```
Content/
├── Assets/
│   ├── Characters/         # Aura (주인공)
│   ├── Enemies/            # Demon, Ghoul, Goblin, Shaman, Shroom
│   ├── Dungeon/            # 모듈식 던전 파츠
│   ├── Effects/            # Niagara 파티클 이펙트
│   ├── Spells/             # 스펠 이펙트 에셋
│   ├── Sounds/             # 사운드 이펙트
│   ├── UI/                 # HUD, 버튼, 글로브, XP바, 스펠트리 등
│   ├── Materials/
│   ├── MagicCircles/
│   └── Pickups/
└── Maps/
    └── StartupMap.umap
```

---

## Git 설정

| 파일 | 설명 |
|---|---|
| `.gitattributes` | `.uasset` / `.umap` 바이너리 처리 (CRLF 변환 차단) |
| `.gitignore` | Binaries, Intermediate, Saved, IDE 파일 제외 |
| `.editorconfig` | C++ 코드 스타일 규칙 (VS/Rider/VSCode 자동 적용) |

### Git LFS (대용량 에셋 협업 시 권장)
```bash
git lfs install
git lfs track "*.uasset"
git lfs track "*.umap"
```
> LFS 사용 시 `.gitattributes`의 binary 라인을 LFS filter로 교체

---
- 강의: *Unreal Engine 5 - Gameplay Ability System - Top Down RPG* by Stephen Ulibarri
