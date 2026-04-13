#!/usr/bin/env python3
"""
Unreal Engine MCP Server
UE PythonScriptPlugin의 remote_execution.py를 직접 연동하는 순수 Python MCP 서버.
Aura 클라이언트/계정 불필요 — 언리얼 에디터가 열려있으면 바로 동작.

사전 조건:
  1. 언리얼 에디터 실행 중
  2. 프로젝트 플러그인에 PythonScriptPlugin 활성화 (Aura.uproject에 이미 추가됨)
  3. 에디터 설정 > Python > Enable Remote Execution 체크
"""

import sys
import os
import time
from pathlib import Path

# ─── UE remote_execution.py 경로 추가 ─────────────────────────────────────────
_UE_PYTHON_PATH = Path("D:/UnrealEngine/UE_5.7/Engine/Plugins/Experimental/PythonScriptPlugin/Content/Python")
if str(_UE_PYTHON_PATH) not in sys.path:
    sys.path.insert(0, str(_UE_PYTHON_PATH))

from fastmcp import FastMCP
from remote_execution import RemoteExecution, MODE_EXEC_STATEMENT, MODE_EVAL_STATEMENT

# ─── MCP 서버 초기화 ──────────────────────────────────────────────────────────
mcp = FastMCP(
    name="unreal-editor",
    instructions="언리얼 에디터와 직접 연동된 MCP 서버입니다. Python Remote Execution으로 에디터 내부 작업을 수행합니다.",
)

_DISCOVER_TIMEOUT = 5.0   # 에디터 노드 탐색 최대 대기 시간(초)
_DISCOVER_INTERVAL = 0.2  # 탐색 폴링 간격(초)


# ─── 내부 헬퍼 ───────────────────────────────────────────────────────────────

def _run_in_editor(code: str, mode: str = MODE_EXEC_STATEMENT, timeout: float = _DISCOVER_TIMEOUT) -> str:
    """에디터에서 Python 코드를 실행하고 출력 문자열을 반환한다."""
    remote_exec = RemoteExecution()
    try:
        remote_exec.start()

        # 에디터 노드 탐색 대기
        deadline = time.time() + timeout
        while time.time() < deadline:
            if remote_exec.remote_nodes:
                break
            time.sleep(_DISCOVER_INTERVAL)

        nodes = remote_exec.remote_nodes
        if not nodes:
            return (
                "❌ 언리얼 에디터를 찾을 수 없습니다.\n"
                "확인 사항:\n"
                "  1. 언리얼 에디터가 실행 중인지 확인\n"
                "  2. Edit > Editor Preferences > Python > Enable Remote Execution 활성화\n"
                "  3. PythonScriptPlugin 플러그인이 활성화되어 있는지 확인 (Aura.uproject에 추가됨)"
            )

        node_id = nodes[0]["node_id"]
        remote_exec.open_command_connection(node_id)
        result = remote_exec.run_command(code, exec_mode=mode)
        remote_exec.close_command_connection()

        # result 구조: {"success": bool, "result": str, "output": [...]}
        if isinstance(result, dict):
            success = result.get("success", True)
            output_lines = result.get("output", [])
            # output은 [{"type": "stdout"/"stderr", "output": str}, ...] 형태
            if isinstance(output_lines, list):
                stdout = "\n".join(
                    o.get("output", "") for o in output_lines if o.get("type") == "stdout"
                )
                stderr = "\n".join(
                    o.get("output", "") for o in output_lines if o.get("type") == "stderr"
                )
            else:
                stdout = str(output_lines)
                stderr = ""

            if not success:
                return f"❌ 실행 오류:\n{stderr or stdout}"

            parts = []
            if stdout:
                parts.append(stdout)
            if stderr:
                parts.append(f"[stderr]\n{stderr}")
            return "\n".join(parts) if parts else "✅ 실행 완료 (출력 없음)"

        return str(result)

    except Exception as e:
        return f"❌ MCP 서버 오류: {e}"
    finally:
        try:
            remote_exec.stop()
        except Exception:
            pass


# ─── Tools ───────────────────────────────────────────────────────────────────

@mcp.tool()
def check_editor_connection() -> str:
    """언리얼 에디터 연결 상태를 확인합니다."""
    remote_exec = RemoteExecution()
    try:
        remote_exec.start()
        time.sleep(2.0)
        nodes = remote_exec.remote_nodes
        if nodes:
            ids = [n.get("node_id", "?") for n in nodes]
            return f"✅ 에디터 연결 성공! 노드 수: {len(nodes)}\n노드 ID: {ids}"
        return (
            "❌ 에디터 미연결\n"
            "  1. 언리얼 에디터 실행 중인지 확인\n"
            "  2. Edit > Editor Preferences > Python > Enable Remote Execution 활성화 필요"
        )
    finally:
        remote_exec.stop()


@mcp.tool()
def execute_python(code: str) -> str:
    """언리얼 에디터 안에서 Python 코드를 실행합니다.
    unreal 모듈을 자유롭게 사용할 수 있습니다.

    Args:
        code: 실행할 Python 코드 (멀티라인 가능)
    """
    return _run_in_editor(code, mode=MODE_EXEC_STATEMENT)


@mcp.tool()
def evaluate_python(expression: str) -> str:
    """언리얼 에디터에서 Python 표현식을 평가하고 결과값을 반환합니다.

    Args:
        expression: 평가할 단일 Python 표현식 (예: "unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).get_current_level().get_name()")
    """
    return _run_in_editor(expression, mode=MODE_EVAL_STATEMENT)


@mcp.tool()
def find_assets(search_path: str = "/Game", asset_class: str = "", recursive: bool = True) -> str:
    """에셋 레지스트리에서 에셋을 검색합니다.

    Args:
        search_path: 검색 시작 경로 (예: "/Game/Blueprints")
        asset_class: 필터링할 에셋 클래스명 (예: "Blueprint", "DataAsset"). 빈 문자열이면 전체 반환
        recursive: 하위 폴더까지 검색 여부
    """
    code = f"""
import unreal
ar = unreal.AssetRegistryHelpers.get_asset_registry()
assets = ar.get_assets_by_path('{search_path}', recursive={recursive})
result = []
for a in assets:
    cls = a.asset_class_path.asset_name if hasattr(a.asset_class_path, 'asset_name') else str(a.asset_class_path)
    if not '{asset_class}' or '{asset_class}'.lower() in cls.lower():
        result.append(f'{{str(a.package_name)}}  [{cls}]')
print(f'총 {{len(result)}}개')
print('\\n'.join(result[:100]))
"""
    return _run_in_editor(code.strip())


@mcp.tool()
def get_level_actors(class_filter: str = "") -> str:
    """현재 열린 레벨의 액터 목록을 가져옵니다.

    Args:
        class_filter: 필터링할 클래스명 (예: "AuraCharacter"). 빈 문자열이면 전체
    """
    code = f"""
import unreal
actors = unreal.EditorLevelLibrary.get_all_level_actors()
result = []
for a in actors:
    cls = a.get_class().get_name()
    name = a.get_name()
    if not '{class_filter}' or '{class_filter}'.lower() in cls.lower():
        result.append(f'{{name}}  ({{cls}})')
print(f'총 {{len(result)}}개')
print('\\n'.join(result[:200]))
"""
    return _run_in_editor(code.strip())


@mcp.tool()
def get_asset_info(asset_path: str) -> str:
    """특정 에셋의 정보와 프로퍼티를 가져옵니다.

    Args:
        asset_path: 에셋 경로 (예: "/Game/Blueprints/AbilitySystem/Data/DA_AttributeInfo")
    """
    code = f"""
import unreal
asset = unreal.load_asset('{asset_path}')
if asset is None:
    print('❌ 에셋을 찾을 수 없습니다: {asset_path}')
else:
    print(f'Name  : {{asset.get_name()}}')
    print(f'Class : {{asset.get_class().get_name()}}')
    print(f'Path  : {{asset.get_path_name()}}')
"""
    return _run_in_editor(code.strip())


@mcp.tool()
def run_python_file(file_path: str) -> str:
    """로컬에 저장된 Python 파일을 언리얼 에디터 안에서 실행합니다.

    Args:
        file_path: 실행할 Python 파일의 절대 경로
                   (예: E:/Git_Project/UE5TopDownRPG/Scripts/populate_da_attribute_info.py)
    """
    safe_path = file_path.replace("\\", "/")
    code = f'exec(open("{safe_path}", encoding="utf-8").read())'
    return _run_in_editor(code, mode=MODE_EXEC_STATEMENT)


@mcp.tool()
def save_asset(asset_path: str) -> str:
    """특정 에셋을 저장합니다.

    Args:
        asset_path: 저장할 에셋 경로 (예: "/Game/Blueprints/AbilitySystem/Data/DA_AttributeInfo")
    """
    code = f"""
import unreal
result = unreal.EditorAssetLibrary.save_asset('{asset_path}', only_if_is_dirty=False)
print('✅ 저장 완료' if result else '⚠️ 저장 실패 (에셋이 없거나 이미 최신 상태)')
"""
    return _run_in_editor(code.strip())


@mcp.tool()
def save_all_dirty_assets() -> str:
    """수정된 모든 에셋을 저장합니다."""
    code = """
import unreal
unreal.EditorAssetLibrary.save_directory('/Game', only_if_is_dirty=True, recursive=True)
print('✅ 전체 에셋 저장 완료')
"""
    return _run_in_editor(code.strip())


# ─── 엔트리포인트 ─────────────────────────────────────────────────────────────

if __name__ == "__main__":
    mcp.run()
