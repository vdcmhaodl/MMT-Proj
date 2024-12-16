#include "list_view.h"

ListViewWindow::ListViewWindow(int WIDTH, int HEIGHT) : BaseWindow<ListViewWindow>(WIDTH, HEIGHT) {}

void ListViewWindow::InitListView()
{
    LVCOLUMN lvColumn;
    
    // Add columns
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lvColumn.cx = 120;

    lvColumn.pszText = (char*)std::string("IP Address").c_str();
    ListView_InsertColumn(hWndListView, 0, &lvColumn);

    lvColumn.pszText = (char*)std::string("Hostname").c_str();
    ListView_InsertColumn(hWndListView, 1, &lvColumn);

    lvColumn.pszText = (char*)std::string("Status").c_str();
    ListView_InsertColumn(hWndListView, 2, &lvColumn);

    // Adjust the width of the last column to fill remaining space
    ListView_SetColumnWidth(hWndListView, 2, LVSCW_AUTOSIZE_USEHEADER);
}

void ListViewWindow::UpdateListView() {
    // Clear existing items
    ListView_DeleteAllItems(hWndListView);

    LVITEM lvItem;
    lvItem.mask = LVIF_TEXT;

    int i = 0;
    ListViewData tmp = list_IP_addr;
    for (const auto& entry : tmp) {
        lvItem.iItem = i;
        lvItem.iSubItem = 0;
        lvItem.pszText = const_cast<char*>(entry.first.c_str());
        ListView_InsertItem(hWndListView, &lvItem);
        ListView_SetItemText(hWndListView, i, 1, const_cast<char*>(entry.second.first.c_str()));
        ListView_SetItemText(hWndListView, i, 2, const_cast<char*>(entry.second.second.c_str()));
        ++i;
    }

    // Adjust the width of the last column again after updating items
    ListView_SetColumnWidth(hWndListView, 2, LVSCW_AUTOSIZE_USEHEADER);
}

PCSTR ListViewWindow::ClassName() const { return "List-View Window Class"; }

LRESULT ListViewWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_CREATE: {
            // Initialize common controls
            INITCOMMONCONTROLSEX icex;
            icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
            icex.dwICC = ICC_LISTVIEW_CLASSES;
            InitCommonControlsEx(&icex);

            // Create the List-View control
            hWndListView = CreateWindowExA(0, WC_LISTVIEW, "",
                                        WS_CHILD | WS_VISIBLE | LVS_REPORT,
                                        10, 10, 400, 200,
                                        m_hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

            // Initialize the List-View columns
            InitListView();

            // Update the List-View with example data
            UpdateListView();

            SendMessage(m_hwnd_parent, WM_IP_REQUEST, 0, 0);
        }
            break;
    case WM_COMMAND: {
        int identifier = LOWORD(wParam);
        switch (identifier) {
        case UPDATE_LIST_VIEW:
            UpdateListView();
            break;
        
        default:
            break;
        }
    } break;

    case WM_IP_NOTIFY: {
        std::any* any_ptr = reinterpret_cast<std::any*>(lParam);
        list_IP_addr = std::any_cast<ListViewData>(*any_ptr);
        delete any_ptr;
        UpdateListView();
        break;
    }

    case WM_DESTROY:
        m_hwnd = NULL;
        // PostQuitMessage(0);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
        EndPaint(m_hwnd, &ps);
    } return 0;
    
    case WM_NCHITTEST: { // Prevent resizing with the mouse 
        LRESULT hit = DefWindowProc(m_hwnd, uMsg, wParam, lParam); 
        if (hit == HTBOTTOM || 
            hit == HTRIGHT || 
            hit == HTBOTTOMRIGHT || 
            hit == HTLEFT || 
            hit == HTTOP || 
            hit == HTTOPLEFT || 
            hit == HTTOPRIGHT || 
            hit == HTBOTTOMLEFT) { 
            return HTCLIENT; 
        } 
        return hit;
    } 

    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return TRUE;
}