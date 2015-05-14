#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>

#define MAX_LIMIT (uint64_t)0x100000000
#define MAX_DIGIT_HEX 8
#define UINT64_HIGH 0xFFFFFFFF00000000
#define UINT64_LOW  0xFFFFFFFF

static const char DigitToHex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

using std::vector;
using std::string;

class BigUInt {
	public:

		BigUInt() {

		}

		BigUInt( const char * in ) {
			string inStr(in);
			initFromString(inStr);
		}

		BigUInt( string in ) {
			initFromString(in);
		}
	
		BigUInt( vector<uint32_t> in ) {
			value = in;
		}
		
		BigUInt( vector<uint32_t>::iterator in1, vector<uint32_t>::iterator in2 ) {
			value = vector<uint32_t>(in1, in2);
			if (value.size() == 0 ) {
				append(0);
			}
		}

		BigUInt( uint64_t in ) {
			if( in >= MAX_LIMIT ) {
				uint64_t high = in & UINT64_HIGH;
				high = high >> 32;
				value.push_back( in & UINT64_LOW  );
				value.push_back( high );
			} else {
				append( in );
			}
		}
		
		
		size_t
		size() {
			return value.size();
		}

		uint64_t
		toUint64() {
			uint64_t ret = 0;
			if        ( value.size() == 1 ) {
				ret = value[0];
				return ret;
			} else if ( value.size() == 2 ) {
				ret |= value[0];
				ret =  ret << 32;
				ret |= value[1];
			} else {
				std::cerr << "Error: Can't convert to uint64, value too large." << std::endl; 
				return 0;
			}

			return ret;
		}

		static BigUInt
		ModExp(BigUInt &inBase, BigUInt &inExp, BigUInt &mod) {
			BigUInt ret = 1;
			BigUInt base = inBase % mod;
			BigUInt exp = inExp;

			while( exp > 0 ) {
				BigUInt check1 = exp % 2;
				if( check1 == 1 ){
					ret = ret * base;
					ret = ret % mod;
				}
				exp = exp >> 1; // exp / 2
				base = base * base;
				base = base % mod;
			}

			return ret;
		}

		string
		toString() const {
			string hex = toHexString();
			string dec = hex;

			if( dec == "0" ) {
				return dec;
			}

			for( auto &x : dec ) {
				x = 0;
			}

			BigUInt pow(1);
			BigUInt result((uint64_t)0);
			BigUInt value((uint64_t)0);
			BigUInt carry((uint64_t)0);
			BigUInt decV((uint64_t)0);

			string revIn = hex;
			std::reverse(revIn.begin(), revIn.end());
			std::transform(revIn.begin(), revIn.end(), revIn.begin(), ::toupper);
			for( auto hexVal : revIn ) {
				//Convert ascii to real values
				if( hexVal < 58 ) {
					carry = hexVal - '0';
				} else {
					carry = hexVal - 55;
				}

				//Multiply to get full value
				carry = carry * pow;
				if( carry == 0 && pow == 1) {
					carry = (uint64_t)0;
				}

				//Add carry to dec string
				for( auto &decVal : dec ) {
					if( carry == 0 && decVal == 0  ) {
						continue;
					}
					decV = decVal;
					result = decV + carry;
					value = result % 10;
					decVal = (char)value.toUint64();
					carry = result / 10;
				}
								
				while( carry > 0 ) {
					value = carry % 10;
					dec.push_back( (char)value.toUint64() );
					carry = carry / 10;
				}
				pow = pow << 4; //pow * 16
			}

			//Remove leading(trailing) zeros
			while( dec.back() == 0 ) {
				dec.pop_back();
			}

			for ( auto &decVal : dec ) {
				if( decVal >= 0 && decVal <= 9 ) {
					decVal += '0';
				} else {
					std::cerr << "Invalid character in string: " << decVal << std::endl;
					return "";
				}
			}

			std::reverse(dec.begin(), dec.end());
			return dec;
		}

		string
		toHexString() const {
			std::stringstream ss;
			auto i = value.rbegin();
			auto end = value.rend();
			while( i != end ) {
				ss << std::hex << *i;
				i++;
			}
			return ss.str();

		}

		BigUInt&
		operator+=( BigUInt &b ) {
			this->value.reserve(this->value.size() + b.value.size() + 1);
			uint64_t size = Pad(*this, b);
			uint64_t carry = 0;
			uint64_t i = 0;
			uint64_t result  = 0;
			uint64_t av = 0;
			uint64_t bv = 0;

			//Going through backwards, so prepend
			for (; i < size; i++) {
				av = this->value[i];
				bv = b.value[i];
				result = av + bv + carry;
				value[i] = result % MAX_LIMIT;
				carry = result / MAX_LIMIT;
			}

			while ( carry > 0 ) {
				prepend( carry % MAX_LIMIT );
				carry = carry / MAX_LIMIT;
			}

			BigUInt::Unpad(*this, b);
			return *this;
		}

	private:
		//Stores value in reverse
		vector<uint32_t> value;

		void
		initFromString(string in) {
			string process = in;
			BigUInt pow(1);
			BigUInt result((uint64_t)0);
			uint8_t e;

			if( process.substr(0,2) == "0x" ) {
				e = 0x10;
				process = process.substr(2);
				std::transform( process.begin(), process.end(), process.begin(), ::toupper );

				for( auto &hexIn : process ) {
					if( hexIn >= '0' && hexIn <= '9' ) {
						hexIn -= '0';
					} else if( hexIn >= 'A' && hexIn <= 'F' ) {
						hexIn -= 55;							
					} else {
						std::cerr << "Error in string - invalid character: " << hexIn << std::endl;
					}
				}
			} else {
				e = 10;
	
				for( auto &numIn : process ) {
					if( numIn >= '0' && numIn <= '9' ) {
						numIn -= '0';
					} else {
						std::cerr << "Error in string - invalid character: " << numIn << std::endl;
					}
				}
			}

			std::reverse( process.begin(), process.end() );
			for( auto c : process ) {
				result = pow * c;
				*this += result;
				pow = pow * e;
			}

		}


		size_t
		getNumberOfHexDigits() {
			size_t num = (value.size()-1) * 8; //4 bytes per entry, so 8 hex values
			uint32_t back = value.back();

			size_t backSize = 0;
			while( back != 0 ) {
				backSize++;
				back = back >> 4;
			}

			return num + backSize;
		}

		//Zero hex based where 0 returns most significant value.
		uint8_t
		getHexValueAt(uint64_t idx) {
			size_t numDigits = getNumberOfHexDigits();
			size_t revIdx = numDigits - idx - 1;

			size_t out = revIdx / 8;
			size_t in  = revIdx % 8;

			uint32_t val = value[out];
			for( int i = 0; i < in; ++i ) {
				val = val >> 4;
			}
			return val & 0xF;
		}

		void prepend( uint32_t a ) {
			value.push_back( a );
		}

		void append( uint32_t a ) {
			value.insert( value.begin(), a );
		}

		static BigUInt
		LeftShift( BigUInt &a ) {
			BigUInt ret = a;
			
			bool addEntry = false;
			auto i = ret.value.rbegin();
			auto end = ret.value.rend();
			auto next = ret.value.rbegin();
			next++;
			uint32_t carry = 0;
		
			//Overflow into new entry
			if( (*i) & 0x80000000 ) {
				addEntry = true;	
			}

			while( i != end ) {

				if( next != end && (*next) & 0x80000000 ){
					carry = 1;	
				} else {
					carry = 0;
				}

				*i = (*i) << 1;
				*i = *i + carry;

				i++;
				next++;
			}

			if( addEntry ) {
				ret.prepend(0x1);
			}

			return ret;
		}
	
		static BigUInt
		RightShift( BigUInt &a ) {
			BigUInt ret = a;

			auto i = ret.value.begin();
			auto end = ret.value.end();
			auto next = ret.value.begin();
			next++;
			uint32_t carry = 0;

			while( i != end ) {

				if( next!= end && (*next) & 0x1 ){
					carry = 0x80000000;	
				} else {
					carry = 0;
				}

				*i = (*i) >> 1;
				*i = *i + carry;

				i++;
				next++;
			}
			
			return ret;
		}
		static BigUInt
		Mul_u32int( BigUInt &a, uint32_t b ) {
			
			BigUInt ret;

			if( b == 2 ) {
				ret = a << 1;
				return ret;
			}

			uint64_t bv = b;
			uint64_t carry = 0;	
			uint64_t value = 0;

			//Starting from LSB, so prepend values
			for( uint64_t i : a.value ) {
				value = i * bv + carry;
				carry = value >> 32;
				ret.prepend(value & UINT64_LOW);
			}

			if( carry ) {
				ret.prepend(carry);
			}
			return ret;

		}
		
		static uint64_t	
		Pad( BigUInt &a, BigUInt &b ) {
			if ( a.value.size() > b.value.size() ) {
				int diff = (a.value.size() - b.value.size());
				for ( int i = 0; i < diff; ++i ) {
					b.prepend(0);
				}
			} else {
				int diff = (b.value.size() - a.value.size());
				for ( int i = 0; i < diff; ++i ) {
					a.prepend(0);
				}
			}
			return a.value.size();
		}

		static void
		Unpad( BigUInt &a, BigUInt &b ) {
			while(a.value.back() == 0 && a.value.size() != 1) {
				a.value.pop_back();
			}
			
			while(b.value.back() == 0 && b.value.size() != 1) {
				b.value.pop_back();
			}
		}
		
		static void
		Unpad( BigUInt &a ) {
			while(a.value.back() == 0 && a.value.size() != 1) {
				a.value.pop_back();
			}
		}

		static BigUInt
		DACDivide( BigUInt &N, BigUInt &D, bool mod = false ) {
			BigUInt ret((uint64_t)0);
			if( D == 2 && mod == false) {
				ret = N >> 1;
				return ret;
			}
			if( D == N ) {
				if( !mod ) {
					ret = 1;
					return ret;
				} else {
					ret = (uint64_t)0;
					return ret;
				}
			}

			if( N < D ) {
				if( mod ) {
					return N;
				} else {
					ret = (uint64_t) 0;
					return ret;
				}
			}
			
			BigUInt max = N;
			BigUInt min = 1;

			BigUInt half = max - min;
			half = half >> 1;
			half += min;

			while(true) {
				BigUInt result = half * D;
				
				if( result == N ) {
					if( !mod ) {
						return half;
					} else {
						BigUInt ret((uint64_t)0);
						return ret;
					}
				}

				if( N < result ) {
					max = half;
				} else if( result < N ){
					min = half;
				}

				half = max - min;

				//TODO this is kind of ugly
				//maybe cleaner solution when half < 5
				if( half < 5 ) {
					max = max + 1;
					while( min != max ) {
						result = min * D;
						if( N == result ) {
							if( !mod ) {
								return min;
							} else {
								BigUInt ret((uint64_t)0);
								return ret;
							}
						} else if( N < result ) {
							BigUInt ret;
							if( !mod ) {
								ret = min - 1;
								return ret;
							} else {
								min = min - 1;
								result = min * D;
								BigUInt ret = N - result;
								return ret;
							}
						}
						min = min + 1;

					}
				}

				half = half >> 1; // half / 2
				half += min;
			}

		}

		static BigUInt
		LongDivide( BigUInt &N, BigUInt &D, bool mod = false ) {
			BigUInt ret((uint64_t)0);

			if( D == 2 && mod == false) {
				ret = N >> 1;
				return ret;
			}

			size_t Nsize = N.getNumberOfHexDigits();

			BigUInt Npart( (uint64_t)0 );
		
			bool solved = false;
			size_t idx = 0;
			while(true) {
				//Find appropriate Npart
				while(Npart < D) {

					if( idx + 1 > Nsize) {
						//Done
						solved = true;
						break;
					} else {
						Npart = Npart << 4;
						Npart = Npart + N.getHexValueAt(idx++);

						//Add digit to result (below)
						ret = ret << 4;
					}
				}
				
				if( solved ) {
					if( !mod ) {
						return ret;
					} else {
						return Npart;
					}
				}

				BigUInt Dmult = D;
				if( Dmult == Npart ) {
					Npart = (uint64_t)0;
					ret = ret + 1;
				} else {

					//till it goes over D
					size_t Qpart = 1;

					while(true) {
						if( Dmult < Npart ) {
							Dmult = D * ++Qpart;
						} else {
							break;
						}
					}
					Dmult = D * --Qpart;

					//Remainder
					Npart = Npart - Dmult;

					//Put value in result.
					ret = ret + Qpart;
				}
			}
		}

		static BigUInt
		Karatsuba( BigUInt &a, BigUInt &b ) {
			if( a.size() == 0 || b.size() == 0 ) {
				std::cerr << "Error: Karatsuba - a value has no contents." << std::endl;
			}
			
			if( a.size() == 1 || b.size() == 1 ) {
				return a * b;
			}
			
			BigUInt ret;

			uint64_t m = Pad(a, b);
			uint64_t m2 = m/2;

			ret.value.reserve(m*2);
			
			BigUInt  low1(a.value.begin()     , a.value.begin() + m2);
			BigUInt high1(a.value.begin() + m2, a.value.end());
			
			BigUInt  low2(b.value.begin()     , b.value.begin() + m2);
			BigUInt high2(b.value.begin() + m2, b.value.end());

			BigUInt z0 = Karatsuba(low1, low2);
		
			BigUInt lowhigh1 = low1 + high1;
			BigUInt lowhigh2 = low2 + high2;
			BigUInt z1 = Karatsuba(lowhigh1, lowhigh2);
			
				
			BigUInt z2 = Karatsuba(high1, high2);
			
			BigUInt zdiff = z1 - z2;
			zdiff = zdiff - z0;

			for (int i = 0; i < m2 * 2; i++) {
				z2.append(0);
			}

			for (int i = 0; i < m2; i++) {
				zdiff.append(0);
			}
		
			ret = z2 + zdiff;
			ret = ret + z0;

			Unpad(a, b);
			Unpad(ret);

			return ret;

		}

	friend bool operator==( BigUInt &a, BigUInt &b );
	friend bool operator==( BigUInt &a, uint32_t b );
	friend bool operator!=( BigUInt &a, BigUInt &b );
	friend bool operator< (BigUInt &a, BigUInt &b);
	friend bool operator< (BigUInt &a, uint32_t b);
	friend bool operator> (BigUInt &a, uint32_t b);
	friend std::ostream& operator <<(std::ostream& stream, const BigUInt& a);
	friend BigUInt operator <<( BigUInt &a, uint64_t b );
	friend BigUInt operator >>( BigUInt &a, uint64_t b );
	friend BigUInt operator+( BigUInt &a, BigUInt &b );
	friend BigUInt operator+( BigUInt &a, uint32_t b );
	friend BigUInt operator-( BigUInt &a, BigUInt &b );
	friend BigUInt operator-( BigUInt &a, uint32_t b );
	friend BigUInt operator*( BigUInt &a, BigUInt &b );
	friend BigUInt operator*( BigUInt &a, uint32_t b );
	friend BigUInt operator^( BigUInt &a, BigUInt &b );
	friend BigUInt operator^( BigUInt &a, uint32_t b );
	friend BigUInt operator/( BigUInt &a, BigUInt &b );
	friend BigUInt operator/( BigUInt &a, uint32_t b );
	friend BigUInt operator%( BigUInt &a, BigUInt &b );
	friend BigUInt operator%( BigUInt &a, uint32_t b );
};

BigUInt
operator>>( BigUInt &a, uint64_t b ) {
	BigUInt ret = a;
	for( uint64_t i = 0; i < b; ++i) {
		ret = BigUInt::RightShift(ret);
	}
	BigUInt::Unpad(ret);
	return ret;
}

BigUInt
operator<<( BigUInt &a, uint64_t b ) {
	BigUInt ret = a;
	for( uint64_t i = 0; i < b; ++i) {
		ret = BigUInt::LeftShift(ret);
	}
	return ret;

}

BigUInt
operator%( BigUInt &a, BigUInt &b ) {
	if( a == 0 ) {
		BigUInt ret((uint64_t)0);
		return ret;
	}

	return BigUInt::DACDivide(a, b, true);
}

BigUInt
operator%( BigUInt &a, uint32_t bv ) {
	BigUInt b(bv);
	return operator%(a, b);
}

BigUInt
operator/( BigUInt &a, BigUInt &b ) {
	if( b == 0 ) {
		std::cerr << "Error: Divide by 0." << std::endl;
		BigUInt ret;
		return ret;
	}

	return BigUInt::DACDivide(a, b);
}

BigUInt
operator/( BigUInt &a, uint32_t bv ) {
	BigUInt b(bv);
	return operator/(a, b);
}

BigUInt
operator^( BigUInt &a, uint32_t bv ) {
	BigUInt b(bv);
	return operator^(a, b);
}

BigUInt
operator^( BigUInt &a, BigUInt &b ) {
	BigUInt ret = a;
	BigUInt count(b-1);
	while( count > 0  ) {
		ret = ret * a;
		count = count - 1;
	}

	return ret;
}

bool
operator> (BigUInt &a, uint32_t b) {
	return a.value.size() == 1 ? a.value[0] > b : false;
}

bool
operator< (BigUInt &a, BigUInt &b) {
	if( a.value.size() != b.value.size() ) {
		return a.value.size() < b.value.size();
	} else {
		for( int i = a.value.size()-1; i >= 0; --i ) {
			if( a.value[i] != b.value[i] ) {
				return a.value[i] < b.value[i];
			}
		}
	}
	return false;
}

bool
operator< (BigUInt &a, uint32_t b) {
	return a.value.size() == 1 ? a.value[0] < b : false;
}

BigUInt
operator-( BigUInt &a, uint32_t bv ) {
	BigUInt b(bv);
	return operator-(a, b);
}
	
BigUInt
operator-( BigUInt &a, BigUInt &b ) {
	BigUInt ret;
	uint64_t size = BigUInt::Pad(a, b);
	ret.value.reserve(size);
	uint64_t i = 0;
	bool borrow = false;
	int64_t result  = 0;

	//Going through backwards, so prepend
	for (; i < size; i++) {
		uint64_t av = a.value[i];
		uint64_t bv = b.value[i];
		result = av - bv;
		if( borrow ) {
			result -= 1;
			borrow = false;
		}

		if( result <  0 ) {
			result = MAX_LIMIT + result;
			borrow = true;
		}

		ret.prepend( result % MAX_LIMIT );
	}

	if( borrow ) {
		uint64_t v = ret.value[i];
		ret.prepend( v - 1 );
	}

	BigUInt::Unpad(a, b);
	BigUInt::Unpad(ret);
	return ret;
}

BigUInt
operator+( BigUInt &a, uint32_t bv ) {
	BigUInt b(bv);
	return operator+(a, b);
}

BigUInt
operator+( BigUInt &a, BigUInt &b ) {
	BigUInt ret;
	ret.value.reserve(a.value.size() + b.value.size() + 1);
	uint64_t size = BigUInt::Pad(a, b);
	uint64_t carry = 0;
	uint64_t i = 0;
	uint64_t result  = 0;
	uint64_t av = 0;
	uint64_t bv = 0;

	//Going through backwards, so prepend
	for (; i < size; i++) {
		av = a.value[i];
		bv = b.value[i];
		result = av + bv + carry;
		ret.prepend( result % MAX_LIMIT );
		carry = result / MAX_LIMIT;
	}

	if ( carry != 0 ) {
		ret.prepend( carry );
	}

	BigUInt::Unpad(a, b);
	return ret;
}

std::ostream&
operator <<(std::ostream& stream, const BigUInt& a) {
	string outs = a.toString();
	stream << outs;
	return stream;
}

BigUInt
operator*( BigUInt &a, BigUInt &b ) {
	if( (a.value.size() == 1 && a.value[0] == 0) || (b.value.size() == 1 && b.value[0] == 0) ) {
		return (uint64_t)0;			
	} else if (a.value.size() == 1 && b.value.size() == 1 ) {
		uint64_t av = a.value[0];
		uint64_t bv = b.value[0];
		return BigUInt(av * bv);
	} else if (  a.value.size() == 1 || b.value.size() == 1 ) {
		if ( a.value.size() == 1 ) {
			return BigUInt::Mul_u32int( b, a.value[0] );
		} else {
			return BigUInt::Mul_u32int( a, b.value[0] );
		}
	} else 	{
		return BigUInt::Karatsuba(a, b);
	}
}

BigUInt
operator*( BigUInt &a, uint32_t buint ) {
	BigUInt b(buint);
	return operator*(a, b);
}

bool
operator!=( BigUInt &a, BigUInt &b ) {
	return !operator==(a, b);
}

bool
operator==( BigUInt &a, uint32_t bv ) {
	BigUInt b(bv);
	return operator==(a, b);
}

bool
operator==( BigUInt &a, BigUInt &b ) {
	if( a.size() != b.size() ){
		return false;
	}

	for( int i = 0; i < a.size(); ++i ) {
		if( a.value[i] != b.value[i] ) {
			return false;
		}
	}
	return true;
}
