#include "uiutils.h"


bool uiutils::is_of_format(std::string str, file_format ff)
{
	switch (ff)
	{
	case uiutils::file_format::IMAGE:
		for (auto i : image)
		{
			if (str == "." + i)
			{
				return true;
			}
		}
		break;
	case uiutils::file_format::TEXT:
		for (auto i : text)
		{
			if (str == "." + i)
			{
				return true;
			}
		}
		break;
	case uiutils::file_format::SHADER:
		for (auto i : shader)
		{
			if (str == "." + i)
			{
				return true;
			}
		}
		break;
	case uiutils::file_format::MODEL:
		for (auto i : model)
		{
			if (str == "." + i)
			{
				return true;
			}
		}
		break;
	case uiutils::file_format::ALL:
		return true;
		break;
	default:
		return false;
		break;
	}
	return false;
}

std::string uiutils::UI::queryFile(std::string file_to_locate, file_format format /*=file_format::ALL*/)
{
	std::cout << "Please locate file [" << file_to_locate << "]:" << std::endl;
	std::filesystem::path p(m_LastUsedPath);
	while (true)
	{
		bool directory_bool = (std::filesystem::is_directory(std::filesystem::status(p)));
		std::string directory_str;
		directory_str = directory_bool ? "is a directory." : "is a file.";

		if (directory_bool)
		{
			std::cout << "currently in [" << p.filename().string() << "] " << std::endl;
			int i = 0;
			std::cout << "0:	../" << std::endl;
			for (const auto& entry : std::filesystem::directory_iterator(p))
			{
				bool directory_bool = (std::filesystem::is_directory(entry.status()));
				std::string extension_str = entry.path().extension().string();
				if ((!directory_bool) && (!is_of_format(extension_str, format)))
				{
					continue;
				}
				i++;
				std::string directory_str;
				directory_str = directory_bool ? "(d)" : "(f)";
				auto fname = entry.path().filename().string();
				std::cout << i << ":	" << fname << " " << directory_str << std::endl;
			}
			std::cout << i + 1 << ":	custom file or directory" << std::endl;
			int a;
			std::cout << "Pick directory or file index (-1 for exit): ";
			std::cin >> a;
			if (a == -1)
			{
				return "";
			}
			if (a == 0)
			{
				p = p.parent_path();
			}
			else
			{
				int i = 0;
				for (const auto& entry : std::filesystem::directory_iterator(p))
				{
					bool directory_bool = (std::filesystem::is_directory(entry.status()));
					std::string extension_str = entry.path().extension().string();
					if ((!directory_bool) && (!is_of_format(extension_str, format)))
					{
						continue;
					}
					i++;
					if (a == i)
					{
						p = entry.path();
						break;
					}
				}
				if (a == i + 1)
				{
					while (true)
					{
						std::string dir;
						std::cout << "Enter custom directory (-1 for exit): ";
						std::cin >> dir;
						if (dir == "-1")
						{
							break;
						}
						std::filesystem::path t(dir);
						if (std::filesystem::exists(t))
						{
							bool directory_bool = (std::filesystem::is_directory(t));
							std::string extension_str = t.extension().string();
							if ((!directory_bool) && (!is_of_format(extension_str, format)))
							{
								std::cout << t.string() << " is not the correct format. ";
								continue;
							}
							p = t;
							break;
						}
						else
						{
							std::cout << t.string() << " does not exist. ";
						}
					}
				}
			}
		}
		else
		{
			m_LastUsedPath = p.parent_path();
			return p.string();
		}
	}
}
