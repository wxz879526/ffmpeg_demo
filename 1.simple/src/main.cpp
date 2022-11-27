#include "window.h"

int main(int argc, char** argv)
{
	QGuiApplication app(argc, argv);

	QSurfaceFormat fmt;
	fmt.setVersion(3, 3);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(fmt);

	Window w;
	w.show();

	app.exec();

	return 0;
}