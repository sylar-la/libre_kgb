#ifndef ENDIANUTILITY_H
#define ENDIANUTILITY_H

#include <algorithm>
#include <stdint.h>

namespace ByteConverter
{
    template <size_t T>
    inline void convert(char *val)
    {
        std::swap(*val, *(val + T - 1));
        convert<T - 2>(val + 1);
    }

    template <>
    inline void convert<0>(char *)
    {
    }
    template <>
    inline void convert<1>(char *)
    {
    }  // ignore central byte

    template <typename T>
    inline void apply(T *val)
    {
        convert<sizeof(T)>((char *) (val));
    }
}  // namespace ByteConverter

/*
 * This class is a utility to standardize network data to the little-endian format.
 * Data from the network is sent/received little-endian. OS/Compiler may not use
 * little-endian format. This class converts helps converting input data to big endian
 * when required (mostly Windows), and output data from big endian to little.
 */
class EndianUtility
{
   public:
    // Returns whether the platform/compiler the software is being compiled uses
    // big (true) or little (false) endianness.
    static bool IsPlatformBigEndian();

    // Converts incoming network data to platform endianness, or outgoing data from
    // platform endianness to network LE.
    template <typename T>
    static inline void Standardize(T &val)
    {
        if (IsPlatformBigEndian())
            ByteConverter::apply<T>(&val);
    }
    template <typename T>
    static inline void Standardize(void *val)
    {
        if (IsPlatformBigEndian())
            ByteConverter::apply<T>(val);
    }

   private:
    static bool isPlatformBigEndian;
    static bool isPlatformBigEndianComputed;
};

#endif
