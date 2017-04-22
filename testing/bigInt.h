//TODO: which functions are inline
//TODO: operator ~ may be mistakenly implemented
//TODO: Default constructor is nos working properly when no arguments are passed

#include <iostream>

#ifndef _BIGINT_H
#define _BIGINT_H

typedef unsigned __int128 uint128_t;

using namespace std;

class BigInt
{
private:
  uint128_t msb, lsb;

public:
  BigInt(int _lsb = 0); //Default constructor with default argument
  BigInt(const BigInt&);      //Copy constructor
  BigInt(uint128_t _msb, uint128_t _lsb);
  ~BigInt();            //Destructor

  uint128_t get_msb() const;
  uint128_t get_lsb() const;

  void set_msb(uint128_t);
  void set_lsb(uint128_t);

  void operator=(const BigInt&);

  BigInt operator~();     // Logical not operator
  BigInt operator++();    // Prefix increment operator
  BigInt operator++(int); // Postfix increment operator
  BigInt operator--();    // Prefix decrement operator
  BigInt operator--(int); // Postfix decrement operator

  BigInt operator+(const BigInt&);
  BigInt operator+(int);
  BigInt operator-(const BigInt&);
  BigInt operator-(int);

  BigInt operator&(const BigInt&);
  uint64_t operator&(int);

  bool operator==(const BigInt&);
  bool operator!=(const BigInt&);

  bool operator==(int);
  bool operator!=(int);

  BigInt operator>>(int);
  BigInt operator<<(int);

  // Friend member functions
  friend std::ostream& operator<<(std::ostream& os, const BigInt& obj) {
    int bits[256] = {0};
    uint128_t p, q;
    int idx;

    p = obj.get_lsb();
    idx = 0;
    do {
      q = p / 2;
      bits[idx] = (p%2);
      p = q;
      idx++;
    } while (q > 0);

    p = obj.get_msb();
    idx = 128;
    do {
      q = p / 2;
      bits[idx] = (p%2);
      p = q;
      idx++;
    } while (q > 0);

    for(int i = 255; i >= 192; i--)
      os << bits[i];
    os << ".";
    for(int i = 191; i >= 128; i--)
      os << bits[i];
    os << ".";
    for(int i = 127; i >=  64; i--)
      os << bits[i];
    os << ".";
    for(int i =  63; i >=   0; i--)
      os << bits[i];

    return os;
  }

  friend bool operator<(const BigInt& lhs, const BigInt& rhs) {
    if(lhs.get_msb() < rhs.get_msb())
      return true;
    if(lhs.get_msb() > rhs.get_msb())
      return false;
    if(lhs.get_lsb() < rhs.get_lsb())
      return true;
    return false;
  }

  friend bool operator>(const BigInt& lhs, const BigInt& rhs) {
    return rhs < lhs;
  }

  friend bool operator<=(const BigInt& lhs, const BigInt& rhs) {
    return !(lhs > rhs);
  }

  friend bool operator>=(const BigInt& lhs, const BigInt& rhs) {
    return !(lhs < rhs);
  }

  friend bool operator<(const BigInt& lhs, int rhs){
    if(lhs.get_msb() > 0)
      return false;
    if(lhs.get_lsb() < (uint128_t)rhs)
      return true;
    return false;
  }

  friend bool operator>(const BigInt& lhs, int rhs){
    return rhs < lhs;
  }

  friend bool operator<=(const BigInt& lhs, int rhs){
    return !(lhs > rhs);
  }

  friend bool operator>=(const BigInt& lhs, int rhs){
    return !(lhs < rhs);
  }

  // Static member functions
  static int log2(BigInt);
  static BigInt pow2(int);
};

BigInt::BigInt(int _lsb) {
  set_msb(0);
  set_lsb((uint128_t)_lsb);
}

BigInt::BigInt(const BigInt& _x) {
  set_msb(_x.get_msb());
  set_lsb(_x.get_lsb());
}

BigInt::BigInt(uint128_t _msb, uint128_t _lsb) : msb(_msb), lsb(_lsb) {}

BigInt::~BigInt() {}

uint128_t BigInt::get_msb() const {
  return msb;
}

uint128_t BigInt::get_lsb() const {
  return lsb;
}

void BigInt::set_msb(uint128_t _msb) {
  msb = _msb;
}

void BigInt::set_lsb(uint128_t _lsb) {
  lsb = _lsb;
}

void BigInt::operator=(const BigInt& obj) {
  set_msb(obj.get_msb());
  set_lsb(obj.get_lsb());
}

BigInt BigInt::operator~() {
  BigInt temp(~msb, ~lsb);
  return temp;
}

BigInt BigInt::operator++() {
  uint128_t current = lsb;
  lsb++;
  msb = msb + (current > lsb) ? 1 : 0;
  return *this;
}

BigInt BigInt::operator++(int) {
  operator++();
  return *this;
}

BigInt BigInt::operator--(){
  uint128_t current = lsb;
  lsb--;
  msb = msb - (current < lsb) ? 1 : 0;
  return *this;
}

BigInt BigInt::operator--(int) {
  operator--();
  return *this;
}

BigInt BigInt::operator+(const BigInt& obj) {
  uint128_t current = lsb;
  BigInt n(0);
  n.set_lsb(lsb + obj.get_lsb());
  n.set_msb(msb + obj.get_msb() + ((current > n.get_lsb()) ? 1 : 0));
  return n;
}

BigInt BigInt::operator+(int x) {
  uint128_t current = lsb;
  BigInt n(0);
  n.set_lsb(lsb + (uint128_t)x);
  n.set_msb(msb + (uint128_t)x + ((current > n.get_lsb()) ? 1 : 0));
  return n;
}

BigInt BigInt::operator-(const BigInt& obj) {
  uint128_t current = lsb;
  BigInt n(0);
  n.set_lsb(lsb - obj.get_lsb());
  n.set_msb(msb - obj.get_msb() - ((current < n.get_lsb()) ? 1 : 0));
  return n;
}

BigInt BigInt::operator-(int x) {
  uint128_t current = lsb;
  BigInt n(0);
  n.set_lsb(lsb - (uint128_t)x);
  n.set_msb(msb - (uint128_t)x - ((current < n.get_lsb()) ? 1 : 0));
  return n;
}

BigInt BigInt::operator&(const BigInt& obj) {
  BigInt temp(0);
  temp.set_msb(msb & obj.get_msb());
  temp.set_lsb(lsb & obj.get_lsb());

  return temp;
}

uint64_t BigInt::operator&(int x) {
  return (uint64_t)(lsb & (uint128_t)x);
}

bool BigInt::operator==(const BigInt& rhs) {
  if((msb == rhs.get_msb()) && (lsb == rhs.get_lsb()))
    return true;
  else
    return false;
}

bool BigInt::operator!=(const BigInt& rhs) {
  return !(*this == rhs);
}

bool BigInt::operator==(int rhs) {
  if((msb == 0) && (lsb == (uint128_t)rhs))
    return true;
  else
    return false;
}

bool BigInt::operator!=(int rhs) {
  return !(*this == rhs);
}

BigInt BigInt::operator>>(int rhs)
{
  int shift = (rhs % 256) + ((rhs < 0) ? 256 : 0);
  BigInt ans(0);

  if(shift <  128) {
    if(shift == 0) {
      ans.set_lsb(lsb);
      ans.set_msb(msb);
    } else {
      ans.set_lsb((msb << (128 - shift)) | (lsb >> shift));
      ans.set_msb(msb >> shift);
    }
  }
  if(shift == 128) {
    ans.set_lsb(msb);
    ans.set_msb((uint128_t)0);
  }
  if(shift >  128) {
    ans.set_lsb(msb >> (shift - 128));
    ans.set_msb((uint128_t)0);
  }
  return ans;
}

BigInt BigInt::operator<<(int rhs)
{
  int shift = (rhs % 256) + ((rhs < 0) ? 256 : 0);
  BigInt ans(0);

  if(shift <  128) {
    if(shift == 0) {
      ans.set_lsb(lsb);
      ans.set_msb(msb);
    } else {
      ans.set_msb((lsb >> (128 - shift)) | (msb << shift));
      ans.set_lsb(lsb << shift);
    }
  }
  if(shift == 128) {
    ans.set_lsb((uint128_t)0);
    ans.set_msb(lsb);
  }
  if(shift >  128) {
    ans.set_lsb((uint128_t)0);
    ans.set_msb(lsb << (shift - 128));
  }
  return ans;
}

int BigInt::log2(BigInt x) {
  int count = 0;
  while((x & 1) == 0)
  {
    x = x >> 1;
    count++;
  }
  return count;
}

BigInt BigInt::pow2(int n) {
  BigInt res(1);
  res = res << n;
  return res;
}

#endif
