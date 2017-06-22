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
        public const string ReadFileStr = "ReadFile";
        public const string DeleteFileWStr = "DeleteFileW";
        public const string MoveFileWStr = "MoveFileW";
        public const string MoveFileExWStr = "MoveFileExW";
        public const string CryptEncryptStr = "CryptEncrypt";
        public const string ShellExecuteExWStr = "ShellExecuteExW";
        public const string WriteProcessMemoryStr = "WriteProcessMemory";
        public const string CreateProcessWStr = "CreateProcessW";
        public const string CreateRemoteThreadStr = "CreateRemoteThread";
        public const string CreateRemoteThreadExStr = "CreateRemoteThreadEx";


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

        [Flags]
        public enum MoveFileFlags
        {
            MOVEFILE_REPLACE_EXISTING = 0x00000001,
            MOVEFILE_COPY_ALLOWED = 0x00000002,
            MOVEFILE_DELAY_UNTIL_REBOOT = 0x00000004,
            MOVEFILE_WRITE_THROUGH = 0x00000008,
            MOVEFILE_CREATE_HARDLINK = 0x00000010,
            MOVEFILE_FAIL_IF_NOT_TRACKABLE = 0x00000020
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SHELLEXECUTEINFO
        {
            public int cbSize;
            public uint fMask;
            public IntPtr hwnd;
            [MarshalAs(UnmanagedType.LPTStr)]
            public string lpVerb;
            [MarshalAs(UnmanagedType.LPTStr)]
            public string lpFile;
            [MarshalAs(UnmanagedType.LPTStr)]
            public string lpParameters;
            [MarshalAs(UnmanagedType.LPTStr)]
            public string lpDirectory;
            public int nShow;
            public IntPtr hInstApp;
            public IntPtr lpIDList;
            [MarshalAs(UnmanagedType.LPTStr)]
            public string lpClass;
            public IntPtr hkeyClass;
            public uint dwHotKey;
            public IntPtr hIcon;
            public IntPtr hProcess;
        }

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern uint GetProcessId(IntPtr handle);


        [StructLayout(LayoutKind.Sequential)]
        public struct SECURITY_ATTRIBUTES
        {
            public int nLength;
            public IntPtr lpSecurityDescriptor;
            public int bInheritHandle;
        }

        [StructLayout(LayoutKind.Sequential)]
        internal struct PROCESS_INFORMATION
        {
            public IntPtr hProcess;
            public IntPtr hThread;
            public int dwProcessId;
            public int dwThreadId;
        }


        /****************************************
         * 
         *          API     HOOKS
         * 
         * *************************************/


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
         * hook for: ReadFile
         ***************************************/
        //delegate
        /// <summary>
        /// The ReadFile delegate, this is needed to create a delegate of our hook function <see cref="ReadFile_Hook(IntPtr, IntPtr, uint, out uint, IntPtr)"/>.
        /// </summary>
        /// <param name="hFile"></param>
        /// <param name="lpBuffer"></param>
        /// <param name="nNumberOfBytesToRead"></param>
        /// <param name="lpNumberOfBytesRead"></param>
        /// <param name="lpOverlapped"></param>
        /// <returns></returns>
        [UnmanagedFunctionPointer(CallingConvention.StdCall, SetLastError = true)]
        public delegate bool ReadFile_Delegate(
            IntPtr hFile,
            IntPtr lpBuffer,
            uint nNumberOfBytesToRead,
            out uint lpNumberOfBytesRead,
            IntPtr lpOverlapped);

        //import of original api
        /// <summary>
        /// Using P/Invoke to call the orginal function
        /// </summary>
        /// <param name="hFile"></param>
        /// <param name="lpBuffer"></param>
        /// <param name="nNumberOfBytesToRead"></param>
        /// <param name="lpNumberOfBytesRead"></param>
        /// <param name="lpOverlapped"></param>
        /// <returns></returns>
        [DllImport("kernel32.dll", SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        static extern bool ReadFile(
            IntPtr hFile,
            IntPtr lpBuffer,
            uint nNumberOfBytesToRead,
            out uint lpNumberOfBytesRead,
            IntPtr lpOverlapped);

        //hook function
        /// <summary>
        /// The ReadFile hook function. This will be called instead of the original ReadFile once hooked.
        /// </summary>
        /// <param name="hFile"></param>
        /// <param name="lpBuffer"></param>
        /// <param name="nNumberOfBytesToRead"></param>
        /// <param name="lpNumberOfBytesRead"></param>
        /// <param name="lpOverlapped"></param>
        /// <returns></returns>
        public static bool ReadFile_Hook(
            IntPtr hFile,
            IntPtr lpBuffer,
            uint nNumberOfBytesToRead,
            out uint lpNumberOfBytesRead,
            IntPtr lpOverlapped)
        {
            string typeAfter = "-1";
            bool result = ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, out lpNumberOfBytesRead, lpOverlapped);
            double entropy = -1;
            try
            {
                //get entropy of buffer
                byte[] buffer = new byte[lpNumberOfBytesRead];
                Marshal.Copy(lpBuffer, buffer, 0, (int)lpNumberOfBytesRead);
                entropy = DataEntropy.GetEntropyOfStream(buffer, (int)lpNumberOfBytesRead);

                //// Retrieve filename from the file handle
                StringBuilder filename = new StringBuilder(255);
                GetFinalPathNameByHandle(hFile, filename, 255, 0);
                filename = filename.Replace("\\\\?\\", "");
                if (!filename.ToString().Equals(""))
                {
                    typeAfter = StreamAnalyzer.getFileType(filename.ToString());
                }

                reportEvent(ReadFileStr, filename.ToString(), typeAfter, entropy + "", lpNumberOfBytesRead + "");

            }
            catch (Exception e)
            {
                reportEvent(ReadFileStr, "exception! " + e.ToString());
                // swallow exceptions so that any issues caused by this code do not crash target process
            }
      
            return result;
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
            //string hashBeforeFile = "", hashAfterFile = "";
            string typeBefore = "", typeAfter = "";
            StringBuilder filename = new StringBuilder(255);
            //int currentPid = EasyHook.RemoteHooking.GetCurrentProcessId();
            double entropy = -1;

            try
            {
                //get entropy of buffer
                byte[] buffer = new byte[nNumberOfBytesToWrite];
                Marshal.Copy(lpBuffer, buffer, 0, (int)nNumberOfBytesToWrite);
                entropy = DataEntropy.GetEntropyOfStream(buffer, (int)nNumberOfBytesToWrite);


                // Retrieve filename from the file handle
                GetFinalPathNameByHandle(hFile, filename, 255, 0);
                filename = filename.Replace("\\\\?\\", "");
                if (!filename.ToString().Equals(""))
                {
                    //hashBeforeFile = StreamAnalyzer.createHashToFile(filename.ToString(), "_" + currentPid + "b");
                    typeBefore = StreamAnalyzer.getFileType(filename.ToString());
                }
            }
            catch (Exception e)
            {
                reportEvent(WriteFileStr, "exception1! " + e.ToString());
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            bool result = false;

            // Call original first so we get lpNumberOfBytesWritten
            result = WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, out lpNumberOfBytesWritten, lpOverlapped);

            try
            {
                //string hashDiffrence = "-1";
                string typeDiffrence = "-1";

                if (!filename.ToString().Equals(""))
                {
                    //    hashAfterFile = StreamAnalyzer.createHashToFile(filename.ToString(), "_" + currentPid + "a");
                    typeAfter = StreamAnalyzer.getFileType(filename.ToString());
                    
                //    hashDiffrence = StreamAnalyzer.compareHashes(hashBeforeFile, hashAfterFile);
                    typeDiffrence = typeBefore.Equals(typeAfter) ? "1" : "0";

                //    //deletes temp files
                //DeleteFileW("-----");
                //    DeleteFileW(hashAfterFile);
                }


                reportEvent(WriteFileStr, filename.ToString(), typeAfter, typeDiffrence, entropy + "" , nNumberOfBytesToWrite + "");
            }
            catch (Exception e)
            {
                reportEvent(WriteFileStr, "exception2! " + e.ToString());
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
                reportEvent(DeleteFileWStr, filename);
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
                reportEvent(MoveFileWStr, lpExistingFileName, lpNewFileName);
            }
            catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            return MoveFileW(lpExistingFileName, lpNewFileName);
        }

        /****************************************
         * hook for: MoveFileExW
         ***************************************/
        // delegate of 
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public delegate bool MoveFileExW_Delegate(
            string lpExistingFileName,
            string lpNewFileName,
            MoveFileFlags dwFlags);

        // import of original api
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool MoveFileExW(
            string lpExistingFileName, 
            string lpNewFileName,
            MoveFileFlags dwFlags);

        // hook function
        public static bool MoveFileExW_Hook(
            string lpExistingFileName,
            string lpNewFileName,
            MoveFileFlags dwFlags)
        {
            try
            {
                reportEvent(MoveFileExWStr, lpExistingFileName, lpNewFileName);
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

        /****************************************
         * hook for: ShellExecuteExW
         ***************************************/
        // delegate of 
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public delegate bool ShellExecuteExW_Delegate(
            ref SHELLEXECUTEINFO lpExecInfo);

        [DllImport(@"shell32.dll", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool ShellExecuteExW(ref SHELLEXECUTEINFO lpExecInfo);

        // hook function
        public static bool ShellExecuteExW_Hook(
            ref SHELLEXECUTEINFO lpExecInfo)
        {
            try
            {
                reportEvent(ShellExecuteExWStr, lpExecInfo.lpFile, lpExecInfo.lpParameters);
            }
            catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            return ShellExecuteExW(ref lpExecInfo);
        }

        /****************************************
         * hook for: WriteProcessMemory
         ***************************************/
        // delegate of 
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public delegate bool WriteProcessMemory_Delegate(
            IntPtr hProcess,
            int lpBaseAddress,
            IntPtr lpBuffer,
            int nSize,
            ref int lpNumberOfBytesWritten);

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        static extern bool WriteProcessMemory(
            IntPtr hProcess,
            int lpBaseAddress,
            IntPtr lpBuffer,
            int nSize,
            ref int lpNumberOfBytesWritten);

        // hook function
        public static bool WriteProcessMemory_Hook(
            IntPtr hProcess,
            int lpBaseAddress,
            IntPtr lpBuffer,
            int nSize,
            ref int lpNumberOfBytesWritten)
        {
            try
            {
                reportEvent(WriteProcessMemoryStr, Convert.ToString(GetProcessId(hProcess)));
            }
            catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            return WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, ref lpNumberOfBytesWritten);
        }

        /****************************************
         * hook for: CreateProcessW
         ***************************************/
        // delegate of 
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public delegate bool CreateProcessW_Delegate(
            string lpApplicationName,
            string lpCommandLine,
            ref SECURITY_ATTRIBUTES lpProcessAttributes,
            ref SECURITY_ATTRIBUTES lpThreadAttributes,
            bool bInheritHandles,
            uint dwCreationFlags,
            IntPtr lpEnvironment,
            string lpCurrentDirectory,
            [In] IntPtr lpStartupInfo,
            out PROCESS_INFORMATION lpProcessInformation);

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        static extern bool CreateProcessW(
            string lpApplicationName,
            string lpCommandLine,
            ref SECURITY_ATTRIBUTES lpProcessAttributes,
            ref SECURITY_ATTRIBUTES lpThreadAttributes,
            bool bInheritHandles,
            uint dwCreationFlags,
            IntPtr lpEnvironment,
            string lpCurrentDirectory,
            [In] IntPtr lpStartupInfo,
            out PROCESS_INFORMATION lpProcessInformation);

        // hook function
        public static bool CreateProcessW_Hook(
            string lpApplicationName,
            string lpCommandLine,
            ref SECURITY_ATTRIBUTES lpProcessAttributes,
            ref SECURITY_ATTRIBUTES lpThreadAttributes,
            bool bInheritHandles,
            uint dwCreationFlags,
            IntPtr lpEnvironment,
            string lpCurrentDirectory,
            [In] IntPtr lpStartupInfo,
            out PROCESS_INFORMATION lpProcessInformation)
        {

            bool result = false;
            result = CreateProcessW(lpApplicationName, lpCommandLine, ref lpProcessAttributes,
                ref lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
                lpCurrentDirectory, lpStartupInfo, out lpProcessInformation);

            try
            {
                reportEvent(CreateProcessWStr, Convert.ToString(GetProcessId(lpProcessInformation.hProcess)),
                    lpApplicationName, lpCommandLine);
            }
            catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            return result;
        }

        /****************************************
         * hook for: CreateRemoteThread
         ***************************************/
        // delegate of 
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.SysInt)]
        public delegate IntPtr CreateRemoteThread_Delegate(
            IntPtr hProcess,
            IntPtr lpThreadAttributes,
            uint dwStackSize,
            IntPtr lpStartAddress,
            IntPtr lpParameter,
            uint dwCreationFlags,
            out IntPtr lpThreadId);

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        static extern IntPtr CreateRemoteThread(
            IntPtr hProcess,
            IntPtr lpThreadAttributes,
            uint dwStackSize,
            IntPtr lpStartAddress,
            IntPtr lpParameter,
            uint dwCreationFlags,
            out IntPtr lpThreadId);


        // hook function
        public static IntPtr CreateRemoteThread_Hook(
            IntPtr hProcess,
            IntPtr lpThreadAttributes,
            uint dwStackSize,
            IntPtr lpStartAddress,
            IntPtr lpParameter,
            uint dwCreationFlags,
            out IntPtr lpThreadId)
        {

            try
            {
                reportEvent(CreateRemoteThreadStr, Convert.ToString(GetProcessId(hProcess)));
            }
            catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            return CreateRemoteThread(hProcess, lpThreadAttributes, dwStackSize,
                lpStartAddress, lpParameter, dwCreationFlags, out lpThreadId);
        }

        /****************************************
         * hook for: CreateRemoteThreadEx
         ***************************************/
        // delegate of 
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.SysInt)]
        public delegate IntPtr CreateRemoteThreadEx_Delegate(
            IntPtr hProcess,
            IntPtr lpThreadAttributes,
            uint dwStackSize,
            IntPtr lpStartAddress,
            IntPtr lpParameter,
            uint dwCreationFlags,
            IntPtr lpAttributeList,
            [Out] IntPtr lpThreadId);

        [DllImport("kernel32.dll", EntryPoint = "CreateRemoteThreadEx", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        public static extern IntPtr CreateRemoteThreadEx(
            IntPtr hProcess,
            IntPtr lpThreadAttributes,
            uint dwStackSize,
            IntPtr lpStartAddress,
            IntPtr lpParameter,
            uint dwCreationFlags,
            IntPtr lpAttributeList,
            [Out] IntPtr lpThreadId);


        // hook function
        public static IntPtr CreateRemoteThreadEx_Hook(
            IntPtr hProcess,
            IntPtr lpThreadAttributes,
            uint dwStackSize,
            IntPtr lpStartAddress,
            IntPtr lpParameter,
            uint dwCreationFlags,
            IntPtr lpAttributeList,
            [Out] IntPtr lpThreadId)
        {

            try
            {
                reportEvent(CreateRemoteThreadExStr, Convert.ToString(GetProcessId(hProcess)));
            }
            catch
            {
                // swallow exceptions so that any issues caused by this code do not crash target process
            }

            return CreateRemoteThreadEx(hProcess, lpThreadAttributes, dwStackSize,
                lpStartAddress, lpParameter, dwCreationFlags, lpAttributeList, lpThreadId);
        }
    }
}

