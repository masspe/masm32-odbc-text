; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

;     include files
;     ~~~~~~~~~~~~~
      include \masm32\INCLUDE\windows.inc
      include \masm32\INCLUDE\masm32.inc
      include \masm32\INCLUDE\gdi32.inc
      include \masm32\INCLUDE\user32.inc
      include \masm32\INCLUDE\kernel32.inc
      include \masm32\INCLUDE\Comctl32.inc
      include \masm32\INCLUDE\comdlg32.inc
      include \masm32\INCLUDE\shell32.inc
      include \masm32\INCLUDE\oleaut32.inc
      include \masm32\INCLUDE\msvcrt.inc
      include  \masm32\INCLUDE\odbc32.inc

      include \masm32\macros\macros.asm

;     libraries
;     ~~~~~~~~~
      includelib \masm32\LIB\masm32.lib
      includelib \masm32\LIB\gdi32.lib
      includelib \masm32\LIB\user32.lib
      includelib \masm32\LIB\kernel32.lib
      includelib \masm32\LIB\Comctl32.lib
      includelib \masm32\LIB\comdlg32.lib
      includelib \masm32\LIB\shell32.lib
      includelib \masm32\LIB\oleaut32.lib
      includelib \masm32\LIB\msvcrt.lib
      includelib \masm32\LIB\odbc32.lib

; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

      ; -----------------
      ; Local prototypes
      ; -----------------
        WndProc          PROTO :DWORD,:DWORD,:DWORD,:DWORD
        MsgLoop          PROTO
        Main             PROTO
        rebar            PROTO :DWORD,:DWORD,:DWORD
        addband          PROTO :DWORD,:DWORD
        TBcreate         PROTO :DWORD
        AboutProc        PROTO :DWORD,:DWORD,:DWORD,:DWORD
        MsgboxI          PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
        StatusBar        PROTO :DWORD
        open_file_dialog PROTO :DWORD,:DWORD,:DWORD,:DWORD
        save_file_dialog PROTO :DWORD,:DWORD,:DWORD,:DWORD
		ODBCConnect 	 PROTO :DWORD
		ODBCDisconnect	 PROTO :DWORD
		RunQuery 		 PROTO :DWORD

; ÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-

      IFNDEF cgv                     ;; include this macro if not defined elsewhere
        cgv MACRO args:VARARG
          LOCAL hndl
          .data?
            hndl dd ?
          .code
            invoke args
            mov hndl, eax
          EXITM < EQU <hndl>>
        ENDM
      ENDIF

    ; ---------------------------
    ; macros for creating toolbar
    ; ---------------------------

      TBbutton MACRO bID, cID
        mov tbb.iBitmap,   bID  ;; button  ID number
        mov tbb.idCommand, cID  ;; command ID number
        mov tbb.fsStyle,   TBSTYLE_BUTTON
        invoke SendMessage,tbhandl,TB_ADDBUTTONS,1,ADDR tbb
      ENDM

    ; ------------------------------

      TBspace MACRO
        mov tbb.iBitmap,   0
        mov tbb.idCommand, 0
        mov tbb.fsStyle,   TBSTYLE_SEP
        invoke SendMessage,tbhandl,TB_ADDBUTTONS,1,ADDR tbb
      ENDM

    ; ------------------------------

      mLOCAL equ <LOCAL>

      ToolbarInit MACRO Wd, Ht, parent

        mLOCAL tbhandl   :DWORD
        mLOCAL bSize     :DWORD
        mLOCAL tbab      :TBADDBITMAP
        mLOCAL tbb       :TBBUTTON

        fn CreateWindowEx,0,"ToolbarWindow32", \
                          ADDR szDisplayName, \
                          WS_CHILD or WS_VISIBLE or TBSTYLE_FLAT or \
                          TBSTYLE_TRANSPARENT or CCS_NODIVIDER, \
                          0,0,500,40, \
                          parent,NULL, \
                          hInstance,NULL
        mov tbhandl, eax

      ; ------------------------------------------------------------------
      ; set vertical and horizontal padding for buttons (units are pixels)
      ; ------------------------------------------------------------------
        mov ax, vpad            ; vertical padding
        rol eax, 16
        mov ax, hpad            ; horizontal padding
        invoke SendMessage,tbhandl,TB_SETPADDING,0,eax

        invoke SendMessage,tbhandl,TB_BUTTONSTRUCTSIZE,sizeof TBBUTTON,0
    
      ; ---------------------------------------
      ; Put width & height of bitmap into DWORD
      ; ---------------------------------------
        mov  ecx,Wd  ;; loword = bitmap Width
        mov  eax,Ht  ;; hiword = bitmap Height
        shl  eax,16
        mov  ax, cx

        mov bSize, eax

        invoke SendMessage,tbhandl,TB_SETBITMAPSIZE,0,bSize
        invoke SendMessage,tbhandl,TB_SETBUTTONSIZE,0,bSize

      ; --------------------------------------------------------
      ; set left side indent for first button (units are pixels)
      ; --------------------------------------------------------
        invoke SendMessage,tbhandl,TB_SETINDENT,lind,0
        
        mov tbab.hInst, 0
        m2m tbab.nID,   hBitmap
        invoke SendMessage,tbhandl,TB_ADDBITMAP,12,ADDR tbab

        mov tbb.fsState,   TBSTATE_ENABLED
        mov tbb.dwData,    0
        mov tbb.iString,   0

      ENDM

; ÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-÷-

  ; -----------------------------
  ; uninitialised data allocation
  ; -----------------------------
    .data?
        hInstance   dd ?
        hWnd        dd ?
        hIcon       dd ?
        hCursor     dd ?
        CommandLine dd ?
        sWid        dd ?
        sHgt        dd ?
        hBitmap     dd ?
        tbTile      dd ?
        hRebar      dd ?
        hToolBar    dd ?        
        hStatus     dd ?
		
	
		hEnv dd ?		; environment handle
		hConn dd ?	; connection handle
		hStmt dd ?
		Conn db 1024 dup(?)
		OutStrLen dd ?
		hMenu dd ?	; handle to the main menu
		hList dd ?		; handle to the listview control
		TheName db 26 dup(?)
		TheSurname db 26 dup(?)
		TelNo db 21 dup(?)
		NameLength dd ?
		SurnameLength dd ?
		TelNoLength dd ?
		SearchName db 26 dup(?)
		ProgPath db 256 dup(?)
		ConnectString db 1024 dup(?)

	.data
		SQLStatement db "select * from main",0
		WhereStatement db " where name=?",0
		strConnect db  "DRIVER={Microsoft Access Driver (*.mdb)};DBQ=",0
		DBName db "test.mdb",0
		ConnectCaption db "Complete Connection String",0
		ConnectOK  db "Connected",0
		Disconnect db "Disconnect successful",0
		AppName db "ODBC Test",0
		AllocEnvFail db "Environment handle allocation failed",0
		AllocConnFail db "Connection handle allocation failed",0
		SetAttrFail db "Cannot set desired ODBC version",0
		NoData db "You must type the name in the edit box",0
		ExecuteFail db "Execution of SQL statement failed",0
		ConnFail db "Connection attempt failed",0
		AllocStmtFail db "Statement handle allocation failed",0
		Heading1 db "Name",0
		Heading2 db "Surname",0
		Heading3 db "Telephone No.",0

IF 0  ; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

    insert additional code files within the .CODE section

ENDIF ; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

  .code
    include misc.asm

; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

  ; ---------------------------
  ; initialised data allocation
  ; ---------------------------
    .data
