// This is the main DLL file.

#include "stdafx.h"

#include "ProcessHookMonitorWrapper.h"
#include "MessageHandlerThunk.h"
#include "FunctionCalledHandlerThunk.h"
#include "ProcessStartEventThunk.h"
#include "ProcessStopEventThunk.h"

//using namespace System;
//using namespace System::IO;
//
//void CopyAll(DirectoryInfo^ source, DirectoryInfo^ target)
//{
//	try {
//		Directory::CreateDirectory(target->FullName);
//		Console::WriteLine(gcnew String("try to copy "));
//		// Copy each file into the new directory.
//		for each(FileInfo^ fi in source->GetFiles())
//		{
//			//Console.WriteLine(@"Copying {0}\{1}", target.FullName, fi.Name);
//			fi->CopyTo(Path::Combine(target->FullName, fi->Name), true);
//		}
//
//		// Copy each subdirectory using recursion.
//		for each(DirectoryInfo^ diSourceSubDir in source->GetDirectories())
//		{
//			DirectoryInfo^ nextTargetSubDir =
//				target->CreateSubdirectory(diSourceSubDir->Name);
//			CopyAll(diSourceSubDir, nextTargetSubDir);
//		}
//	}
//	catch (Exception^ e) {
//		Console::WriteLine(e->ToString());
//	}
//}
//
//void CopyDir(String^ sourceDirectory, String^ targetDirectory)
//{
//	DirectoryInfo^ diSource = gcnew DirectoryInfo(sourceDirectory);
//	DirectoryInfo^ diTarget = gcnew DirectoryInfo(targetDirectory);
//
//	CopyAll(diSource, diTarget);
//}

void ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::initialize()
{
	//CopyDir(gcnew String("utils"), ProcessHookMonitor::ProcessHookMonitor::getWorkPath() + "\\utils");
	ProcessHookMonitor::ProcessHookMonitor::initialize();
}

void ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::setStatusHandler(MessageHandlerWrapper* listener)
{

	MessageHandlerThunk ^thunk = gcnew MessageHandlerThunk(listener);

	// Bind the ontick event with the thunk
	ProcessHookMonitor::ProcessHookMonitor::setStatusHandler(
		gcnew ProcessHookMonitor::MessageHandler(thunk, &MessageHandlerThunk::report));
	 
}

void ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::setFunctionListener(int pid, FunctionCalledHandlerWrapper * listener)
{
	FunctionCalledHandlerThunk ^thunk = gcnew FunctionCalledHandlerThunk(listener);

	// Bind the ontick event with the thunk
	ProcessHookMonitor::ProcessHookMonitor::setFunctionListener(
		pid, gcnew ProcessHookMonitor::FunctionCalledHandler(thunk, &FunctionCalledHandlerThunk::report));
}

void ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::removeFunctionListener(int pid)
{
	ProcessHookMonitor::ProcessHookMonitor::removeFunctionListener(pid);
}

void ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::inject(int pid, FunctionCalledHandlerWrapper * listener)
{
	FunctionCalledHandlerThunk ^thunk = gcnew FunctionCalledHandlerThunk(listener);

	// Bind the ontick event with the thunk
	ProcessHookMonitor::ProcessHookMonitor::inject(
		pid, gcnew ProcessHookMonitor::FunctionCalledHandler(thunk, &FunctionCalledHandlerThunk::report));
}

void ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::close()
{
	/*DirectoryInfo^ target = gcnew DirectoryInfo(ProcessHookMonitor::ProcessHookMonitor::getWorkPath());
	Directory::Delete(target->FullName, true);*/
	ProcessHookMonitor::ProcessHookMonitor::close();
}

void ProcessHookMonitorWrapper::ProcessTraceWrapper::listenProcessesCreation(ProcessStartEventWrapper * listener)
{
	ProcessStartEventThunk ^thunk = gcnew ProcessStartEventThunk(listener);

	// Bind the ontick event with the thunk
	ProcessHookMonitor::ProcessTrace::listenProcessesCreation(
		gcnew ProcessHookMonitor::ProcessStartEvent(thunk, &ProcessStartEventThunk::notify));
}

void ProcessHookMonitorWrapper::ProcessTraceWrapper::listenProcessesTermination(ProcessStopEventWrapper * listener)
{
	ProcessStopEventThunk ^thunk = gcnew ProcessStopEventThunk(listener);

	// Bind the ontick event with the thunk
	ProcessHookMonitor::ProcessTrace::listenProcessesTermination(
		gcnew ProcessHookMonitor::ProcessStopEvent(thunk, &ProcessStopEventThunk::notify));
}

void ProcessHookMonitorWrapper::ProcessTraceWrapper::unlistenProcessesCreation()
{
	ProcessHookMonitor::ProcessTrace::unlistenProcessesCreation();
}

void ProcessHookMonitorWrapper::ProcessTraceWrapper::unlistenProcessesTermination()
{
	ProcessHookMonitor::ProcessTrace::unlistenProcessesTermination();
}
