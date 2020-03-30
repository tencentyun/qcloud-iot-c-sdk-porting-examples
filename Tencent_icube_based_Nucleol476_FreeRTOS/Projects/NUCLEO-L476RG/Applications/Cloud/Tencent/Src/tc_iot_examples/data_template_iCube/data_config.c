/*-----------------data config start  -------------------*/

#define TOTAL_PROPERTY_COUNT 3

static sDataPoint    sg_DataTemplate[TOTAL_PROPERTY_COUNT];

typedef struct _ProductDataDefine {
    TYPE_DEF_TEMPLATE_FLOAT m_temperature;
    TYPE_DEF_TEMPLATE_FLOAT m_hum;
    TYPE_DEF_TEMPLATE_BOOL m_led_ctrl;
} ProductDataDefine;

static   ProductDataDefine     sg_ProductData;

static void _init_data_template(void)
{
    sg_ProductData.m_temperature = 0;
    sg_DataTemplate[0].data_property.data = &sg_ProductData.m_temperature;
    sg_DataTemplate[0].data_property.key  = "temperature";
    sg_DataTemplate[0].data_property.type = TYPE_TEMPLATE_FLOAT;
    sg_DataTemplate[0].state = eCHANGED;

    sg_ProductData.m_hum = 0;
    sg_DataTemplate[1].data_property.data = &sg_ProductData.m_hum;
    sg_DataTemplate[1].data_property.key  = "hum";
    sg_DataTemplate[1].data_property.type = TYPE_TEMPLATE_FLOAT;
    sg_DataTemplate[1].state = eCHANGED;

    sg_ProductData.m_led_ctrl = 0;
    sg_DataTemplate[2].data_property.data = &sg_ProductData.m_led_ctrl;
    sg_DataTemplate[2].data_property.key  = "led_ctrl";
    sg_DataTemplate[2].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_DataTemplate[2].state = eCHANGED;

};
