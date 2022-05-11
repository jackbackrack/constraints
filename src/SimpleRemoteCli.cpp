#include <cstdlib>
#if defined(USE_EXPERIMENTAL_FS)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#include <unistd.h>
#else
#include <filesystem>
namespace fs = std::filesystem;
#if defined(__APPLE__)
#include <unistd.h>
#endif
#endif
#include <cstdint>
#include <iomanip>
#include "CRSDK/CameraRemote_SDK.h"
#include "CameraDevice.h"
#include "Text.h"

extern "C" {
  int close_sony_remote(void);
  int open_sony_remote(void);
  int snap_sony_remote(void);
};

//#define LIVEVIEW_ENB

namespace SDK = SCRSDK;

// Global dll object
// cli::CRLibInterface* cr_lib = nullptr;

typedef std::shared_ptr<cli::CameraDevice> CameraDevicePtr;
typedef std::vector<CameraDevicePtr> CameraDeviceList;

static CameraDeviceList cameraList;
static CameraDevicePtr camera;

int close_sony_remote (void) {
  cli::tout << "--- CLOSING.\n";
  CameraDeviceList::const_iterator it = cameraList.begin();
  for (std::int32_t j = 0; it != cameraList.end(); ++j, ++it) {
    if ((*it)->is_connected()) {
      cli::tout << "Initiate disconnect sequence.\n";
      auto disconnect_status = (*it)->disconnect();
      if (!disconnect_status) {
        // try again
        disconnect_status = (*it)->disconnect();
      }
      if (!disconnect_status)
        cli::tout << "Disconnect failed to initiate.\n";
      else
        cli::tout << "Disconnect successfully initiated!\n\n";
    }
    (*it)->release();
  }
  cli::tout << "Release SDK resources.\n";
  // cr_lib->Release();
  SDK::Release();

  // cli::free_cr_lib(&cr_lib);

  cli::tout << "Exiting application.\n";
  std::exit(EXIT_SUCCESS);
  return 0;
}

int snap_sony_remote (void) {
  camera->capture_image();
  return 0;
}

int open_sony_remote(void)
{
  // Change global locale to native locale
  std::locale::global(std::locale(""));

  // Make the stream's locale the same as the current global locale
  cli::tin.imbue(std::locale());
  cli::tout.imbue(std::locale());

  cli::tout << "RemoteSampleApp v1.05.00 running...\n\n";

  CrInt32u version = SDK::GetSDKVersion();
  int major = (version & 0xFF000000) >> 24;
  int minor = (version & 0x00FF0000) >> 16;
  int patch = (version & 0x0000FF00) >> 8;
  // int reserved = (version & 0x000000FF);

  cli::tout << "Remote SDK version: ";
  cli::tout << major << "." << minor << "." << std::setfill(TEXT('0')) << std::setw(2) << patch << "\n";

  // Load the library dynamically
  // cr_lib = cli::load_cr_lib();

  cli::tout << "Initialize Remote SDK...\n";
    
#if defined(__APPLE__)
  char path[255]; /*MAX_PATH*/
  getcwd(path, sizeof(path) -1);
  cli::tout << "Working directory: " << path << '\n';
#else
  cli::tout << "Working directory: " << fs::current_path() << '\n';
#endif
  // auto init_success = cr_lib->Init(0);
  auto init_success = SDK::Init();
  if (!init_success) {
    cli::tout << "Failed to initialize Remote SDK. Terminating.\n";
    // cr_lib->Release();
    SDK::Release();
    std::exit(EXIT_FAILURE);
  }
  cli::tout << "Remote SDK successfully initialized.\n\n";

    cli::tout << "Enumerate connected camera devices...\n";
    SDK::ICrEnumCameraObjectInfo* camera_list = nullptr;
    // auto enum_status = cr_lib->EnumCameraObjects(&camera_list, 3);
    auto enum_status = SDK::EnumCameraObjects(&camera_list);
    if (CR_FAILED(enum_status) || camera_list == nullptr) {
        cli::tout << "No cameras detected. Connect a camera and retry.\n";
        // cr_lib->Release();
        SDK::Release();
        std::exit(EXIT_FAILURE);
    }
    auto ncams = camera_list->GetCount();
    cli::tout << "Camera enumeration successful. " << ncams << " detected.\n\n";

  for (CrInt32u i = 0; i < ncams; ++i) {
    auto camera_info = camera_list->GetCameraObjectInfo(i);
    cli::text conn_type(camera_info->GetConnectionTypeName());
    cli::text model(camera_info->GetModel());
    cli::text id = TEXT("");
    if (TEXT("IP") == conn_type) {
      cli::NetworkInfo ni = cli::parse_ip_info(camera_info->GetId(), camera_info->GetIdSize());
      id = ni.mac_address;
    }
    else id = ((TCHAR*)camera_info->GetId());
    cli::tout << '[' << i + 1 << "] " << model.data() << " (" << id.data() << ")\n";
  }

  cli::text connectNo;

  cli::tsmatch smatch;
  CrInt32u no = 1;

  typedef std::shared_ptr<cli::CameraDevice> CameraDevicePtr;
  typedef std::vector<CameraDevicePtr> CameraDeviceList;
  std::int32_t cameraNumUniq = 1;
  std::int32_t selectCamera = 1;

  cli::tout << "Connect to selected camera...\n";
  auto* camera_info = camera_list->GetCameraObjectInfo(no - 1);

  cli::tout << "Create camera SDK camera callback object.\n";
  camera = CameraDevicePtr(new cli::CameraDevice(cameraNumUniq, nullptr, camera_info));
  cameraList.push_back(camera); // add 1st

  cli::tout << "Release enumerated camera list.\n";
  camera_list->Release();

  if (camera->is_connected()) {
    cli::tout << "Please disconnect\n";
  } else {
    camera->connect(SDK::CrSdkControlMode_Remote);
  }
  camera->set_save_info();
  return 0;
}

/*
int main () {
  open_sony_remote();
  snap_sony_remote();
  sleep(1);
  snap_sony_remote();
  sleep(4);
  close_sony_remote();
}
*/
