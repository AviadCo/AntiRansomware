using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ProcessHook
{
    class FunctionHooks
    {
        
        /****************************************
         * 
         * Constants of messages
         * 
         ***************************************/
        private const string CreateFileWStr = "CreateFileW";
        private const string WriteFileStr = "WriteFile";
        private const string DeleteFileStr = "DeleteFile";
        private const string MoveFileStr = "MoveFile";


        /****************************************
         * 
         *  dll api imports
         * 
         * *************************************/
        /// <summary>
        /// P/Invoke to determine the filename from a file handle
        /// https://msdn.microsoft.com/en-us/library/windows/desktop/aa364962(v=vs.85).aspx
        /// </summary>
        /// <param name="hFile"></param>
        /// <param name="lpszFilePath"></param>
        /// <param name="cchFilePath"></param>
        /// <param name="dwFlags"></param>
        /// <returns></returns>
        [DllImport("Kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        static extern uint GetFinalPathNameByHandle(IntPtr hFile, [MarshalAs(UnmanagedType.LPTStr)] StringBuilder lpszFilePath, uint cchFilePath, uint dwFlags);


        /****************************************
         * hook for: CreateFileW
         ***************************************/
        //delegate
        [UnmanagedFunctionPointer(CallingConvention.StdCall,
                     CharSet = CharSet.Unicode,
                     SetLastError = true)]
        public delegate IntPtr CreateFileW_Delegate(
                     String filename,
                     UInt32 desiredAccess,
                     UInt32 shareMode,
                     IntPtr securityAttributes,
                     UInt32 creationDisposition,
                     UInt32 flagsAndAttributes,
                     IntPtr templateFile);

        //import of original api
        [DllImport("kernel32.dll",
            CharSet = CharSet.Unicode,
            SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        static extern IntPtr CreateFileW(
            String filename,
            UInt32 desiredAccess,
            UInt32 shareMode,
            IntPtr securityAttributes,
            UInt32 creationDisposition,
            UInt32 flagsAndAttributes,
            IntPtr templateFile);

        //hook function
        public static IntPtr CreateFileW_Hook(
        String filename,
        UInt32 desiredAccess,
        UInt32 shareMode,
        IntPtr securityAttributes,
        UInt32 creationDisposition,
        UInt32 flagsAndAttributes,
        IntPtr templateFile)
        {
            try
            {
                ProcessHook.enqueueEvent(CreateFileWStr, filename);
            } catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            // now call the original API...
            return CreateFileW(
                filename,
                desiredAccess,
                shareMode,
                securityAttributes,
                creationDisposition,
                flagsAndAttributes,
                templateFile);

        }


        /****************************************
         * hook for: WriteFile
         ***************************************/
        // delegate of 
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public delegate bool WriteFile_Delegate(
            IntPtr hFile,
            IntPtr lpBuffer,
            uint nNumberOfBytesToWrite,
            out uint lpNumberOfBytesWritten,
            IntPtr lpOverlapped);

        // import of original api
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool WriteFile(
            IntPtr hFile,
            IntPtr lpBuffer,
            uint nNumberOfBytesToWrite,
            out uint lpNumberOfBytesWritten,
            IntPtr lpOverlapped);


        // hook function
        public static bool WriteFile_Hook(
        IntPtr hFile,
        IntPtr lpBuffer,
        uint nNumberOfBytesToWrite,
        out uint lpNumberOfBytesWritten,
        IntPtr lpOverlapped)
        {
            bool result = false;

            // Call original first so we get lpNumberOfBytesWritten
            result = WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, out lpNumberOfBytesWritten, lpOverlapped);

            try
            {
                // Retrieve filename from the file handle
                StringBuilder filename = new StringBuilder(255);
                GetFinalPathNameByHandle(hFile, filename, 255, 0);

                ProcessHook.enqueueEvent(WriteFileStr, filename.ToString());

            }
            catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            return result;
        }

        /****************************************
         * hook for: DeleteFile
         ***************************************/
        // delegate of 
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public delegate bool DeleteFile_Delegate(
            string filename);

        // import of original api
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool DeleteFile(string lpFileName);

        // hook function
        public static bool DeleteFile_Hook(
            string filename)
        {
            try
            {
                ProcessHook.enqueueEvent(DeleteFileStr, filename);
            }
            catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            return DeleteFile(filename);
        }


        /****************************************
         * hook for: MoveFile
         ***************************************/
        // delegate of 
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public delegate bool MoveFile_Delegate(string lpExistingFileName,
            string lpNewFileName);

        // import of original api
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool MoveFile(string lpExistingFileName,
            string lpNewFileName);

        // hook function
        public static bool MoveFile_Hook(string lpExistingFileName,
           string lpNewFileName)
        {
            try
            {
                ProcessHook.enqueueEvent(DeleteFileStr, lpExistingFileName + "|||" + lpNewFileName);
            }
            catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            return MoveFile(lpExistingFileName, lpNewFileName);
        }
    }
}
