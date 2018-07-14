//-----------------------------------------------------------------------------
// An abstraction for platform-dependent GUI functionality.
//
// Copyright 2018 whitequark
//-----------------------------------------------------------------------------

#ifndef SOLVESPACE_GUI_H
#define SOLVESPACE_GUI_H

namespace Platform {

//-----------------------------------------------------------------------------
// Events
//-----------------------------------------------------------------------------

// A keyboard input event.
struct KeyboardEvent {
    enum class Type {
        PRESS,
        RELEASE,
    };

    enum class Key {
        CHARACTER,
        FUNCTION,
    };

    Type        type;
    Key         key;
    union {
        char32_t    chr; // for Key::CHARACTER
        int         num; // for Key::FUNCTION
    };
    bool        shiftDown;
    bool        controlDown;

    bool Equals(const KeyboardEvent &other) {
        return type == other.type && key == other.key && num == other.num &&
            shiftDown == other.shiftDown && controlDown == other.controlDown;
    }
};

std::string AcceleratorDescription(const KeyboardEvent &accel);

// A mouse input event.
class MouseEvent {
public:
    enum class Type {
        MOTION,
        PRESS,
        DBL_PRESS,
        RELEASE,
        SCROLL_VERT,
        LEAVE,
    };

    enum class Button {
        NONE,
        LEFT,
        MIDDLE,
        RIGHT,
    };

    Type        type;
    double      x;
    double      y;
    Button      button;
    bool        shiftDown;
    bool        controlDown;
    int         scrollDelta;
};

//-----------------------------------------------------------------------------
// Interfaces
//-----------------------------------------------------------------------------

// A native single-shot timer.
class Timer {
public:
    std::function<void()>   onTimeout;

    virtual ~Timer() {}

    virtual void WindUp(unsigned milliseconds) = 0;
};

typedef std::unique_ptr<Timer> TimerRef;

// A native menu item.
class MenuItem {
public:
    enum class Indicator {
        NONE,
        CHECK_MARK,
        RADIO_MARK,
    };

    std::function<void()>   onTrigger;

    virtual ~MenuItem() {}

    virtual void SetAccelerator(KeyboardEvent accel) = 0;
    virtual void SetIndicator(Indicator type) = 0;
    virtual void SetEnabled(bool enabled) = 0;
    virtual void SetActive(bool active) = 0;
};

typedef std::shared_ptr<MenuItem> MenuItemRef;

// A native menu.
class Menu {
public:
    virtual ~Menu() {}

    virtual std::shared_ptr<MenuItem> AddItem(
        const std::string &label, std::function<void()> onTrigger = std::function<void()>()) = 0;
    virtual std::shared_ptr<Menu> AddSubMenu(const std::string &label) = 0;
    virtual void AddSeparator() = 0;

    virtual void PopUp() = 0;

    virtual void Clear() = 0;
};

typedef std::shared_ptr<Menu> MenuRef;

// A native menu bar.
class MenuBar {
public:
    virtual ~MenuBar() {}

    virtual std::shared_ptr<Menu> AddSubMenu(const std::string &label) = 0;

    virtual void Clear() = 0;
};

typedef std::shared_ptr<MenuBar> MenuBarRef;

// A native top-level window, with an OpenGL context, and an editor overlay.
class Window {
public:
    enum class Kind {
        TOPLEVEL,
        TOOL,
    };

    enum class Cursor {
        POINTER,
        HAND
    };

    std::function<void()>               onClose;
    std::function<void(bool)>           onFullScreen;
    std::function<bool(MouseEvent)>     onMouseEvent;
    std::function<bool(KeyboardEvent)>  onKeyboardEvent;
    std::function<void(std::string)>    onEditingDone;
    std::function<void(double)>         onScrollbarAdjusted;
    std::function<void()>               onRender;

    virtual ~Window() {}

    virtual int GetIntegralScaleFactor() = 0;       // raster graphics and coordinate scale
    virtual double GetFractionalScaleFactor() = 0;  // vector font scale relative to 96 dpi
    virtual double GetPixelDensity() = 0;           // physical display dpi

    virtual bool IsVisible() = 0;
    virtual void SetVisible(bool visible) = 0;

    virtual bool IsFullScreen() = 0;
    virtual void SetFullScreen(bool fullScreen) = 0;

    virtual void SetTitle(const std::string &title) = 0;
    virtual bool SetTitleForFilename(const Path &filename) { return false; }

    virtual void SetMenuBar(MenuBarRef menuBar) = 0;

    virtual void GetContentSize(double *width, double *height) = 0;
    virtual void SetMinContentSize(double width, double height) = 0;

    virtual void FreezePosition(const std::string &key) = 0;
    virtual void ThawPosition(const std::string &key) = 0;

    virtual void SetCursor(Cursor cursor) = 0;
    virtual void SetTooltip(const std::string &text) = 0;

    virtual bool IsEditorVisible() = 0;
    virtual void ShowEditor(double x, double y, double fontHeight, double minWidth,
                            bool isMonospace, const std::string &text) = 0;
    virtual void HideEditor() = 0;

    virtual void SetScrollbarVisible(bool visible) = 0;
    virtual void ConfigureScrollbar(double min, double max, double pageSize) = 0;
    virtual double GetScrollbarPosition() = 0;
    virtual void SetScrollbarPosition(double pos) = 0;

    virtual void Invalidate() = 0;
    virtual void Redraw() = 0;

    virtual void *NativePtr() = 0;
};

typedef std::shared_ptr<Window> WindowRef;

//-----------------------------------------------------------------------------
// Factories
//-----------------------------------------------------------------------------

TimerRef CreateTimer();
MenuRef CreateMenu();
MenuBarRef GetOrCreateMainMenu(bool *unique);
WindowRef CreateWindow(Window::Kind kind = Window::Kind::TOPLEVEL,
                       WindowRef parentWindow = NULL);

//-----------------------------------------------------------------------------
// Application-wide APIs
//-----------------------------------------------------------------------------

void Exit();

}

#endif
