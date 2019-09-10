# 解析配置，并生成宏
if(${BUILD_TYPE} STREQUAL  "release")
	option(IOT_DEBUG "Enable IOT_DEBUG" OFF)
elseif(${BUILD_TYPE} STREQUAL  "debug")
	option(IOT_DEBUG "Enable IOT_DEBUG" ON)
else()
	message(FATAL_ERROR "INVAILD BUILD_TYPE:${BUILD_TYPE}!")
endif()

if(${FEATURE_AUTH_MODE} STREQUAL  "KEY")
	option(AUTH_MODE_KEY "Enable AUTH_MODE_KEY" ON)
	option(AUTH_MODE_CERT "Enable AUTH_MODE_CERT" OFF)
elseif(${FEATURE_AUTH_MODE} STREQUAL  "CERT" AND ${FEATURE_AUTH_WITH_NOTLS} STREQUAL "OFF")
	option(AUTH_MODE_KEY "Enable AUTH_MODE_KEY" OFF)
	option(AUTH_MODE_CERT "Enable AUTH_MODE_CERT" ON)
	option(AUTH_WITH_NOTLS "Enable AUTH_WITH_NOTLS" OFF)
else()
	message(FATAL_ERROR "INVAILD AUTH_MODE:${FEATURE_AUTH_MODE} WITH AUTH_WITH_NOTLS:${FEATURE_AUTH_WITH_NOTLS}!")
endif()

if(${FEATURE_OTA_COMM_ENABLED} STREQUAL "ON")
	if(${FEATURE_OTA_SIGNAL_CHANNEL} STREQUAL "MQTT" AND ${FEATURE_MQTT_COMM_ENABLED} STREQUAL "ON" )
		option(OTA_MQTT_CHANNEL "Enable TA_MQTT_CHANNEL" ON)
	elseif(${FEATURE_OTA_SIGNAL_CHANNEL} STREQUAL "COAP" AND ${FEATURE_COAP_COMM_ENABLED} STREQUAL "ON")
		option(OTA_MQTT_CHANNEL "Enable TA_MQTT_CHANNEL" OFF)
	else()
		message(FATAL_ERROR "INVAILD OTA_SIGNAL_CHANNEL:${FEATURE_OTA_SIGNAL_CHANNEL} OR DISABLED OTA_COMM, MQTT_COMM, COAP_COMM!")
	endif()
endif()

option(AUTH_WITH_NOTLS "Enable AUTH_WITH_NOTLS" ${FEATURE_AUTH_WITH_NOTLS})
option(AT_TCP_ENABLED "Enable AT_TCP" ${FEATURE_AT_TCP_ENABLED})
option(AT_UART_RECV_IRQ "Enable AT_UART_RECV_IRQ" ${FEATURE_AT_UART_RECV_IRQ})
option(OS_USED "Enable AT_UART_RECV_IRQ" ${FEATURE_AT_OS_USED})
option(AT_DEBUG "Enable AT_DEBUG" ${FEATURE_AT_DEBUG})
option(COAP_COMM_ENABLED "Enable COAP_COMM" ${FEATURE_COAP_COMM_ENABLED})
option(SYSTEM_COMM "Enable SYSTEM_COMM" ${FEATURE_SYSTEM_COMM_ENABLED})
option(EVENT_POST_ENABLED "Enable EVENT_POST" ${FEATURE_EVENT_POST_ENABLED})
option(DEV_DYN_REG_ENABLED "Enable DEV_DYN_REG" ${FEATURE_DEV_DYN_REG_ENABLED})
option(LOG_UPLOAD "Enable LOG_UPLOAD" ${FEATURE_LOG_UPLOAD_ENABLED})
option(DEBUG_DEV_INFO_USED "Enable DEBUG_DEV_INFO_USED" ${FEATURE_DEBUG_DEV_INFO_USED})

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/include/config.h" 
  @ONLY
)

# 解析配置，加入需要编译的源文件

# UTILS
file(GLOB src_utils	${CMAKE_CURRENT_SOURCE_DIR}/utils/*.c)
set(src_sdk  ${src_utils})

# MQTT
if(${FEATURE_MQTT_COMM_ENABLED} STREQUAL "ON")
	# mqtt
	file(GLOB src_mqtt ${CMAKE_CURRENT_SOURCE_DIR}/protocol/mqtt/*.c)
	list(APPEND src_sdk ${src_mqtt})
	
	# shadow 
	if(${FEATURE_MQTT_DEVICE_SHADOW} STREQUAL "ON")
		file(GLOB src_shadow ${CMAKE_CURRENT_SOURCE_DIR}/services/shadow/*.c)
		list(APPEND src_sdk ${src_shadow})
	endif()
	
	# ota mqtt
	if(${FEATURE_OTA_COMM_ENABLED} STREQUAL "ON" AND ${FEATURE_OTA_SIGNAL_CHANNEL} STREQUAL "MQTT")
		file(GLOB src_mqtt_ota 
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_client.c
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_fetch.c
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_lib.c
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_mqtt.c)
		list(APPEND src_sdk ${src_mqtt_ota})
	endif()
	
	# gateway
	if(${FEATURE_GATEWAY_ENABLED} STREQUAL "ON")
		file(GLOB src_gateway ${CMAKE_CURRENT_SOURCE_DIR}/services/gateway/*.c)
		list(APPEND src_sdk ${src_gateway})
	endif()
endif()
 
# COAP 
if(${FEATURE_COAP_COMM_ENABLED} STREQUAL "ON")
	# coap
	file(GLOB src_coap ${CMAKE_CURRENT_SOURCE_DIR}/protocol/coap/*.c)
	list(APPEND src_sdk ${src_coap})
	
	# ota coap
	if(${FEATURE_OTA_COMM_ENABLED} STREQUAL "ON" AND ${FEATURE_OTA_SIGNAL_CHANNEL} STREQUAL "COAP")
		file(GLOB src_coap_ota 
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_client.c
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_fetch.c
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_lib.c
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_coap.c)
		list(APPEND src_sdk ${src_coap_ota})
	endif()
endif()
 
# HTTP 
if(${FEATURE_OTA_COMM_ENABLED} STREQUAL "ON" OR ${FEATURE_DEV_DYN_REG_ENABLED} STREQUAL "ON" OR ${FEATURE_LOG_UPLOAD_ENABLED} STREQUAL "ON")
	# http
	file(GLOB src_http 	${CMAKE_CURRENT_SOURCE_DIR}/protocol/http/*.c)
	list(APPEND src_sdk ${src_http})
endif()

# AT OR TCP
if(${FEATURE_AT_TCP_ENABLED} STREQUAL "ON")
	# at
	file(GLOB src_at_socket ${CMAKE_CURRENT_SOURCE_DIR}/network/at_socket/*.c)
	list(APPEND src_sdk ${src_at_socket})
endif()

# TLS
if(${FEATURE_AUTH_WITH_NOTLS} STREQUAL "OFF")
	file(GLOB src_tls ${CMAKE_CURRENT_SOURCE_DIR}/network/tls/*.c)
	list(APPEND src_sdk ${src_tls})
endif()

# NETWORK
# tcp
file(GLOB src_socket ${CMAKE_CURRENT_SOURCE_DIR}/network/socket/*.c)
list(APPEND src_sdk ${src_socket})
# interface
file(GLOB src_network ${CMAKE_CURRENT_SOURCE_DIR}/network/*.c)
list(APPEND src_sdk ${src_network})

# OTHRE SERVICES
if(${FEATURE_LOG_UPLOAD_ENABLED} STREQUAL "ON")
	file(GLOB src_log_upload ${CMAKE_CURRENT_SOURCE_DIR}/services/log/*.c)
	list(APPEND src_sdk ${src_log_upload})
endif()

if(${FEATURE_DEV_DYN_REG_ENABLED} STREQUAL "ON")
	file(GLOB src_dyn_reg ${CMAKE_CURRENT_SOURCE_DIR}/services/dynreg/*.c)
	list(APPEND src_sdk ${src_dyn_reg})
endif()

if(${FEATURE_EVENT_POST_ENABLED} STREQUAL "ON")
	file(GLOB src_event ${CMAKE_CURRENT_SOURCE_DIR}/services/event/*.c)
	list(APPEND src_sdk ${src_event})
endif()

if(${FEATURE_SYSTEM_COMM_ENABLED} STREQUAL "ON")
	file(GLOB src_system ${CMAKE_CURRENT_SOURCE_DIR}/services/system/*.c)
	list(APPEND src_sdk ${src_system})
endif()

# 生成库，抽取源码
if(${EXTRACT_SRC} STREQUAL  "ON")
	file(COPY ${src_sdk} DESTINATION ${PROJECT_SOURCE_DIR}/output/sdk_src/)
endif()
set(service_target "libsdk")
add_library(${service_target} STATIC ${src_sdk})

