// Copyright David Bundgaard (C) 2023-04-03
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // Used to get the Windows I/O 

#include <iostream>
#include <iomanip>

int wmain(int argc, wchar_t** argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: hexview file" << std::endl;
		exit(1);
	}

	HANDLE File = CreateFileW(
		argv[1],
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (File)
	{
		LARGE_INTEGER FileSize{};
		if (GetFileSizeEx(File, &FileSize))
		{
			LPVOID Buffer = VirtualAlloc(nullptr, FileSize.QuadPart, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (Buffer)
			{
				DWORD dwReadBytes{};
				if (ReadFile(File, Buffer, FileSize.QuadPart, &dwReadBytes, nullptr))
				{
					std::wcout.setf(std::ios_base::left, std::ios_base::adjustfield);
					std::wcout << std::right << std::setw(24) << L"HEX VIEW" << L" name=" << argv[1] << L" size=" << FileSize.QuadPart << L" bytes" << "\n";
					std::wcout << "\n";

					uint8_t* Ptr = reinterpret_cast<uint8_t*>(Buffer);
					int Width = 16;
					for (int j = 0; j < FileSize.QuadPart; j += Width)
					{
						std::cout.width(8);
						std::cout.fill('0');
						std::cout << std::hex << j;

						std::cout << ' ';


						for (int i = 0; i < Width; i++)
						{
							std::cout.width(2);
							std::cout.fill('0');
							auto Char = *(Ptr + i + j);
							if (Char > 0 && Char < 32) // ASCII code 1 to 31
							{
								Char = '0' - Char;
							}
							std::cout << std::hex << static_cast<int>(Char);
							if (i >= 0 && i < Width)
							{
								if (i == 7)
								{
									std::cout << ' ';
								}
								std::cout << ' ';
							}
						}

						std::cout.width(4);
						std::cout.fill(' ');
						std::cout << ' ';
						std::cout.width(0);
						std::cout << '|';
						for (int i = 0; i < 16; i++)
						{
							auto Char = *(Ptr + i + j);
							if (Char > 0 && Char < 32)
							{
								Char = '.';
							}
							std::cout << Char;

							if ((i + 1) % 16 == 0)
							{

								std::cout.width(0);
								std::cout << '|';
								std::cout << "\n";
							}
						}
					}
				}
				VirtualFree(Buffer, 0, MEM_FREE);
			}
		}
		CloseHandle(File);
	}

	return 0;
}