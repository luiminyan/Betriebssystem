#include <stddef.h>

/**
 * @brief Analog zu strchr(3)
 *
 * @return NULL falls c nicht in str
 * @return Pointer str + i für erstes i mit str[i] = c falls c in str
 */
char *bs_strchr(const char *str, char c);

/**
 * @brief Analog zu strlen(3)
 *
 * @return Länge von str
 */
size_t bs_strlen(const char *str);

/**
 * @brief Analog zu strcpy (3)
 *
 * Kopiert src in Puffer von dst
 *
 * @return Pointer zu dst
 */
char *bs_strcpy(char *dst, const char *src);

/**
 * @brief Analog zu strcat(3)
 *
 * Kopiert src in Puffer von dst, beginnend bei '\0' in dst.
 *
 * @return Pointer zu dst
 */
char *bs_strcat(char *dst, const char *src);

