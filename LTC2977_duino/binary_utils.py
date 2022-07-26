# LTC L11 to float and L16 to float conversion 
import struct

def printBits(s,start,end=0):
    bs = []
    for i in range(start,end-1,-1):
        bs.append(str((s >> i) & 1))
    return "".join(bs)


def two_complement(s,b):
    """ 

      s is a number
      b the number of bit to consider (don't forget that we start from zero)
 
    function to compute the two complement of a number
      if the b bit of s is equal to 0 then we return the number
      else we return the number - 2**b
    """
    if s & 2**(b-1):
        return s-2**b
    else:
        return s


def L11ToFloat(f):
    # get the 11 lowest bits the mantissa 
    s = f & (2**11-1)
    Y =  two_complement(s,11)

    # get the bit from [11:15] the exponent
    s = f >> 11 & (2**5-1)
    N = two_complement(s,5)
    return Y*2**N

def FloatToL11_old(f):
    exponent = -16
    mantissa = int(f/(2**exponent))

    while not (-1024 < mantissa < 1023):
        exponent += 1
        mantissa = int(f /(2**exponent))

    return (exponent << 11) + mantissa




def FloatToL11(f):
    exponent = -16
    mantissa = int(f/(2**exponent))

    while not (-1024 < mantissa < 1023):
        exponent += 1
        mantissa = int(f /(2**exponent))

    return int("0x%s" % struct.pack(">h",(exponent << 11) + mantissa).hex(), 0)




def L16ToFloat(l16):
    return l16*2**(-13)

def FloatToL16(f):
    return int(f*2**13)













comment="""    

void printbits(unsigned char v) {
  int i; // for C89 compatability
  for(i = 7; i >= 0; i--) putchar('0' + ((v >> i) & 1));
}

"""
    
