# Aura - UE5 Top-Down RPG (Gameplay Ability System)

Unreal Engine 5 기반의 탑다운 RPG 프로젝트입니다.
GAS(Gameplay Ability System)를 활용한 AAA급 코드 아키텍처를 학습하는 것이 목표입니다.

---

## 작업 기간

**2026.3.26 ~ 2026.5.27 (8주 / 완료)**

주차별로 마일스톤 브랜치(`Milestone_Week_1` ~ `Milestone_Week_8`)를 두고 작업 후 `master`에 병합하는 방식으로 진행했습니다.

### Week 1 — 프로젝트 기반 & GAS 골격 (3.26 ~ 4.06)
1. C++ 기반 Unreal 프로젝트 셋업, GameMode / PlayerController / PlayerState 구성
2. 플레이어(Aura) · NPC(Goblin) 캐릭터 클래스 및 애니메이션, 카메라 세팅
3. Enhanced Input 기반 입력 시스템 구축
4. HighlightInterface를 통한 적 아웃라인(커스텀 뎁스) 처리
5. Ability System Component(ASC) · AttributeSet 초기 구성 (Player / Enemy)
6. HUD · WidgetController(Overlay/Health/Mana) 및 HP/MP Globe UI 구현
7. GameplayEffect 기반 EffectActor(포션·크리스탈 등) 및 CurveTable 레벨 스케일링

### Week 2 — 어트리뷰트 시스템 & Ability Input (4.06 ~ 4.13)
1. Primary / Secondary / Vital Attribute 정의 및 커스텀 ModMagnitude 계산
2. CombatInterface, AttributeSystemLibrary 구현
3. AssetManager 등록 및 Native GameplayTag 시스템 구축
4. Attribute 상태 메뉴 UI 및 WidgetController 데이터 바인딩
5. GameplayAbility · AbilityInputComponent로 입력 태그 기반 어빌리티 발동 구조 구현

### Week 3 — 전투 · 데미지 · Enemy AI (4.13 ~ 4.20)
1. Projectile(Firebolt) 시스템 및 공격 몽타주 연동
2. 적 체력 위젯, 데미지 적용 파이프라인 구현
3. ExecCalc(GameplayEffectExecutionCalculation) 기반 데미지 산출
4. Block / Critical Hit 처리 및 CustomNetSerialize 적용
5. Resistance(피해 저항) Attribute 및 플로팅 데미지 텍스트 구현
6. 캐릭터 HitReact · Die 처리
7. AIController + BehaviorTree 기반 적 AI 및 근접 공격 구현

### Week 4 — 몬스터 다양화 & 레벨업 시스템 (4.20 ~ 4.27)
1. 적 몬스터 확장: Ghoul, GoblinShaman, DemonWarrior, Goblin Slingshot
2. GameplayCue 기반 전투 연출 및 몬스터 디테일 개선
3. Shaman의 미니언 소환(SpawnMonster) 스킬 구현
4. 신규 던전 맵 및 카메라 가림 지형 Fade 시스템
5. 어빌리티 Cooldown · Cost 시스템 구현
6. XP · 레벨업 시스템 및 PlayerInterface 구현
7. PassiveSkill 및 이벤트 구독(Listen Event) 구조 구현

### Week 5 — 스펠 메뉴 & 포인트 시스템 (4.27 ~ 5.04)
1. C++ SpellMenuWidgetController 구현
2. 스펠 트리 UI(스킬 장착 · 레벨 업) 구현
3. Attribute Point 적용 위젯 구현
4. Type / Status / Input GameplayTag 체계 정비
5. 멀티플레이 환경 플레이어 공격 동기화 버그 수정

### Week 6 — 데미지 파라미터화 & 디버프/추가 스펠 (5.04 ~ 5.11)
1. DamageEffectParams 구조화 및 데미지 어빌리티 리팩토링
2. Debuff 시스템(Stun, Shock, Burn) 및 DeathImpulse·Knockback 구현
3. Firebolt Homing(유도) 컴포넌트 추가
4. 마우스 클릭 Niagara 이펙트 구현
5. Lightning 계열 스펠(Chain Beam, Electrocute) 구현
6. PassiveSpell 구현

### Week 7 — 공격 스펠 확장 & 메뉴 (5.11 ~ 5.18)
1. PassiveSpell 멀티플레이 동기화 수정
2. ArcaneShards 공격 스펠 및 DamageCalc 구현
3. FireBlast 공격 스펠 및 GameplayCue 연출 구현
4. 빌드 세팅 정리
5. Start Menu / Load Menu UI 구현

### Week 8 — 세이브/로드 · 맵 전환 · 마무리 (5.18 ~ 5.27)
1. 게임 진행 상황 저장(GameData·World SaveData) 시스템 구현
2. SaveData Checkpoint 및 맵 로드 연동
3. 맵 간 이동(MapEntrance · ActorToLocation) 구현
4. 적 스폰 시스템 및 플레이어 사망 시 리스폰/로딩 처리
5. 루트(Loot) 시스템 구현
6. CustomDepth 정의 정리, HighlightInterface 개선
7. 게임 종료(Quit) 기능 및 전반적인 코드 리팩토링

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

### Gameplay Ability System (GAS) 아키텍처
- ASC(Ability System Component)의 Owner / Avatar 모델과 PlayerState·Character 배치 전략
- GameplayAbility의 라이프사이클(Activate → Commit → End)과 입력 태그 기반 발동 구조
- GameplayEffect의 Instant / Duration / Infinite 정책과 Modifier 적용
- AttributeSet의 `PreAttributeChange` / `PostGameplayEffectExecute` 훅을 통한 클램핑·파생 계산
- `ATTRIBUTE_ACCESSORS` 매크로를 활용한 Attribute 정의 패턴

### 데미지 & 전투 파이프라인
- `GameplayEffectExecutionCalculation(ExecCalc)`을 이용한 커스텀 데미지 산출
- Capture된 Attribute(Armor, Penetration, Block, Critical)로 피해 보정 로직 구현
- `FGameplayEffectContext` 확장 및 `NetSerialize` 커스터마이징으로 Block/Critical 정보 복제
- 구조화된 `DamageEffectParams`로 데미지·디버프·임펄스 파라미터 일괄 전달

### 네트워크 복제(Replication)
- Attribute `RepNotify`(`OnRep_`) 기반 클라이언트 동기화
- 멀티플레이 환경에서의 어빌리티 발동·패시브 스펠 동기화 이슈 해결
- DeathImpulse / Knockback 등 물리 효과의 서버 권한 처리

### GameplayTag 시스템
- Native GameplayTag 선언 및 `InitializeNativeGameplayTags()` 패턴
- Type / Status / Input / Attribute 태그 계층 설계
- 태그를 매개로 한 데이터 에셋·위젯·어빌리티 간 느슨한 결합

### 데이터 주도 설계 (Data-Driven)
- DataAsset / DataTable / CurveTable을 통한 레벨 스케일링 및 메타데이터 분리
- AssetManager 등록을 통한 어빌리티·태그 초기화
- 데이터 에셋 기반 Attribute 메뉴·스펠 트리 구성

### UI 아키텍처 (WidgetController)
- WidgetController를 통한 Model(ASC/Attribute) ↔ View(Widget) 분리
- 델리게이트·Attribute 변경 브로드캐스트로 UI 갱신
- HUD, Attribute 메뉴, Spell Tree, Globe(HP/MP), Start/Load 메뉴 구현

### 적 AI
- AIController + BehaviorTree + Blackboard 기반 의사결정
- 근접/원거리/마법(소환) 등 적 타입별 행동 분기
- AI의 어빌리티 시전 및 미니언 소환(SpawnMonster) 로직

### 세이브 / 로드 & 월드 관리
- SaveGame 객체 기반 게임 진행·월드 상태 직렬화
- Checkpoint 저장 및 맵 전환(MapEntrance) 간 데이터 유지
- 적 스폰·루트(Loot) 시스템과 플레이어 사망 후 리스폰/로딩 처리

### 비주얼 & 기타
- Niagara 기반 스펠 이펙트 및 GameplayCue 연동
- 커스텀 뎁스(CustomDepth) 기반 적 하이라이트 및 지형 Fade 처리
- Ability Task / Async Task, Blueprint Function Library 활용

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
