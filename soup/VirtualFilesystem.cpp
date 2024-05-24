#include "VirtualFilesystem.hpp"

#if (SOUP_WINDOWS && !SOUP_CROSS_COMPILE) || SOUP_LINUX

#if SOUP_WINDOWS
#include <Objbase.h> // CoCreateGuid
#pragma comment(lib, "Ole32.lib") // CoCreateGuid

#include "Exception.hpp"
#include "string.hpp"
#include "unicode.hpp"
#else
#include <cstring> // memset
#include <fcntl.h> // O_ACCMODE
#include <signal.h>
#include <sys/stat.h>

#include "macros.hpp" // COUNT

using fuse_main_real_t = void(*)(int argc, char* argv[], const struct fuse_operations* op, size_t op_size, void* private_data);
using fuse_get_context_t = struct fuse_context*(*)();

using fuse_fill_dir_t = int(*)(void*, const char* name, const struct stat* stbuf, off_t off);

struct fuse_context
{
	struct fuse* fuse;
	uid_t uid;
	gid_t gid;
	pid_t pid;
	void* private_data;
	mode_t umask;
};

// Can be found in /usr/include/fuse/fuse.h after `sudo apt install libfuse-dev`
struct fuse_operations
{
	int(*getattr)(const char*, struct stat*) = nullptr; // 0
	void* _1 = nullptr;
	void* _2 = nullptr;
	void* _3 = nullptr;
	void* _4 = nullptr;
	void* _5 = nullptr;
	void* _6 = nullptr;
	void* _7 = nullptr;
	void* _8 = nullptr;
	void* _9 = nullptr;
	void* _10 = nullptr;
	void* _11 = nullptr;
	void* _12 = nullptr;
	void* _13 = nullptr;
	int(*open)(const char*, struct fuse_file_info*) = nullptr; // 14
	int(*read)(const char*, char*, size_t, off_t, struct fuse_file_info*) = nullptr; // 15
	void* _16 = nullptr;
	void* _17 = nullptr;
	void* _18 = nullptr;
	void* _19 = nullptr;
	void* _20 = nullptr;
	void* _21 = nullptr;
	void* _22 = nullptr;
	void* _23 = nullptr;
	void* _24 = nullptr;
	void* _25 = nullptr;
	int(*readdir)(const char*, void*, fuse_fill_dir_t, off_t) = nullptr; // 26
};

// /use/include/fuse/fuse_common.h
struct fuse_file_info
{
	int flags;
};
#endif

NAMESPACE_SOUP
{
	bool VirtualFilesystem::isAvailable()
	{
		return lib.isLoaded()
#if SOUP_WINDOWS
			|| lib.load("projectedfslib")
#else
			|| lib.load("libfuse.so.2")
#endif
			;
	}

#if SOUP_WINDOWS
	static HRESULT __stdcall StartDirectoryEnumerationCallback(_In_ const PRJ_CALLBACK_DATA* callbackData, _In_ const GUID* enumerationId)
	{
		auto vfs = static_cast<VirtualFilesystem*>(callbackData->InstanceContext);
		vfs->dir_enums.emplace_back(VirtualFilesystem::DirectoryEnumeration{
			*enumerationId,
			vfs->getDirectoryContents(unicode::utf16_to_utf8<std::wstring>(callbackData->FilePathName))
		});
		return S_OK;
	}

	static HRESULT __stdcall EndDirectoryEnumerationCallback(_In_ const PRJ_CALLBACK_DATA* callbackData, _In_ const GUID* enumerationId)
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

	static HRESULT __stdcall GetDirectoryEnumerationCallback(_In_ const PRJ_CALLBACK_DATA* callbackData, _In_ const GUID* enumerationId, _In_opt_ PCWSTR searchExpression, _In_ PRJ_DIR_ENTRY_BUFFER_HANDLE dirEntryBufferHandle)
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

	static HRESULT __stdcall GetPlaceholderInfoCallback(_In_ const PRJ_CALLBACK_DATA* callbackData)
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

	static HRESULT __stdcall GetFileDataCallback(_In_ const PRJ_CALLBACK_DATA* callbackData, _In_ UINT64 byteOffset, _In_ UINT32 length)
	{
		auto vfs = static_cast<VirtualFilesystem*>(callbackData->InstanceContext);
		auto path = unicode::utf16_to_utf8<std::wstring>(callbackData->FilePathName);
		void* buf = vfs->lib.getAddress<decltype(PrjAllocateAlignedBuffer)*>("PrjAllocateAlignedBuffer")(callbackData->NamespaceVirtualizationContext, length);
		vfs->getFileContents(path, buf, static_cast<size_t>(byteOffset), length);
		vfs->lib.getAddress<decltype(PrjWriteFileData)*>("PrjWriteFileData")(callbackData->NamespaceVirtualizationContext, &callbackData->DataStreamId, buf, byteOffset, length);
		vfs->lib.getAddress<decltype(PrjFreeAlignedBuffer)*>("PrjFreeAlignedBuffer")(buf);
		return S_OK;
	}
#else
	static fuse_get_context_t fuse_get_context;

	static int virt_getattr(const char* path, struct stat* stbuf)
	{
		memset(stbuf, 0, sizeof(struct stat));

		if (strcmp(path, "/") == 0)
		{
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 1;
			return 0;
		}

		auto vfs = static_cast<VirtualFilesystem*>(fuse_get_context()->private_data);

		auto target_path = std::filesystem::path(path);
		auto target_name = target_path.filename().string();
		for (const auto& fi : vfs->getDirectoryContents(target_path.parent_path().string().substr(1)))
		{
			if (fi.name == target_name)
			{
				stbuf->st_nlink = 1;
				if (fi.is_dir)
				{
					stbuf->st_mode = S_IFDIR | 0755;
				}
				else
				{
					stbuf->st_mode = S_IFREG | 0444;
					stbuf->st_size = fi.size;
				}
				return 0;
			}
		}

		return -ENOENT;
	}

	static int virt_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset)
	{
		auto vfs = static_cast<VirtualFilesystem*>(fuse_get_context()->private_data);

		filler(buf, ".", nullptr, 0);
		filler(buf, "..", nullptr, 0);
		for (const auto& fi : vfs->getDirectoryContents(path + 1))
		{
			filler(buf, fi.name.c_str(), nullptr, 0);
		}
		return 0;
	}

	static int virt_open(const char* path, struct fuse_file_info* fi)
	{
		if ((fi->flags & O_ACCMODE) != O_RDONLY)
		{
			return -EACCES;
		}

		return 0;
	}

	static int virt_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info*)
	{
		auto vfs = static_cast<VirtualFilesystem*>(fuse_get_context()->private_data);
		return vfs->getFileContents(path + 1, buf, offset, size);
	}

	static struct fuse_operations virt_oper = {
		.getattr = virt_getattr,
		.open = virt_open,
		.read = virt_read,
		.readdir = virt_readdir,
	};

	struct CaptureVfsMount
	{
		VirtualFilesystem* vfs;
		std::string root;
	};
#endif

	void VirtualFilesystem::mount(const std::filesystem::path& root)
	{
#if SOUP_WINDOWS
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
#else
		SOUP_ASSERT(isAvailable());

		std::filesystem::create_directory(root);

		fuse_get_context = (fuse_get_context_t)lib.getAddressMandatory("fuse_get_context");

		thrd.start([](Capture&& _cap)
		{
			auto& cap = _cap.get<CaptureVfsMount>();

			const char *argv[] = { "", "-f", cap.root.c_str() };
			((fuse_main_real_t)cap.vfs->lib.getAddressMandatory("fuse_main_real"))(COUNT(argv), const_cast<char**>(argv), &virt_oper, sizeof(virt_oper), cap.vfs);
		}, CaptureVfsMount{
			this,
			root.string()
		});
#endif
	}

	bool VirtualFilesystem::isMounted() const noexcept
	{
#if SOUP_WINDOWS
		return ctx != nullptr;
#else
		return thrd.isRunning();
#endif
	}

	void VirtualFilesystem::unmount()
	{
#if SOUP_WINDOWS
		lib.getAddress<decltype(PrjStopVirtualizing)*>("PrjStopVirtualizing")(ctx);

		ctx = nullptr;
#else
		pthread_kill(thrd.handle, SIGINT); // Send Ctrl+C to the FUSE thread
		thrd.awaitCompletion();
#endif
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
