#include "EditorApplication.h"

int32_t main() {
	ApplicationCreateInfo appCreateInfo = {
		.Width = 1280,
		.Height = 720,
		.Title = "PathTracer Editor"
	};

	Application* app = new EditorApplication(appCreateInfo);
	app->Initialize();
	app->Run();
	app->Shutdown();
	delete app;
}