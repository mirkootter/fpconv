//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <QtWidgets/QApplication>

#include <lib/fpconv.h>
#include "app_mainwindow.h"

int main (int argc, char** argv)
{
  QApplication app (argc, argv);
  app.setStyle ("fusion");

  M::App::MainWindow window;
  window.show ();
  return app.exec ();
}