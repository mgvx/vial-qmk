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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <ch.h>
#include <hal.h>

typedef enum { FLASH_BUSY = 1, FLASH_ERROR_PG, FLASH_ERROR_WRP, FLASH_ERROR_OPT, FLASH_COMPLETE, FLASH_TIMEOUT, FLASH_BAD_ADDRESS } FLASH_Status;

/* Delay definition */
#define EraseTimeout ((uint32_t)0x00000FFF)
#define ProgramTimeout ((uint32_t)0x0000001F)
#define ASSERT(exp) (void)((0))

#define IS_FLASH_ADDRESS(ADDRESS) (((ADDRESS) >= 0x08000000) && ((ADDRESS) < 0x0807FFFF))
#define IS_SECTOR_NUMBER(NUMBER) (((NUMBER) >= 0) && ((NUMBER) < 8))

FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
FLASH_Status FLASH_ProgramFullWord(uint32_t Address, uint32_t Data);

void FLASH_Unlock(void);
void FLASH_Lock(void);
void FLASH_ClearFlag(uint32_t FLASH_FLAG);
void FLASH_Init(void);

#ifdef __cplusplus
}
#endif
