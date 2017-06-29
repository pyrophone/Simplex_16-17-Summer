#include "main.h"

int main()
{
	//Create an app, call appropritate methods, check return values
	GLFWApp app;
	if (app.init() != 0)
		return -1;
	if (app.run() != 0)
		return -1;
	if (app.end() != 0)
		return -1;
	return 0;
}