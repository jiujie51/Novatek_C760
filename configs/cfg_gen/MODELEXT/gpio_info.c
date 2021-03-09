#include "modelext_info.h"
#include "gpio_info.h"
#include "IOCfg.h"

GPIO_INIT_OBJ ind_gpio_info_data[] __attribute__((section("modelext_data.gpio_info"))) = {
#if 0
	{  GPIO_LENS_RESET,        GPIO_DIR_OUTPUT,    PAD_NONE,             PAD_PIN_NOT_EXIST   },
	{  GPIO_LENS_ZOOM_INTR,    GPIO_DIR_INPUT,     PAD_PULLDOWN,         PAD_LENS_ZOOM_INTR  },
	{  GPIO_LENS_FOCUS_INTR,   GPIO_DIR_INPUT,     PAD_PULLDOWN,         PAD_LENS_FOCUS_INTR },
	{  GPIO_LENS_IR_CTRL0,     GPIO_DIR_OUTPUT,    PAD_NONE,             PAD_PIN_NOT_EXIST   },
	{  GPIO_LENS_IR_CTRL1,     GPIO_DIR_OUTPUT,    PAD_NONE,             PAD_PIN_NOT_EXIST   },

	{  GPIO_KEY_LEFT,          GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_LEFT        },
	{  GPIO_KEY_ENTER,         GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_ENTER       },
	{  GPIO_KEY_UP,            GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_UP          },
	{  GPIO_KEY_RIGHT,         GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_RIGHT       },
	{  GPIO_KEY_PLAYBACK,      GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_PLAYBACK    },
	{  GPIO_KEY_DOWN,          GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_DOWN        },
	{  GPIO_KEY_MODE,          GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_MODE        },
#endif
	{  GPIO_KEY_MENU,          GPIO_DIR_INPUT,     PAD_PULLUP,           PAD_KEY_MENU        },
#if defined(_MCU_ENABLE_)
	{  PIR_POWER_ON,           GPIO_DIR_OUTPUT,    PAD_NONE,             PAD_PIN_NOT_EXIST   },
	{  PIR_POWER_OFF,          GPIO_DIR_OUTPUT,    PAD_NONE,             PAD_PIN_NOT_EXIST   },
#endif
    {  GPIO_SENSOR_SIE,		      GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_HI,			 PAD_PIN_NOT_EXIST	 },
    {  GPIO_SENSOR_PWR1_8,		   GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_LOW,			 PAD_PIN_NOT_EXIST	 },
    {  GPIO_SENSOR_PWR1_2,		   GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_LOW,			 PAD_PIN_NOT_EXIST	 },
    {  GPIO_SENSOR_PWR3_3,		   GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_LOW,			 PAD_PIN_NOT_EXIST	 },
    {  GPIO_SENSOR_RESET,          GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,           PAD_PIN_NOT_EXIST   },

    {  GPIO_ETH_PWR,            GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,           PAD_PIN_NOT_EXIST   },

//WIFI power 
	{  GPIO_WIFI_PWR_ON,        GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_LOW,    PAD_PIN_NOT_EXIST   }, // high active
	{  GPIO_WIFI_CS,        	GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_HI,    PAD_PIN_NOT_EXIST   },

//gsensor reset
	{  GPIO_GS_RST,             GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_LOW,    GPIO_SET_NONE       },

//GPS EN	
	{  GPIO_GPS_EN,             GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_HI,    GPIO_SET_NONE       },

#if (LCD_BACKLIGHT_CTRL == LCD_BACKLIGHT_BY_GPIO)
    {  GPIO_LCD_BLG_PCTL,			GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_LOW,			 PAD_PIN_NOT_EXIST	 },
#endif
    {  GPIO_BL_PWR,			GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,			 PAD_PIN_NOT_EXIST	 },
    {  GPIO_LCD_RESET,      GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,			 PAD_PIN_NOT_EXIST},
    {  GPIO_VSP_VSN_EN,     GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,           PAD_PIN_NOT_EXIST},
    {  GPIO_VGL_VGH_EN,     GPIO_DIR_OUTPUT,   GPIO_SET_OUTPUT_HI,           PAD_PIN_NOT_EXIST},

//SPEAKER
    {  GPIO_SPK_PWR_CTRL,       GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_HI,    PAD_PIN_NOT_EXIST},

//LED 
	{  GPIO_MOVIE_LED,       GPIO_DIR_OUTPUT,    GPIO_SET_OUTPUT_HI,    PAD_PIN_NOT_EXIST}
};

MODELEXT_HEADER ind_gpio_info_header __attribute__((section("modelext_header.gpio_info"))) = {
	.size = sizeof(ind_gpio_info_data) + sizeof(MODELEXT_HEADER),
	.type = MODELEXT_TYPE_GPIO_INFO,
	.number = sizeof(ind_gpio_info_data) / sizeof(GPIO_INIT_OBJ),
	.version = GPIO_INFO_VER,
};
