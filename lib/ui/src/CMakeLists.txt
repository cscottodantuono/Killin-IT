SET(SOURCES components/ui_comp_clear.c
    components/ui_comp.c
    screens/ui_Clear_Screen.c
    screens/ui_Active_Screen.c
    screens/ui_Standby_Screen.c
    screens/ui_No_Connection_Clear.c
    screens/ui_No_Connection_Active.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.cpp
    fonts/ui_font_Akira_Extended.c
    fonts/ui_font_Designer.c
    fonts/ui_font_Designer24.c)

add_library(ui ${SOURCES})
