#pragma once

#include <QtWidgets/QWidget>
#include "ui_app_mainwindow.h"

namespace M
{
  namespace App
  {
    class MainWindow : public QWidget, private Ui::MainWindow
    {
      Q_OBJECT
    public:
      MainWindow ();

    private:
      void updateNumber ();
    };
  }
}