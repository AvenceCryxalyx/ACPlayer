#pragma once

// Define UARTLIB_EXPORTS when building the DLL
// Automatically handled by the project settings
#ifdef UARTLIB_EXPORTS
#define UARTLIB_API __declspec(dllexport)
#else
#define UARTLIB_API __declspec(dllimport)
#endif

#include <string>
#include <vector>

// ============================================================
// SerialPort Class - exported from DLL
// ============================================================

class UARTLIB_API SerialPort {
public:
    // Constructor
    SerialPort();
    
    // Destructor
    ~SerialPort();
    
    // Open serial port
    // portName: COM port name (e.g., "COM3")
    // baudRate: Baud rate (e.g., 9600, 115200)
    // Returns: true if successful, false otherwise
    bool Open(const std::string& portName, unsigned long baudRate = 9600);
    
    // Close serial port
    void Close();
    
    // Check if port is open
    bool IsOpen() const;
    
    // Send data
    // data: pointer to data to send
    // length: number of bytes to send
    // Returns: number of bytes actually sent, -1 on error
    int SendData(const unsigned char* data, int length);
    
    // Send string
    // str: string to send
    // Returns: number of bytes sent, -1 on error
    int SendString(const std::string& str);
    
    // Receive data (non-blocking)
    // buffer: pointer to buffer to store received data
    // maxLength: maximum number of bytes to read
    // Returns: number of bytes read, 0 if no data available, -1 on error
    int ReceiveData(unsigned char* buffer, int maxLength);
    
    // Wait for data with timeout
    // maxWaitMs: maximum time to wait in milliseconds
    // Returns: true if data is available, false if timeout or error
    bool WaitForData(unsigned long maxWaitMs = 1000);
    
    // Get last error message
    std::string GetLastError() const;
    
    // Set communication parameters
    bool SetBaudRate(unsigned long baudRate);
    bool SetDataBits(unsigned char dataBits = 8);
    bool SetStopBits(unsigned char stopBits = 1);
    bool SetParity(unsigned char parity = 0); // 0=NOPARITY
    
    // Purge buffers
    void PurgeBuffers();
    
    // List available COM ports
    static std::vector<std::string> ListAvailablePorts();

private:
    void* hPort;  // HANDLE - using void* to avoid including windows.h
    std::string lastError;
    
    // Helper function to update error message
    void UpdateErrorMessage(const std::string& context, unsigned long errorCode = 0);
};

// ============================================================
// UARTPacket Structure - exported from DLL
// ============================================================

struct UARTLIB_API UARTPacket {
    // Byte 0: Header (always 0xCC)
    unsigned char header;
    
    // Byte 1: Command type (1-3)
    unsigned char commandType;
    
    // Bytes 2-6: Data values
    unsigned char data1;
    unsigned char data2;
    unsigned char data3;
    unsigned char data4;
    unsigned char data5;
    
    // Byte 7: Footer (always 0xAA)
    unsigned char footer;
    
    // Bytes 8-9: Reserved (no value)
    unsigned char reserved1;
    unsigned char reserved2;
    
    // Bytes 10-14: Reserved/unused (to make 15 bytes total)
    unsigned char reserved3;
    unsigned char reserved4;
    unsigned char reserved5;
    unsigned char reserved6;
    unsigned char reserved7;
    
    // Constructor with default values
    UARTPacket();
    
    // Constructor with data
    UARTPacket(unsigned char cmd, unsigned char d1, unsigned char d2,
               unsigned char d3, unsigned char d4, unsigned char d5);
    
    // Convert packet to byte array (15 bytes)
    std::vector<unsigned char> ToByteArray() const;
    
    // Create packet from byte array
    static UARTPacket FromByteArray(const unsigned char* data, int length);
    
    // Validate packet structure
    bool IsValid() const;
    
    // Get human-readable packet info
    std::string GetInfo() const;
};
