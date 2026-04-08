# Claude Code 프로젝트 가이드 — UE5TopDownRPG

## MCP 사용 시 필독

UE5 에디터와 연동해 데이터 에셋·블루프린트 등을 자동 편집할 때는  
반드시 아래 가이드를 먼저 읽고 작업할 것:

**`Scripts/MCP_UE5_Python_Guide.md`**

### 핵심 규칙 (요약)

- 멀티라인 Python → `run_python_file` 사용 (파일 먼저 Write 후 실행)
- `execute_python` → 세미콜론(`;`) 단일 라인만 가능
- 실행 결과 → `Saved/Logs/Aura.log` 에서 Grep으로 확인
- USTRUCT 프로퍼티 편집 → `export_text()`로 포맷 확인 후 `import_text()`로 세팅
- `FGameplayTag.TagName` 은 read-only → `import_text()` 로 우회
- UE 내부 에셋 경로 형식 → `/Game/경로/에셋명` (확장자 없음)

---

## 프로젝트 구조

```
Source/Aura/
├── Public/
│   ├── AuraGameplayTags.h              # Gameplay Tag 선언
│   ├── AbilitySystem/
│   │   ├── AuraAttributeSet.h          # 속성 정의 (ATTRIBUTE_ACCESSORS)
│   │   └── Data/AttributeInfo.h        # FAuraAttributeInfo 구조체
│   └── UI/Widget/
│       └── AttributeWidgetMenuController.h
├── Private/
│   ├── AuraGameplayTags.cpp            # InitializeNativeGameplayTags() — 태그 16개 정의
│   ├── AbilitySystem/Data/AttributeInfo.cpp
│   └── UI/Widget/AttributeWidgetMenuController.cpp

Content/Blueprints/AbilitySystem/Data/
└── DA_AttributeInfo.uasset             # 속성 메타데이터 데이터 에셋 (16개 항목)

Scripts/
├── MCP_UE5_Python_Guide.md             # MCP Python 실행 가이드
└── populate_da_attribute_info.py       # DA_AttributeInfo 16개 속성 추가 스크립트
```

---

## Gameplay Tag → 속성 매핑

| Gameplay Tag | 속성명 | 카테고리 |
|---|---|---|
| Attribute.Primary.Strength | Strength | Primary |
| Attribute.Primary.Intelligence | Intelligence | Primary |
| Attribute.Primary.Resilience | Resilience | Primary |
| Attribute.Primary.Vigor | Vigor | Primary |
| Attribute.Secondary.Armor | Armor | Secondary |
| Attribute.Secondary.ArmorPenetration | Armor Penetration | Secondary |
| Attribute.Secondary.BlockChance | Block Chance | Secondary |
| Attribute.Secondary.CriticalHitChance | Critical Hit Chance | Secondary |
| Attribute.Secondary.CriticalHitDamage | Critical Hit Damage | Secondary |
| Attribute.Secondary.CriticalHitReduction | Critical Hit Reduction | Secondary |
| Attribute.Secondary.HealthRegeneration | Health Regeneration | Secondary |
| Attribute.Secondary.ManaRegeneration | Mana Regeneration | Secondary |
| Attribute.Secondary.MaxHealth | Max Health | Secondary |
| Attribute.Secondary.MaxMana | Max Mana | Secondary |
| Attribute.Vital.Health | Health | Vital |
| Attribute.Vital.Mana | Mana | Vital |
