#include <iostream>
#include <string>
#include <sstream>  
#include <filesystem>  

namespace disk_stats 
{

//-------------------------------------------------------------------------

namespace fs = std::experimental::filesystem;

//-------------------------------------------------------------------------

std::uintmax_t ComputeFileSize(const fs::path& pathToCheck)
{
	if (fs::exists(pathToCheck) && fs::is_regular_file(pathToCheck))
	{
		auto err = std::error_code{};
		auto filesize = fs::file_size(pathToCheck, err);
		if (filesize != static_cast<uintmax_t>(-1))
			return filesize;
	}

	return static_cast<uintmax_t>(-1);
}

//-------------------------------------------------------------------------

std::uintmax_t directorySize(const fs::path& directory)
{
	std::uintmax_t size{ 0 };
	for (const auto& entry : fs::recursive_directory_iterator(directory))
	{
		auto file_path = entry.path();

		if (fs::is_regular_file(entry.status()))
		{
			size += ComputeFileSize(file_path);
		}

		if (fs::is_directory(entry.status()))
		{
			directorySize(entry);
		}
	}
	return size;
}

//-------------------------------------------------------------------------

void DisplayFileInfo(
	const std::experimental::filesystem::v1::directory_entry & entry,
	std::string &lead, 
	std::experimental::filesystem::v1::path &filename)
{
	time_t cftime = std::chrono::system_clock::to_time_t(fs::last_write_time(entry));
	std::cout << lead << filename << ", "
		<< ComputeFileSize(entry) << " bytes, "
        << "time: " << std::asctime(std::localtime(&cftime)) << "\n";
}

//-------------------------------------------------------------------------

void DisplayDirectoryTreeImp(const fs::path& pathToShow, int level)
{
	if (fs::exists(pathToShow) && fs::is_directory(pathToShow))
	{
		auto lead = std::string(level * 3, ' ');
		for (const auto& entry : fs::directory_iterator(pathToShow))
		{
			auto filename = entry.path().filename();
			if (fs::is_directory(entry.status()))
			{
				std::cout << lead << filename << " [DIR] " 
					 << directorySize(entry) << " bytes" << "\n";
				DisplayDirectoryTreeImp(entry, level + 1);
				std::cout << "\n";
			}
			else if (fs::is_regular_file(entry.status()))
				DisplayFileInfo(entry, lead, filename);
			else
				std::cout << lead << " [?]" << filename << "\n";
		}
	}
}

//-------------------------------------------------------------------------

void DisplayDirectoryTree(const fs::path& pathToShow)
{
	DisplayDirectoryTreeImp(pathToShow, 0);
}

//-------------------------------------------------------------------------

}

//-------------------------------------------------------------------------

int main()
{
	std::string path = "D:\\root";

	disk_stats::DisplayDirectoryTree(path);

	system("pause");
	return 0;
}

//-------------------------------------------------------------------------