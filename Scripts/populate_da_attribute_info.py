import unreal
import re
import os

# --- Config ---
CPP_PATH = os.path.join(unreal.Paths.project_dir(), 'Source', 'Aura', 'Private', 'AuraGameplayTags.cpp')
ASSET_PATH = '/Game/Blueprints/AbilitySystem/Data/DA_AttributeInfo'

# --- Step 1: Parse AuraGameplayTags.cpp ---
if not os.path.exists(CPP_PATH):
    unreal.log_error('[PopulateDA] File not found: ' + CPP_PATH)
else:
    with open(CPP_PATH, 'r', encoding='utf-8') as f:
        cpp_content = f.read()

    pattern = r'AddNativeGameplayTag\(\s*FName\("([^"]+)"\),\s*FString\("([^"]+)"\)\)'
    matches = re.findall(pattern, cpp_content)

    if not matches:
        unreal.log_error('[PopulateDA] No tags found in ' + CPP_PATH)
    else:
        unreal.log('[PopulateDA] Found {} tags'.format(len(matches)))

        # --- Step 2: Build FAuraAttributeInfo entries ---
        entries = []
        for tag_name, description in matches:
            # Extract last segment and convert CamelCase to spaced name
            last_segment = tag_name.rsplit('.', 1)[-1]
            attr_name = re.sub(r'(?<=[a-z])(?=[A-Z])', ' ', last_segment)

            entry = unreal.AuraAttributeInfo()
            import_str = '(AttributeTag=(TagName="{}"),AttributeName="{}",AttributeDescription="{}")'.format(
                tag_name, attr_name, description
            )
            entry.import_text(import_str)
            entries.append(entry)
            unreal.log('[PopulateDA]   + {} -> {}'.format(tag_name, attr_name))

        # --- Step 3: Load DataAsset and set entries ---
        data_asset = unreal.load_object(None, ASSET_PATH)
        if not data_asset:
            unreal.log_error('[PopulateDA] Failed to load asset: ' + ASSET_PATH)
        else:
            data_asset.set_editor_property('attribute_entries', entries)
            unreal.EditorAssetLibrary.save_asset(ASSET_PATH, only_if_is_dirty=False)
            unreal.log('[PopulateDA] Saved {} entries to {}'.format(len(entries), ASSET_PATH))
