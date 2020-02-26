#ifdef EVENT_POST_ENABLED

#define EVENT_COUNTS     (1)

static TYPE_DEF_TEMPLATE_STRING sg_button_event_msg[128 + 1] = {0};
static DeviceProperty g_propertyEvent_button_event[] = {

    {.key = "event_msg", .data = sg_button_event_msg, .type = TYPE_TEMPLATE_STRING},
};


static sEvent g_events[] = {

    {
        .event_name = "button_event",
        .type = "alert",
        .timestamp = 0,
        .eventDataNum = sizeof(g_propertyEvent_button_event) / sizeof(g_propertyEvent_button_event[0]),
        .pEventData = g_propertyEvent_button_event,
    },
};

#endif
