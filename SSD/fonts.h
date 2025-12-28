#ifndef FONTS_H
#define FONTS_H

/*includes*/
#include "stm32f4xx.h"
#include "string.h"

/*defaines*/

/*var*/

/*structures*/
/*Структура шрифтов для LCD*/
typedef struct {
	uint8_t FontWidth;    /*!< ширина шрифта в пикселях */
	uint8_t FontHeight;   /*!< высота шрифта в пикселях */
	const uint16_t *data; /*!< указатель на массив */
} FontDef_t;

/*5 x 7 pixels*/
extern FontDef_t Font_5x7;

/*7 x 10 pixels*/
extern FontDef_t Font_7x10;

/*11 x 18 pixels*/
extern FontDef_t Font_11x18;

/*16 x 26 pixels*/
extern FontDef_t Font_16x26;

/*Структура размеров шрифта*/
typedef struct {
	uint16_t Length;      /*!< длинна шрифта в пикселях */
	uint16_t Height;      /*!< высота шрифта в пикселях */
} FONTS_SIZE_t;

/*func*/
/**
 * @brief  Вычисляет длину и высоту строки в пикселях в зависимости от используемой строки и шрифта. 
 * @param  *str: Строка, которую необходимо проверить на длину и высоту
 * @param  *SizeStruct: Указатель на пустую структуру @ref FONTS_SIZE_t, где будет сохранена информация
 * @param  *Font: Указатель на шрифт @ref FontDef_t, используемый для расчетов
 * @retval Указатель на строку, используемую для длины и высоты
 */
char* FONTS_GetStringSize(char* str, FONTS_SIZE_t* SizeStruct, FontDef_t* Font);

#endif