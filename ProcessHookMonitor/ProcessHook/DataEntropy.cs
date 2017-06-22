using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ProcessHook
{
    public class DataEntropy
    {
        // Stores the number of times each symbol appears
        Dictionary<byte, int> distributionDict;
        // Stores the entropy for each character
        Dictionary<byte, double> probabilityDict;

        string filename;

        double overalEntropy;
        // Bytes of data processed
        int dataSize;


        public double Entropy
        {
            get { return GetEntropy(); }
        }

        double GetEntropy()
        {
            ProcessHook.reportStatus("HERE");
            if (!File.Exists(filename))
            {
                return -1;
            }
            
            ProcessHook.reportStatus("and here HERE");
            ExamineChunk(filename);
            ProcessHook.reportStatus("and also HERE");
            // Reset values
            overalEntropy = 0;
            probabilityDict = new Dictionary<byte, double>();

            foreach (KeyValuePair<byte, int> entry in distributionDict)
            {
                // Probability = Freq of symbol / # symbols examined thus far
                probabilityDict.Add(
                    entry.Key,
                    (double)distributionDict[entry.Key] / (double)dataSize
                );
            }

            foreach (KeyValuePair<byte, double> entry in probabilityDict)
            {
                // Entropy = probability * Log2(1/probability)
                overalEntropy += entry.Value * Math.Log((1 / entry.Value), 2);
            }
            return overalEntropy;
        }

        public void ExamineChunk(string filename)
        {
            //System.IO.FileStream file = new System.IO.FileStream(filename, FileMode.Open);


            int actualRead = 0;
            const int CHUNK_SIZE = 1024;
            byte[] buffer = new byte[CHUNK_SIZE];
            distributionDict = new Dictionary<byte, int>();

            FileReader fr = new FileReader();

            if (fr.Open(filename))
            {
                // Assume that an ASCII file is being read.
                System.Text.ASCIIEncoding Encoding = new System.Text.ASCIIEncoding();

                do
                {
                    actualRead = fr.Read(buffer, 0, buffer.Length);
                    //string content = Encoding.GetString(buffer, 0, actualRead);
                    //System.Console.Write("{0}", content);

                    for (int i = 0; i < actualRead; ++i)
                    {
                        byte bite = buffer[i];
                        if (!distributionDict.ContainsKey(bite))
                        {
                            distributionDict.Add(bite, 1);
                        }
                        else
                        {
                            
                        }
                        dataSize++;
                    }
                } while (actualRead > 0);

                fr.Close();

            }
            ProcessHook.reportStatus(Marshal.GetExceptionForHR(Marshal.GetHRForLastWin32Error()).ToString());
            /*
            while ((actualRead = file.Read(buffer, 0 ,CHUNK_SIZE)) != 0)
            {
                for (int i = 0; i<actualRead; ++i)
                {

                    byte bite = buffer[i];
                    if (!distributionDict.ContainsKey(bite))
                    {
                        distributionDict.Add(bite, 1);
                    }
                    else
                    {
                        distributionDict[bite]++;
                    }
                    dataSize++;
                }

            }

            file.Close();
            */

        }

        public static double GetEntropyOfStream(byte[] buffer, int len)
        {

            // Stores the number of times each symbol appears
            Dictionary<byte, int> distributionDict = new Dictionary<byte, int>();
            // Stores the entropy for each character
            Dictionary<byte, double> probabilityDict;

            for (int i = 0; i < len; ++i)
            {
                byte bite = buffer[i];
                if (!distributionDict.ContainsKey(bite))
                {
                    distributionDict.Add(bite, 1);
                }
                else
                {
                    distributionDict[bite]++;
                }
            }

            // Reset values
            double overalEntropy = 0;
            probabilityDict = new Dictionary<byte, double>();

            foreach (KeyValuePair<byte, int> entry in distributionDict)
            {
                // Probability = Freq of symbol / # symbols examined thus far
                probabilityDict.Add(
                    entry.Key,
                    (double)distributionDict[entry.Key] / (double)len
                );
            }

            foreach (KeyValuePair<byte, double> entry in probabilityDict)
            {
                // Entropy = probability * Log2(1/probability)
                overalEntropy += entry.Value * Math.Log((1 / entry.Value), 2);
            }
            return overalEntropy;
        }

        public DataEntropy(string fileName)
        {
            this.filename = fileName;
        }
    }

}
