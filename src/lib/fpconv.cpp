#include "fpconv.h"
#include <cassert>
#include <cmath>
#include <cstdint>

namespace M
{
  namespace Lib
  {
    namespace
    {
      /** \brief Represents the nonzero positiv decimal number f * 10^(exp - dcpnt). f must be integer >0
       *  
       *  This number will be printed as str(f * 10^-dcpnt) + "10^" + str (exp)
      */
      class Decimal
      {
      public:
        Decimal (double value);
        void round (int prec);
        void trim ();
        void beautify (int prec);
        QChar* print (QChar* buffer) const;

      private:
        static QChar* generateDigitsFromRightToLeft (uint64_t& x, unsigned int count, QChar* pch);
        static QChar* generateSuperscriptDigitsFromLeftToRight (int x, QChar* pch);

      private:
        uint64_t _f;           //!< Digits.
        int _exp;              //!< Exponent to be printed. Must be between -999 and +999, i.e. 3 digits
        unsigned int _dcpnt;   //!< Position of decimal point from the right.
        int _number_of_digits; //!< Number of digits. This value is >=1 and <=20, since f is 64 bit
      };

      /*! \brief Generates an decimal representation of a double value. It is not beautiful and has
       *         either 18 or 19 decimal digits. value must be strictly positive and neither nan nor inf
       *
       *  The constructor sets dcpnt to zero, i.e. the represented number is integer, if we ignore
       *  the exponent. The integer value lies between 2^(delta - 1) * 0.75 and 2^delta * 15,
       *  which is a very rough bound.
       *  
       *  The bound gives an even rougher bound: {2 * 10^17 , ... , 9 * 10^18 }
       *  ==> We get exactly 18 or 19 digits (of course, the last ones can be zero)
      */
      Decimal::Decimal (double value) :
        _dcpnt (0),
        _number_of_digits (18)
      {
        int binary_exp;
        (void) std::frexp (value, &binary_exp);

        {
          int delta = 59;
          int m = binary_exp - delta;

          double h = 0.3010299956639812 * m; // Approximation of m*log(2)/log(10)
          _exp = int (h);
          if (_exp > h) // can happen for h<0
            --_exp;

          assert (_exp <= h);
        }

        // Approximate value * 10^-exp with high accuracy
        // The accuracy has to be about 61 bits, otherwise the last digits might
        // not be exact
        double x_hat = value * std::pow (10, -_exp); // TODO: This is NOT accurate enough
        _f = uint64_t (x_hat);

        // This holds in any case (requiring only that value's accuracy is about 3 bit)
        assert (_f >= 200000000000000000U); // min. 18 digits
        assert (_f < 9000000000000000000U); // max. 19 digits

        if (_f >= 1000000000000000000U)
          _number_of_digits = 19;
      }

      QChar* Decimal::generateDigitsFromRightToLeft (uint64_t& x, unsigned int count, QChar* pch)
      {
        do
        {
          --count;
          *pch = char ('0' + x % 10);
          --pch;

          x /= 10;
        } while (count);
        return pch;
      }

      QChar* Decimal::generateSuperscriptDigitsFromLeftToRight (int x, QChar* pch)
      {
        assert (x >= 0 && x <= 999);
        if (x >= 100)
          pch += 2;
        else if (x >= 10)
          pch += 1;
       
        QChar* result = pch + 1;

        do
        {
          int digit = x % 10;
          switch (digit)
          {
          case 1:
            *pch = 0x00B9; // unicode superscript 1
            break;
          case 2:
          case 3:
            *pch = 0x00B0 + digit;
            break;
          default:
            *pch = short (0x2070 + digit);
            break;
          }

          --pch;
          x /= 10;
        } while (x);

        return result;
      }

      QChar* Decimal::print (QChar* buffer) const
      {
        int dcpnt = _dcpnt;
        int leading_zeros = dcpnt - _number_of_digits;
        if (leading_zeros > 0)
        {
          assert (_exp == 0); // this should only be allowed in non scientific notation
          assert (leading_zeros < 4); // max. 6 extra chars (scientific notation would be min. 6 extra chars)

          buffer[0] = '0';
          buffer[1] = '.';
          buffer += 2;
          while (leading_zeros)
          {
            *buffer = '0';
            ++buffer;
            --leading_zeros;
          }

          dcpnt = 0; // fallthrough to case without decimal point
        }
        
        buffer += _number_of_digits; // Position of suffix ("e-123") if there is no decimal point
        int integer_digits = _number_of_digits;

        // Print fractional part
        uint64_t f = _f;
        QChar* rtl_cursor; // "right to left" cursor
        {
          if (dcpnt > 0)
          {
            rtl_cursor = generateDigitsFromRightToLeft (f, dcpnt, buffer);
            *rtl_cursor = '.';
            --rtl_cursor;
            ++buffer;
            integer_digits -= dcpnt;
          }
          else {
            rtl_cursor = buffer - 1;
          }
        }
        
        // Print integer part
        assert (integer_digits > 0);
        (void)generateDigitsFromRightToLeft (f, integer_digits, rtl_cursor);

        if (_exp != 0)
        {
          buffer[0] = 0x22C5;
          buffer[1] = '1';
          buffer[2] = '0';

          buffer += 3;
          int exp = _exp;
          if (exp < 0)
          {
            exp = -exp;
            *buffer = 0x207B;
            ++buffer;
          }

          buffer = generateSuperscriptDigitsFromLeftToRight (exp, buffer);
        }

        return buffer;
      }

      static uint64_t pow10[19] = {
        uint64_t (1e0), uint64_t (1e1), uint64_t (1e2), uint64_t (1e3), uint64_t (1e4),
        uint64_t (1e5), uint64_t (1e6), uint64_t (1e7), uint64_t (1e8), uint64_t (1e9),
        uint64_t (1e10), uint64_t (1e11), uint64_t (1e12), uint64_t (1e13), uint64_t (1e14),
        uint64_t (1e15), uint64_t (1e16), uint64_t (1e17), uint64_t (1e18)
      };

      /*! \brief Only call this method if you are sure that you have 18 or 19 digits. This method reduces the
       *         number of digits to the specified precision
       *
       * Note: It is always safe to call this method after the constructor, because the constructor
       *       ensures that the number of digits is exactly 18 or 19
      */
      void Decimal::round (int prec)
      {
        // Sanity checks
        assert (_number_of_digits == 18 || _number_of_digits == 19);
        assert (_f < 9000000000000000000U); // Even after rounding, we cannot exceed 19 digits

        if (prec < 1)
          prec = 1;
        if (prec >= _number_of_digits)
          return; // already done

        int shift = _number_of_digits - prec; // between 1 and 18

        _f += 5 * pow10[shift - 1]; // Strategie: Away from zero
        if (_number_of_digits == 18 && _f >= uint64_t (1e19))
          ++_number_of_digits;

        _f /= pow10[shift];
        _exp += shift;
        _number_of_digits -= shift;
      }

      void Decimal::trim ()
      {
        while (_f > 0 && _f % 10 == 0)
        {
          _f /= 10;
          --_number_of_digits;
          ++_exp;
        }
      }

      void Decimal::beautify (int prec)
      {
        int dcpnt = int (_dcpnt);
        int integer_digits_after_shift = (_number_of_digits - dcpnt + _exp) % 3;
        if (integer_digits_after_shift <= 0)
          integer_digits_after_shift += 3;

        int shift = _number_of_digits - integer_digits_after_shift - dcpnt;
        if (dcpnt + shift < 0)
        {
          // We cannot move the decimal point so far, because _dcpnt must not become negative
          // ==> Split the shift into two shifts: [_dcpnt + shift] and -_dcpnt
          int s2 = shift + dcpnt;
          shift = -dcpnt;

          // s2 shift has to be performed by adding zeros
          _f *= pow10[-s2];
          _number_of_digits -= s2;
          _exp += s2;
        }
        _dcpnt += shift;
        _exp += shift;
      }
    }

    QString DoubleToString::operator () (double value) const
    {
      if (!value)
        return "0";

      // Sign; Digits; Decimal point; Suffix
      constexpr int max_number_of_digits = 1 + 18 + 1 + 7;
      QChar digits[max_number_of_digits];

      QChar* cursor = &digits[0];
      QChar* start = cursor;

      if (std::isnan (value))
      {
        start[0] = 0xD83D;
        start[1] = 0xDE41;
        return QString (start, 2);
      }

      if (value < 0)
      {
        *cursor = '-';
        ++cursor;
        value = -value;
      }

      if (std::isinf (value))
      {
        *(cursor++) = 0x221E;
        return QString (start, cursor - start);
      }

      Decimal decimal (value);
      decimal.round (_prec);
      if (_trim)
      {
        decimal.trim ();
      }
      decimal.beautify (_prec);

      cursor = decimal.print (cursor);

      return QString (start, cursor - start);
    }
  }
}
