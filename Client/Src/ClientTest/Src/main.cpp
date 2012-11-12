#include "stdafx.h"
#include <GameWorld.h>
#include <Logic/Session.h>

int GenerateDump(EXCEPTION_POINTERS* pExceptionPointers)
{
	BOOL bMiniDumpSuccessful;
	TCHAR szPath[MAX_PATH]; 
	TCHAR szFileName[MAX_PATH]; 
	TCHAR* szAppName = "GameWorld";
	DWORD dwBufferSize = MAX_PATH;
	HANDLE hDumpFile;
	SYSTEMTIME stLocalTime;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;

	GetLocalTime( &stLocalTime );
	GetTempPath( dwBufferSize, szPath );

	StringCchPrintf( szFileName, MAX_PATH, "%s%s", szPath, szAppName );
	CreateDirectory( szFileName, NULL );

	StringCchPrintf( szFileName, MAX_PATH, "Data\\Online\\%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp", 
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, 
		GetCurrentProcessId(), GetCurrentThreadId());
	hDumpFile = CreateFile(szFileName, GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	ExpParam.ThreadId = GetCurrentThreadId();
	ExpParam.ExceptionPointers = pExceptionPointers;
	ExpParam.ClientPointers = TRUE;

	bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
		hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);

	Debug::ShowMessageBox("Skyrim Online just crashed ! Please send the .dmp file in Data/Online on the crash forums.");

	return EXCEPTION_EXECUTE_HANDLER;
}

void Init()
{
	System::Log::Create("GameWorldClient.log");
	Skyrim::Logic::Session::Init();
}

void ShowVersion()
{
	std::ostringstream os;
	os << "You need the game in 1.8.151.0 to play Skyrim Online. Hash dump : " << std::hex << *(DWORD *)(0x00DDDC00);
	Debug::ShowMessageBox((char*)os.str().c_str());
}

void NewInstance()
{
	Skyrim::TheGameWorld = new Skyrim::GameWorld;
}

void ChatSystem()
{
	while(1)
	{
		Framework::Network::Packet packet(kClientChatMessage);
		std::string message;
		std::cin >> message;
		packet << message;
		TheMassiveMessageMgr->SendMessageTo(::Game::kPlayerServer, packet);
	}
}

void main()
{
	Init();

	srand((unsigned int)time(NULL));

	NewInstance();
	Skyrim::TheGameWorld->Setup();
	boost::thread t(ChatSystem);
	Skyrim::TheGameWorld->Run();
}