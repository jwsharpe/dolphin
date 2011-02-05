// Copyright (C) 2003 Dolphin Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official SVN repository and contact information can be found at
// http://code.google.com/p/dolphin-emu/


#ifndef __FRAME_H_
#define __FRAME_H_

#include <wx/wx.h> // wxWidgets
#include <wx/busyinfo.h>
#include <wx/mstream.h>
#include <wx/listctrl.h>
#include <wx/artprov.h>
#include <wx/aui/aui.h>
#include <string>
#include <vector>

#include "CDUtils.h"
#include "CodeWindow.h"
#include "LogWindow.h"
#if defined(HAVE_X11) && HAVE_X11
#include "X11Utils.h"
#endif

// A shortcut to access the bitmaps
#define wxGetBitmapFromMemory(name) _wxGetBitmapFromMemory(name, sizeof(name))
static inline wxBitmap _wxGetBitmapFromMemory(const unsigned char* data, int length)
{
	wxMemoryInputStream is(data, length);
	return(wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1));
}

// Class declarations
class CGameListCtrl;
class CLogWindow;

// The CPanel class to receive MSWWindowProc messages from the video backend.
class CPanel : public wxPanel
{
	public:
		CPanel(
			wxWindow* parent,
			wxWindowID id = wxID_ANY
			);

	private:
		DECLARE_EVENT_TABLE();

		#ifdef _WIN32
			// Receive WndProc messages
			WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
		#endif
};

class CRenderFrame : public wxFrame
{
	public:
		CRenderFrame(wxFrame* parent,
			wxWindowID id = wxID_ANY,
			const wxString& title = wxT("Dolphin"),
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

	private:
		#ifdef _WIN32
			// Receive WndProc messages
			WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
		#endif
};

class CFrame : public CRenderFrame
{
	public:
		CFrame(wxFrame* parent,
			wxWindowID id = wxID_ANY,
			const wxString& title = wxT("Dolphin"),
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			bool _UseDebugger = false,
			bool _BatchMode = false,
			bool ShowLogWindow = false,
			long style = wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

		virtual ~CFrame();

		void* GetRenderHandle()
		{
			#ifdef _WIN32
				return (void *)m_RenderParent->GetHandle();
			#elif defined(HAVE_X11) && HAVE_X11
				return (void *)X11Utils::XWindowFromHandle(m_RenderParent->GetHandle());
			#else
				return m_RenderParent;
			#endif
		}

		// These have to be public		
		CCodeWindow* g_pCodeWindow;
		void InitBitmaps();
		void DoPause();
		void DoStop();
		void DoRecordingSave();
		bool bRenderToMain;
		bool bNoWiimoteMsg;
		void UpdateGUI();
		void UpdateGameList();
		void ToggleLogWindow(bool bShow);
		void ToggleConsole(bool bShow);		
		void PostEvent(wxCommandEvent& event);
		void StatusBarMessage(const char * Text, ...);
		void ClearStatusBar();
		void GetRenderWindowSize(int& x, int& y, int& width, int& height);
		void OnRenderWindowSizeRequest(int width, int height);
		void BootGame(const std::string& filename);
		void OnRenderParentClose(wxCloseEvent& event);
		void OnRenderParentMove(wxMoveEvent& event);
		bool RendererHasFocus();
		void DoFullscreen(bool bF);
		void ToggleDisplayMode (bool bFullscreen);
		static void ConnectWiimote(int wm_idx, bool connect);

		#ifdef __WXGTK__
		Common::Event panic_event;
		bool bPanicResult;
		Common::Event keystate_event;
		bool bKeyStateResult;
		#endif

		#if defined(HAVE_XRANDR) && HAVE_XRANDR
		X11Utils::XRRConfiguration *m_XRRConfig;
		#endif

		// AUI
		wxAuiManager *m_Mgr;
		wxAuiToolBar *m_ToolBar, *m_ToolBarDebug, *m_ToolBarAui;
		bool bFloatWindow[IDM_CODEWINDOW - IDM_LOGWINDOW + 1];

		// Perspectives (Should find a way to make all of this private)
		void DoAddPage(wxWindow *Win, int i, bool Float);
		void DoRemovePage(wxWindow *, bool bHide = true);
		struct SPerspectives
		{
			std::string Name;
			wxString Perspective;
			std::vector<int> Width, Height;
		};
		std::vector<SPerspectives> Perspectives;
		u32 ActivePerspective;	

	private:
		CGameListCtrl* m_GameListCtrl;
		wxPanel* m_Panel;
		CRenderFrame* m_RenderFrame;
		wxPanel* m_RenderParent;
		CLogWindow* m_LogWindow;
		bool UseDebugger;
		bool m_bBatchMode;
		bool m_bEdit;
		bool m_bTabSplit;
		bool m_bNoDocking;
		bool m_bGameLoading;

		std::vector<std::string> drives;

		enum EToolbar
		{
			Toolbar_FileOpen,
			Toolbar_Refresh,
			Toolbar_Browse,
			Toolbar_Play,
			Toolbar_Stop,
			Toolbar_Pause,
			Toolbar_Screenshot,
			Toolbar_FullScreen,
			Toolbar_ConfigMain,
			Toolbar_ConfigGFX,
			Toolbar_ConfigDSP,
			Toolbar_ConfigPAD,
			Toolbar_Wiimote,			
			Toolbar_Help,
			EToolbar_Max
		};

		enum EBitmapsThemes
		{
			BOOMY,
			VISTA,
			XPLASTIK,
			KDE,
			THEMES_MAX
		};

		wxBitmap m_Bitmaps[EToolbar_Max];
		wxBitmap m_BitmapsMenu[EToolbar_Max];

		void PopulateToolbar(wxAuiToolBar* toolBar);
		void PopulateToolbarAui(wxAuiToolBar* toolBar);
		void RecreateToolbar();
		void CreateMenu();

		// Utility
		wxString GetMenuLabel(int Id);
		wxWindow * GetNotebookPageFromId(wxWindowID Id);
		wxAuiNotebook * GetNotebookFromId(u32 NBId);
		int GetNotebookCount();
		wxAuiNotebook *CreateEmptyNotebook();

		// Perspectives
		void AddRemoveBlankPage();
		void OnNotebookPageClose(wxAuiNotebookEvent& event);
		void OnAllowNotebookDnD(wxAuiNotebookEvent& event);
		void OnNotebookPageChanged(wxAuiNotebookEvent& event);
		void OnFloatWindow(wxCommandEvent& event);
		void ToggleFloatWindow(int Id);
		void OnTab(wxAuiNotebookEvent& event);
		int GetNotebookAffiliation(wxWindowID Id);
		void ClosePages();
		void CloseAllNotebooks();
		void TogglePane();
		void SetPaneSize();
		void ResetToolbarStyle();
		void TogglePaneStyle(bool On, int EventId);
		void ToggleNotebookStyle(bool On, long Style);
		void ResizeConsole();
		// Float window
		void DoUnfloatPage(int Id);
		void OnFloatingPageClosed(wxCloseEvent& event);
		void OnFloatingPageSize(wxSizeEvent& event);
		void DoFloatNotebookPage(wxWindowID Id);
		wxFrame * CreateParentFrame(wxWindowID Id = wxID_ANY,
			   	const wxString& title = wxT(""),
			   	wxWindow * = NULL);
		wxString AuiFullscreen, AuiCurrent;
		void AddPane();
		void UpdateCurrentPerspective();
		void SaveIniPerspectives();
		void LoadIniPerspectives();
		void OnPaneClose(wxAuiManagerEvent& evt);
		void ReloadPanes();
		void DoLoadPerspective();
		void OnDropDownToolbarSelect(wxCommandEvent& event);		
		void OnDropDownSettingsToolbar(wxAuiToolBarEvent& event);
		void OnDropDownToolbarItem(wxAuiToolBarEvent& event);
		void OnSelectPerspective(wxCommandEvent& event);		

#ifdef _WIN32
		// Override window proc for tricks like screensaver disabling
		WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
#endif
		// Event functions
		void OnQuit(wxCommandEvent& event);
		void OnHelp(wxCommandEvent& event);
		void OnToolBar(wxCommandEvent& event);
		void OnAuiToolBar(wxAuiToolBarEvent& event);

		void OnOpen(wxCommandEvent& event); // File menu
		void DoOpen(bool Boot);
		void OnRefresh(wxCommandEvent& event);
		void OnBrowse(wxCommandEvent& event);
		void OnBootDrive(wxCommandEvent& event);

		void OnPlay(wxCommandEvent& event); // Emulation
		void OnStop(wxCommandEvent& event);
		void OnReset(wxCommandEvent& event);
		void OnRecord(wxCommandEvent& event);
		void OnPlayRecording(wxCommandEvent& event);
		void OnRecordExport(wxCommandEvent& event);
		void OnChangeDisc(wxCommandEvent& event);
		void OnScreenshot(wxCommandEvent& event);
		void OnActive(wxActivateEvent& event);
		void OnClose(wxCloseEvent &event);	
		void OnLoadState(wxCommandEvent& event);
		void OnSaveState(wxCommandEvent& event);
		void OnLoadStateFromFile(wxCommandEvent& event);
		void OnSaveStateToFile(wxCommandEvent& event);
		void OnLoadLastState(wxCommandEvent& event);
		void OnUndoLoadState(wxCommandEvent& event);
		void OnUndoSaveState(wxCommandEvent& event);
		
		void OnFrameSkip(wxCommandEvent& event);
		void OnFrameStep(wxCommandEvent& event);
		
		void OnConfigMain(wxCommandEvent& event); // Options
		void OnConfigGFX(wxCommandEvent& event);
		void OnConfigDSP(wxCommandEvent& event);
		void OnConfigPAD(wxCommandEvent& event);
		void OnConfigWiimote(wxCommandEvent& event);

		void OnToggleFullscreen(wxCommandEvent& event);
		void OnToggleDualCore(wxCommandEvent& event);
		void OnToggleSkipIdle(wxCommandEvent& event);
		void OnToggleThrottle(wxCommandEvent& event);
		void OnManagerResize(wxAuiManagerEvent& event);
		void OnMove(wxMoveEvent& event);
		void OnResize(wxSizeEvent& event);
		void OnToggleToolbar(wxCommandEvent& event);
		void DoToggleToolbar(bool);
		void OnToggleStatusbar(wxCommandEvent& event);
		void OnToggleWindow(wxCommandEvent& event);

		void OnKeyDown(wxKeyEvent& event); // Keyboard
		void OnKeyUp(wxKeyEvent& event);

		void OnMouse(wxMouseEvent& event); // Mouse
		
		void OnHostMessage(wxCommandEvent& event);

		void OnMemcard(wxCommandEvent& event); // Misc
		void OnImportSave(wxCommandEvent& event);

		void OnNetPlay(wxCommandEvent& event);

		void OnShow_CheatsWindow(wxCommandEvent& event);
		void OnLoadWiiMenu(wxCommandEvent& event);
		void OnConnectWiimote(wxCommandEvent& event);
		void GameListChanged(wxCommandEvent& event);

		void OnGameListCtrl_ItemActivated(wxListEvent& event);
		void OnRenderParentResize(wxSizeEvent& event);
		bool RendererIsFullscreen();
		void StartGame(const std::string& filename);

		// Event table
		DECLARE_EVENT_TABLE();
};


#endif  // __FRAME_H_

