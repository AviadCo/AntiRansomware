sdhash - similarity digest hashing.  

We have statically linked in libraries to make a portable executable for this
testing release.  If you encounter any problems with running this executable
related to dependencies and libraries please contact us.

We have included the Microsoft Redistributable OpenMP library in this 
release in case it is not pre-installed, and static linked the main ones.

Please note that we do not yet have support for reading non-ascii 'sdbf' files
or hash-file-lists.  Using powershell it is best to use the --output file.sdbf
option for creating hashes.  

All programs are command line.  The standalone program is sdhash.exe

In addition, we have an apache thrift-based server now, which will open 
port 9090, and a web-based user interface which will open up port 8080.

There's a chance you may need to download a vc++ runtime to run the
user interface -- for Visual C++ 2008, x64.

Start the server with:  sdhash-srv.exe
Use the command line client: sdhash-cli.exe
Start the user interface:  sdhash-ui\sdhashuiserver.exe

Thanks for trying this out!

Candice Quates
candice@egobsd.org

One last note:  If you have a 64-bit capable processor, please use the 64-bit
version as the comparisons have been rewritten to take advantage of more modern
instructions if they're available.