#include <ArchiveParser/ArchiveParser.h>
#include <ArchiveParser/ArchiveReader.h>
#include <ArchiveParser/MetadataMapper.h>
#include <iostream>
#include <tinyxml2/tinyxml2.h>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>

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

template <typename T>
void forEachFile(const Archive::ArchivePath& path, bool recursiveSearch, const T& callback)
{
	size_t childFiles = path.ChildFiles();
	size_t childDirectories = path.ChildDirectories();

	for (size_t i = 0; i < childFiles; ++i)
		callback(path.ChildFile(i));

	if (recursiveSearch)
		for (size_t i = 0; i < childDirectories; ++i)
			forEachFile(path.ChildDirectory(i), recursiveSearch, callback);
}

struct XmlAttribute
{
	std::string Name;
};

struct XmlElement
{
	std::string Name;

	std::vector<XmlAttribute> Attributes;
	std::vector<XmlElement> Children;
};

FILE* fmemopen(void* buf, size_t len, const char* type)
{
	int fd;
	FILE* fp;
	char tp[MAX_PATH - 13];
	char fn[MAX_PATH + 1];
	char tfname[] = "MemTF_";
	if (!GetTempPathA(sizeof(tp), tp))
		return NULL;
	if (!GetTempFileNameA(tp, tfname, 0, fn))
		return NULL;
	int* pfd = &fd;
	int retner = -1;
	retner = _sopen_s(pfd, fn, _O_CREAT | _O_SHORT_LIVED | _O_TEMPORARY | _O_RDWR | _O_BINARY | _O_NOINHERIT, _SH_DENYRW, _S_IREAD | _S_IWRITE);
	if (retner != 0)
		return NULL;
	if (fd == -1)
		return NULL;
	fp = _fdopen(fd, "wb+");
	if (!fp) {
		_close(fd);
		return NULL;
	}
	/*File descriptors passed into _fdopen are owned by the returned FILE * stream.If _fdopen is successful, do not call _close on the file descriptor.Calling fclose on the returned FILE * also closes the file descriptor.*/
	fwrite(buf, len, 1, fp);
	rewind(fp);
	return fp;
}

//FILE* fmemopen()
//{
//	wchar_t tp[MAX_PATH - 13];
//	wchar_t fn[MAX_PATH + 1];
//	wchar_t tfname[] = L"MemTF_";
//	if (!GetTempPathW(sizeof(tp), tp))
//		return NULL;
//	if (!GetTempFileNameW(tp, tfname, 0, fn))
//		return NULL;
//
//	auto c = CreateFile(tp, 0, 0, 0, 0, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, 0);
//
//	int fd = _open_osfhandle((intptr_t)c, _O_RDWR);
//}

void VisitAttribute(const tinyxml2::XMLAttribute* attribute, XmlAttribute& schema)
{

}

void VisitElement(tinyxml2::XMLElement* element, XmlElement& schema)
{
	for (const tinyxml2::XMLAttribute* attribute = element->FirstAttribute(); attribute; attribute = attribute->Next())
	{
		size_t index = (size_t)-1;
		const char* name = attribute->Name();

		for (size_t i = 0; i < schema.Attributes.size() && index == (size_t)-1; ++i)
			if (schema.Attributes[i].Name == name)
				index = i;

		if (index == (size_t)-1)
		{
			index = schema.Attributes.size();
			schema.Attributes.push_back({});
			schema.Attributes[index].Name = name;
		}

		VisitAttribute(attribute, schema.Attributes[index]);
	}

	for (tinyxml2::XMLElement* child = element->FirstChildElement(); child; child = child->NextSiblingElement())
	{
		size_t index = (size_t)-1;
		const char* name = child->Name();

		if (strcmp(name, "kfm") == 0)
			name += 0;

		for (size_t i = 0; i < schema.Children.size() && index == (size_t)-1; ++i)
			if (schema.Children[i].Name == name)
				index = i;

		if (index == (size_t)-1)
		{
			index = schema.Children.size();
			schema.Children.push_back({});
			schema.Children[index].Name = name;
		}

		VisitElement(child, schema.Children[index]);
	}
}

int main()
{
	
	fs::path ms2Root = "B:/Files/Maplstory 2 Client/appdata/";

	if (!fs::exists(ms2Root)) return -1;

	Archive::ArchiveReader reader(ms2Root / "Data", false);

	Archive::ArchivePath path = reader.GetPath("Xml/additionaleffect/", true);
	
	std::string output;
	XmlElement schema;

	const auto visit = [&output, &schema](const Archive::ArchivePath& path)
	{
		output.clear();

		path.Read(output);

		if (output == "")
			return;

		//GetTempPath()
		FILE* file = fmemopen(output.data(), output.size(), nullptr);

		tinyxml2::XMLDocument document;

		document.LoadFile(file);

		tinyxml2::XMLElement* rootElement = document.RootElement();

		VisitElement(rootElement, schema);

		fclose(file);
	};

	forEachFile(path, true, visit);
	//
	//size_t childFiles = path.ChildFiles();
	//size_t childDirectories = path.ChildDirectories();
	//
	//std::string output;
	//path.Read(output);

	//fs::path webMetaCache = "./cache/asset-web-metadata-cache";
	//
	//if (!Archive::Metadata::Entry::LoadCached(webMetaCache))
	//{
	//	Archive::Metadata::Entry::LoadEntries(reader, "Resource/asset-web-metadata");
	//	Archive::Metadata::Entry::Cache(webMetaCache);
	//}

	int i = 0;
	i += 0;
}