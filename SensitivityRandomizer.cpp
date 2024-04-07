#include <iostream>
#include <Windows.h>
#include <random>

// Settings
int DefaultSens = 10;
int ChangeTime = 4000; // Time in miliseconds till next sens change
bool Relative = true;
int MaxChange = 1; // Max relative change
int MaxSens = 16;
int MinSens = 4;
bool ForceDifference = true; // Force new sensitivity to be different by atleast 1

int Sensitivity = 10; // Default sensitivity
int PreviousSens = 10;

// Random
std::random_device RD;
std::mt19937 Gen(RD());
std::uniform_int_distribution<> NormalRandom(1, 20);
std::uniform_int_distribution<> RelativeRandom(-MaxChange, MaxChange);

bool Running = true;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{

	case CTRL_C_EVENT: {
		Running = false;
		SystemParametersInfo(
			SPI_SETMOUSESPEED,
			0,
			(LPVOID)DefaultSens,
			SPIF_UPDATEINIFILE | SPIF_SENDCHANGE | SPIF_SENDWININICHANGE
		);
		return TRUE;
	}

	case CTRL_CLOSE_EVENT: {
		Running = false;
		SystemParametersInfo(
			SPI_SETMOUSESPEED,
			0,
			(LPVOID)DefaultSens,
			SPIF_UPDATEINIFILE | SPIF_SENDCHANGE | SPIF_SENDWININICHANGE
		);
		return TRUE;
	}
	case CTRL_BREAK_EVENT: {
		return FALSE;
	}
	case CTRL_LOGOFF_EVENT: {
		return FALSE;
	}
	case CTRL_SHUTDOWN_EVENT: {
		return FALSE;
	}
	default: {
		return FALSE;
	}
	}
}

int GetRandomSens(int CurrentSens, bool Relative) {
	if (Relative) {
		CurrentSens += RelativeRandom(Gen);
	}
	else {
		CurrentSens = NormalRandom(Gen);
	}
	return CurrentSens;
}

int main() {
	LARGE_INTEGER Frequency, Time1, Time2;

	// Console exit handler
	SetConsoleCtrlHandler(CtrlHandler, TRUE);

	while (Running) {
		QueryPerformanceCounter(&Time1);

		std::cout << "Getting new sensitivity" << "\n";
		// Get new sensitivity
		if (ForceDifference) {
			while (Sensitivity == PreviousSens) {
				Sensitivity = GetRandomSens(Sensitivity, Relative);
			}
		}
		else {
			Sensitivity = GetRandomSens(Sensitivity, Relative);
		}

		// Clamp
		if (Sensitivity > MaxSens) {
			Sensitivity = MaxSens;
		}
		else if (Sensitivity < MinSens) {
			Sensitivity = MinSens;
		}

		std::cout << "Changing sensitivity" << "\n";
		SystemParametersInfo(
			SPI_SETMOUSESPEED,
			0,
			(LPVOID)Sensitivity,
			SPIF_UPDATEINIFILE | SPIF_SENDCHANGE | SPIF_SENDWININICHANGE
		);

		std::cout << "Sensitivity changed to " << Sensitivity << "\n";

		QueryPerformanceCounter(&Time2);
		QueryPerformanceFrequency(&Frequency);
		double TimeTaken = (Time2.QuadPart - Time1.QuadPart) / (double)Frequency.QuadPart;

		std::cout << TimeTaken << "\n";

		std::cout << "Sleeping" << "\n\n";
		PreviousSens = Sensitivity;

		// Remove the time taken to set the sensitivity (usually around 0.6s) from the sleep time
		// to ensure consistency
		Sleep(ChangeTime - (TimeTaken * 1000));
	}
}


