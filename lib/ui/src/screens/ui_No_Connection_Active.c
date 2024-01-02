// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: Killin-IT_TEST

#include "../ui.h"

void ui_No_Connection_Active_screen_init(void)
{
ui_No_Connection_Active = lv_obj_create(NULL);
lv_obj_clear_flag( ui_No_Connection_Active, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_No_Conn_Active_Arc = lv_arc_create(ui_No_Connection_Active);
lv_obj_set_width( ui_No_Conn_Active_Arc, 240);
lv_obj_set_height( ui_No_Conn_Active_Arc, 240);
lv_obj_set_align( ui_No_Conn_Active_Arc, LV_ALIGN_CENTER );
lv_arc_set_value(ui_No_Conn_Active_Arc, 1);
lv_arc_set_bg_angles(ui_No_Conn_Active_Arc,150,30);
lv_obj_set_style_arc_color(ui_No_Conn_Active_Arc, lv_color_hex(0xFF8C00), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_arc_opa(ui_No_Conn_Active_Arc, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_arc_width(ui_No_Conn_Active_Arc, 24, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_set_style_arc_color(ui_No_Conn_Active_Arc, lv_color_hex(0x4040FF), LV_PART_INDICATOR | LV_STATE_DEFAULT );
lv_obj_set_style_arc_opa(ui_No_Conn_Active_Arc, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);

lv_obj_set_style_bg_color(ui_No_Conn_Active_Arc, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_No_Conn_Active_Arc, 0, LV_PART_KNOB| LV_STATE_DEFAULT);

ui_No_Network_Active = lv_label_create(ui_No_Connection_Active);
lv_obj_set_width( ui_No_Network_Active, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_No_Network_Active, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_No_Network_Active, -1 );
lv_obj_set_y( ui_No_Network_Active, 61 );
lv_obj_set_align( ui_No_Network_Active, LV_ALIGN_CENTER );
lv_label_set_text(ui_No_Network_Active,"No\nNetwork");
lv_obj_set_style_text_color(ui_No_Network_Active, lv_color_hex(0xFF8C00), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_No_Network_Active, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_No_Network_Active, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_No_Network_Active, &ui_font_Designer24, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label13 = lv_label_create(ui_No_Connection_Active);
lv_obj_set_width( ui_Label13, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label13, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label13, -1 );
lv_obj_set_y( ui_Label13, -20 );
lv_obj_set_align( ui_Label13, LV_ALIGN_CENTER );
lv_label_set_text(ui_Label13,"KILLIN'-IT");
lv_obj_set_style_text_font(ui_Label13, &ui_font_Akira_Extended, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label14 = lv_label_create(ui_No_Connection_Active);
lv_obj_set_width( ui_Label14, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label14, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label14, -1 );
lv_obj_set_y( ui_Label14, 10 );
lv_obj_set_align( ui_Label14, LV_ALIGN_CENTER );
lv_label_set_text(ui_Label14,"E-STOP SYSTEM");
lv_obj_set_style_text_font(ui_Label14, &lv_font_montserrat_18, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label15 = lv_label_create(ui_No_Connection_Active);
lv_obj_set_width( ui_Label15, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label15, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label15, 0 );
lv_obj_set_y( ui_Label15, 110 );
lv_obj_set_align( ui_Label15, LV_ALIGN_CENTER );
lv_label_set_text(ui_Label15,"CSD DESIGN");
lv_obj_set_style_text_font(ui_Label15, &lv_font_montserrat_8, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Active_No_Network_Tag = lv_label_create(ui_No_Connection_Active);
lv_obj_set_width( ui_Active_No_Network_Tag, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Active_No_Network_Tag, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Active_No_Network_Tag, -1 );
lv_obj_set_y( ui_Active_No_Network_Tag, -55 );
lv_obj_set_align( ui_Active_No_Network_Tag, LV_ALIGN_CENTER );
lv_label_set_text(ui_Active_No_Network_Tag,"ACTIVE");
lv_obj_set_style_text_color(ui_Active_No_Network_Tag, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_Active_No_Network_Tag, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_Active_No_Network_Tag, &ui_font_Designer24, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_add_event_cb(ui_No_Connection_Active, ui_event_No_Connection_Active, LV_EVENT_ALL, NULL);

}
