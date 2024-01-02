// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: Killin-IT_TEST

#include "../ui.h"


// COMPONENT Clear

lv_obj_t *ui_Clear_create(lv_obj_t *comp_parent) {

lv_obj_t *cui_Clear;
cui_Clear = lv_label_create(comp_parent);
lv_obj_set_width( cui_Clear, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( cui_Clear, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( cui_Clear, -1 );
lv_obj_set_y( cui_Clear, 70 );
lv_obj_set_align( cui_Clear, LV_ALIGN_CENTER );
lv_label_set_text(cui_Clear,"CLEAR");
lv_obj_set_style_text_color(cui_Clear, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(cui_Clear, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(cui_Clear, &ui_font_Designer, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_t ** children = lv_mem_alloc(sizeof(lv_obj_t *) * _UI_COMP_CLEAR_NUM);
children[UI_COMP_CLEAR_CLEAR] = cui_Clear;
lv_obj_add_event_cb(cui_Clear, get_component_child_event_cb, LV_EVENT_GET_COMP_CHILD, children);
lv_obj_add_event_cb(cui_Clear, del_component_child_event_cb, LV_EVENT_DELETE, children);
ui_comp_Clear_create_hook(cui_Clear);
return cui_Clear; 
}

