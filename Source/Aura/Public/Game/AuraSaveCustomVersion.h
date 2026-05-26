// CopyrightHATO

#pragma once

#include "CoreMinimal.h"

/**
 *  Aura 세이브 데이터 직렬화 버전.
 *  FObjectAndNameAsStringProxyArchive 사용 시 Archive.UsingCustomVersion(FAuraSaveCustomVersion::GUID)로 등록한다.
 *  필드/구조체 형태가 바뀔 때마다 새 enum 값을 LatestVersion 위에 추가하고,
 *  로드 측에서 Ar.CustomVer(FAuraSaveCustomVersion::GUID)로 분기 처리한다.
 */
struct FAuraSaveCustomVersion
{
	enum Type
	{
		// 커스텀 버전이 도입되기 전 세이브.
		BeforeCustomVersionWasAdded = 0,

		// 최초 버전.
		InitialVersion = 1,

		// -- 새 버전은 이 줄 위에 추가하세요 --
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	static const FGuid GUID;

private:
	FAuraSaveCustomVersion() {}
};
