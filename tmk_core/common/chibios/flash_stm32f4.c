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

#if defined(EEPROM_EMU_STM32F411xE)
#    define STM32F411xE
#    include "stm32f4xx.h"
#else
#    error "not implemented."
#endif

#include "flash_stm32f4.h"

#if defined(EEPROM_EMU_STM32F411xE)
#    define FLASH_SR_PGERR  (FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR)
#endif

#if STM32_FLASH_PSIZE != 2
#error This code only supports writing 32 bits at a time, try running your MCU at higher voltage?
#endif

/**
 * @brief  Erases a specified FLASH sector.
 * @param  Sector_Number: The sector number to be erased (0-7).
 * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *   FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_EraseSector(uint16_t Sector_Number) {
    FLASH_Status status = FLASH_COMPLETE;
    /* Check the parameters */
    ASSERT(IS_SECTOR_NUMBER(Sector_number));
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(EraseTimeout);
    if (status == FLASH_COMPLETE) {
        /* if the previous operation is completed, proceed to erase the sector */
        FLASH->CR &= ~FLASH_CR_PSIZE_Msk;
        FLASH->CR |= STM32_FLASH_PSIZE << FLASH_CR_PSIZE_Pos;
        FLASH->CR &= ~FLASH_CR_SNB_Msk;
        FLASH->CR |= FLASH_CR_SER | (Sector_Number << FLASH_CR_SNB_Pos);
        FLASH->CR |= FLASH_CR_STRT;

        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation(EraseTimeout);
        if (status != FLASH_TIMEOUT) {
            /* if the erase operation is completed, disable the SER Bit */
            FLASH->CR &= ~FLASH_CR_SER;
        }
        FLASH->SR = (FLASH_SR_EOP | FLASH_SR_PGERR | FLASH_SR_WRPERR);
    }
    /* Return the Erase Status */
    return status;
}

/**
 * @brief  Programs a full word at a specified address.
 * @param  Address: specifies the address to be programmed.
 * @param  Data: specifies the data to be programmed.
 * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
 *   FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_ProgramFullWord(uint32_t Address, uint32_t Data) {
    FLASH_Status status = FLASH_BAD_ADDRESS;

    if (IS_FLASH_ADDRESS(Address)) {
        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation(ProgramTimeout);
        if (status == FLASH_COMPLETE) {
            /* if the previous operation is completed, proceed to program the new data */
            FLASH->CR &= ~FLASH_CR_PSIZE_Msk;
            FLASH->CR |= STM32_FLASH_PSIZE << FLASH_CR_PSIZE_Pos;
            FLASH->CR |= FLASH_CR_PG;
            *(__IO uint32_t*)Address = Data;
            /* Wait for last operation to be completed */
            status = FLASH_WaitForLastOperation(ProgramTimeout);
            if (status != FLASH_TIMEOUT) {
                /* if the program operation is completed, disable the PG Bit */
                FLASH->CR &= ~FLASH_CR_PG;
            }
            FLASH->SR = (FLASH_SR_EOP | FLASH_SR_PGERR | FLASH_SR_WRPERR);
        }
    }
    return status;
}

void FLASH_Init(void) {
    FLASH->SR = (FLASH_SR_EOP | FLASH_SR_PGERR | FLASH_SR_WRPERR);
}
