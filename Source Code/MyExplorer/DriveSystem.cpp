#include "stdafx.h"
#include "DriveSystem.h"

DriveSystem::DriveSystem()
{
	m_DriveName = NULL;
	m_VolumeLabel = NULL;
	m_DriveType = NULL;
	NumberDrive = 0;
	m_DriveSize = NULL;

}


DriveSystem::~DriveSystem()
{
	for (int i = 0; i < NumberDrive; ++i)
	{
		delete[] m_DriveName[i];
		delete[] m_VolumeLabel[i];
		delete[] m_DriveType[i];
	}

	delete[] m_DriveName;
	delete[] m_VolumeLabel;
	delete[] m_DriveType;
	NumberDrive = 0;
}

TCHAR* DriveSystem::getDriveType(int position)
{
	return m_DriveType[position];
}

TCHAR* DriveSystem::getDriveName(const int &i)
{
	return m_DriveName[i];
}


TCHAR* DriveSystem::getDisplayName(const int &i)
{
	return m_VolumeLabel[i];
}

int DriveSystem::getCount()
{
	return NumberDrive;
}

LPWSTR DriveSystem::getFreeSpace(int i)
{
	return SizeDrive::Convert(this->m_DriveSize[i]->freeSpace);
}

LPWSTR DriveSystem::getTotalSize(int i)
{
	return SizeDrive::Convert(this->m_DriveSize[i]->totalSize);
}

SizeDrive* DriveSystem::getSizeDrive(int index){
	__int64 totalSize;
	__int64 freeSpace;

	SHGetDiskFreeSpaceEx(this->m_DriveName[index], NULL, (PULARGE_INTEGER)&totalSize, (PULARGE_INTEGER)&freeSpace);

	SizeDrive* driveSize = new SizeDrive();
	driveSize->freeSpace = freeSpace;
	driveSize->totalSize = totalSize;

	return driveSize;
}


void DriveSystem::getSystemDrives()
{

	TCHAR buffer[MAX_BUFFER];
	TCHAR* tVolumeName = new TCHAR[MAX_BUFFER];

	GetLogicalDriveStrings(MAX_BUFFER, buffer);


	//Đếm số lượng ổ đĩa 
	this->NumberDrive = 0;
	for (int i = 0; (buffer[i] != '\0') || (buffer[i - 1] != '\0'); i++)
	{
		if (buffer[i] == '\0')
		{
			this->NumberDrive++;
		}
	}

	//Cấp phát bộ nhớ để chứa chuỗi tương ứng
	this->m_DriveName = new TCHAR*[this->NumberDrive];
	this->m_VolumeLabel = new TCHAR*[this->NumberDrive];
	this->m_DriveType = new TCHAR*[this->NumberDrive];
	this->m_DriveSize = new SizeDrive*[this->NumberDrive];

	for (int i = 0; i < this->NumberDrive; ++i)
	{
		this->m_DriveName[i] = new TCHAR[MAX_BUFFER];
		this->m_VolumeLabel[i] = new TCHAR[MAX_BUFFER];
		this->m_DriveType[i] = new TCHAR[MAX_BUFFER];
	}

	int i, j;
	int index = 0;

	//Lấy tên drive C,D,E
	for (i = 0; i < this->NumberDrive; ++i)
	{
		j = 0;
		while (buffer[index] != 0)
		{
			this->m_DriveName[i][j++] = buffer[index++];
		}
		this->m_DriveName[i][j] = '\0';
		++index;
	}

	///Lấy tên loại drive
	int n_Type;
	for (int i = 0; i < this->NumberDrive; ++i)
	{
		n_Type = GetDriveType(this->m_DriveName[i]); // lấy type
		this->m_DriveSize[i] = this->getSizeDrive(i); // lấy size
		switch (n_Type)
		{
		case DRIVE_FIXED:
			StrCpy(this->m_DriveType[i], L"Local Disk");
			break;
		case DRIVE_REMOVABLE:
			StrCpy(this->m_DriveType[i], L"Removable Drive");
			break;
		case DRIVE_REMOTE:
			StrCpy(this->m_DriveType[i], L"Network Drive");
			break;
		case DRIVE_CDROM:
			StrCpy(this->m_DriveType[i], L"CD-ROM");
			break;
		default:
			break;
		}
		StrCpy(buffer, _T(""));


		if ((n_Type == DRIVE_FIXED) || ((i > 1) && (n_Type == DRIVE_REMOVABLE)))  //Nếu là ổ cứng hay USB
		{
			GetVolumeInformation(this->m_DriveName[i], buffer, MAX_BUFFER, NULL, NULL, NULL, NULL, 0);
			StrCpy(tVolumeName, buffer);
		}
		else if (n_Type == DRIVE_CDROM)
		{
			GetVolumeInformation(this->m_DriveName[i], buffer, MAX_BUFFER, NULL, NULL, NULL, NULL, 0);
			if (wcslen(buffer) == 0)
				StrCpy(tVolumeName, _T("CD-ROM"));
			else
				StrCpy(tVolumeName, buffer);
		}
		else if (((i == 0) || (i == 1)) && (n_Type == DRIVE_REMOVABLE)) //Nếu là ổ mềm
		{
			StrCpy(tVolumeName, _T("3½ Floppy"));
		}

		if (wcslen(tVolumeName) == 0) //Hổng có label T_T
			StrCpy(this->m_VolumeLabel[i], _T("Local Disk"));
		else
			StrCpy(this->m_VolumeLabel[i], tVolumeName);

		//Thêm vào phần sau hai dấu mở ngoặc. Ví dụ: (C:) như ta thường thấy WinXp (C:)
		StrCat(this->m_VolumeLabel[i], _T(" ("));
		StrNCat(this->m_VolumeLabel[i], this->m_DriveName[i], 3);
		StrCat(this->m_VolumeLabel[i], _T(")"));
	}
}

