
#ifndef _FS_H_INC_
#define _FS_H_INC_

#include <memory>
#include <string_view>

namespace fs
{
	struct IDirectoryEnum;

	struct IFileSystemEntry
	{
		virtual ~IFileSystemEntry() {}

		virtual std::string_view name() = 0;
		virtual std::shared_ptr<IDirectoryEnum> children() = 0;
	};

	struct IDirectoryEnum
	{
		virtual ~IDirectoryEnum() {}

		virtual void reset() = 0;
		virtual bool next( std::shared_ptr<IFileSystemEntry>& file ) = 0;
	};

	struct IFileSystem
	{
		virtual ~IFileSystem() {}

		virtual std::shared_ptr<IDirectoryEnum> root_directory() = 0;
	};
}

#endif // _FS_H_INC_
