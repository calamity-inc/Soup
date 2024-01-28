#include "VirtualFilesystem.hpp"

#if SOUP_WINDOWS
#include <Objbase.h> // CoCreateGuid
#pragma comment(lib, "Ole32.lib") // CoCreateGuid

#include "Exception.hpp"
#include "string.hpp"
#include "unicode.hpp"

namespace soup
{
	bool VirtualFilesystem::isAvailable()
	{
		return lib.isLoaded() || lib.load("projectedfslib");
	}

	static __stdcall HRESULT StartDirectoryEnumerationCallback(_In_ const PRJ_CALLBACK_DATA* callbackData, _In_ const GUID* enumerationId)
	{
		auto vfs = static_cast<VirtualFilesystem*>(callbackData->InstanceContext);
		vfs->dir_enums.emplace_back(VirtualFilesystem::DirectoryEnumeration{
			*enumerationId,
			vfs->getDirectoryContents(unicode::utf16_to_utf8<std::wstring>(callbackData->FilePathName))
		});
		return S_OK;
	}

	static __stdcall HRESULT EndDirectoryEnumerationCallback(_In_ const PRJ_CALLBACK_DATA* callbackData, _In_ const GUID* enumerationId)
	{
		auto vfs = static_cast<VirtualFilesystem*>(callbackData->InstanceContext);
		for (auto i = vfs->dir_enums.begin(); i != vfs->dir_enums.end(); ++i)
		{
			if (i->guid == *enumerationId)
			{
				vfs->dir_enums.erase(i);
				return S_OK;
			}
		}
		return E_INVALIDARG;
	}

	static decltype(PrjFileNameCompare)* fpPrjFileNameCompare;

	static __stdcall HRESULT GetDirectoryEnumerationCallback(_In_ const PRJ_CALLBACK_DATA* callbackData, _In_ const GUID* enumerationId, _In_opt_ PCWSTR searchExpression, _In_ PRJ_DIR_ENTRY_BUFFER_HANDLE dirEntryBufferHandle)
	{
		auto vfs = static_cast<VirtualFilesystem*>(callbackData->InstanceContext);
		auto& dir_enum = vfs->getDirectoryEnumeration(*enumerationId);

		if (dir_enum.index == 0)
		{
			if (*searchExpression != '\0')
			{
				// Search expression is only given first time this is called, so remove all non-matching results now.
				auto fpPrjFileNameMatch = vfs->lib.getAddress<decltype(PrjFileNameMatch)*>("PrjFileNameMatch");
				for (auto i = dir_enum.files.begin(); i != dir_enum.files.end(); )
				{
					auto name_utf16 = unicode::utf8_to_utf16(i->name);
					if (fpPrjFileNameMatch(name_utf16.c_str(), searchExpression))
					{
						++i;
					}
					else
					{
						i = dir_enum.files.erase(i);
					}
				}
			}

			std::sort(dir_enum.files.begin(), dir_enum.files.end(), [](const VirtualFilesystem::FileInfo& a, const VirtualFilesystem::FileInfo& b)
			{
				auto a_name_utf16 = unicode::utf8_to_utf16(a.name);
				auto b_name_utf16 = unicode::utf8_to_utf16(b.name);
				return fpPrjFileNameCompare(a_name_utf16.c_str(), b_name_utf16.c_str()) < 0;
			});
		}

		while (dir_enum.index != dir_enum.files.size())
		{
			auto& file = dir_enum.files.at(dir_enum.index++);

			auto name_utf16 = unicode::utf8_to_utf16(file.name);

			PRJ_FILE_BASIC_INFO prj_info{};
			prj_info.FileSize = file.size;
			prj_info.IsDirectory = file.is_dir;
			prj_info.FileAttributes = FILE_ATTRIBUTE_READONLY;
			if (vfs->fpPrjFillDirEntryBuffer(name_utf16.c_str(), &prj_info, dirEntryBufferHandle) != S_OK)
			{
				break;
			}
		}
		return S_OK;
	}

	static __stdcall HRESULT GetPlaceholderInfoCallback(_In_ const PRJ_CALLBACK_DATA* callbackData)
	{
		auto vfs = static_cast<VirtualFilesystem*>(callbackData->InstanceContext);
		auto target_utf16 = std::wstring(callbackData->FilePathName);
		auto target_path = std::filesystem::path(target_utf16);
		auto target_utf8 = string::fixType(target_path.filename().u8string());
		for (const auto& file : vfs->getDirectoryContents(string::fixType(target_path.parent_path().u8string())))
		{
			if (file.name == target_utf8)
			{
				PRJ_PLACEHOLDER_INFO phinfo{};
				phinfo.FileBasicInfo.FileSize = file.size;
				phinfo.FileBasicInfo.IsDirectory = file.is_dir;
				phinfo.FileBasicInfo.FileAttributes = FILE_ATTRIBUTE_READONLY;
				return vfs->fpPrjWritePlaceholderInfo(callbackData->NamespaceVirtualizationContext, callbackData->FilePathName, &phinfo, sizeof(phinfo));
			}
		}
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}

	static __stdcall HRESULT GetFileDataCallback(_In_ const PRJ_CALLBACK_DATA* callbackData, _In_ UINT64 byteOffset, _In_ UINT32 length)
	{
		auto vfs = static_cast<VirtualFilesystem*>(callbackData->InstanceContext);
		auto path = unicode::utf16_to_utf8<std::wstring>(callbackData->FilePathName);
		void* buf = vfs->lib.getAddress<decltype(PrjAllocateAlignedBuffer)*>("PrjAllocateAlignedBuffer")(callbackData->NamespaceVirtualizationContext, length);
		vfs->getFileContents(path, buf, byteOffset, length);
		vfs->lib.getAddress<decltype(PrjWriteFileData)*>("PrjWriteFileData")(callbackData->NamespaceVirtualizationContext, &callbackData->DataStreamId, buf, byteOffset, length);
		vfs->lib.getAddress<decltype(PrjFreeAlignedBuffer)*>("PrjFreeAlignedBuffer")(buf);
		return S_OK;
	}

	void VirtualFilesystem::mount(const std::filesystem::path& root)
	{
		if (!isAvailable())
		{
			SOUP_THROW(Exception("Virtual filesystem cannot be mounted. Ensure you're on Windows 10 version 1809 or higher and the \"Windows Projected File System\" feature is enabled."));
		}
		fpPrjFileNameCompare = lib.getAddress<decltype(PrjFileNameCompare)*>("PrjFileNameCompare");
		fpPrjFillDirEntryBuffer = lib.getAddress<decltype(PrjFillDirEntryBuffer)*>("PrjFillDirEntryBuffer");
		fpPrjWritePlaceholderInfo = lib.getAddress<decltype(PrjWritePlaceholderInfo)*>("PrjWritePlaceholderInfo");

		std::filesystem::create_directory(root);

		GUID guid;
		CoCreateGuid(&guid);
		lib.getAddress<decltype(PrjMarkDirectoryAsPlaceholder)*>("PrjMarkDirectoryAsPlaceholder")(root.wstring().c_str(), nullptr, nullptr, &guid);

		PRJ_CALLBACKS cbs{};
		cbs.StartDirectoryEnumerationCallback = StartDirectoryEnumerationCallback;
		cbs.EndDirectoryEnumerationCallback = EndDirectoryEnumerationCallback;
		cbs.GetDirectoryEnumerationCallback = GetDirectoryEnumerationCallback;
		cbs.GetPlaceholderInfoCallback = GetPlaceholderInfoCallback;
		cbs.GetFileDataCallback = GetFileDataCallback;
		lib.getAddress<decltype(PrjStartVirtualizing)*>("PrjStartVirtualizing")(root.wstring().c_str(), &cbs, this, nullptr, &ctx);
	}

	void VirtualFilesystem::unmount()
	{
		lib.getAddress<decltype(PrjStopVirtualizing)*>("PrjStopVirtualizing")(ctx);

		ctx = nullptr;
	}

	VirtualFilesystem::~VirtualFilesystem()
	{
		if (isMounted())
		{
			unmount();
		}
	}
}
#endif
