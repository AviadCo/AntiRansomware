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
        private static string CreateFileWStr = "CreateFileW";


        /*
         * hook for: CreateFile
         */
        //delegate
        [UnmanagedFunctionPointer(CallingConvention.StdCall,
                     CharSet = CharSet.Unicode,
                     SetLastError = true)]
        public delegate IntPtr CreateFileWDelegate(
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
        public static IntPtr CreateFileWHook(
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
                ProcessHook.enqueueEvent(CreateFileWStr);
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
    }
}
