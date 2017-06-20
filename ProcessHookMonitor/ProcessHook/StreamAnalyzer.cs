using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProcessHook
{
    class StreamAnalyzer
    {
        public static readonly string appWorkPath = Path.GetTempPath() + "ant.ram.temp\\";

        private static string runShell(string command, string workingPath, string commandArgs)
        {
            string path = appWorkPath + "utils\\";
            //Create process
            System.Diagnostics.Process pProcess = new System.Diagnostics.Process();

            //strCommand is path and file name of command to run
            pProcess.StartInfo.FileName = path + command;

            //strCommandParameters are parameters to pass to program
            pProcess.StartInfo.Arguments = commandArgs;

            pProcess.StartInfo.CreateNoWindow = true;
            pProcess.StartInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;

            pProcess.StartInfo.UseShellExecute = false;

            //Set output of program to be written to process output stream
            pProcess.StartInfo.RedirectStandardOutput = true;

            //Optional
            //pProcess.StartInfo.WorkingDirectory = Path.GetTempPath() + "ant.ram.temp\\utils";
            pProcess.StartInfo.WorkingDirectory = workingPath;

            //Start the process
            pProcess.Start();
            
            //Get program output
            string strOutput = pProcess.StandardOutput.ReadToEnd();

            //Wait for process to finish
            pProcess.WaitForExit();

            return strOutput;
        }


        public static String getFileType(String filename)
        {
            string result = runShell("file\\bin\\file.exe", "", "\"" + filename + "\"" );
            int startPos = result.IndexOf(";");
            return startPos == -1 ? result : result.Substring(startPos + 2);
        }

        public static String createHashToFile(String filename, String suffix)
        {
            string refactorFilename = Path.GetFileName(filename);
            string hashfilename = filename.Replace("\\", "").Replace(":", "") + suffix + ".ant.ram.temp";
            runShell("sdhash\\sdhash.exe", Path.GetDirectoryName(filename), refactorFilename + " -o " + "\"" + appWorkPath + hashfilename + "\"");
            return hashfilename + ".sdbf";
        }

        public static String compareHashes(String filenameBefore, String filenameAfter)
        {
            
            string output = runShell("sdhash\\sdhash.exe", appWorkPath, " -c " + "\"" + appWorkPath + filenameBefore + "\"" +  " " + "\"" + appWorkPath + filenameAfter + "\"");
            if (output.Equals(""))
                return "-1";
            else
                return output.Split('|')[2];
        }
    }
}
