#include "Buffer.h"

#include <assert.h>
#include <cstring>

void Buffer::Clear()
{
    m_data.clear();

    m_readPosition = 0;
    m_writePosition = 0;
}

uint8_t* Buffer::GetBuffer()
{
    return m_data.data();
}

uint32_t Buffer::GetSize()
{
    return (uint32_t) m_data.size();
}

uint32_t Buffer::GetRemaining()
{
    return (uint32_t) m_data.size() - m_readPosition;
}

void Buffer::Resize(uint32_t size)
{
    m_data.resize(size);
}

void Buffer::Append(uint8_t* data, uint32_t len)
{
    if (data && len > 0)
    {
        if (m_data.size() < m_writePosition + len)
        {
            // Extend the storage.
            m_data.resize(m_writePosition + len);
        }

        std::memcpy(&m_data[m_writePosition], data, len);

        m_writePosition += len;
    }
}

void Buffer::WriteString(const std::string& str)
{
    uint16_t len = (uint16_t) str.length();
    WriteUInt16(len);
    if (len > 0)
    {
        Append((uint8_t*) str.c_str(), len);
    }
}

void Buffer::Read(uint8_t* buffer, uint32_t len, bool movePos)
{
    if (m_readPosition + len > m_data.size())
        return;

    std::memcpy(buffer, &m_data[m_readPosition], len);

    if (movePos)
        m_readPosition += len;
}

int8_t Buffer::ReadInt8()
{
    return Read<int8_t>();
}

uint8_t Buffer::ReadUInt8()
{
    return Read<uint8_t>();
}

int16_t Buffer::ReadInt16()
{
    return Read<int16_t>();
}

uint16_t Buffer::ReadUInt16()
{
    return Read<uint16_t>();
}

int32_t Buffer::ReadInt32()
{
    return Read<int32_t>();
}

uint32_t Buffer::ReadUInt32()
{
    return Read<uint32_t>();
}

int64_t Buffer::ReadInt64()
{
    return Read<int64_t>();
}

uint64_t Buffer::ReadUInt64()
{
    return Read<uint64_t>();
}

float Buffer::ReadFloat()
{
    return Read<float>();
}

double Buffer::ReadDouble()
{
    return Read<double>();
}

std::string Buffer::ReadString()
{
    uint16_t strSize = ReadUInt16();

    if (strSize == 0)
        return std::string();

    std::string str = std::string((char*) GetBuffer() + GetReadPosition(), strSize);

    SetReadPosition(GetReadPosition() + strSize);

    return str;
}

void Buffer::WriteInt8(int8_t value)
{
    Append<int8_t>(value);
}

void Buffer::WriteUInt8(uint8_t value)
{
    Append<uint8_t>(value);
}

void Buffer::WriteInt16(int16_t value)
{
    Append<int16_t>(value);
}

void Buffer::WriteUInt16(uint16_t value)
{
    Append<uint16_t>(value);
}

void Buffer::WriteInt32(int32_t value)
{
    Append<int32_t>(value);
}

void Buffer::WriteUInt32(uint32_t value)
{
    Append<uint32_t>(value);
}

void Buffer::WriteInt64(int64_t value)
{
    Append<int64_t>(value);
}

void Buffer::WriteUInt64(uint64_t value)
{
    Append<uint64_t>(value);
}

void Buffer::WriteFloat(float value)
{
    Append<float>(value);
}

void Buffer::WriteDouble(double value)
{
    Append<double>(value);
}

uint32_t Buffer::GetReadPosition()
{
    return m_readPosition;
}

void Buffer::SetReadPosition(uint32_t position)
{
    m_readPosition = position;
}

uint32_t Buffer::GetWritePosition()
{
    return m_writePosition;
}

void Buffer::SetWritePosition(uint32_t position)
{
    m_writePosition = position;
}

void Buffer::MoveData(uint32_t fromPosition, uint32_t toPosition, uint32_t length)
{
    if (toPosition + length > m_data.size())
        return;
    memmove(&m_data[toPosition], &m_data[fromPosition], length);
}
