using System;
using System.Runtime.InteropServices;
using System.Text;

namespace ProcessHook
{
    class FunctionHooks
    {
        public const string argsDemiliter = "|/|/|";
        /****************************************
         * 
         * Constants of messages
         * 
         ***************************************/
        public const string CreateFileWStr = "CreateFileW";
        public const string WriteFileStr = "WriteFile";
        public const string DeleteFileStr = "DeleteFileW";
        public const string MoveFileStr = "MoveFileW";
        public const string CryptEncryptStr = "CryptEncrypt";


        /****************************************
         * 
         *  private helper functions
         * 
         * *************************************/
        private static void reportEvent(string functionName, params string[] args)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(args[0]);
            for (int i = 1; i < args.Length; i++)
            {
                sb.Append(argsDemiliter);
                sb.Append(args[i]);
            }

            ProcessHook.enqueueEvent(functionName, sb.ToString());
        }

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
                reportEvent(CreateFileWStr, filename);
            }
            catch
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

                reportEvent(WriteFileStr, filename.ToString());

            }
            catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            return result;
        }

        /****************************************
         * hook for: DeleteFileW
         ***************************************/
        // delegate of 
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public delegate bool DeleteFileW_Delegate(
            string filename);

        // import of original api
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool DeleteFileW(string lpFileName);

        // hook function
        public static bool DeleteFileW_Hook(
            string filename)
        {
            try
            {
                reportEvent(DeleteFileStr, filename);
            }
            catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            return DeleteFileW(filename);
        }


        /****************************************
         * hook for: MoveFileW
         ***************************************/
        // delegate of 
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public delegate bool MoveFileW_Delegate(string lpExistingFileName,
            string lpNewFileName);

        // import of original api
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool MoveFileW(string lpExistingFileName,
            string lpNewFileName);

        // hook function
        public static bool MoveFileW_Hook(string lpExistingFileName,
           string lpNewFileName)
        {
            try
            {
                reportEvent(MoveFileStr, lpExistingFileName, lpNewFileName);
            }
            catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            return MoveFileW(lpExistingFileName, lpNewFileName);
        }



        /****************************************
         * hook for: CryptEncrypt
         ***************************************/
        // delegate of 
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public delegate bool CryptEncrypt_Delegate(
            IntPtr hKey,
            IntPtr hHash,
            int Final,
            uint dwFlags,
            byte[] pbData,
            ref uint pdwDataLen,
            uint dwBufLen);

        // import of original api
        [DllImport(@"advapi32.dll", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool CryptEncrypt(
            IntPtr hKey,
            IntPtr hHash,
            int Final,
            uint dwFlags,
            byte[] pbData,
            ref uint pdwDataLen,
            uint dwBufLen);

        // hook function
        public static bool CryptEncrypt_Hook(
            IntPtr hKey,
            IntPtr hHash,
            int Final,
            uint dwFlags,
            byte[] pbData,
            ref uint pdwDataLen,
            uint dwBufLen)
        {
            try
            {
                reportEvent(CryptEncryptStr, "");
            }
            catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            return CryptEncrypt(hKey, hHash, Final, dwFlags, pbData, ref pdwDataLen, dwBufLen);
        }
    }
}
