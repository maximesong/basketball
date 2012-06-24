#include <QApplication>

#ifdef __linux__
#include "GL/glut.h"
#endif

#include "BasketballWidget.h"

int main(int argc, char **argv)
{
#ifdef __linux__
	glutInit(&argc, argv);
#endif
	QApplication app(argc, argv);
	BasketballWidget widget;
	widget.show();
	return app.exec();
}
