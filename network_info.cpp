#include <wx/wx.h>
#include <wx/clipbrd.h>
#include <string>
#include <vector>

#ifdef __linux__
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/if_arp.h>
#elif defined(_WIN32)
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#endif

enum
{
    ID_Refresh = wxID_HIGHEST + 1,
    ID_Exit,
    ID_About,
    ID_CopyToClipboard
};

std::string MacToString(unsigned char *mac, size_t length)
{
    if (!mac || length < 6)
        return "00-00-00-00-00-00";
    char buffer[18];
    snprintf(buffer, sizeof(buffer), "%02X-%02X-%02X-%02X-%02X-%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return std::string(buffer);
}

std::vector<wxString> GetNetworkInfo()
{
    std::vector<wxString> info;

#ifdef __linux__
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1)
    {
        info.push_back("Failed to get network interfaces");
        return info;
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET)
            continue;

        int s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s == -1)
            continue;

        struct ifreq ifr;
        strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ - 1);

        bool is_ethernet = false;
        if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0)
        {
            if (ifr.ifr_hwaddr.sa_family == ARPHRD_ETHER)
            {
                is_ethernet = true;
            }
        }

        if (!is_ethernet)
        {
            close(s);
            continue;
        }

        info.push_back(wxString::Format("Type: Ethernet (%s)", ifa->ifa_name));

        if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0)
        {
            std::string mac = MacToString((unsigned char *)ifr.ifr_hwaddr.sa_data, 6);
            info.push_back(wxString::Format("MAC Address: %s", mac.c_str()));
        }

        close(s);

        char address[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, address, sizeof(address));
        info.push_back(wxString::Format("IP Address: %s", address));

        info.push_back("----------------------------------------");
    }
    freeifaddrs(ifaddr);

#elif defined(_WIN32)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        info.push_back("WSAStartup failed");
        return info;
    }

    ULONG bufferSize = 0;
    GetAdaptersAddresses(AF_UNSPEC, 0, NULL, NULL, &bufferSize);
    std::vector<BYTE> buffer(bufferSize);
    PIP_ADAPTER_ADDRESSES adapters = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());

    if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, adapters, &bufferSize) == ERROR_SUCCESS)
    {
        for (PIP_ADAPTER_ADDRESSES adapter = adapters; adapter; adapter = adapter->Next)
        {
            if (adapter->IfType != IF_TYPE_ETHERNET_CSMACD)
                continue;

            info.push_back(wxString::Format("Type: Ethernet (%s)", adapter->FriendlyName));

            if (adapter->PhysicalAddressLength > 0)
            {
                std::string mac = MacToString(adapter->PhysicalAddress, adapter->PhysicalAddressLength);
                info.push_back(wxString::Format("MAC Address: %s", mac.c_str()));
            }

            for (PIP_ADAPTER_UNICAST_ADDRESS unicast = adapter->FirstUnicastAddress; unicast; unicast = unicast->Next)
            {
                if (unicast->Address.lpSockaddr->sa_family == AF_INET)
                {
                    char address[INET6_ADDRSTRLEN] = {0};
                    sockaddr_in *ipv4 = reinterpret_cast<sockaddr_in *>(unicast->Address.lpSockaddr);
                    inet_ntop(AF_INET, &ipv4->sin_addr, address, sizeof(address));
                    info.push_back(wxString::Format("IP Address: %s", address));
                }
            }

            info.push_back("----------------------------------------");
        }
    }
    WSACleanup();
#endif

    return info;
}

class NetworkFrame : public wxFrame
{
public:
    NetworkFrame() : wxFrame(nullptr, wxID_ANY, "Network Information", wxPoint(50, 50), wxSize(700, 550))
    {
        CreateStatusBar();
        SetStatusText("[INFO] Ready");

        LogInfo("Application started");

        wxMenuBar *menuBar = new wxMenuBar();

        wxMenu *fileMenu = new wxMenu();
        fileMenu->Append(ID_Refresh, "&Refresh\tF5", "Refresh network information");
        fileMenu->Append(ID_CopyToClipboard, "&Copy to Clipboard\tCtrl+C", "Copy network information to clipboard");
        fileMenu->AppendSeparator();
        fileMenu->Append(ID_Exit, "E&xit\tAlt-X", "Exit the application");
        menuBar->Append(fileMenu, "&File");

        wxMenu *helpMenu = new wxMenu();
        helpMenu->Append(ID_About, "&About\tF1", "About this application");
        menuBar->Append(helpMenu, "&Help");

        SetMenuBar(menuBar);

        wxPanel *panel = new wxPanel(this, wxID_ANY);
        wxButton *refreshButton = new wxButton(panel, wxID_ANY, "Refresh", wxPoint(10, 10), wxSize(100, 30));
        wxButton *copyButton = new wxButton(panel, wxID_ANY, "Copy to Clipboard", wxPoint(120, 10), wxSize(150, 30));
        listBox = new wxListBox(panel, wxID_ANY, wxPoint(10, 50), wxSize(660, 450), 0, nullptr, wxLB_HSCROLL);

        refreshButton->Bind(wxEVT_BUTTON, &NetworkFrame::OnRefresh, this);
        copyButton->Bind(wxEVT_BUTTON, &NetworkFrame::OnCopyToClipboard, this);
        Bind(wxEVT_MENU, &NetworkFrame::OnRefresh, this, ID_Refresh);
        Bind(wxEVT_MENU, &NetworkFrame::OnCopyToClipboard, this, ID_CopyToClipboard);
        Bind(wxEVT_MENU, &NetworkFrame::OnExit, this, ID_Exit);
        Bind(wxEVT_MENU, &NetworkFrame::OnAbout, this, ID_About);

        UpdateNetworkInfo();
        SetMinSize(wxSize(700, 550));
    }

private:
    wxListBox *listBox;

    void LogInfo(const wxString &message)
    {
        SetStatusText(wxString::Format("[INFO] %s", message));
    }

    void LogError(const wxString &message)
    {
        SetStatusText(wxString::Format("[ERROR] %s", message));
    }

    void UpdateNetworkInfo()
    {
        listBox->Clear();
        std::vector<wxString> info = GetNetworkInfo();
        if (info.empty())
        {
            LogError("Failed to retrieve network information");
            return;
        }
        for (const auto &line : info)
        {
            listBox->Append(line);
        }
        LogInfo("Network information updated");
    }

    void OnRefresh(wxCommandEvent &event)
    {
        LogInfo("Refresh initiated");
        UpdateNetworkInfo();
    }

    void OnCopyToClipboard(wxCommandEvent &event)
    {
        wxString clipboardText;
        for (unsigned int i = 0; i < listBox->GetCount(); i++)
        {
            clipboardText += listBox->GetString(i) + "\n";
        }

        if (wxTheClipboard->Open())
        {
            wxTheClipboard->SetData(new wxTextDataObject(clipboardText));
            wxTheClipboard->Close();
            LogInfo("Network information copied to clipboard");
        }
        else
        {
            LogError("Failed to access clipboard");
        }
    }

    void OnExit(wxCommandEvent &event)
    {
        LogInfo("Exit requested");
        Close(true);
    }

    void OnAbout(wxCommandEvent &event)
    {
        LogInfo("About dialog opened");
        wxMessageBox("Network Information Viewer\nVersion 1.2.0\n\nA cross-platform application to display network interface information.\nBuilt with wxWidgets\n\n(c) 2025 Muhammad Ruslan",
                     "About Network Information",
                     wxOK | wxICON_INFORMATION, this);
        LogInfo("About dialog closed");
    }

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(NetworkFrame, wxFrame)
EVT_MENU(ID_Refresh, NetworkFrame::OnRefresh)
EVT_MENU(ID_CopyToClipboard, NetworkFrame::OnCopyToClipboard)
EVT_MENU(ID_Exit, NetworkFrame::OnExit)
EVT_MENU(ID_About, NetworkFrame::OnAbout)
wxEND_EVENT_TABLE()

class NetworkApp : public wxApp
{
	public:
	    bool OnInit() override
	    {
		NetworkFrame *frame = new NetworkFrame();
		frame->Show(true);
		return true;
	    }
};

wxIMPLEMENT_APP(NetworkApp);
