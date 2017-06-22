using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProcessHook
{
class FileReader
{
    const uint GENERIC_READ = 0x80000000;
    const uint OPEN_EXISTING = 3;
    System.IntPtr handle;

    [System.Runtime.InteropServices.DllImport("kernel32", SetLastError = true, ThrowOnUnmappableChar = true, CharSet = System.Runtime.InteropServices.CharSet.Unicode)]
     static extern unsafe System.IntPtr CreateFileW(
            String filename,            // file name
            UInt32 desiredAccess,       // access mode
            UInt32 shareMode,           // share mode
            IntPtr securityAttributes,  // Security Attributes
            UInt32 creationDisposition, // how to create
            UInt32 flagsAndAttributes,  // file attributes
            IntPtr templateFile         // handle to template file      
    );

    [System.Runtime.InteropServices.DllImport("kernel32", SetLastError = true)]
    static extern unsafe bool ReadFile
    (
        System.IntPtr hFile,      // handle to file
        void* pBuffer,            // data buffer
        int NumberOfBytesToRead,  // number of bytes to read
        int* pNumberOfBytesRead,  // number of bytes read
        int Overlapped            // overlapped buffer
    );

    [System.Runtime.InteropServices.DllImport("kernel32", SetLastError = true)]
    static extern unsafe bool CloseHandle
    (
        System.IntPtr hObject // handle to object
    );

    public bool Open(string FileName)
    {
        // open the existing file for reading       
        handle = CreateFileW
        (
            FileName,
            GENERIC_READ,
            0,
            IntPtr.Zero,
            OPEN_EXISTING,
            0,
            IntPtr.Zero
        );

        if (handle != System.IntPtr.Zero)
        {
            return true;
        }
        else
        {

            return false;
        }
    }

    public unsafe int Read(byte[] buffer, int index, int count)
    {
        int n = 0;
        fixed (byte* p = buffer)
        {
            if (!ReadFile(handle, p + index, count, &n, 0))
            {
                return 0;
            }
        }
        return n;
    }

    public bool Close()
    {
        return CloseHandle(handle);
    }
}
}
