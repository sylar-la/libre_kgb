#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include "EndianUtility.h"

#include <stdint.h>
#include <string>
#include <vector>

class Buffer
{
   public:
    // Appends the specified data to the packet, moving the write head by the specified length.
    void Append(uint8_t* data, uint32_t len);
    // Appends the specified value to the packet, moving the write head by the size of the specified value type.
    template <typename T>
    void Append(T value)
    {
        static_assert(std::is_fundamental<T>::value, "append(compound)");
        EndianUtility::Standardize(value);
        Append((uint8_t*) &value, sizeof(value));
    }

    // Reads the specified value type at the location of the read head, moving the read head by the size of the
    // specified value type.
    template <typename T>
    T Read()
    {
        T res = Read<T>(m_readPosition);
        m_readPosition += sizeof(T);
        return res;
    }
    // Reads the specified value type at the specified location. This function does not move the read head.
    template <typename T>
    T Read(uint32_t pos)
    {
        if (pos + sizeof(T) > m_data.size())
            return T();
        T val = *((T const*) &m_data[pos]);
        EndianUtility::Standardize(val);
        return val;
    }
    // Reads the specified amount of data from the packet to the specified buffer. movePos specifies whether to move the
    // read head by the specified length.
    void Read(uint8_t* buffer, uint32_t len, bool movePos = true);

    // Returns a pointer to the underlying buffer.
    uint8_t* GetBuffer();

    // Returns the size of the underlying buffer - aka how many bytes are stored in this packet.
    uint32_t GetSize();

    // Returns the number of bytes remaining to be read in the buffer.
    uint32_t GetRemaining();

    // Clears the contents of this packet, resetting the read/write heads.
    void Clear();

    // Resizes the underlying buffer to the specified size.
    void Resize(uint32_t size);

    // Moves the specified amount of data within the buffer from the specified position to the specified position.
    void MoveData(uint32_t fromPosition, uint32_t toPosition, uint32_t length);

    uint32_t GetReadPosition();
    void SetReadPosition(uint32_t position);

    uint32_t GetWritePosition();
    void SetWritePosition(uint32_t position);

    // Prints a dump of this packet in hex + ASCII format.
    void HexDump();

   public:
    // Functions used to read/write to the buffer.
    int8_t ReadInt8();
    uint8_t ReadUInt8();
    int16_t ReadInt16();
    uint16_t ReadUInt16();
    int32_t ReadInt32();
    uint32_t ReadUInt32();
    int64_t ReadInt64();
    uint64_t ReadUInt64();
    float ReadFloat();
    double ReadDouble();

    std::string ReadString();

    void WriteInt8(int8_t value);
    void WriteUInt8(uint8_t value);
    void WriteInt16(int16_t value);
    void WriteUInt16(uint16_t value);
    void WriteInt32(int32_t value);
    void WriteUInt32(uint32_t value);
    void WriteInt64(int64_t value);
    void WriteUInt64(uint64_t value);
    void WriteFloat(float value);
    void WriteDouble(double value);

    void WriteString(const std::string& str);

   private:
    // The STL vector holding the data for this packet.
    std::vector<uint8_t> m_data;
    // The position of the read head.
    uint32_t m_readPosition = 0;
    // The position of the write head.
    uint32_t m_writePosition = 0;
};

#endif
