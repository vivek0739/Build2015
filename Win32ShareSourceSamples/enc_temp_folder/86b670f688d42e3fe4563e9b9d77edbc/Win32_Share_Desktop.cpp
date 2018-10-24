// Win32_Share_Desktop.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Win32_Share_Desktop.h"
#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <windows.applicationmodel.datatransfer.h>
#include <windows.applicationmodel.datatransfer.h>
#include <ShObjIdl.h>
#include <wrl\event.h>
#include <ppltasks.h>
#include "Resource.h"
#include <winstring.h>
#include <wrl.h>
#include <WinBase.h>


using namespace std;
using namespace Platform;
using namespace ABI::Windows::ApplicationModel::DataTransfer;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Storage;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace concurrency;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

Microsoft::WRL::ComPtr<IDataTransferManagerInterop> pDTM;
Microsoft::WRL::ComPtr<IStorageFile> pSTF;
Microsoft::WRL::ComPtr<IDataTransferManagerStatics> dtmStatics;
ComPtr<ABI::Windows::Storage::Streams::IRandomAccessStreamReference> streamRef;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32_SHARE_DESKTOP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32_SHARE_DESKTOP));
	
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32_SHARE_DESKTOP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN32_SHARE_DESKTOP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


void func()
{
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   OutputDebugStringW(L"inside ");
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   ::RoInitialize(RO_INIT_SINGLETHREADED);   
   
   HRESULT hr = GetActivationFactory(Wrappers::HStringReference(RuntimeClass_Windows_ApplicationModel_DataTransfer_DataTransferManager).Get(), &dtmStatics);
   hr = dtmStatics.As(&pDTM);

   Microsoft::WRL::ComPtr<IDataTransferManager> spSharingManager;
   hr = pDTM->GetForWindow(hWnd, IID_PPV_ARGS(&spSharingManager));


    ComPtr<IActivationFactory> pFactory;
   hr = Windows::Foundation::GetActivationFactory(Microsoft::WRL::Wrappers::HStringReference(RuntimeClass_Windows_Storage_StorageFile).Get(), &pFactory);
   if (S_OK != hr)
	   __debugbreak();

   ComPtr<ABI::Windows::Storage::IStorageFileStatics> pStorageFileStatics;
   hr = pFactory.As(&pStorageFileStatics);
   if (S_OK != hr)
	   __debugbreak();

   // Open the file.
   const WCHAR*    ICON_FILE_NAME = L"\\IMG_2151-10.jpg";
   WCHAR   wchFullPath[_MAX_PATH] = {};
   GetCurrentDirectory(_MAX_PATH, wchFullPath);
   wcscat_s(wchFullPath, ICON_FILE_NAME);

   Wrappers::HStringReference path(wchFullPath);
   ComPtr<ABI::Windows::Foundation::__FIAsyncOperation_1_Windows__CStorage__CStorageFile_t> async;



   struct OPEN_STREAM_RESULT
   {
	   ComPtr<ABI::Windows::Storage::Streams::IRandomAccessStreamReference> streamRef;
	   HRESULT	hr;
	   HANDLE	hCompleteEvent;
   };

   OPEN_STREAM_RESULT	result;
   result.hCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
   result.hr = S_FALSE;

   OPEN_STREAM_RESULT*	pInOutResult = &result;
   typedef IAsyncOperationCompletedHandler<ABI::Windows::Storage::StorageFile*> HandlerType;
   auto handler = Microsoft::WRL::Callback<HandlerType>([pInOutResult](IAsyncOperation<ABI::Windows::Storage::StorageFile*>* async, AsyncStatus status)  -> HRESULT {
	   HRESULT		hr = S_FALSE;
	   OutputDebugStringW(L"inside handler1");

	   //std::cout << "inside handler" << endl;
	   ComPtr<ABI::Windows::Storage::IStorageFile> file;
	   hr = async->GetResults(&file);
	   if (file.Get()) {
		   //std::cout << "inside file" << endl;
		   ComPtr<IActivationFactory> pStreamRefFactory;
		   const WCHAR* runtime_class_name = L"Windows.Storage.Streams.RandomAccessStreamReference";
		   hr = Windows::Foundation::GetActivationFactory(Microsoft::WRL::Wrappers::HStringReference(runtime_class_name).Get(), &pStreamRefFactory);
		   if (S_OK != hr)
			   __debugbreak();

		   ComPtr<ABI::Windows::Storage::Streams::IRandomAccessStreamReferenceStatics> pStreamRefStatic;
		   hr = pStreamRefFactory.As(&pStreamRefStatic);
		   if (S_OK != hr)
			   __debugbreak();

		   hr = pStreamRefStatic->CreateFromFile(file.Get(), &streamRef);

	   }
	   pInOutResult->hr = hr;
	   SetEvent(pInOutResult->hCompleteEvent);
	   return hr;
   });

   pStorageFileStatics->GetFileFromPathAsync(path.Get(), &async);
   async->put_Completed(handler.Get());
 //  WaitForSingleObject(pInOutResult->hCompleteEvent, INFINITE);
   CloseHandle(pInOutResult->hCompleteEvent);



   auto callback = Callback < ITypedEventHandler<DataTransferManager*, DataRequestedEventArgs* >> (
	   [&](IDataTransferManager*, IDataRequestedEventArgs* pArgs) -> HRESULT
   {
	   OutputDebugStringW(L"inside handler");
	   ComPtr<IDataRequest> spDataRequest;
	   pArgs->get_Request(&spDataRequest);

	   // Create Sharing data
	   ComPtr<IDataPackage> spDataPackage;
	   spDataRequest->get_Data(&spDataPackage);

	   ComPtr<IDataPackagePropertySet> spDataPacakgeProperties;
	   spDataPackage->get_Properties(&spDataPacakgeProperties);

	   spDataPacakgeProperties->put_Title(Wrappers::HStringReference(L"Sample win32 app").Get());
	   spDataPacakgeProperties->put_Description(Wrappers::HStringReference(L"Hello...world !!!").Get());

	   spDataPackage->SetText(Wrappers::HStringReference(L"Say hello!").Get());

	   spDataPacakgeProperties->put_Thumbnail(streamRef.Get());
	   spDataPackage->SetBitmap(streamRef.Get());
	  
	   return S_OK;
   });

	  
   
   EventRegistrationToken _ertSharingContentRequested;

   hr = spSharingManager->add_DataRequested(callback.Get(), &_ertSharingContentRequested);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_SHARETEXT:
		{
			HRESULT hr = S_OK;
			hr = pDTM->ShowShareUIForWindow(hWnd);
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		}
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

/*
void DataRequested(DataTransferManager sender, DataRequestedEventArgs e)
{
	DataPackage ^dataPackage = e.Request->Data;

	dataPackage->Properties->Title = "Share Text Example";
	dataPackage->Properties->Description = "An example of how to share text.";
	dataPackage->SetText("Hello world");

}*/
