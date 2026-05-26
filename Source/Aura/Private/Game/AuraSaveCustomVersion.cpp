// CopyrightHATO


#include "Game/AuraSaveCustomVersion.h"

#include "Serialization/CustomVersion.h"

const FGuid FAuraSaveCustomVersion::GUID(0xA8E12F00, 0x4B1A4D9C, 0xB3F7E25A, 0x9C8D1E0F);

// 엔진 CustomVersion 레지스트리에 등록.
FCustomVersionRegistration GRegisterAuraSaveCustomVersion(
	FAuraSaveCustomVersion::GUID,
	FAuraSaveCustomVersion::LatestVersion,
	TEXT("AuraSaveVersion"));
