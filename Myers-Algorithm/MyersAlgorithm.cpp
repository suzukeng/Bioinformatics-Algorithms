#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
const int32_t bitSize = 64;  // 符号なし64bit整数でまとめて演算することを想定
template <class T>
class BigInt {
 private:
  int32_t BlockSize;
  std::vector<T> BlockArray;
  const T max = std::numeric_limits<T>::max();

 public:
  BigInt(int32_t len, bool all1 = false)
      : BlockSize((len + bitSize - 1) / bitSize) {
    if (all1) {
      BlockArray.assign(BlockSize, max);
    } else {
      BlockArray.assign(BlockSize, 0);
    }
  }
  BigInt() {}
  int8_t getIthBit(int32_t i) {
    int32_t BlockArrayIdx = i / BlockSize;
    int32_t BlockIdx = i % BlockSize;
    return (BlockArray[BlockArrayIdx] & (1ull << BlockIdx)) > 0;
  }
  void setIthBit(int32_t i) {
    int32_t BlockArrayIdx = i / BlockSize;
    int32_t BlockIdx = i % BlockSize;
    BlockArray[BlockArrayIdx] |= (1ull << BlockIdx);
  }

  BigInt operator~() const {
    BigInt cp = (*this);
    for (int32_t i = 0; i < BlockSize; i++) {
      (cp.BlockArray[i]) = ~(cp.BlockArray[i]);
    }
    return cp;
  }
  BigInt& operator+=(const BigInt& val) {
    bool carryFlag = false;
    for (int32_t i = 0; i < BlockSize; i++) {
      T res = (carryFlag ? 1 : 0);
      carryFlag = false;
      carryFlag = (max - val.BlockArray[i] < (this->BlockArray[i]));
      (this->BlockArray[i]) += val.BlockArray[i] + res;
    }
    return *(this);
  }
  BigInt& operator|=(const BigInt& val) {
    for (int32_t i = 0; i < BlockSize; i++) {
      (this->BlockArray[i]) |= val.BlockArray[i];
    }
    return *(this);
  }
  BigInt& operator&=(const BigInt& val) {
    for (int32_t i = 0; i < BlockSize; i++) {
      (this->BlockArray[i]) &= val.BlockArray[i];
    }
    return *(this);
  }
  BigInt& operator^=(const BigInt& val) {
    for (int32_t i = 0; i < BlockSize; i++) {
      (this->BlockArray[i]) ^= val.BlockArray[i];
    }
    return *(this);
  }
  BigInt operator+(const BigInt& val) { return BigInt(*this) += val; }
  BigInt operator|(const BigInt& val) { return BigInt(*this) |= val; }
  BigInt operator&(const BigInt& val) { return BigInt(*this) &= val; }
  BigInt operator^(const BigInt& val) { return BigInt(*this) ^= val; }
  BigInt operator=(const BigInt& val) { return BigInt(val); }

  void shift() {
    bool MSB = false;
    for (int32_t i = 0; i < BlockSize; i++) {
      T cp = (BlockArray[i] << 1);
      if (MSB) {
        cp &= (1ull);
      }
      if (BlockArray[i] & (1ull << (bitSize - 1))) {
        MSB = true;
      } else {
        MSB = false;
      }
      BlockArray[i] = cp;
    }
  }
};
int main() {
  // len1=m
  std::string read1, read2;
  std::cin >> read1 >> read2;
  int32_t len1 = read1.size(), len2 = read2.size();
  std::vector<BigInt<int64_t>> EqFlag(1024);
  std::vector<std::vector<int32_t>> read1CharIndexList(1024);
  for (int32_t idx = 0; idx < len1; idx++) {
    read1CharIndexList[read1[idx]].emplace_back(idx);
  }
  {  // 一致している場所のフラグを作成
     // TODO:この辺バグっている
    const BigInt<int64_t> allZeroList(len2);
    for (char c2 : read2) {
      EqFlag[c2] = allZeroList;
      for (int32_t read1CharIndex : read1CharIndexList[c2]) {
        EqFlag[c2].setIthBit(read1CharIndex);
      }
    }
  }
  int32_t C = len2;
  //   BigInt<int64_t> D0(len2), Pv(len2, true), Ph(len2), Nv(len2), Nh(len2);
  //   auto shift = [&](BigInt<int64_t>& v) -> BigInt<int64_t> {
  //     BigInt<int64_t> ret = v;
  //     ret.shift();
  //     return ret;
  //   };
  //   for (char c2 : read2) {
  //     BigInt<int64_t> Eq = EqFlag[c2];
  //     D0 = (((Eq & Pv) + Pv) ^ Pv) | Eq | Nv;
  //     Ph = Nv | (~(D0 | Pv));
  //     Nh = D0 & Pv;
  //     if (Ph.getIthBit(len2 - 1)) {
  //       C++;
  //     } else if (Nh.getIthBit(len2 - 1)) {
  //       C--;
  //     }
  //     BigInt<int64_t> cp = shift(Ph);
  //     cp.setIthBit(0);
  //     Pv = (shift(Nh) | (~(D0 | cp)));
  //     cp = shift(Ph);
  //     cp.setIthBit(0);
  //     Nv = D0 & cp;
  //   }
  //   std::cout << C << std::endl;
}