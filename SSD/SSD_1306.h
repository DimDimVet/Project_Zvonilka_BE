#ifndef SSD1306_H
#define SSD1306_H 

/*includes*/
#include "stm32f4xx.h"
//#include "stdlib.h"
//#include "string.h"
#include "fonts.h"
#include "I2C_STM32F407.h"
#include "RCC_STM32F407.h"

/*defaines*/
#define ssd1306_I2C_TIMEOUT					20000

/*Ширина SSD1306 в пикселях*/
#define SSD1306_WIDTH            128 
/*Высота SSD1306 в пикселях*/
#define SSD1306_HEIGHT           64 

/* Запись команды */
#define SSD1306_WRITECOMMAND(SSD1306_I2C_ADDR,command)      ssd1306_I2C_Write(SSD1306_I2C_ADDR, 0x00, (command))
/* Запись даты */
#define SSD1306_WRITEDATA(SSD1306_I2C_ADDR,data)            ssd1306_I2C_Write(SSD1306_I2C_ADDR, 0x40, (data))
/* Абсолютное значение */
#define ABS(x)   ((x) > 0 ? (x) : -(x))

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A
#define SSD1306_DEACTIVATE_SCROLL                    0x2E // Stop прокрутка
#define SSD1306_ACTIVATE_SCROLL                      0x2F // Start прокрутка
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3 // Установка диапозона прокрутки

#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_INVERTDISPLAY       0xA7

/*var*/
/* SSD1306 data buffer */
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

/*structures*/
typedef struct
{
    uint16_t x0;
		char* str0;
		
		uint16_t x1;
		char* str1;
		
		uint16_t x2;
		char* str2;
		
		uint16_t x3;
		char* str3;
		
		uint16_t x4;
		char* str4;
		
		uint16_t x5;
		char* str5;
		
} Screen_Structure;

typedef struct
{
    I2C_TypeDef *instance;
    uint16_t adress_I2C;
		
		uint16_t currentX;
    uint16_t currentY;
    uint8_t inverted;
    uint8_t initialized;

} SSD1306_Structure;



/*Цвета SSD1306*/
typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /*!< Черный цвет */
	SSD1306_COLOR_WHITE = 0x01  /*!< Белый цвет */
} SSD1306_COLOR_t;

/*func*/
/*Инициализация SSD1306*/
uint8_t SSD1306_Init(SSD1306_Structure *structure);

/*Заполняет весь экран цветом*/
void SSD1306_Fill(SSD1306_COLOR_t Color);

/*Обновляет буфер из RAM SSD1306*/
void SSD1306_UpdateScreen(SSD1306_Structure *structure);

/*Включает/выключает инверсию пикселей SSD1306*/
void SSD1306_StructureoggleInvert(SSD1306_Structure *structure) ;

/**
 * @brief  Рисует пиксель
 * @note   @ref SSD1306_UpdateScreen() необходимо вызвать для обновления
 * @param  x: X location. Этот параметр может иметь значение между 0 и SSD1306_WIDTH - 1
 * @param  y: Y location. Этот параметр может иметь значение между 0 и SSD1306_HEIGHT - 1
 * @param  color: Цвет, который будет использоваться для заливки экрана
 * @retval None
 */
void SSD1306_DrawPixel(SSD1306_Structure *structure, uint16_t x, uint16_t y, SSD1306_COLOR_t color);

/*Устанавливает указатель курсора в желаемое место для строк*/
void SSD1306_GotoXY(SSD1306_Structure *structure, uint16_t x, uint16_t y);

/**
 * @brief  Помещает символ в RAM
 * @note   @ref SSD1306_UpdateScreen() необходимо вызвать для обновления
 * @param  ch: символ который будет написан
 * @param  *Font: Указатель @ref FontDef_t на структуру шрифта
 * @param  color: Цвет, который будет использоваться для заливки экрана
 * @retval Character written
 */
char SSD1306_Putc(SSD1306_Structure *structure, char ch, FontDef_t *Font, SSD1306_COLOR_t color);

/**
 * @brief  Помещает строку в RAM
 * @note   @ref SSD1306_UpdateScreen() необходимо вызвать для обновления
 * @param  *str: строка который будет написан
 * @param  *Font: Указатель @ref FontDef_t на структуру шрифта
 * @param  color: Цвет, который будет использоваться для заливки экрана
 * @retval Ноль в случае успеха или символьное значение в случае сбоя функции
 */
char SSD1306_Puts(SSD1306_Structure *structure, char *str, FontDef_t *Font, SSD1306_COLOR_t color);

/**
 * @brief  Рисование линии в LCD
 * @note   @ref SSD1306_UpdateScreen() необходимо вызвать для обновления
 * @param  x0: Line X start point. Размеры от 0 до SSD1306_WIDTH - 1
 * @param  y0: Line Y start point. Размеры от 0 до SSD1306_HEIGHT - 1
 * @param  x1: Line X end point. Размеры от 0 до SSD1306_WIDTH - 1
 * @param  y1: Line Y end point. Размеры от 0 до SSD1306_HEIGHT - 1
 * @param  c: Цвет, который будет использоваться для заливки экрана
 * @retval None
 */
void SSD1306_DrawLine(SSD1306_Structure *structure, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c);

/**
 * @brief  Рисование прямоугольника в LCD
 * @note   @ref SSD1306_UpdateScreen() необходимо вызвать для обновления
 * @param  x: Top left X start point. Размеры от 0 до SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Размеры от 0 до SSD1306_HEIGHT - 1
 * @param  w: Ширина прямоугольника в pixels
 * @param  h: Высота прямоугольника в pixels
 * @param  c: Цвет, который будет использоваться для заливки экрана
 * @retval None
 */
void SSD1306_DrawRectangle(SSD1306_Structure *structure, uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);

/**
 * @brief  Рисование прямоугольника с заливкой в LCD
 * @note   @ref SSD1306_UpdateScreen() необходимо вызвать для обновления
 * @param  x: Top left X start point. Размеры от 0 до SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Размеры от 0 до SSD1306_HEIGHT - 1
 * @param  w: Ширина прямоугольника в pixels
 * @param  h: Высота прямоугольника в pixels
 * @param  c: Цвет, который будет использоваться для заливки экрана
 * @retval None
 */
void SSD1306_DrawFilledRectangle(SSD1306_Structure *structure, uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);

/**
 * @brief  Рисование треугольника в LCD
 * @note   @ref SSD1306_UpdateScreen() необходимо вызвать для обновления
 * @param  x1: First coordinate X location. Размеры от 0 до SSD1306_WIDTH - 1
 * @param  y1: First coordinate Y location. Размеры от 0 до SSD1306_HEIGHT - 1
 * @param  x2: Second coordinate X location. Размеры от 0 до SSD1306_WIDTH - 1
 * @param  y2: Second coordinate Y location. Размеры от 0 до SSD1306_HEIGHT - 1
 * @param  x3: Third coordinate X location. Размеры от 0 до SSD1306_WIDTH - 1
 * @param  y3: Third coordinate Y location. Размеры от 0 до SSD1306_HEIGHT - 1
 * @param  c: Цвет, который будет использоваться для заливки экрана
 * @retval None
 */
void SSD1306_DrawTriangle(SSD1306_Structure *structure, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color);

/**
 * @brief  Рисование круга в LCD
 * @note   @ref SSD1306_UpdateScreen() необходимо вызвать для обновления
 * @param  x: X location for center of circle. Размеры от 0 до SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Размеры от 0 до SSD1306_HEIGHT - 1
 * @param  r: Радиус круга в pixels
 * @param  c: Цвет, который будет использоваться для заливки экрана
 * @retval None
 */
void SSD1306_DrawCircle(SSD1306_Structure *structure, int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);

/**
* @brief  Рисование круга с заливкойв LCD
 * @note   @ref SSD1306_UpdateScreen() необходимо вызвать для обновления
 * @param  x: X location for center of circle. Размеры от 0 до SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Размеры от 0 до SSD1306_HEIGHT - 1
 * @param  r: Радиус круга в pixels
 * @param  c: Цвет, который будет использоваться для заливки экрана
 * @retval None
 */
void SSD1306_DrawFilledCircle(SSD1306_Structure *structure, int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);

/**
* @brief Записывает один байт в подчиненное устройство
* @param *I2Cx: используется I2C
* @param address: 7-битный адрес подчиненного устройства, выровнен по левому краю, используются биты 7:1, бит LSB не используется
* @param reg: регистр для записи
* @param data: данные для записи
* @retval None
*/
void ssd1306_I2C_Write(uint8_t address, uint8_t reg, uint8_t data);

/**
* @brief Записывает несколько байтов в подчиненное устройство
* @param *I2Cx: используется I2C
* @param address: 7-битный адрес подчиненного устройства, выровнен по левому краю, используются биты 7:1, бит LSB не используется
* @param reg: регистр для записи
* @param *data: указатель на массив данных для записи в подчиненное устройство
* @param count: сколько байт будет записано
* @retval None
*/
void ssd1306_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count);

/**
* @brief Рисует растровое изображение
* @param X: Координата X для начала рисования
* @param Y: Координата Y для начала рисования
* @param *bitmap : Указатель на растровое изображение
* @param W : ширина изображения
* @param H : высота изображения
* @param color : 1-> белый/синий, 0-> черный
*/
void SSD1306_DrawBitmap(SSD1306_Structure *structure, int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color);

/*прокрутите экран для фиксированных строк*/
void SSD1306_ScrollRight(uint8_t address,uint8_t start_row, uint8_t end_row);

void SSD1306_ScrollLeft(uint8_t address,uint8_t start_row, uint8_t end_row);

void SSD1306_Scrolldiagright(uint8_t address,uint8_t start_row, uint8_t end_row);

void SSD1306_Scrolldiagleft(uint8_t address,uint8_t start_row, uint8_t end_row);

void SSD1306_Stopscroll(uint8_t address);

/*инвертирует отображение i = 1->инвертированный, i = 0->нормальный*/
void SSD1306_InvertDisplay(SSD1306_Structure *structure,int i);

/*очистить дисплей*/
void SSD1306_Clear(SSD1306_Structure *structure);

/*вспомог. функции*/
void Main_Screen(SSD1306_Structure *ssd, Screen_Structure* screen, uint8_t clr_time);
void Work_Screen(SSD1306_Structure *ssd, Screen_Structure* screen);
#endif
