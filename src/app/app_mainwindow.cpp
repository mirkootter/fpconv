#include "app_mainwindow.h"
#include <lib/fpconv.h>

namespace M
{
  namespace App
  {
    MainWindow::MainWindow ()
    {
      setupUi (this);

      setMinimumWidth (3 * logicalDpiX ());

      connect (_input_wgt, &QLineEdit::textChanged, this, &MainWindow::updateNumber);
      connect (_prec_wgt, static_cast <void (QSpinBox::*) (int)> (&QSpinBox::valueChanged), this, &MainWindow::updateNumber);
      connect (_trim_wgt, &QCheckBox::stateChanged, this, &MainWindow::updateNumber);

      updateNumber ();
    }

    void MainWindow::updateNumber ()
    {
      bool ok = false;
      double value = _input_wgt->text ().toDouble (&ok);
      if (!ok)
        _output_lbl->setText ("");
      else
      {
        int prec = _prec_wgt->value ();
        bool trim = _trim_wgt->isChecked ();

        Lib::DoubleToString double_to_string (prec, trim);

        QString result = double_to_string (value);
        QChar x = result[0];
        _output_lbl->setText (result);
      }
    }
  }
}