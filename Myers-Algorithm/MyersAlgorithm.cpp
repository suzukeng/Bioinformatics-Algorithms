#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
const uint32_t bitSize = 64;  // 符号なし64bit整数でまとめて演算することを想定
template <class T>
class BigInt {
 public:
  uint32_t len;
  uint32_t BlockSize;
  std::vector<T> BlockArray;
  const T max = std::numeric_limits<T>::max();

 public:
  BigInt(uint32_t len_, bool all1 = false)
      : len(len_), BlockSize((len_ + bitSize - 1) / bitSize) {
    if (all1) {
      BlockArray.assign(BlockSize, max);
    } else {
      BlockArray.assign(BlockSize, 0);
    }
  }
  BigInt() {}
  int8_t getIthBit(uint32_t i) {
    uint32_t BlockArrayIdx = i / bitSize;
    uint32_t BlockIdx = i % bitSize;
    return (BlockArray[BlockArrayIdx] & (1ull << BlockIdx)) > 0;
  }
  void setIthBit(uint32_t i) {
    uint32_t BlockArrayIdx = i / bitSize;
    uint32_t BlockIdx = i % bitSize;
    BlockArray[BlockArrayIdx] |= (1ull << BlockIdx);
  }

  BigInt operator~() const {
    BigInt cp = (*this);
    for (uint32_t i = 0; i < BlockSize; i++) {
      (cp.BlockArray[i]) = ~(cp.BlockArray[i]);
    }
    uint32_t topbit = (bitSize - (len % bitSize)) % bitSize;
    for (uint32_t i = 0; i < topbit; i++) {
      cp.BlockArray.back() ^= (1ull << (bitSize - 1 - i));
    }
    return cp;
  }
  BigInt& operator+=(const BigInt& val) {
    bool carryFlag = false;
    for (uint32_t i = 0; i < BlockSize; i++) {
      T res = (carryFlag ? 1 : 0);
      carryFlag = false;
      carryFlag = (max - val.BlockArray[i] < (this->BlockArray[i]));
      (this->BlockArray[i]) += val.BlockArray[i] + res;
    }
    return *(this);
  }
  BigInt& operator|=(const BigInt& val) {
    for (uint32_t i = 0; i < BlockSize; i++) {
      (this->BlockArray[i]) |= val.BlockArray[i];
    }
    return *(this);
  }
  BigInt& operator&=(const BigInt& val) {
    for (uint32_t i = 0; i < BlockSize; i++) {
      (this->BlockArray[i]) &= val.BlockArray[i];
    }
    return *(this);
  }
  BigInt& operator^=(const BigInt& val) {
    for (uint32_t i = 0; i < BlockSize; i++) {
      (this->BlockArray[i]) ^= val.BlockArray[i];
    }
    return *(this);
  }
  BigInt operator+(const BigInt& val) { return BigInt(*this) += val; }
  BigInt operator|(const BigInt& val) { return BigInt(*this) |= val; }
  BigInt operator&(const BigInt& val) { return BigInt(*this) &= val; }
  BigInt operator^(const BigInt& val) { return BigInt(*this) ^= val; }
  BigInt operator=(const BigInt& val) {
    (this->BlockSize) = val.BlockSize;
    (this->BlockArray) = val.BlockArray;
    return *this;
  }

  void shift() {
    bool MSB = false;
    for (uint32_t i = 0; i < BlockSize; i++) {
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
  uint32_t len1 = read1.size(), len2 = read2.size();
  std::vector<BigInt<uint64_t>> EqFlag(1024);
  std::vector<std::vector<uint32_t>> read1CharIndexList(1024);
  for (uint32_t idx = 0; idx < len1; idx++) {
    read1CharIndexList[read1[idx]].emplace_back(idx);
  }
  {  // 一致している場所のフラグを作成
     // TODO:この辺バグっている
    const BigInt<uint64_t> allZeroList(len2);
    for (char c2 : read2) {
      EqFlag[c2] = allZeroList;
      for (uint32_t read1CharIndex : read1CharIndexList[c2]) {
        EqFlag[c2].setIthBit(read1CharIndex);
      }
    }
  }
  uint32_t C = len2;
  BigInt<uint64_t> D0(len2), Pv(len2, true), Ph(len2), Nv(len2), Nh(len2);
  auto shift = [&](BigInt<uint64_t>& v) -> BigInt<uint64_t> {
    BigInt<uint64_t> ret = v;
    ret.shift();
    return ret;
  };
  for (char c2 : read2) {
    BigInt<uint64_t> Eq = EqFlag[c2];
    D0 = (((Eq & Pv) + Pv) ^ Pv) | Eq | Nv;
    Ph = Nv | (~(D0 | Pv));
    Nh = D0 & Pv;
    if (Ph.getIthBit(len2 - 1)) {
      C++;
    } else if (Nh.getIthBit(len2 - 1)) {
      C--;
    }
    BigInt<uint64_t> cp = shift(Ph);
    cp.setIthBit(0);
    Pv = (shift(Nh) | (~(D0 | cp)));
    cp = shift(Ph);
    cp.setIthBit(0);
    Nv = D0 & cp;
  }
  std::cout << C << std::endl;
}