/*
 * This software is experimental and a work in progress.
 * Under no circumstances should these files be used in relation to any critical system(s).
 * Use of these files is at your own risk.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This files are free to use from https://github.com/rogerclarkmelbourne/Arduino_STM32 and
 * https://github.com/leaflabs/libmaple
 *
 * Modifications for QMK and STM32F303 by Yiancar
 */

#if defined(EEPROM_EMU_STM32F303xC)
#    define STM32F303xC
#    include "stm32f3xx.h"
#elif defined(EEPROM_EMU_STM32F103xB)
#    define STM32F103xB
#    include "stm32f1xx.h"
#elif defined(EEPROM_EMU_STM32F072xB)
#    define STM32F072xB
#    include "stm32f0xx.h"
#elif defined(EEPROM_EMU_STM32F042x6)
#    define STM32F042x6
#    include "stm32f0xx.h"
#elif defined(EEPROM_EMU_STM32F411xE)
#    define STM32F411xE
#    include "stm32f4xx.h"
#else
#    error "not implemented."
#endif

#include "flash_stm32.h"

#if defined(EEPROM_EMU_STM32F103xB)
#    define FLASH_SR_WRPERR FLASH_SR_WRPRTERR
#endif

#if defined(EEPROM_EMU_STM32F411xE)
#    define FLASH_SR_PGERR  (FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR)
#    define FLASH_OBR_OPTERR  0
#    define FLASH_KEY1 0x45670123
#    define FLASH_KEY2 0xCDEF89AB
#endif

/**
 * @brief  Inserts a time delay.
 * @param  None
 * @retval None
 */
static void delay(void) {
    __IO uint32_t i = 0;
    for (i = 0xFF; i != 0; i--) {
    }
}

/**
 * @brief  Returns the FLASH Status.
 * @param  None
 * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *   FLASH_ERROR_WRP or FLASH_COMPLETE
 */
FLASH_Status FLASH_GetStatus(void) {
    if ((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY) return FLASH_BUSY;

    if ((FLASH->SR & FLASH_SR_PGERR) != 0) return FLASH_ERROR_PG;

    if ((FLASH->SR & FLASH_SR_WRPERR) != 0) return FLASH_ERROR_WRP;

    if ((FLASH->SR & FLASH_OBR_OPTERR) != 0) return FLASH_ERROR_OPT;

    return FLASH_COMPLETE;
}

/**
 * @brief  Waits for a Flash operation to complete or a TIMEOUT to occur.
 * @param  Timeout: FLASH progamming Timeout
 * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
 *   FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout) {
    FLASH_Status status;

    /* Check for the Flash Status */
    status = FLASH_GetStatus();
    /* Wait for a Flash operation to complete or a TIMEOUT to occur */
    while (status == FLASH_BUSY) {
        delay();
        status = FLASH_GetStatus();
    }
    if (Timeout == 0) status = FLASH_TIMEOUT;
    /* Return the operation status */
    return status;
}

/**
 * @brief  Unlocks the FLASH Program Erase Controller.
 * @param  None
 * @retval None
 */
void FLASH_Unlock(void) {
    /* Authorize the FPEC Access */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}

/**
 * @brief  Locks the FLASH Program Erase Controller.
 * @param  None
 * @retval None
 */
void FLASH_Lock(void) {
    /* Set the Lock Bit to lock the FPEC and the FCR */
    FLASH->CR |= FLASH_CR_LOCK;
}

/**
 * @brief  Clears the FLASH's pending flags.
 * @param  FLASH_FLAG: specifies the FLASH flags to clear.
 *   This parameter can be any combination of the following values:
 *     @arg FLASH_FLAG_PGERR: FLASH Programming error flag flag
 *     @arg FLASH_FLAG_WRPERR: FLASH Write protected error flag
 *     @arg FLASH_FLAG_EOP: FLASH End of Programming flag
 * @retval None
 */
void FLASH_ClearFlag(uint32_t FLASH_FLAG) {
    /* Clear the flags */
    FLASH->SR = FLASH_FLAG;
}
