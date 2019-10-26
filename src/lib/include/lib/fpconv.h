#pragma once

#include <QtCore/QString>

namespace M
{
  namespace Lib
  {
    class DoubleToString
    {
    public:
      DoubleToString (int prec = 6, bool trim = true) :
        _prec (prec),
        _trim (trim)
      {}

      QString operator () (double value) const;

    private:
      int _prec;
      bool _trim;
    };
  }
}