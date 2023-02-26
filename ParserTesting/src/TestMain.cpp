#include <ArchiveParser/ArchiveParser.h>
#include <ArchiveParser/ArchiveReader.h>
#include <ArchiveParser/MetadataMapper.h>
#include <iostream>

template <typename T>
void forEachFile(const fs::path& path, bool recursiveSearch, const T& callback)
{
	for (auto& file : fs::directory_iterator(path))
	{
		if (file.is_directory())
		{
			if (recursiveSearch)
				forEachFile(file.path(), true, callback);

			continue;
		}

		callback(file.path());
	}
}

void traverse(const Archive::ArchivePath& path)
{
	std::cout << path.GetPath() << "\n";

	size_t childFiles = path.ChildFiles();
	size_t childDirectories = path.ChildDirectories();

	for (size_t i = 0; i < childFiles; ++i)
		traverse(path.ChildFile(i));

	for (size_t i = 0; i < childDirectories; ++i)
		traverse(path.ChildDirectory(i));
}

int main()
{

	fs::path ms2Root = "B:/Files/Maplstory 2 Client/appdata/";

	if (!fs::exists(ms2Root)) return -1;

	Archive::ArchiveReader reader(ms2Root / "Data", false);

	//Archive::ArchivePath path = reader.GetPath("Xml/npc/", true);
	//
	//traverse(path);
	//
	//size_t childFiles = path.ChildFiles();
	//size_t childDirectories = path.ChildDirectories();
	//
	//std::string output;
	//path.Read(output);

	fs::path webMetaCache = "./cache/asset-web-metadata-cache";

	if (!Archive::Metadata::Entry::LoadCached(webMetaCache))
	{
		Archive::Metadata::Entry::LoadEntries(reader, "Resource/asset-web-metadata");
		Archive::Metadata::Entry::Cache(webMetaCache);
	}

	int i = 0;
	i += 0;
}