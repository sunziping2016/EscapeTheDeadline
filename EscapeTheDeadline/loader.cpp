#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "loader.h"

#include "player.h"

#define BUFFERSIZE		200

#define MAX_COMMANDSLEN	100

static struct
{
	TCHAR *commandName;
	int(*commandFunc)(TCHAR *command);
} Commands[MAX_COMMANDSLEN];

static int commandsEnd;
static wchar_t lastFile[MAX_PATH];

int LoaderLoad(TCHAR *filename)
{
	wchar_t buffer[BUFFERSIZE], command[BUFFERSIZE];
	FILE *file;
	_wfopen_s(&file, filename, L"r");
	int error = 0, i, ret;
	while (fgetws(buffer, BUFFERSIZE, file) != NULL) {
		if (buffer[0] == '\n' || buffer[0] == '#') continue;
		if (swscanf(buffer, L"%s", command) != 1) {
			error = 1;
			break;
		}
		for (i = 0; i < commandsEnd; ++i)
			if (wcscmp(Commands[i].commandName, command) == 0) {
				ret = Commands[i].commandFunc(buffer);
				break;
			}
		if (ret != 0 || i == commandsEnd) {
			error = 1;
			break;
		}
	}
	if (error == 0)
		wcscpy(lastFile, filename);
	fclose(file);
	return error;
}
int LoaderReload()
{
	if (lastFile[0] != '\0' && LoaderLoad(lastFile) == 0)
		return 0;
	return 1;
}

int LoaderAdd(TCHAR *commandName, int(*commandFunc)(TCHAR *command))
{
	if (commandsEnd == MAX_COMMANDSLEN) return 1;
	Commands[commandsEnd].commandName = commandName;
	Commands[commandsEnd].commandFunc = commandFunc;
	++commandsEnd;
	return 0;
}

void LoaderInit() {}
void LoaderDestroy() {}
